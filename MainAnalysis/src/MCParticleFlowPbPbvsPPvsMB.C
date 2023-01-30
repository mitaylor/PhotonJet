#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/overflow_angles.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "TFile.h"
#include "TDirectory.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TChain.h"
#include "TStyle.h"

#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include "TMath.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TProfile2D.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;



int hf_shift(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto aa_input = conf->get<std::vector<std::string>>("aa_input");
    // auto mb_input = conf->get<std::vector<std::string>>("mb_input");
    auto pp_input = conf->get<std::vector<std::string>>("pp_input");

    auto tag = conf->get<std::string>("tag");

    // auto const photon_pt_min = conf->get<float>("photon_pt_min");
    // auto const photon_eta_abs = conf->get<float>("photon_eta_abs");

    // auto const aa_hovere_max = conf->get<float>("aa_hovere_max");
    // auto const aa_see_max = conf->get<float>("aa_see_max");
    // auto const aa_iso_max = conf->get<float>("aa_iso_max");

    // auto const pp_hovere_max = conf->get<float>("pp_hovere_max");
    // auto const pp_see_max = conf->get<float>("pp_see_max");
    // auto const pp_iso_max = conf->get<float>("pp_iso_max");

    /* define histograms */
    auto ieta = new interval("PF Eta"s, 10, -5, 5);
    auto iphi = new interval("PF Phi"s, 10, -3.15, 3.15);
    auto ienergy = new interval("PF Energy"s, 50, 0, 1400);
    auto ipt = new interval("PF Pt"s, 50, 0, 100);
    auto isum = new interval("PF Energy Sum"s, 50, 0, 2500);

    auto feta = std::bind(&interval::book<TH1F>, ieta, _1, _2, _3);
    auto fphi = std::bind(&interval::book<TH1F>, iphi, _1, _2, _3);
    auto fenergy = std::bind(&interval::book<TH1F>, ienergy, _1, _2, _3);
    auto fpt = std::bind(&interval::book<TH1F>, ipt, _1, _2, _3);
    auto fsum = std::bind(&interval::book<TH1F>, isum, _1, _2, _3);

    auto aa_eta = new history<TH1F>("aa_eta"s, "", feta, 1);
    auto aa_phi = new history<TH1F>("aa_phi"s, "", fphi, 1);
    auto aa_energy = new history<TH1F>("aa_energy"s, "", fenergy, 1);
    auto aa_pt = new history<TH1F>("aa_pt"s, "", fpt, 1);
    auto aa_sum = new history<TH1F>("aa_energy_sum"s, "", fsum, 1);

    // auto mb_eta = new history<TH1F>("mb_eta"s, "", feta, 1);
    // auto mb_phi = new history<TH1F>("mb_phi"s, "", fphi, 1);
    // auto mb_energy = new history<TH1F>("mb_energy"s, "", fenergy, 1);
    // auto mb_pt = new history<TH1F>("mb_pt"s, "", fpt, 1);

    auto pp_eta = new history<TH1F>("pp_eta"s, "", feta, 1);
    auto pp_phi = new history<TH1F>("pp_phi"s, "", fphi, 1);
    auto pp_energy = new history<TH1F>("pp_energy"s, "", fenergy, 1);
    auto pp_pt = new history<TH1F>("pp_pt"s, "", fpt, 1);
    auto pp_sum = new history<TH1F>("pp_energy_sum"s, "", fsum, 1);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    double naa = 0;
    double npp = 0;
    // double nmb = 0;

    /* read in information from TTrees */
    for (auto const& file : aa_input) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*) f->Get("pj");
        auto pjt = new pjtree(true, false, true, t, { 1, 1, 1, 1, 1, 0, 1, 1, 0 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());
        nentries = (nentries > 30000) ? 30000 : nentries;

        for (int64_t i = 0; i < nentries; ++i) {
            if (i % (nentries/20) == 0) std::cout << i << " / " << nentries << std::endl;

            t->GetEntry(i);

            if (std::abs(pjt->vz) > 15) { continue; }
            if (pjt->Ncoll > 1) { continue; }

            // int64_t leading = -1;
            // float leading_pt = 0;
            // for (int64_t j = 0; j < pjt->nPho; ++j) {
            //     if ((*pjt->phoEt)[j] <= 30) { continue; }
            //     if (std::abs((*pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
            //     if ((*pjt->phoHoverE)[j] > aa_hovere_max) { continue; }

            //     auto pho_et = (*pjt->phoEtErNew)[j];

            //     if (pho_et < photon_pt_min) { continue; }
            //     if (pho_et > leading_pt) {
            //         leading = j;
            //         leading_pt = pho_et;
            //     }
            // }

            // if (leading < 0) { continue; }
            // if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > aa_see_max) { continue; }

            // float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
            //     + (*pjt->pho_hcalRechitIsoR3)[leading]
            //     + (*pjt->pho_trackIsoR3PtCut20)[leading];
            // if (isolation > aa_iso_max) { continue; }

            // /* leading photon axis */
            // auto photon_eta = (*pjt->phoEta)[leading];
            // auto photon_phi = convert_radian((*pjt->phoPhi)[leading]);

            // /* electron rejection */
            // bool electron = false;
            // for (int64_t j = 0; j < pjt->nEle; ++j) {
            //     if (std::abs((*pjt->eleSCEta)[j]) > 1.4442) { continue; }

            //     auto deta = photon_eta - (*pjt->eleEta)[j];
            //     if (deta > 0.1) { continue; }

            //     auto ele_phi = convert_radian((*pjt->elePhi)[j]);
            //     auto dphi = revert_radian(photon_phi - ele_phi);
            //     auto dr2 = deta * deta + dphi * dphi;

            //     if (dr2 < 0.01 && passes_electron_id<
            //                 det::barrel, wp::loose, pjtree
            //             >(pjt, j, true)) {
            //         electron = true; break; }
            // }

            // if (electron) { continue; }

            // if (leading_pt > 200) { continue; } // new

            float pf_sum = 0;

            for (size_t j = 0; j < pjt->pfEnergy->size(); ++j) {
                // (*aa_eta)[0]->Fill((*pjt->pfEta)[j], pjt->w);
                // (*aa_phi)[0]->Fill((*pjt->pfPhi)[j], pjt->w);
                // (*aa_energy)[0]->Fill((*pjt->pfEnergy)[j], pjt->w);
                // (*aa_pt)[0]->Fill((*pjt->pfPt)[j], pjt->w);

                if ((*pjt->pfId)[j] >= 6) {
                    (*aa_eta)[0]->Fill((*pjt->pfEta)[j]);
                    (*aa_phi)[0]->Fill((*pjt->pfPhi)[j]);
                    (*aa_energy)[0]->Fill((*pjt->pfEnergy)[j]);
                    (*aa_pt)[0]->Fill((*pjt->pfPt)[j]);
                    pf_sum += (*pjt->pfEnergy)[j];
                }
            }

            (*aa_sum)[0]->Fill(pf_sum);

            // naa += pjt->w;
            naa++;
        }

        f->Close();
    }

    for (auto const& file : pp_input) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*) f->Get("pj");
        auto pjt = new pjtree(true, false, false, t, { 1, 1, 1, 1, 1, 0, 0, 1, 1 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());
        nentries = (nentries > 30000) ? 30000 : nentries;

        for (int64_t i = 0; i < nentries; ++i) {
            if (i % (nentries/20) == 0) std::cout << i << " / " << nentries << std::endl;

            t->GetEntry(i);

            if (std::abs(pjt->vz) > 15) { continue; }
            if ((*pjt->npus)[5] != 0) { continue; }

            // int64_t leading = -1;
            // float leading_pt = 0;
            // for (int64_t j = 0; j < pjt->nPho; ++j) {
            //     if ((*pjt->phoEt)[j] <= 30) { continue; }
            //     if (std::abs((*pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
            //     if ((*pjt->phoHoverE)[j] > pp_hovere_max) { continue; }

            //     auto pho_et = (*pjt->phoEt)[j];

            //     if (pho_et < photon_pt_min) { continue; }
            //     if (pho_et > leading_pt) {
            //         leading = j;
            //         leading_pt = pho_et;
            //     }
            // }

            // if (leading < 0) { continue; }
            // if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > pp_see_max) { continue; }

            // float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
            //     + (*pjt->pho_hcalRechitIsoR3)[leading]
            //     + (*pjt->pho_trackIsoR3PtCut20)[leading];
            // if (isolation > pp_iso_max) { continue; }

            // /* leading photon axis */
            // auto photon_eta = (*pjt->phoEta)[leading];
            // auto photon_phi = convert_radian((*pjt->phoPhi)[leading]);

            // /* electron rejection */
            // bool electron = false;
            // for (int64_t j = 0; j < pjt->nEle; ++j) {
            //     if (std::abs((*pjt->eleSCEta)[j]) > 1.4442) { continue; }

            //     auto deta = photon_eta - (*pjt->eleEta)[j];
            //     if (deta > 0.1) { continue; }

            //     auto ele_phi = convert_radian((*pjt->elePhi)[j]);
            //     auto dphi = revert_radian(photon_phi - ele_phi);
            //     auto dr2 = deta * deta + dphi * dphi;

            //     if (dr2 < 0.01 && passes_electron_id<
            //                 det::barrel, wp::loose, pjtree
            //             >(pjt, j, true)) {
            //         electron = true; break; }
            // }

            // if (electron) { continue; }

            // if (leading_pt > 200) { continue; }

            float pf_sum = 0;

            for (size_t j = 0; j < pjt->pfEnergy->size(); ++j) {
                // (*pp_eta)[0]->Fill((*pjt->pfEta)[j], pjt->w);
                // (*pp_phi)[0]->Fill((*pjt->pfPhi)[j], pjt->w);
                // (*pp_energy)[0]->Fill((*pjt->pfEnergy)[j], pjt->w);
                // (*pp_pt)[0]->Fill((*pjt->pfPt)[j], pjt->w);

                if ((*pjt->pfId)[j] >= 6) {
                    (*pp_eta)[0]->Fill((*pjt->pfEta)[j]);
                    (*pp_phi)[0]->Fill((*pjt->pfPhi)[j]);
                    (*pp_energy)[0]->Fill((*pjt->pfEnergy)[j]);
                    (*pp_pt)[0]->Fill((*pjt->pfPt)[j]);
                    pf_sum += (*pjt->pfEnergy)[j];
                }
            }

            (*pp_sum)[0]->Fill(pf_sum);

            // npp += pjt->w;
            npp++;
        }

        f->Close();
    }

    /* normalize distributions */
    (*aa_eta)[0]->Scale(1/naa);
    (*aa_phi)[0]->Scale(1/naa);
    (*aa_energy)[0]->Scale(1/naa);
    (*aa_pt)[0]->Scale(1/naa);
    (*aa_sum)[0]->Scale(1/naa);

    (*pp_eta)[0]->Scale(1/npp);
    (*pp_phi)[0]->Scale(1/npp);
    (*pp_energy)[0]->Scale(1/npp);
    (*pp_pt)[0]->Scale(1/npp);
    (*pp_sum)[0]->Scale(1/npp);

    /* draw distributions */
    auto system_tag = "PbPb #sqrt{s_{NN}} = 5.02 TeV"s; 
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Simulation}}"s;
    // cms += "         p_{T}^{#gamma} > 40 GeV";

    auto hb = new pencil();
    hb->category("type", "PbPb", "PP"); 

    auto c1 = new paper(tag + "_eta", hb); 
    apply_style(c1, cms, system_tag);
    c1->add((*aa_eta)[0], "PbPb");
    c1->stack((*pp_eta)[0], "PP");

    auto c2 = new paper(tag + "_phi", hb); 
    apply_style(c2, cms, system_tag);
    c2->add((*aa_phi)[0], "PbPb");
    c2->stack((*pp_phi)[0], "PP");

    auto c3 = new paper(tag + "_energy", hb); 
    apply_style(c3, cms, system_tag);
    c3->set(paper::flags::logy);
    c3->add((*aa_energy)[0], "PbPb");
    c3->stack((*pp_energy)[0], "PP");

    auto c4 = new paper(tag + "_pt", hb); 
    apply_style(c4, cms, system_tag);
    c4->set(paper::flags::logy);
    c4->add((*aa_pt)[0], "PbPb");
    c4->stack((*pp_pt)[0], "PP");

    auto c5 = new paper(tag + "_energy_sum", hb); 
    apply_style(c5, cms, system_tag);
    c5->set(paper::flags::logy);
    c5->add((*aa_sum)[0], "PbPb");
    c5->stack((*pp_sum)[0], "PP");

    auto aa_style = [](TH1* h) {
        h->SetMarkerStyle(34);
        h->SetMarkerColor(95);
        h->SetMarkerSize(0.60);
    };

    auto pp_style = [](TH1* h) {
        h->SetMarkerStyle(34);
        h->SetMarkerColor(51);
        h->SetMarkerSize(0.60);
    };

    hb->style("PbPb", pp_style);
    hb->style("PP", aa_style);

    hb->sketch();
    c1->draw("pdf");
    c2->draw("pdf");
    c3->draw("pdf");
    c4->draw("pdf");
    c5->draw("pdf");

    auto fout = new TFile(output, "recreate");

    aa_eta->save();
    aa_phi->save();
    aa_energy->save();
    aa_pt->save();
    aa_sum->save();
    pp_eta->save();
    pp_phi->save();
    pp_energy->save();
    pp_pt->save();
    pp_sum->save();

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return hf_shift(argv[1], argv[2]);

    return 0;
}
