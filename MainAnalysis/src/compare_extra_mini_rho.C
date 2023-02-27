#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TH1F.h"
#include "TLatex.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int narrate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto nominal_miniaod = conf->get<std::vector<std::string>>("nominal_miniaod");
    auto old_extra_aod = conf->get<std::vector<std::string>>("old_extra_aod");
    auto new_extra_aod = conf->get<std::vector<std::string>>("new_extra_aod");

    TH1::SetDefaultSumw2();
    
    auto irho = new interval("#rho"s, 100, 0, 400);
    auto frho = std::bind(&interval::book<TH1F>, irho, _1, _2, _3);

    auto rho_ana_nominal_miniaod = new history<TH1F>("rho_ana_nominal_miniaod"s, "", frho, 1, 1);
    auto rho_ana_old_extra_aod = new history<TH1F>("rho_ana_old_extra_aod"s, "", frho, 1, 1);
    auto rho_ana_new_extra_aod = new history<TH1F>("rho_ana_new_extra_aod"s, "", frho, 1, 1);

    auto rho_pho_nominal_miniaod = new history<TH1F>("rho_pho_nominal_miniaod"s, "", frho, 1, 1);
    auto rho_pho_old_extra_aod = new history<TH1F>("rho_pho_old_extra_aod"s, "", frho, 1, 1);
    auto rho_pho_new_extra_aod = new history<TH1F>("rho_pho_new_extra_aod"s, "", frho, 1, 1);

    auto rho_ana_ratio_old_extra_aod = new history<TH1F>("rho_ana_ratio_old_extra_aod"s, "", frho, 1, 1);
    auto rho_ana_ratio_new_extra_aod = new history<TH1F>("rho_ana_ratio_new_extra_aod"s, "", frho, 1, 1);

    auto rho_pho_ratio_old_extra_aod = new history<TH1F>("rho_pho_ratio_old_extra_aod"s, "", frho, 1, 1);
    auto rho_pho_ratio_new_extra_aod = new history<TH1F>("rho_pho_ratio_new_extra_aod"s, "", frho, 1, 1);

    for (auto const& file : nominal_miniaod) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(false, false, true, t, { 1, 0, 1, 1, 0, 0, 1, 0, 0 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        for (int64_t i = 0; i < nentries; ++i) {
            if (i % 100000 == 0)
                printf("entry: %li/%li\n", i, nentries);

            t->GetEntry(i);

            if (std::abs(pjt->vz) > 15) { continue; }

            int64_t leading = -1;
            float leading_pt = 0;
            for (int64_t j = 0; j < pjt->nPho; ++j) {
                if ((*pjt->phoEt)[j] <= 30) { continue; }
                if (std::abs((*pjt->phoSCEta)[j]) >= 1.442) { continue; }
                if ((*pjt->phoHoverE)[j] > 0.119947) { continue; }

                auto pho_et = (*pjt->phoEtErNew)[j];
                if (pho_et < 40) { continue; }

                if (pho_et > leading_pt) {
                    leading = j;
                    leading_pt = pho_et;
                }
            }

            if (leading < 0) { continue; }
            if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > 0.010392) { continue; }
            // if (in_pho_failure_region(pjt, leading)) { continue; }

            float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
                + (*pjt->pho_hcalRechitIsoR3)[leading]
                + (*pjt->pho_trackIsoR3PtCut20)[leading];
            if (isolation > 2.099277) { continue; }

            auto avg_rho = get_avg_rho(pjt, -1.442, 1.442);

            (*rho_ana_nominal_miniaod)[0]->Fill(avg_rho);
            (*rho_pho_nominal_miniaod)[0]->Fill(pjt->rho);
        }
    }

    for (auto const& file : old_extra_aod) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(false, false, true, t, { 1, 0, 1, 1, 0, 0, 1, 0, 0 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        for (int64_t i = 0; i < nentries; ++i) {
            if (i % 100000 == 0)
                printf("entry: %li/%li\n", i, nentries);

            t->GetEntry(i);

            if (std::abs(pjt->vz) > 15) { continue; }

            int64_t leading = -1;
            float leading_pt = 0;
            for (int64_t j = 0; j < pjt->nPho; ++j) {
                if ((*pjt->phoEt)[j] <= 30) { continue; }
                if (std::abs((*pjt->phoSCEta)[j]) >= 1.442) { continue; }
                if ((*pjt->phoHoverE)[j] > 0.119947) { continue; }

                auto pho_et = (*pjt->phoEtErNew)[j];
                if (pho_et < 40) { continue; }

                if (pho_et > leading_pt) {
                    leading = j;
                    leading_pt = pho_et;
                }
            }

            if (leading < 0) { continue; }
            if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > 0.010392) { continue; }
            // if (in_pho_failure_region(pjt, leading)) { continue; }

            float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
                + (*pjt->pho_hcalRechitIsoR3)[leading]
                + (*pjt->pho_trackIsoR3PtCut20)[leading];
            if (isolation > 2.099277) { continue; }

            auto avg_rho = get_avg_rho(pjt, -1.442, 1.442);

            (*rho_ana_old_extra_aod)[0]->Fill(avg_rho);
            (*rho_pho_old_extra_aod)[0]->Fill(pjt->rho);
        }
    }

    for (auto const& file : new_extra_aod) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(false, false, true, t, { 1, 0, 1, 1, 0, 0, 1, 0, 0 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        for (int64_t i = 0; i < nentries; ++i) {
            if (i % 100000 == 0)
                printf("entry: %li/%li\n", i, nentries);

            t->GetEntry(i);

            if (std::abs(pjt->vz) > 15) { continue; }

            int64_t leading = -1;
            float leading_pt = 0;
            for (int64_t j = 0; j < pjt->nPho; ++j) {
                if ((*pjt->phoEt)[j] <= 30) { continue; }
                if (std::abs((*pjt->phoSCEta)[j]) >= 1.442) { continue; }
                if ((*pjt->phoHoverE)[j] > 0.119947) { continue; }

                auto pho_et = (*pjt->phoEtErNew)[j];
                if (pho_et < 40) { continue; }

                if (pho_et > leading_pt) {
                    leading = j;
                    leading_pt = pho_et;
                }
            }

            if (leading < 0) { continue; }
            if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > 0.010392) { continue; }
            // if (in_pho_failure_region(pjt, leading)) { continue; }

            float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
                + (*pjt->pho_hcalRechitIsoR3)[leading]
                + (*pjt->pho_trackIsoR3PtCut20)[leading];
            if (isolation > 2.099277) { continue; }

            auto avg_rho = get_avg_rho(pjt, -1.442, 1.442);

            (*rho_ana_new_extra_aod)[0]->Fill(avg_rho);
            (*rho_pho_new_extra_aod)[0]->Fill(pjt->rho);
        }
    }

    (*rho_ana_nominal_miniaod)[0]->Scale(1. / (*rho_ana_nominal_miniaod)[0]->Integral());
    (*rho_pho_nominal_miniaod)[0]->Scale(1. / (*rho_pho_nominal_miniaod)[0]->Integral());
    (*rho_ana_old_extra_aod)[0]->Scale(1. / (*rho_ana_old_extra_aod)[0]->Integral());
    (*rho_pho_old_extra_aod)[0]->Scale(1. / (*rho_pho_old_extra_aod)[0]->Integral());
    (*rho_ana_new_extra_aod)[0]->Scale(1. / (*rho_ana_new_extra_aod)[0]->Integral());
    (*rho_pho_new_extra_aod)[0]->Scale(1. / (*rho_pho_new_extra_aod)[0]->Integral());

    (*rho_ana_ratio_old_extra_aod)[0]->Divide((*rho_ana_old_extra_aod)[0], (*rho_ana_nominal_miniaod)[0]);
    (*rho_pho_ratio_old_extra_aod)[0]->Divide((*rho_pho_old_extra_aod)[0], (*rho_pho_nominal_miniaod)[0]);
    (*rho_ana_ratio_new_extra_aod)[0]->Divide((*rho_ana_new_extra_aod)[0], (*rho_ana_nominal_miniaod)[0]);
    (*rho_pho_ratio_new_extra_aod)[0]->Divide((*rho_pho_new_extra_aod)[0], (*rho_pho_nominal_miniaod)[0]);

    /* draw rho distributions */
    auto system_tag = "PbPb  #sqrt{s_{NN}} = 5.02 TeV"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;
    cms += "         p_{T}^{#gamma} > 40 GeV";

    auto hb = new pencil();
    hb->category("type", "nominal_miniaod", "new_extra_aod", "old_extra_aod");
    hb->category("system", "pp", "aa", "ss");

    hb->alias("nominal_miniaod", "Nominal MC MiniAOD");
    hb->alias("new_extra_aod", "New Extra MC AOD");
    hb->alias("old_extra_aod", "Old Extra MC AOD");
    
    auto c1 = new paper("aa_qcd_hiPuRhoAnalyzer_normal", hb);
    apply_style(c1, cms, system_tag);
    c1->add((*rho_ana_nominal_miniaod)[0], "nominal_miniaod");
    c1->stack((*rho_ana_old_extra_aod)[0], "old_extra_aod");
    c1->stack((*rho_ana_new_extra_aod)[0], "new_extra_aod");
    
    auto c2 = new paper("aa_qcd_hiPuRhoAnalyzer_log", hb);
    apply_style(c2, cms, system_tag);
    c2->set(paper::flags::logy);
    c2->add((*rho_ana_nominal_miniaod)[0], "nominal_miniaod");
    c2->stack((*rho_ana_old_extra_aod)[0], "old_extra_aod");
    c2->stack((*rho_ana_new_extra_aod)[0], "new_extra_aod");

    auto c3 = new paper("aa_qcd_hiPuRhoAnalyzer_ratio", hb);
    apply_style(c3, cms, system_tag);
    c3->add((*rho_ana_ratio_old_extra_aod)[0], "old_extra_aod");
    c3->stack((*rho_ana_ratio_new_extra_aod)[0], "new_extra_aod");

    auto c4 = new paper("aa_qcd_ggHiNtuplizer_normal", hb);
    apply_style(c4, cms, system_tag);
    c4->add((*rho_pho_nominal_miniaod)[0], "nominal_miniaod");
    c4->stack((*rho_pho_old_extra_aod)[0], "old_extra_aod");
    c4->stack((*rho_pho_new_extra_aod)[0], "new_extra_aod");

    auto c5 = new paper("aa_qcd_ggHiNtuplizer_log", hb);
    apply_style(c5, cms, system_tag);
    c5->set(paper::flags::logy);
    c5->add((*rho_pho_nominal_miniaod)[0], "nominal_miniaod");
    c5->stack((*rho_pho_old_extra_aod)[0], "old_extra_aod");
    c5->stack((*rho_pho_new_extra_aod)[0], "new_extra_aod");

    auto c6 = new paper("aa_qcd_ggHiNtuplizer_ratio", hb);
    apply_style(c6, cms, system_tag);
    c6->add((*rho_pho_ratio_old_extra_aod)[0], "old_extra_aod");
    c6->stack((*rho_pho_ratio_new_extra_aod)[0], "new_extra_aod");

    hb->sketch();
    c1->draw("pdf");
    c2->draw("pdf");
    c3->draw("pdf");
    c4->draw("pdf");
    c5->draw("pdf");
    c6->draw("pdf");

    /* save output */
    in(output, [&]() {
        rho_ana_nominal_miniaod->save();
        rho_pho_nominal_miniaod->save();
        rho_ana_old_extra_aod->save();
        rho_pho_old_extra_aod->save();
        rho_ana_new_extra_aod->save();
        rho_pho_new_extra_aod->save();
        rho_ana_ratio_old_extra_aod->save();
        rho_pho_ratio_old_extra_aod->save();
        rho_ana_ratio_new_extra_aod->save();
        rho_pho_ratio_new_extra_aod->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return narrate(argv[1], argv[2]);

    return 0;
}
