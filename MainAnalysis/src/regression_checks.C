#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/train.h"
#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TLine.h"
#include "TTree.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

void fill_hist(pjtree* p, int type, int index, TH1F* hist, bool heavyion, 
    float iso_max, float geniso_max, float see_max, float see_min) {

    if ((*p->phoSigmaIEtaIEta_2012)[index] < see_max
        || (*p->phoSigmaIEtaIEta_2012)[index] > see_min) {

        /* hem failure region exclusion */
        if (!heavyion || !within_hem_failure_region(p, index)) { 

            /* isolation requirement */
            float isolation = (*p->pho_ecalClusterIsoR3)[index]
                + (*p->pho_hcalRechitIsoR3)[index]
                + (*p->pho_trackIsoR3PtCut20)[index];

            if (isolation < iso_max) { 

                int64_t gen_index = (*p->pho_genMatchedIndex)[index];

                if (gen_index == -1) { continue; }

                auto pid = (*p->mcPID)[gen_index];
                auto mpid = (*p->mcMomPID)[gen_index];

                if (pid != 22 || (std::abs(mpid) > 22 && mpid != -999)) { continue; }
                if ((*p->mcCalIsoDR04)[gen_index] > geniso_max) { continue; }

                float phoEt = 0;
                if (type == 1) phoEt = (*p->phoEt)[index];
                if (type == 2) phoEt = (*p->phoEtEr)[index];
                if (type == 3) phoEt = (*p->phoEtErNew)[index];

                auto ratio = phoEt / (*p->mcEt)[gen_index];

                hist->Fill(ratio, p->weight);
            }
        }
    }
}

int regression_checks(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto max_entries = conf->get<int64_t>("max_entries");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto heavyion = conf->get<bool>("heavyion");

    auto const eta_abs = conf->get<float>("eta_abs");
    auto const hovere_max = conf->get<float>("hovere_max");
    auto const see_min = conf->get<float>("see_min");
    auto const see_max = conf->get<float>("see_max");
    auto const iso_max = conf->get<float>("iso_max");
    auto geniso_max = conf->get<float>("geniso_max");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto mpthf = new multival(dpt, dhf);
    auto hf_min = dhf.front();

    /* load forest */
    TFile* f = new TFile(input.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto p = new pjtree(true, true, false, t, { 1, 1, 1, 1, 0, 1, 0 });

    auto hscale = new TH1F("photon_energy_scale","Photon Energy Scale",50,0.5,1.5);
    auto hscale_cor = new TH1F("photon_energy_scale_cor","Corrected Photon Energy Scale v1",50,0.5,1.5);
    auto hscale_cor_2 = new TH1F("photon_energy_scale_cor_2","Corrected Photon Energy Scale v2",50,0.5,1.5);


    int stats = 0;
    /* iterate */
    auto nentries = static_cast<int64_t>(t->GetEntries());
    if (max_entries) nentries = std::min(nentries, max_entries);
    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 100000 == 0)
            printf("entry: %li/%li\n", i, nentries);

        t->GetEntry(i);

        if (std::abs(p->vz) > 15) { continue; }
        if (p->hiHF <= hf_min) { continue; }

        int64_t leading = -1;
        float leading_pt = 0;

        int64_t leading_cor = -1;
        float leading_pt_cor = 0;

        for (int64_t j = 0; j < p->nPho; ++j) {
            if ((*p->phoEt)[j] <= 25) { continue; }
            if (std::abs((*p->phoSCEta)[j]) >= eta_abs) { continue; }
            if ((*p->phoHoverE)[j] > hovere_max) { continue; }

            if ((*p->phoEt)[j] > leading_pt) {
                leading = j;
                leading_pt = (*p->phoEt)[j];
            }

            if ((*p->phoEtErNew)[j] > leading_pt_cor) {
                leading_cor = j;
                leading_pt_cor = (*p->phoEtErNew)[j];
            }
        }

        /* require leading photon */
        if (leading < 0) { continue; }

        fill_hist(p, 1, leading, hscale, heavyion, iso_max, geniso_max, see_max, see_min);
        fill_hist(p, 2, leading, hscale_cor, heavyion, iso_max, geniso_max, see_max, see_min);
        fill_hist(p, 3, leading, hscale_cor_2, heavyion, iso_max, geniso_max, see_max, see_min);
    }

    std::cout << stats << std::endl;

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, mpthf, pt_info, hf_info); };

    auto mean_info = [&](int64_t index) {
        char buffer[128] = { '\0' };

        TLatex* info = new TLatex();
        info->SetTextFont(43);
        info->SetTextSize(11);

        sprintf(buffer, "Uncor. mean: %.3f", hscale->GetMean());
        info->DrawLatexNDC(0.675, 0.78, buffer);
        sprintf(buffer, "Uncor. sigma: %.3f", hscale->GetMeanError());
        info->DrawLatexNDC(0.675, 0.75, buffer);

        sprintf(buffer, "Cor. v1 mean: %.3f", hscale_cor->GetMean());
        info->DrawLatexNDC(0.675, 0.71, buffer);
        sprintf(buffer, "Cor. v1 sigma: %.3f", hscale_cor->GetMeanError());
        info->DrawLatexNDC(0.675, 0.68, buffer);

        sprintf(buffer, "Cor. v2 mean: %.3f", hscale_cor_2->GetMean());
        info->DrawLatexNDC(0.675, 0.64, buffer);
        sprintf(buffer, "Cor. v2 sigma: %.3f", hscale_cor_2->GetMeanError());
        info->DrawLatexNDC(0.675, 0.61, buffer);

        std::cout << index << std::endl;
    };

    auto hb = new pencil();
    hb->category("type", "Uncorrected", "Corrected");

    auto c1 = new paper(tag + "photon_energy_resolution", hb);
    apply_style(c1, system + " #sqrt{s} = 5.02 TeV"s);
    c1->accessory(pthf_info);
    c1->accessory(mean_info);

    c1->add(hscale, "Uncorrected");
    c1->stack(hscale_cor, "Corrected v1");
    c1->stack(hscale_cor, "Corrected v2");

    hb->sketch();
    c1->draw("pdf");

    /* save histograms */
    TFile* o = new TFile(output, "recreate");

    hscale->Write();
    hscale_cor->Write();
    hscale_cor_2->Write();

    o->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return regression_checks(argv[1],argv[2]);

    return 0;
}
