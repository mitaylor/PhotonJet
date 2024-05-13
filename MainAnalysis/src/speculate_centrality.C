#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/train.h"
#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TLine.h"
#include "TTree.h"
#include "TMath.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

static float dr2(float eta1, float eta2, float phi1, float phi2) {
    auto deta = eta1 - eta2;
    float dphi = std::abs(phi1 - phi2);
    if (dphi > TMath::Pi()) dphi = std::abs(dphi - 2*TMath::Pi());

    return deta * deta + dphi * dphi;
}

int speculate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto max_entries = conf->get<int64_t>("max_entries");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto mc_branches = conf->get<bool>("mc_branches");
    auto ele_rej = conf->get<bool>("ele_rej");
    auto apply_er = conf->get<bool>("apply_er");

    auto rpt = conf->get<std::vector<float>>("pt_range");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto sel = new configurer(selections);

    /* selections */
    auto set = sel->get<std::string>("set");

    auto heavyion = sel->get<bool>("heavyion");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");
    auto const hovere_max = sel->get<float>("hovere_max");
    auto const see_min = sel->get<float>("see_min");
    auto const see_max = sel->get<float>("see_max");
    auto const iso_max = sel->get<float>("iso_max");

    /* load forest */
    TFile* f = new TFile(input.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto p = new pjtree(mc_branches, true, false, t, { 1, 0, 1, 1, 0, 1, 0, 0, 0 });

    auto ipt = new interval("photon p_{T}"s, rpt);
    auto ihf = new interval(dhf);
    auto fpt = std::bind(&interval::book<TH1F>, ipt, _1, _2, _3);

    auto numerator = new history<TH1F>("numerator", "counts", fpt, ihf->size());
    auto denominator = new history<TH1F>("denominator", "counts", fpt, ihf->size());

    /* iterate */
    auto nentries = static_cast<int64_t>(t->GetEntries());
    if (max_entries) nentries = std::min(nentries, max_entries);
    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 100000 == 0)
            printf("entry: %li/%li\n", i, nentries);

        t->GetEntry(i);

        double hf = p->hiHF;
        auto hf_x = ihf->index_for(hf);

        if (hf <= dhf.front()) { continue; }
        if (hf >= dhf.back()) { continue; }
        if (std::abs(p->vz) > 15) { continue; }

        int64_t photon_index = -1;
        float photon_pt = 0;

        for (int64_t j = 0; j < p->nPho; ++j) {
            auto temp_photon_pt = (*p->phoEt)[j];

            if (temp_photon_pt <= 30) { continue; }
            if (std::abs((*p->phoEta)[j]) >= photon_eta_abs) { continue; }
            if ((*p->phoHoverE)[j] > hovere_max) { continue; }
            if (apply_er && temp_photon_pt > 30) temp_photon_pt = (heavyion) ? (*p->phoEtErNew)[j] : (*p->phoEtEr)[j];

            if (temp_photon_pt > photon_pt) {
                photon_index = j;
                photon_pt = temp_photon_pt;
            }
        }

        /* require leading photon */
        if (photon_index < 0) { continue; }
        if ((*p->phoSigmaIEtaIEta_2012)[photon_index] > see_max || (*p->phoSigmaIEtaIEta_2012)[photon_index] < see_min) { continue; }

        /* hem failure region exclusion */
        if (heavyion && in_pho_failure_region(p, photon_index)) { continue; }

        /* isolation requirement */
        if ((*p->pho_ecalClusterIsoR3)[photon_index] + (*p->pho_hcalRechitIsoR3)[photon_index] + (*p->pho_trackIsoR3PtCut20)[photon_index] > iso_max) { continue; }

        /* leading photon axis */
        auto photon_eta = (*p->phoEta)[photon_index];
        auto photon_phi = (*p->phoPhi)[photon_index];

        /* electron rejection */
        if (ele_rej) {
            bool electron = false;

            for (int64_t j = 0; j < p->nEle; ++j) {
                if (std::abs((*p->eleEta)[j]) > 1.4442) { continue; }

                auto dr = std::sqrt(dr2(photon_eta, (*p->eleEta)[j], photon_phi, (*p->elePhi)[j]));

                if (dr < 0.1 && passes_electron_id<det::barrel, wp::loose, pjtree>(p, j, heavyion)) {
                    electron = true; break;
                }
            }

            if (electron) { continue; }
        }

        if (mc_branches) {
            (*denominator)[hf_x]->Fill(photon_pt, p->weight);
            if ((*p->accepts)[0] == 1) {
                (*numerator)[hf_x]->Fill(photon_pt, p->weight);
            }
        }
        else {
            (*denominator)[hf_x]->Fill(photon_pt);
            if ((*p->accepts)[0] == 1) {
                (*numerator)[hf_x]->Fill(photon_pt);
            }
        }
    }

    /* calculate efficiency */
    TGraphAsymmErrors* eff[4];

    for (int64_t i = 0; i < ihf->size(); ++i) {
        std::string name = "graph_" + to_text(i);
        eff[i] = new TGraphAsymmErrors((*numerator)[i], (*denominator)[i], "cl=0.683 b(1,1) mode");
        eff[i]->SetName(name.c_str());
    }

    auto hf_info = [&](int64_t index) {
        info_text(index, 0.75, "Cent. %i - %i%%", dcent, true); };

    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;
    
    /* draw efficiency */
    auto hb = new pencil();
    hb->category("sample", "pp", "PbPb");

    auto c1 = new paper(set + "_" + tag + "_efficiency", hb);
    apply_style(c1, cms, system_tag, 0., 1.2);
    c1->divide(ihf->size()/2, -1);
    c1->accessory(hf_info);
    c1->accessory(std::bind(line_at, _1, 1., rpt.front(), rpt.back()));

    for (int64_t i = 0; i < ihf->size(); ++i) {
        std::string name = "frame_" + to_text(i);
        auto hframe = frame((*denominator)[i]->GetXaxis(), (*denominator)[i]->GetYaxis());
        hframe->SetName(name.c_str());

        hframe->GetYaxis()->SetTitle("L1+HLT efficiency");
        hframe->GetXaxis()->SetTitle("photon p_{T}");

        c1->add(hframe);
        c1->stack(eff[i], system);
    }

    hb->sketch();
    c1->draw("pdf");

    /* save output */
    in(output, [&]() { 
        numerator->save(tag);
        denominator->save(tag); });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return speculate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
