#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2F.h"

#include <memory>
#include <string>
#include <vector>
#include <ctime>

using namespace std::literals::string_literals;
using namespace std::placeholders;

static bool in_hem_failure_region(float eta, float phi) {
    return (eta < -1.242 && -1.72 < phi && phi < -0.72);
}

int populate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto entries = conf->get<int64_t>("entries");
    auto frequency = conf->get<int64_t>("frequency");

    /* options */
    auto heavyion = conf->get<bool>("heavyion");
    auto ele_rej = conf->get<bool>("electron_rejection");
    auto apply_er = conf->get<bool>("apply_er");
    auto apply_jes_flex = conf->get<bool>("apply_jes_flex");
    auto apply_jes_stat = conf->get<bool>("apply_jes_stat");
    auto filter = conf->get<bool>("filter");

    /* selections */
    auto const photon_eta_abs = conf->get<float>("photon_eta_abs");
    auto const hovere_max = conf->get<float>("hovere_max");
    auto const see_min = conf->get<float>("see_min");
    auto const see_max = conf->get<float>("see_max");
    auto const iso_max = conf->get<float>("iso_max");

    auto const jet_pt_min = conf->get<float>("jet_pt_min");
    auto const jet_eta_abs = conf->get<float>("jet_eta_abs");

    /* create histograms */

    auto photonEtaPhi = new TH2F("photon_eta_phi_nosel","Photon Eta Phi Distibution",100,-photon_eta_abs,photon_eta_abs,100,-3.15,3.15);
    auto photonSelectedEtaPhi = new TH2F("photon_eta_phi_sel","Selected Photon Eta Phi Distibution",100,-photon_eta_abs,photon_eta_abs,100,-3.15,3.15);
    auto jetEtaPhi = new TH2F("jet_eta_phi_nosel","Jet Eta Phi Distibution",100,-jet_eta_abs,jet_eta_abs,100,-3.15,3.15);
    auto jetSelectedEtaPhi = new TH2F("jet_eta_phi_sel","Selected Jet Eta Phi Distibution",100,-photon_eta_abs,photon_eta_abs,100,-3.15,3.15);

    auto photonEtaPhiEx = new TH2F("photon_eta_phi_nosel_ex","Photon Eta Phi Distibution",100,-photon_eta_abs,photon_eta_abs,100,-3.15,3.15);
    auto photonSelectedEtaPhiEx = new TH2F("photon_eta_phi_sel_ex","Selected Photon Eta Phi Distibution",100,-photon_eta_abs,photon_eta_abs,100,-3.15,3.15);
    auto jetEtaPhiEx = new TH2F("jet_eta_phi_nosel_ex","Jet Eta Phi Distibution",100,-jet_eta_abs,jet_eta_abs,100,-3.15,3.15);
    auto jetSelectedEtaPhiEx = new TH2F("jet_eta_phi_sel_ex","Selected Jet Eta Phi Distibution",100,-photon_eta_abs,photon_eta_abs,100,-3.15,3.15);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    TFile* f = new TFile(input.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto pjt = new pjtree(false, false, false, t, { 1, 1, 1, 1, 1, 0, 0 });

    printf("iterate..\n");

    int64_t nentries = static_cast<int64_t>(t->GetEntries());
    int64_t mod = 1;
    if (entries) { mod = nentries / entries; }
    if (mod !=1) { std::cout << "mod: " << mod << std::endl; }

    for (int64_t i = 0; i < nentries; ++i) {
        if (i % frequency == 0) { printf("entry: %li/%li\n", i, nentries); }
        if (i % mod != 0) { continue; }

        t->GetEntry(i);

        for (int64_t j = 0; j < pjt->nref; ++j) {
            auto jet_eta = (*pjt->jteta)[j];
            auto jet_phi = (*pjt->jtphi)[j];

            /* hem failure region exclusion */
            if (heavyion && !in_hem_failure_region(jet_eta, jet_phi)) { jetEtaPhiEx->Fill(jet_eta, jet_phi); }
            
            jetEtaPhi->Fill(jet_eta, jet_phi);
        }  


        int64_t leading = -1;
        float leading_pt = 0;
        for (int64_t j = 0; j < pjt->nPho; ++j) {
            photonEtaPhi->Fill((*pjt->phoEta)[j], (*pjt->phoPhi)[j]);
            if (!within_hem_failure_region(pjt, j)) {
                photonEtaPhiEx->Fill((*pjt->phoEta)[j], (*pjt->phoPhi)[j]);
            }

            float pho_et = (*pjt->phoEt)[j];
            if (heavyion && apply_er) pho_et = (*pjt->phoEtErNew)[j];
            if (!heavyion && apply_er) pho_et = (*pjt->phoEtEr)[j];
            if (filter && pho_et/(*pjt->phoEt)[j] > 1.2) { continue; }
            
            if (pho_et <= 40) { continue; }
            if (std::abs((*pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
            if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }
            if (pho_et > leading_pt) {
                leading = j;
                leading_pt = pho_et;
            }
        }

        /* require leading photon */
        if (leading < 0) { continue; }

        if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > see_max || (*pjt->phoSigmaIEtaIEta_2012)[leading] < see_min)
            { continue; }

        /* isolation requirement */
        float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
            + (*pjt->pho_hcalRechitIsoR3)[leading]
            + (*pjt->pho_trackIsoR3PtCut20)[leading];
        if (isolation > iso_max) { continue; }
        
        auto photon_eta = (*pjt->phoEta)[leading];
        auto photon_phi = convert_radian((*pjt->phoPhi)[leading]);

        /* electron rejection */
        if (ele_rej) {
            bool electron = false;
            for (int64_t j = 0; j < pjt->nEle; ++j) {
                if (std::abs((*pjt->eleSCEta)[j]) > 1.4442) { continue; }

                auto deta = photon_eta - (*pjt->eleEta)[j];
                if (deta > 0.1) { continue; }

                auto ele_phi = convert_radian((*pjt->elePhi)[j]);
                auto dphi = revert_radian(photon_phi - ele_phi);
                auto dr2 = deta * deta + dphi * dphi;

                if (dr2 < 0.01 && passes_electron_id<
                            det::barrel, wp::loose, pjtree
                        >(pjt, j, heavyion)) {
                    electron = true; break; }
            }

            if (electron) { continue; }
        }

        photonSelectedEtaPhi->Fill((*pjt->phoEta)[leading], (*pjt->phoPhi)[leading]);
        if (heavyion && !within_hem_failure_region(pjt, leading)) { 
            photonSelectedEtaPhiEx->Fill((*pjt->phoEta)[leading], (*pjt->phoPhi)[leading]);
        }

        for (int64_t j = 0; j < pjt->nref; ++j) {
            auto jet_pt = (*pjt->jtpt)[j];
            if (heavyion && apply_jes_flex) jet_pt = (*pjt->jtptCor)[j];
            f (heavyion && apply_jes_stat) jet_pt = (*pjt->jtptCorScale)[j];

            if (jet_pt <= jet_pt_min) { continue; }

            auto jet_eta = (*pjt->jteta)[j];

            if (std::abs(jet_eta) >= jet_eta_abs) { continue; }

            auto jet_phi = (*pjt->jtphi)[j];

            /* hem failure region exclusion */
            if (heavyion && !in_hem_failure_region(jet_eta, jet_phi)) { jetSelectedEtaPhiEx->Fill(jet_eta, jet_phi); }

            jetSelectedEtaPhi->Fill(jet_eta, jet_phi);
        }  
    }

    auto hb = new pencil();
    hb->category("type", "data");
    hb->alias("data", "");

    auto c1 = new paper(tag + "photon_distribution", hb);
    apply_style(c1, tag);
    c1->divide(4, -1);

    photonEtaPhi->SetMinimum(0);
    photonEtaPhi->SetMaximum(800);
    c1->add(photonEtaPhi);
    c1->adjust(photonEtaPhi, "colz", "");

    photonEtaPhiEx->SetMinimum(0);
    photonEtaPhiEx->SetMaximum(800);
    c1->add(photonEtaPhiEx);
    c1->adjust(photonEtaPhiEx, "colz", "");

    photonSelectedEtaPhi->SetMinimum(0);
    photonSelectedEtaPhi->SetMaximum(80);
    c1->add(photonSelectedEtaPhi);
    c1->adjust(photonSelectedEtaPhi, "colz", "");

    photonSelectedEtaPhiEx->SetMinimum(0);
    photonSelectedEtaPhiEx->SetMaximum(80);
    c1->add(photonSelectedEtaPhiEx);
    c1->adjust(photonSelectedEtaPhiEx, "colz", "");


    auto c2 = new paper(tag + "jet_distribution", hb);
    apply_style(c2, tag);
    c2->divide(4, -1);

    jetEtaPhi->SetMinimum(0);
    jetEtaPhi->SetMaximum(40000);
    c2->add(jetEtaPhi);
    c2->adjust(jetEtaPhi, "colz", "");

    jetEtaPhiEx->SetMinimum(0);
    jetEtaPhiEx->SetMaximum(40000);
    c2->add(jetEtaPhiEx);
    c2->adjust(jetEtaPhiEx, "colz", "");

    jetSelectedEtaPhi->SetMinimum(0);
    jetSelectedEtaPhi->SetMaximum(600);
    c2->add(jetSelectedEtaPhi);
    c2->adjust(jetSelectedEtaPhi, "colz", "");

    jetSelectedEtaPhiEx->SetMinimum(0);
    jetSelectedEtaPhiEx->SetMaximum(600);
    c2->add(jetSelectedEtaPhiEx);
    c2->adjust(jetSelectedEtaPhiEx, "colz", "");

    hb->sketch();

    /* save histograms */
    TFile* o = new TFile(output, "recreate");

    photonEtaPhi->Write();
    photonSelectedEtaPhi->Write();
    jetEtaPhi->Write();
    jetSelectedEtaPhi->Write();

    photonEtaPhiEx->Write();
    photonSelectedEtaPhiEx->Write();
    jetEtaPhiEx->Write();
    jetSelectedEtaPhiEx->Write();

    o->Close();

    printf("destroying objects..\n");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return populate(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
