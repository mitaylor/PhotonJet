#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"
#include "../include/text.h"

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

void fill_hist(pjtree* p, int type, int index, memory<TH1F>* hist, TH2F* hcorrelation, bool heavyion, bool gen,
    multival* mpthf, float photon_pt_min, float iso_max, float gen_iso_max, float see_max, float see_min) {

    if ((*p->phoSigmaIEtaIEta_2012)[index] < see_max
        && (*p->phoSigmaIEtaIEta_2012)[index] > see_min) {

        /* hem failure region exclusion */
        if (!heavyion || !in_pho_failure_region(p, index)) { 

            /* isolation requirement */
            float isolation = (*p->pho_ecalClusterIsoR3)[index]
                + (*p->pho_hcalRechitIsoR3)[index]
                + (*p->pho_trackIsoR3PtCut20)[index];

            if (isolation < iso_max) { 
                float phoEt = 0;
                if (type == 1) phoEt = (*p->phoEt)[index];
                if (type == 2 && !heavyion) phoEt = (*p->phoEtEr)[index];
                if (type == 2 && heavyion) phoEt = (*p->phoEtErNew)[index];

                if (heavyion && type == 1)  hcorrelation->Fill((*p->phoEt)[index], (*p->phoEtErNew)[index], p->weight);
                if (!heavyion && type == 1) hcorrelation->Fill((*p->phoEt)[index], (*p->phoEtEr)[index], p->weight);

                int64_t gen_index = (*p->pho_genMatchedIndex)[index];

                if (gen_index == -1) { return; }

                auto pid = (*p->mcPID)[gen_index];
                auto mpid = (*p->mcMomPID)[gen_index];

                if (pid != 22 || (std::abs(mpid) > 22 && mpid != -999)) { return; }
                if ((*p->mcCalIsoDR04)[gen_index] > gen_iso_max) { return; }

                auto mcEt = (*p->mcEt)[gen_index];
                auto ratio = phoEt / mcEt;

                auto binningEt = gen ? mcEt : phoEt;

                if (photon_pt_min < binningEt && binningEt < 200) {
                    int64_t index = mpthf->index_for(v{binningEt, p->hiHF});
                    (*hist)[index]->Fill(ratio, p->weight);
                }
            }
        }
    }
}

