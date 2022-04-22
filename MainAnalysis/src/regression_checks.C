#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/train.h"
#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TLine.h"
#include "TTree.h"
#include "TH2F.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

void fill_hist(pjtree* p, int type, int index, memory<TH1F>* hist, TH2F* hetaphi, bool heavyion, 
    multival* mpthf, float iso_max, float geniso_max, float see_max, float see_min) {

    if ((*p->phoSigmaIEtaIEta_2012)[index] < see_max
        || (*p->phoSigmaIEtaIEta_2012)[index] > see_min) {

        /* hem failure region exclusion */
        if (!heavyion || !within_hem_failure_region(p, index)) { 

            /* isolation requirement */
            float isolation = (*p->pho_ecalClusterIsoR3)[index]
                + (*p->pho_hcalRechitIsoR3)[index]
                + (*p->pho_trackIsoR3PtCut20)[index];

            if (isolation < iso_max) { 
                float phoEt = 0;
                if (type == 1) phoEt = (*p->phoEt)[index];
                if (type == 2) phoEt = (*p->phoEtEr)[index];
                if (type == 3) phoEt = (*p->phoEtErNew)[index];

                if (phoEt > 40) {
                    hetaphi->Fill((*p->phoEta)[index], (*p->phoPhi)[index], p->weight);
                }

                int64_t gen_index = (*p->pho_genMatchedIndex)[index];

                if (gen_index == -1) { return; }

                auto pid = (*p->mcPID)[gen_index];
                auto mpid = (*p->mcMomPID)[gen_index];

                if (pid != 22 || (std::abs(mpid) > 22 && mpid != -999)) { return; }
                if ((*p->mcCalIsoDR04)[gen_index] > geniso_max) { return; }

                auto mcEt = (*p->mcEt)[gen_index];
                auto ratio = phoEt / mcEt;

                if (30 < phoEt && phoEt < 200) {
                    int64_t index = mpthf->index_for(v{phoEt, p->hiHF});
                    (*hist)[index]->Fill(ratio, p->weight);
                }
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

    auto ipt = new interval(dpt);
    auto iratio = new interval("Reco Et / Gen Et"s, 100, 0.5, 2);
    auto fratio = std::bind(&interval::book<TH1F>, iratio, _1, _2, _3);

    /* load forest */
    TFile* f = new TFile(input.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto p = new pjtree(true, true, false, t, { 1, 1, 1, 1, 0, 1, 0 });

    auto hscale = new memory<TH1F>("photon_energy_scale"s, "counts", fratio, mpthf);
    auto hscale_cor = new memory<TH1F>("photon_energy_scale_cor"s, "counts", fratio, mpthf);
    auto hscale_cor_2 = new memory<TH1F>("photon_energy_scale_cor_2"s, "counts", fratio, mpthf);

    auto hetaphi = new TH2F("photon_eta_phi","",60,-1.442,1.442,60,-3.15,3.15);
    auto hetaphi_cor = new TH2F("photon_eta_phi_cor","",60,-1.442,1.442,60,-3.15,3.15);
    auto hetaphi_cor_2 = new TH2F("photon_eta_phi_cor_2","",60,-1.442,1.442,60,-3.15,3.15);

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

        int64_t leading_cor_2 = -1;
        float leading_pt_cor_2 = 0;

        for (int64_t j = 0; j < p->nPho; ++j) {
            if ((*p->phoEt)[j] <= 30) { continue; }
            if (std::abs((*p->phoSCEta)[j]) >= eta_abs) { continue; }
            if ((*p->phoHoverE)[j] > hovere_max) { continue; }

            if ((*p->phoEt)[j] > leading_pt) {
                leading = j;
                leading_pt = (*p->phoEt)[j];
            }

            if ((*p->phoEtErNew)[j] > leading_pt_cor) {
                leading_cor = j;
                leading_pt_cor = (*p->phoEtEr)[j];
            }

            if ((*p->phoEtErNew)[j] > leading_pt_cor_2) {
                leading_cor_2 = j;
                leading_pt_cor_2 = (*p->phoEtErNew)[j];
            }
        }

        /* require leading photon */
        if (leading < 0) { continue; }

        fill_hist(p, 1, leading, hscale, hetaphi, heavyion, mpthf, iso_max, geniso_max, see_max, see_min);
        fill_hist(p, 2, leading_cor, hscale_cor, hetaphi_cor, heavyion, mpthf, iso_max, geniso_max, see_max, see_min);
        fill_hist(p, 3, leading_cor_2, hscale_cor_2, hetaphi_cor_2, heavyion, mpthf, iso_max, geniso_max, see_max, see_min);
    }

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < Reco p_{T}^{#gamma} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, mpthf, pt_info, hf_info); };

    auto mean_info = [&](int64_t index) {
        char buffer[128] = { '\0' };

        TLatex* info = new TLatex();
        info->SetTextFont(43);
        info->SetTextSize(11);

        sprintf(buffer, "Uncor. mean: %.3f", (*hscale)[index - 1]->GetMean());
        info->DrawLatexNDC(0.5, 0.64, buffer);
        sprintf(buffer, "Uncor. sigma: %.3f", (*hscale)[index - 1]->GetStdDev());
        info->DrawLatexNDC(0.5, 0.61, buffer);

        sprintf(buffer, "Cor. v1 mean: %.3f", (*hscale_cor)[index - 1]->GetMean());
        info->DrawLatexNDC(0.5, 0.57, buffer);
        sprintf(buffer, "Cor. v1 sigma: %.3f", (*hscale_cor)[index - 1]->GetStdDev());
        info->DrawLatexNDC(0.5, 0.54, buffer);

        sprintf(buffer, "Cor. v2 mean: %.3f", (*hscale_cor_2)[index - 1]->GetMean());
        info->DrawLatexNDC(0.5, 0.50, buffer);
        sprintf(buffer, "Cor. v2 sigma: %.3f", (*hscale_cor_2)[index - 1]->GetStdDev());
        info->DrawLatexNDC(0.5, 0.47, buffer);
    };

    auto hb = new pencil();
    hb->category("type", "Uncorrected", "Corrected v1", "Corrected v2");

    auto c1 = new paper(tag + "_photon_energy_resolution_cut30", hb);
    apply_style(c1, system + " #sqrt{s} = 5.02 TeV"s);
    c1->accessory(pthf_info);
    c1->accessory(mean_info);
    c1->divide(ipt->size(), -1);
    // c1->set(paper::flags::logy);


    for (int64_t i = 0; i < mpthf->size(); ++i) {
        c1->add((*hscale)[i], "Uncorrected");
        c1->stack((*hscale_cor)[i], "Corrected v1");
        c1->stack((*hscale_cor_2)[i], "Corrected v2");
    }

    hb->sketch();
    c1->draw("pdf");

    /* save histograms */

    TFile* o = new TFile(output, "recreate");

    hetaphi->Write();
    hetaphi_cor->Write();
    hetaphi_cor_2->Write();

    o->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return regression_checks(argv[1],argv[2]);

    return 0;
}
