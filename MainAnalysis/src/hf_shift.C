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
#include "TProfile.h"
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

    auto tag = conf->get<std::string>("tag");

    auto const photon_pt_min = conf->get<float>("photon_pt_min");
    auto const photon_eta_abs = conf->get<float>("photon_eta_abs");

    auto const hovere_max = conf->get<float>("hovere_max");
    auto const see_max = conf->get<float>("see_max");
    auto const iso_max = conf->get<float>("iso_max");

    TH1::SetDefaultSumw2();
    
    /* define histograms */
    auto irho = new interval("#rho"s, 100, 0, 400);
    auto ihf = new interval("HF Energy"s, 100, 0, 7000);
    auto in = new interval("Ncoll"s, 100, 0, 2100);

    auto mhn = new multival(*ihf, *in);
    auto mrn = new multival(*irho, *in);

    auto fhn = std::bind(&multival::book<TH2F>, mhn, _1, _2, _3); // hf:ncoll
    auto frn = std::bind(&multival::book<TH2F>, mrn, _1, _2, _3); // rho:ncoll

    auto hp_hn = new history<TH2F>("hp_hn"s, "Pythia+Hydjet", fhn, 1);
    auto mb_hn = new history<TH2F>("mb_hn"s, "Hydjet", fhn, 1);

    auto hp_rn = new history<TH2F>("hp_rn"s, "Pythia+Hydjet", frn, 1);
    auto mb_rn = new history<TH2F>("mb_rn"s, "Hydjet", frn, 1);

    auto fhnp = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";Ncoll;HF Energy;"s + label).data(), 100, 0, 2100, 0, 7000, "LE"); };
    auto frnp = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";Ncoll;#rho;"s + label).data(), 100, 0, 2100, 0, 400, "LE"); };

    auto hp_hn_p = new history<TProfile>("hp_hn_p"s, "Pythia+Hydjet", fhnp, 1);
    auto mb_hn_p = new history<TProfile>("mb_hn_p"s, "Hydjet", fhnp, 1);

    auto hp_rn_p = new history<TProfile>("hp_rn_p"s, "Pythia+Hydjet", frnp, 1);
    auto mb_rn_p = new history<TProfile>("mb_rn_p"s, "Hydjet", frnp, 1);

    /* read input files */
    TFile* hp_f = new TFile(hp_input.data(), "read");
    TTree* hp_t = (TTree*) hp_f->Get("pj");
    auto hp_pjt = new pjtree(true, false, true, hp_t, { 1, 1, 1, 1, 1, 0, 1, 1, 0 });

    TFile* mb_f = new TFile(mb_input.data(), "read");
    TTree* mb_t = (TTree*) mb_f->Get("pj");
    auto mb_pjt = new pjtree(true, false, true, mb_t, { 1, 1, 1, 1, 1, 0, 1, 1, 0 });

    double hp_avg_hf = 0;
    double hp_avg_rho = 0;

    int64_t nentries = static_cast<int64_t>(hp_t->GetEntries());
    nentries = nentries > 100000 ? 100000 : nentries;
    double nphotons = 0;

    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 100000 == 0)
            printf("entry: %li/%li\n", i, nentries);

        hp_t->GetEntry(i);

        int64_t leading = -1;
        float leading_pt = 0;
        for (int64_t j = 0; j < hp_pjt->nPho; ++j) {
            if ((*hp_pjt->phoEt)[j] <= photon_pt_min) { continue; }
            if (std::abs((*hp_pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
            if ((*hp_pjt->phoHoverE)[j] > hovere_max) { continue; }

            if ((*hp_pjt->phoEt)[j] > leading_pt) {
                leading = j;
                leading_pt = (*hp_pjt->phoEt)[j];
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

        nphotons += hp_pjt->w;

        auto avg_rho = get_avg_rho(hp_pjt, -photon_eta_abs, photon_eta_abs);
        float pf_sum = 0;

        for (size_t j = 0; j < hp_pjt->pfPt->size(); ++j) {
            if (std::abs((*hp_pjt->pfEta)[j]) > 3 && std::abs((*hp_pjt->pfEta)[j]) < 5) {
                pf_sum += (*hp_pjt->pfPt)[j];
            }
        }

        hp_avg_hf += pf_sum * hp_pjt->w;
        hp_avg_rho += avg_rho * hp_pjt->w;

        (*hp_hn)[0]->Fill(pf_sum, hp_pjt->Ncoll, hp_pjt->w);
        (*hp_rn)[0]->Fill(avg_rho, hp_pjt->Ncoll, hp_pjt->w);

        (*hp_hn_p)[0]->Fill(hp_pjt->Ncoll, pf_sum, hp_pjt->w);
        (*hp_rn_p)[0]->Fill(hp_pjt->Ncoll, avg_rho, hp_pjt->w);
    }

    nentries = static_cast<int64_t>(mb_t->GetEntries());
    nentries = nentries > 100000 ? 100000 : nentries;
    double nmb = 0;

    double mb_avg_hf = 0;
    double mb_avg_rho = 0;

    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 100000 == 0)
            printf("entry: %li/%li\n", i, nentries);

        mb_t->GetEntry(i);

        nmb += mb_pjt->w;

        auto avg_rho = get_avg_rho(mb_pjt, -photon_eta_abs, photon_eta_abs);
        float pf_sum = 0;

        for (size_t j = 0; j < mb_pjt->pfPt->size(); ++j) {
            if (std::abs((*mb_pjt->pfEta)[j]) > 3 && std::abs((*mb_pjt->pfEta)[j]) < 5) {
                pf_sum += (*mb_pjt->pfPt)[j];
            }
        }

        std::cout << pf_sum << std::endl;

        mb_avg_hf += pf_sum * mb_pjt->w;
        mb_avg_rho += avg_rho * mb_pjt->w;

        (*mb_hn)[0]->Fill(pf_sum, mb_pjt->Ncoll, mb_pjt->w);
        (*mb_rn)[0]->Fill(avg_rho, mb_pjt->Ncoll, mb_pjt->w);

        (*mb_hn_p)[0]->Fill(mb_pjt->Ncoll, pf_sum, mb_pjt->w);
        (*mb_rn_p)[0]->Fill(mb_pjt->Ncoll, avg_rho, mb_pjt->w);
    }

    /* subtract distributions */
    auto hp_hn_h = new TH1F("hp_hn_h", ";;Orange - Purple", 100, 0, 2100);
    auto hp_rn_h = new TH1F("hp_rn_h", ";;Orange - Purple", 100, 0, 2100);

    auto mb_hn_h = new TH1F("mb_hn_h", ";;Orange - Purple", 100, 0, 2100);
    auto mb_rn_h = new TH1F("mb_rn_h", ";;Orange - Purple", 100, 0, 2100);

    for (int i = 1; i <= hp_hn_h->GetNbinsX(); ++i) {
        hp_hn_h->SetBinContent(i, (*hp_hn_p)[0]->GetBinContent(i));
        hp_hn_h->SetBinError(i, (*hp_hn_p)[0]->GetBinError(i));
        hp_rn_h->SetBinContent(i, (*hp_rn_p)[0]->GetBinContent(i));
        hp_rn_h->SetBinError(i, (*hp_rn_p)[0]->GetBinError(i));
        mb_hn_h->SetBinContent(i, (*mb_hn_p)[0]->GetBinContent(i));
        mb_hn_h->SetBinError(i, (*mb_hn_p)[0]->GetBinError(i));
        mb_rn_h->SetBinContent(i, (*mb_rn_p)[0]->GetBinContent(i));
        mb_rn_h->SetBinError(i, (*mb_rn_p)[0]->GetBinError(i));
    }

    if (!(hp_hn_h->GetSumw2N() > 0)) hp_hn_h->Sumw2(true);
    if (!(hp_rn_h->GetSumw2N() > 0)) hp_rn_h->Sumw2(true);

    hp_hn_h->Add(mb_hn_h, -1);
    hp_rn_h->Add(mb_rn_h, -1);

    hp_hn_h->SetMaximum(300);
    hp_hn_h->SetMinimum(-300);
    hp_rn_h->SetMaximum(10);
    hp_rn_h->SetMinimum(-10);

    hp_avg_hf /= nphotons;
    hp_avg_rho /= nphotons;

    mb_avg_hf /= nmb;
    mb_avg_rho /= nmb;

    std::cout << "HF difference: " << hp_avg_hf - mb_avg_hf << std::endl;
    std::cout << "Rho difference: " << hp_avg_rho - mb_avg_rho << std::endl;

    /* draw rho distributions */
    /* draw distributions */
    auto system_tag = "PbPb #sqrt{s_{NN}} = 5.02 TeV"s; 
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Simulation}}"s;
    // cms += "         p_{T}^{#gamma} > 40 GeV";

    auto hb = new pencil();
    hb->category("type", "Pythia+Hydjet", "Hydjet"); 

    auto c3 = new paper(tag + "_pythia_hydjet_hf_v_ncoll", hb); 
    apply_style(c3, cms, system_tag);
    c3->set(paper::flags::logz);
    c3->add((*hp_hn)[0], "Pythia+Hydjet");
    c3->adjust((*hp_hn)[0], "col", "");

    auto c4 = new paper(tag + "_hydjet_hf_v_ncoll", hb);
    apply_style(c4, cms, system_tag);
    c4->set(paper::flags::logz);
    c4->add((*mb_hn)[0], "Hydjet");
    c4->adjust((*mb_hn)[0], "col", "");

    auto c5 = new paper(tag + "_pythia_hydjet_rho_v_ncoll", hb); 
    apply_style(c5, cms, system_tag);
    c5->set(paper::flags::logz);
    c5->add((*hp_rn)[0], "Pythia+Hydjet");
    c5->adjust((*hp_rn)[0], "col", "");

    auto c6 = new paper(tag + "_hydjet_rho_v_ncoll", hb);
    apply_style(c6, cms, system_tag);
    c6->set(paper::flags::logz);
    c6->add((*mb_rn)[0], "Hydjet");
    c6->adjust((*mb_rn)[0], "col", "");

    auto c8 = new paper(tag + "_comp_hf_v_ncoll", hb);
    c8->divide(1, -1);
    apply_style(c8, cms, system_tag);
    c8->add((*mb_hn_p)[0], "Hydjet");
    c8->stack((*hp_hn_p)[0], "Pythia+Hydjet");
    c8->add(hp_hn_h);

    auto c9 = new paper(tag + "_comp_rho_v_ncoll", hb);
    c9->divide(1, -1);
    apply_style(c9, cms, system_tag);
    c9->add((*mb_rn_p)[0], "Hydjet");
    c9->stack((*hp_rn_p)[0], "Pythia+Hydjet");
    c9->add(hp_rn_h);

    auto hp_style = [](TH1* h) {
        h->SetMarkerStyle(34);
        h->SetMarkerColor(95);
        h->SetMarkerSize(0.60);
    };

    auto mb_style = [](TH1* h) {
        h->SetMarkerStyle(34);
        h->SetMarkerColor(51);
        h->SetMarkerSize(0.60);
    };

    hb->style("Hydjet", mb_style);
    hb->style("Pythia+Hydjet", hp_style);

    hb->sketch();
    c3->draw("pdf");
    c4->draw("pdf");
    c5->draw("pdf");
    c6->draw("pdf");
    c8->draw("pdf");
    c9->draw("pdf");

    auto fout = new TFile(output, "recreate");

    hp_hn->save();
    hp_rn->save();
    mb_hn->save();
    mb_rn->save();

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return hf_shift(argv[1], argv[2]);

    return 0;
}