int regression_checks(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::vector<std::string>>("input");
    
    auto max_entries = conf->get<int64_t>("max_entries");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto heavyion = conf->get<bool>("heavyion");
    auto gen = conf->get<bool>("gen");
    auto bkg = conf->get<bool>("bkg");

    auto dpt = conf->get<std::vector<float>>("photon_pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto sel = new configurer(selections);

    auto const photon_pt_min = conf->get<float>("photon_pt_min");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");
    auto const hovere_max = sel->get<float>("hovere_max");
    auto see_min = sel->get<float>("see_min");
    auto see_max = sel->get<float>("see_max");
    auto const iso_max = sel->get<float>("iso_max");
    auto gen_iso_max = sel->get<float>("gen_iso_max");

    if (bkg) see_min = 0.012;
    if (bkg) see_max = 0.02;

    auto mpthf = new multival(dpt, dhf);
    auto hf_min = dhf.front();

    auto ipt = new interval(dpt);
    auto iratio = new interval("Reco Et / Gen Et"s, 100, 0.5, 2);
    auto fratio = std::bind(&interval::book<TH1F>, iratio, _1, _2, _3);

    auto hscale = new memory<TH1F>("photon_energy_scale"s, "counts", fratio, mpthf);
    auto hscale_corrected = new memory<TH1F>("photon_energy_scale_cor"s, "counts", fratio, mpthf);

    auto hcorrelation = new TH2F("photon_enery_correlation", ";phoEt;phoEtEr", 60, photon_pt_min, 80, 60, 40, 150);
    auto hcorrelation_selected = new TH2F("photon_enery_correlation_selected", ";phoEt;phoEtEr", 60, photon_pt_min, 80, 60, 40, 150);

    /* iterate */
    for (auto const& file : input) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto p = new pjtree(true, false, false, t, { 1, 1, 1, 1, 0, 0, 0, 0, 0 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());
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
                if (heavyion) hcorrelation->Fill((*p->phoEt)[j], (*p->phoEtErNew)[j], p->weight);
                else          hcorrelation->Fill((*p->phoEt)[j], (*p->phoEtEr)[j], p->weight);

                if ((*p->phoEt)[j] <= 25) { continue; }
                if (std::abs((*p->phoSCEta)[j]) >= photon_eta_abs) { continue; }
                if ((*p->phoHoverE)[j] > hovere_max) { continue; }

                if ((*p->phoEt)[j] > leading_pt) {
                    leading = j;
                    leading_pt = (*p->phoEt)[j];
                }

                if (!heavyion && (*p->phoEtEr)[j] > leading_pt_cor) {
                    leading_cor = j;
                    leading_pt_cor = (*p->phoEtEr)[j];
                }

                if (heavyion && (*p->phoEtErNew)[j] > leading_pt_cor) {
                    leading_cor = j;
                    leading_pt_cor = (*p->phoEtErNew)[j];
                }
            }

            /* require leading photon */
            if (leading < 0) { continue; }

            fill_hist(p, 1, leading, hscale, hcorrelation_selected, heavyion, gen, mpthf, photon_pt_min, iso_max, gen_iso_max, see_max, see_min);
            fill_hist(p, 2, leading_cor, hscale_corrected, hcorrelation_selected, heavyion, gen, mpthf, photon_pt_min, iso_max, gen_iso_max, see_max, see_min);
        }
    }

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        if (gen) {
            info_text(x, pos, "%.0f < Gen p_{T}^{#gamma} < %.0f", dpt, false);
        }
        else {
            info_text(x, pos, "%.0f < Reco p_{T}^{#gamma} < %.0f", dpt, false);
        }
    };

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

        sprintf(buffer, "Cor. mean: %.3f", (*hscale_corrected)[index - 1]->GetMean());
        info->DrawLatexNDC(0.5, 0.57, buffer);
        sprintf(buffer, "Cor. sigma: %.3f", (*hscale_corrected)[index - 1]->GetStdDev());
        info->DrawLatexNDC(0.5, 0.54, buffer);
    };

    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV"s;
    system_tag += (system == "PbPb") ? ", 1.69 nb^{-1}"s : ", 302 pb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}}"s;
    cms += " #it{#scale[1.2]{Preliminary}}"s;

    auto hb = new pencil();
    hb->category("type", "Uncorrected", "Corrected");

    auto type = gen ? "gen"s : "reco"s;
    auto region = bkg ? "bkg"s : "sig"s;

    auto c1 = new paper(tag + "_photon_energy_resolution_" + type + "_" + to_text(photon_pt_min) + "_" + region, hb);
    apply_style(c1, cms, system_tag);
    c1->accessory(pthf_info);
    c1->accessory(mean_info);
    c1->divide(ipt->size(), -1);

    for (int64_t i = 0; i < mpthf->size(); ++i) {
        c1->add((*hscale)[i], "Uncorrected");
        c1->stack((*hscale_corrected)[i], "Corrected");
    }

    auto c2 = new paper(tag + "_photon_energy_regression_" + type + "_" + to_text(photon_pt_min) + "_" + region, hb);
    apply_style(c2, cms, system_tag);
    c2->add(hcorrelation);
    c2->adjust(hcorrelation, "colz", "");

    auto c3 = new paper(tag + "_photon_energy_regression_selected_" + type + "_" + to_text(photon_pt_min) + "_" + region, hb);
    apply_style(c3, cms, system_tag);
    c3->add(hcorrelation_selected);
    c3->adjust(hcorrelation_selected, "colz", "");

    hb->sketch();
    c1->draw("pdf");
    c2->draw("pdf");
    c3->draw("pdf");

    /* save histograms */
    TFile* o = new TFile(output, "recreate");

    hcorrelation->Write();
    hcorrelation_selected->Write();

    o->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return regression_checks(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
