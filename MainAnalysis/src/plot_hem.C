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

#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2F.h"
#include "TMath.h"

#include <memory>
#include <string>
#include <vector>
#include <ctime>

using namespace std::literals::string_literals;
using namespace std::placeholders;

static float dr2(float eta1, float eta2, float phi1, float phi2) {
    auto deta = eta1 - eta2;
    float dphi = std::abs(phi1 - phi2);
    if (dphi > TMath::Pi()) dphi = std::abs(dphi - 2*TMath::Pi());

    return deta * deta + dphi * dphi;
}

float back_to_back(float photon_phi, float jet_phi, float threshold) {
    float dphi = std::abs(photon_phi - jet_phi);
    if (dphi > TMath::Pi()) dphi = std::abs(dphi - 2*TMath::Pi());

    return dphi > threshold * TMath::Pi();
}

int plot_hem(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::vector<std::string>>("input");
    auto frequency = conf->get<int64_t>("frequency");

    auto tag = conf->get<std::string>("tag");

    auto ele_rej = conf->get<bool>("electron_rejection");
    auto apply_er = conf->get<bool>("apply_er");
    auto apply_jes_flex = conf->get<bool>("apply_jes_flex");
    auto apply_jes_stat = conf->get<bool>("apply_jes_stat");

    /* selections */
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");

    auto const photon_pt_min = sel->get<float>("photon_pt_min");
    auto const photon_pt_max = sel->get<float>("photon_pt_max");
    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");
    auto const hovere_max = sel->get<float>("hovere_max");
    auto const see_min = sel->get<float>("see_min");
    auto const see_max = sel->get<float>("see_max");
    auto const iso_max = sel->get<float>("iso_max");

    auto const jet_pt_min = sel->get<float>("jet_pt_min");
    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto heavyion = sel->get<bool>("heavyion");

    /* create histograms */
    auto photonEtaPhi = new TH2F("photon_eta_phi_nosel",";#eta;#phi",100,-photon_eta_abs,photon_eta_abs,100,-3.15,3.15);
    auto photonSelectedEtaPhi = new TH2F("photon_eta_phi_sel",";#eta;#phi",100,-photon_eta_abs,photon_eta_abs,100,-3.15,3.15);
    auto jetEtaPhi = new TH2F("jet_eta_phi_nosel",";#eta;#phi",100,-jet_eta_abs,jet_eta_abs,100,-3.15,3.15);
    auto jetSelectedEtaPhi = new TH2F("jet_eta_phi_sel",";#eta;#phi",100,-jet_eta_abs,jet_eta_abs,100,-3.15,3.15);

    auto photonEtaPhiEx = new TH2F("photon_eta_phi_nosel_ex",";#eta;#phi",100,-photon_eta_abs,photon_eta_abs,100,-3.15,3.15);
    auto photonSelectedEtaPhiEx = new TH2F("photon_eta_phi_sel_ex",";#eta;#phi",100,-photon_eta_abs,photon_eta_abs,100,-3.15,3.15);
    auto jetEtaPhiEx = new TH2F("jet_eta_phi_nosel_ex",";#eta;#phi",100,-jet_eta_abs,jet_eta_abs,100,-3.15,3.15);
    auto jetSelectedEtaPhiEx = new TH2F("jet_eta_phi_sel_ex",";#eta;#phi",100,-jet_eta_abs,jet_eta_abs,100,-3.15,3.15);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    for (auto const& file : input) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(false, false, false, t, { 1, 1, 1, 1, 1, 0, 0, 0, 0 });
        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        for (int64_t i = 0; i < nentries; ++i) {
            if (i % frequency == 0) { printf("entry: %li/%li\n", i, nentries); }

            t->GetEntry(i);

            if (std::abs(pjt->vz) > 15) { continue; }

            for (int64_t j = 0; j < pjt->nref; ++j) {
                auto jet_eta = (*pjt->jteta)[j];
                auto jet_phi = (*pjt->jtphi)[j];

                /* hem failure region exclusion */
                if (heavyion && !in_jet_failure_region(pjt,j)) { jetEtaPhiEx->Fill(jet_eta, jet_phi); }
                
                jetEtaPhi->Fill(jet_eta, jet_phi);
            }  

            int64_t photon_index = -1;
            float photon_pt = 0;

            for (int64_t j = 0; j < pjt->nPho; ++j) {
                photonEtaPhi->Fill((*pjt->phoEta)[j], (*pjt->phoPhi)[j]);

                if (!in_pho_failure_region(pjt, j)) {
                    photonEtaPhiEx->Fill((*pjt->phoEta)[j], (*pjt->phoPhi)[j]);
                }

                auto temp_photon_pt = (*pjt->phoEt)[j];

                if (temp_photon_pt <= 30) { continue; }
                if (std::abs((*pjt->phoEta)[j]) >= photon_eta_abs) { continue; }
                if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }
                if (apply_er) temp_photon_pt = (heavyion) ? (*pjt->phoEtErNew)[j] : (*pjt->phoEtEr)[j];
                
                if (temp_photon_pt < photon_pt_min || temp_photon_pt > photon_pt_max) { continue; }
                
                if (temp_photon_pt > photon_pt) {
                    photon_index = j;
                    photon_pt = temp_photon_pt;
                }
            }

            /* require leading photon */
            if (photon_index < 0) { continue; }
            if ((*pjt->phoSigmaIEtaIEta_2012)[photon_index] > see_max || (*pjt->phoSigmaIEtaIEta_2012)[photon_index] < see_min) { continue; }

            /* isolation requirement */
            if ((*pjt->pho_ecalClusterIsoR3)[photon_index] + (*pjt->pho_hcalRechitIsoR3)[photon_index] + (*pjt->pho_trackIsoR3PtCut20)[photon_index] > iso_max) { continue; }
            
            auto photon_eta = (*pjt->phoEta)[photon_index];
            auto photon_phi = (*pjt->phoPhi)[photon_index];

            /* electron rejection */
            if (ele_rej) {
                bool electron = false;

                for (int64_t j = 0; j < pjt->nEle; ++j) {
                    if (std::abs((*pjt->eleEta)[j]) > 1.4442) { continue; }

                    auto dr = std::sqrt(dr2(photon_eta, (*pjt->eleEta)[j], photon_phi, (*pjt->elePhi)[j]));

                    if (dr < 0.1 && passes_electron_id<det::barrel, wp::loose, pjtree>(pjt, j, heavyion)) {
                        electron = true; break;
                    }
                }

                if (electron) { continue; }
            }

            photonSelectedEtaPhi->Fill((*pjt->phoEta)[photon_index], (*pjt->phoPhi)[photon_index]);
            
            if (heavyion && !in_pho_failure_region(pjt, photon_index)) { 
                photonSelectedEtaPhiEx->Fill((*pjt->phoEta)[photon_index], (*pjt->phoPhi)[photon_index]);
            }

            for (int64_t j = 0; j < pjt->nref; ++j) {
                auto jet_pt = (*pjt->jtpt)[j];

                if (heavyion && apply_jes_flex) jet_pt = (*pjt->jtptCor)[j];
                if (heavyion && apply_jes_stat) jet_pt = (*pjt->jtptCorScale)[j];

                if (jet_pt <= jet_pt_min) { continue; }

                auto jet_eta = (*pjt->jteta)[j];
                auto jet_phi = (*pjt->jtphi)[j];

                if (std::abs(jet_eta) >= jet_eta_abs) { continue; }
                if (!back_to_back(photon_phi, jet_phi, dphi_min_numerator/dphi_min_denominator)) { continue; }

                /* hem failure region exclusion */
                if (heavyion && !in_jet_failure_region(pjt,j)) { 
                    jetSelectedEtaPhiEx->Fill(jet_eta, jet_phi); 
                }

                jetSelectedEtaPhi->Fill(jet_eta, jet_phi);
            }  
        }

        f->Close();
    }

    auto system_tag = "PbPb  #sqrt{s_{NN}} = 5.02 TeV, 1.69 nb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}}"s;
    cms += " #it{#scale[1.2]{Preliminary}}"s;

    std::function<void(int64_t, float)> jet_kinematics = [&](int64_t x, float pos) {
        if (x > 0) {
            auto jet_selections = "anti-k_{T} R = 0.3, p_{T}^{jet} > "s + to_text(jet_pt_min) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);
            auto photon_selections = "p_{T}^{#gamma} > "s + to_text(photon_pt_min) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs) + 
                ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, pos, "jets associated with selected photons");
            l->DrawLatexNDC(0.865, pos-0.06, jet_selections.data());
            l->DrawLatexNDC(0.865, pos-0.12, photon_selections.data());
        }
    };

    std::function<void(int64_t, float)> pho_kinematics = [&](int64_t x, float pos) {
        if (x > 0) {
            auto photon_selections = "p_{T}^{#gamma} > "s + to_text(photon_pt_min) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, pos, "selected photons");
            l->DrawLatexNDC(0.865, pos-0.06, photon_selections.data());
        }
    };

    auto hb = new pencil();
    hb->category("type", "data");
    hb->alias("data", "");

    auto c1 = new paper(set + "_" + tag + "_photon_distribution", hb);
    apply_style(c1, cms, system_tag);
    c1->divide(2, -1);
    c1->accessory(std::bind(pho_kinematics, _1, 0.85));

    photonSelectedEtaPhiEx->Scale(1/photonSelectedEtaPhi->Integral());
    photonSelectedEtaPhi->Scale(1/photonSelectedEtaPhi->Integral());
    
    photonSelectedEtaPhi->SetMinimum(0);
    photonSelectedEtaPhi->SetMaximum(0.00025);
    c1->add(photonSelectedEtaPhi);
    c1->adjust(photonSelectedEtaPhi, "colz", "");

    photonSelectedEtaPhiEx->SetMinimum(0);
    photonSelectedEtaPhiEx->SetMaximum(0.00025);
    c1->add(photonSelectedEtaPhiEx);
    c1->adjust(photonSelectedEtaPhiEx, "colz", "");

    auto c2 = new paper(set + "_" + tag + "_jet_distribution", hb);
    apply_style(c2, cms, system_tag);
    c2->divide(2, -1);
    c2->accessory(std::bind(jet_kinematics, _1, 0.85));

    jetSelectedEtaPhiEx->Scale(1/jetSelectedEtaPhi->Integral());
    jetSelectedEtaPhi->Scale(1/jetSelectedEtaPhi->Integral());

    jetSelectedEtaPhi->SetMinimum(0);
    jetSelectedEtaPhi->SetMaximum(0.00025);
    c2->add(jetSelectedEtaPhi);
    c2->adjust(jetSelectedEtaPhi, "colz", "");

    jetSelectedEtaPhiEx->SetMinimum(0);
    jetSelectedEtaPhiEx->SetMaximum(0.00025);
    c2->add(jetSelectedEtaPhiEx);
    c2->adjust(jetSelectedEtaPhiEx, "colz", "");

    hb->sketch();

    c1->draw("pdf");
    c2->draw("pdf");

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
    if (argc == 4)
        return plot_hem(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
