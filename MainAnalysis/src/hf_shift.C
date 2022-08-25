#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/overflow_angles.h"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLatex.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int hf_shift(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto hp_input = conf->get<std::string>("hp_input");
    auto mb_input = conf->get<std::string>("mb_input");

    auto const photon_pt_min = conf->get<float>("photon_pt_min");
    auto const photon_eta_abs = conf->get<float>("photon_eta_abs");

    auto const hovere_max = conf->get<float>("hovere_max");
    auto const see_max = conf->get<float>("see_max");
    auto const iso_max = conf->get<float>("iso_max");

    TH1::SetDefaultSumw2();
    
    auto irho = new interval("#rho"s, 100, 0, 400);
    auto ihf = new interval("HF Energy"s, 150, 0, 6000);

    auto mrh = new multival(*irho, *ihf);

    auto frh = std::bind(&multival::book<TH2F>, mrh, _1, _2, _3);

    auto hp_rh = new history<TH2F>("hp_rh"s, "Pythia+Hydjet", frh, 1);
    auto mb_rh = new history<TH2F>("mb_rh"s, "Hydjet", frh, 1);

    TFile* hp_f = new TFile(hp_input.data(), "read");
    TTree* hp_t = (TTree*) hp_f->Get("pj");
    auto hp_pjt = new pjtree(false, false, true, hp_t, { 1, 1, 1, 1, 1, 0, 1, 0 });

    TFile* mb_f = new TFile(mb_input.data(), "read");
    TTree* mb_t = (TTree*) mb_f->Get("pj");
    auto mb_pjt = new pjtree(false, false, true, mb_t, { 1, 1, 1, 1, 1, 0, 1, 0 });

    std::cout << static_cast<int64_t>(mb_t->GetEntries()) << std::endl;
    mb_t->GetEntry(1);
    int64_t nentries = 10000;

    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 100000 == 0)
            printf("entry: %li/%li\n", i, nentries);

        hp_t->GetEntry(i);

        int64_t leading = -1;
        float leading_pt = 0;
        bool apply_er = 1;
        for (int64_t j = 0; j < hp_pjt->nPho; ++j) {
            if ((*hp_pjt->phoEt)[j] <= 30) { continue; }
            if (std::abs((*hp_pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
            if ((*hp_pjt->phoHoverE)[j] > hovere_max) { continue; }

            auto pho_et = (*hp_pjt->phoEt)[j];
            if (apply_er) pho_et = (*hp_pjt->phoEtErNew)[j];

            if (pho_et < photon_pt_min) { continue; }

            if (pho_et > leading_pt) {
                leading = j;
                leading_pt = pho_et;
            }
        }

        if (leading < 0) { continue; }
        if ((*hp_pjt->phoSigmaIEtaIEta_2012)[leading] > see_max) { continue; }

        float isolation = (*hp_pjt->pho_ecalClusterIsoR3)[leading]
            + (*hp_pjt->pho_hcalRechitIsoR3)[leading]
            + (*hp_pjt->pho_trackIsoR3PtCut20)[leading];
        if (isolation > iso_max) { continue; }

        /* leading photon axis */
        auto photon_eta = (*hp_pjt->phoEta)[leading];
        auto photon_phi = convert_radian((*hp_pjt->phoPhi)[leading]);

        /* electron rejection */
        bool electron = false;
        for (int64_t j = 0; j < hp_pjt->nEle; ++j) {
            if (std::abs((*hp_pjt->eleSCEta)[j]) > 1.4442) { continue; }

            auto deta = photon_eta - (*hp_pjt->eleEta)[j];
            if (deta > 0.1) { continue; }

            auto ele_phi = convert_radian((*hp_pjt->elePhi)[j]);
            auto dphi = revert_radian(photon_phi - ele_phi);
            auto dr2 = deta * deta + dphi * dphi;

            if (dr2 < 0.01 && passes_electron_id<
                        det::barrel, wp::loose, pjtree
                    >(hp_pjt, j, true)) {
                electron = true; break; }
        }

        if (electron) { continue; }

        auto avg_rho = get_avg_rho(hp_pjt, -photon_eta_abs, photon_eta_abs);
        (*hp_rh)[0]->Fill(avg_rho, hp_pjt->hiHF, hp_pjt->w);
    }

    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 100000 == 0)
            printf("entry: %li/%li\n", i, nentries);
 std::cout << __LINE__ << std::endl;
        mb_t->GetEntry(i); std::cout << __LINE__ << std::endl;

        int64_t leading = -1;
        float leading_pt = 0;
        bool apply_er = 1;
        for (int64_t j = 0; j < mb_pjt->nPho; ++j) {
            if ((*mb_pjt->phoEt)[j] <= 30) { continue; }
            if (std::abs((*mb_pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
            if ((*mb_pjt->phoHoverE)[j] > hovere_max) { continue; }

            auto pho_et = (*mb_pjt->phoEt)[j];
            if (apply_er) pho_et = (*mb_pjt->phoEtErNew)[j];

            if (pho_et < photon_pt_min) { continue; }

            if (pho_et > leading_pt) {
                leading = j;
                leading_pt = pho_et;
            }
        }

        if (leading < 0) { continue; }
        if ((*mb_pjt->phoSigmaIEtaIEta_2012)[leading] > see_max) { continue; }

        float isolation = (*mb_pjt->pho_ecalClusterIsoR3)[leading]
            + (*mb_pjt->pho_hcalRechitIsoR3)[leading]
            + (*mb_pjt->pho_trackIsoR3PtCut20)[leading];
        if (isolation > iso_max) { continue; } std::cout << __LINE__ << std::endl;

        /* leading photon axis */
        auto photon_eta = (*mb_pjt->phoEta)[leading];
        auto photon_phi = convert_radian((*mb_pjt->phoPhi)[leading]);

        /* electron rejection */
        bool electron = false;
        for (int64_t j = 0; j < mb_pjt->nEle; ++j) {
            if (std::abs((*mb_pjt->eleSCEta)[j]) > 1.4442) { continue; }

            auto deta = photon_eta - (*mb_pjt->eleEta)[j];
            if (deta > 0.1) { continue; } std::cout << __LINE__ << std::endl;

            auto ele_phi = convert_radian((*mb_pjt->elePhi)[j]);
            auto dphi = revert_radian(photon_phi - ele_phi);
            auto dr2 = deta * deta + dphi * dphi;

            if (dr2 < 0.01 && passes_electron_id<
                        det::barrel, wp::loose, pjtree
                    >(mb_pjt, j, true)) {
                electron = true; break; }
        }

        if (electron) { continue; }
 std::cout << __LINE__ << std::endl;
        auto avg_rho = get_avg_rho(mb_pjt, -photon_eta_abs, photon_eta_abs);
        (*mb_rh)[0]->Fill(avg_rho, mb_pjt->hiHF, mb_pjt->w);
    }
    std::cout << __LINE__ << std::endl;
    /* draw rho distributions */
    auto system_tag = "PbPb #sqrt{s_{NN}} = 5.02 TeV"s; std::cout << __LINE__ << std::endl;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;
    // cms += "         p_{T}^{#gamma} > 40 GeV";
 std::cout << __LINE__ << std::endl;
    auto hb = new pencil();
    hb->category("type", "Pythia+Hydjet", "Hydjet"); std::cout << __LINE__ << std::endl;
    
    auto c1 = new paper("pythia_hydjet_rho_v_hf", hb); std::cout << __LINE__ << std::endl;
    apply_style(c1, cms, system_tag);
    // c1->set(paper::flags::logy);
    c1->add((*hp_rh)[0], "Pythia+Hydjet");
    c1->adjust((*hp_rh)[0], "colz", "");

    auto c2 = new paper("hydjet_rho_v_hf", hb); std::cout << __LINE__ << std::endl;
    apply_style(c2, cms, system_tag);
    // c2->set(paper::flags::logy);
    c2->add((*mb_rh)[0], "Hydjet");
    c2->adjust((*mb_rh)[0], "colz", "");
 std::cout << __LINE__ << std::endl;
    hb->sketch(); std::cout << __LINE__ << std::endl;
    c1->draw("pdf"); std::cout << __LINE__ << std::endl;
    c2->draw("pdf"); std::cout << __LINE__ << std::endl;
 std::cout << __LINE__ << std::endl;
    /* save output */
    in(output, [&]() {
        mb_rh->save();
        hp_rh->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return hf_shift(argv[1], argv[2]);

    return 0;
}
