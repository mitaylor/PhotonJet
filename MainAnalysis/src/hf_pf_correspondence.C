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
#include "TF1.h"
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
    auto ihf = new interval("hiHF"s, 200, 0, 7000);
    auto ipf = new interval("PF HF Sum"s, 200, 0, 7000);

    auto mhp = new multival(*ihf, *ipf);

    auto fhp = std::bind(&multival::book<TH2F>, mhp, _1, _2, _3); // hf:ncoll

    auto hp_hf_pf = new history<TH2F>("hp_hf_pf"s, "Pythia+Hydjet", fhp, 1);
    auto mb_hf_pf = new history<TH2F>("mb_hf_pf"s, "Hydjet", fhp, 1);

    auto fhpp = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";PF HF Sum;hiHF;"s + label).data(), 200, 0, 7000, 0, 7000, "LE"); };

    auto hp_hf_pf_p = new history<TProfile>("hp_hf_pf_p"s, "Pythia+Hydjet", fhpp, 1);
    auto mb_hf_pf_p = new history<TProfile>("mb_hf_pf_p"s, "Hydjet", fhpp, 1);

    /* read input files */
    TFile* hp_f = new TFile(hp_input.data(), "read");
    TTree* hp_t = (TTree*) hp_f->Get("pj");
    auto hp_pjt = new pjtree(true, false, true, hp_t, { 1, 1, 1, 1, 1, 0, 1, 1, 0 });

    TFile* mb_f = new TFile(mb_input.data(), "read");
    TTree* mb_t = (TTree*) mb_f->Get("pj");
    auto mb_pjt = new pjtree(true, false, true, mb_t, { 1, 1, 1, 1, 1, 0, 1, 1, 0 });

    int64_t nentries = static_cast<int64_t>(hp_t->GetEntries());
    // nentries = nentries > 100000 ? 100000 : nentries;
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

        float pf_sum = 0;

        for (size_t j = 0; j < hp_pjt->pfPt->size(); ++j) {
            if ((*hp_pjt->pfId)[j] >= 6) {
                pf_sum += (*hp_pjt->pfPt)[j];
            }
        }

        (*hp_hf_pf)[0]->Fill(hp_pjt->hiHF, pf_sum, hp_pjt->w);
        (*hp_hf_pf_p)[0]->Fill(pf_sum, hp_pjt->hiHF, hp_pjt->w);
    }

    nentries = static_cast<int64_t>(mb_t->GetEntries());
    // nentries = nentries > 100000 ? 100000 : nentries;
    double nmb = 0;

    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 100000 == 0)
            printf("entry: %li/%li\n", i, nentries);

        mb_t->GetEntry(i);

        nmb += mb_pjt->w;

        float pf_sum = 0;

        for (size_t j = 0; j < mb_pjt->pfPt->size(); ++j) {
            if ((*mb_pjt->pfId)[j] >= 6) {
                pf_sum += (*mb_pjt->pfPt)[j];
            }
        }

        (*mb_hf_pf)[0]->Fill(mb_pjt->hiHF, pf_sum, mb_pjt->w);
        (*mb_hf_pf_p)[0]->Fill(pf_sum, mb_pjt->hiHF, mb_pjt->w);
    }


    /* draw distributions */
    auto system_tag = "PbPb #sqrt{s_{NN}} = 5.02 TeV"s; 
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Simulation}}"s;
    // cms += "         p_{T}^{#gamma} > 40 GeV";

    auto hb = new pencil();
    hb->category("type", "Pythia+Hydjet", "Hydjet"); 

    auto c1 = new paper(tag + "_pythia_hydjet_hf_v_pf_profile", hb);
    c1->divide(1, -1);
    apply_style(c1, cms, system_tag);
    c1->add((*hp_hf_pf_p)[0], "Hydjet");

    auto c2 = new paper(tag + "_hydjet_hf_v_pf_profile", hb);
    c2->divide(1, -1);
    apply_style(c2, cms, system_tag);
    c2->add((*mb_hf_pf_p)[0], "Pythia+Hydjet");

    auto c3 = new paper(tag + "_pythia_hydjet_hf_v_pf", hb); 
    apply_style(c3, cms, system_tag);
    c3->set(paper::flags::logz);
    c3->add((*hp_hf_pf)[0], "Pythia+Hydjet");
    c3->adjust((*hp_hf_pf)[0], "col", "");

    auto c4 = new paper(tag + "_hydjet_hf_v_pf", hb);
    apply_style(c4, cms, system_tag);
    c4->set(paper::flags::logz);
    c4->add((*mb_hf_pf)[0], "Hydjet");
    c4->adjust((*mb_hf_pf)[0], "col", "");

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
    c1->draw("pdf");
    c2->draw("pdf");
    c3->draw("pdf");
    c4->draw("pdf");

    auto fout = new TFile(output, "recreate");

    hp_hf_pf->save();
    mb_hf_pf->save();
    hp_hf_pf_p->save();
    mb_hf_pf_p->save();

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return hf_shift(argv[1], argv[2]);

    return 0;
}
