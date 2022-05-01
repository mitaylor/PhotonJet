#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/train.h"
#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TLine.h"
#include "TTree.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int speculate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto max_entries = conf->get<int64_t>("max_entries");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto heavyion = conf->get<bool>("heavyion");
    auto mc_branches = conf->get<bool>("mc_branches");
    auto ele_rej = conf->get<bool>("ele_rej");
    auto apply_er = conf->get<bool>("apply_er");
    auto filter = conf->get<bool>("filter");

    auto const eta_abs = conf->get<float>("eta_abs");
    auto const hovere_max = conf->get<float>("hovere_max");
    auto const see_min = conf->get<float>("see_min");
    auto const see_max = conf->get<float>("see_max");
    auto const iso_max = conf->get<float>("iso_max");

    auto rpt = conf->get<std::vector<float>>("pt_range");

    /* load forest */
    TFile* f = new TFile(input.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto p = new pjtree(mc_branches, true, false, t, { 1, 0, 1, 0, 0, 1, 0 });

    auto ipt = new interval("photon p_{T}"s, rpt);
    auto fpt = std::bind(&interval::book<TH1F>, ipt, _1, _2, _3);

    auto counts = new history<TH1F>("count", "counts", fpt, 2);

    /* iterate */
    auto nentries = static_cast<int64_t>(t->GetEntries());
    if (max_entries) nentries = std::min(nentries, max_entries);
    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 100000 == 0)
            printf("entry: %li/%li\n", i, nentries);

        t->GetEntry(i);

        if (std::abs(p->vz) > 15) { continue; }

        int64_t leading = -1;
        float leading_pt = 0;
        for (int64_t j = 0; j < p->nPho; ++j) {
            if (std::abs((*p->phoSCEta)[j]) >= eta_abs) { continue; }
            if ((*p->phoHoverE)[j] > hovere_max) { continue; }

            float pho_et = (*p->phoEt)[j];
            if (pho_et > 30 && heavyion && apply_er) pho_et = (*p->phoEtErNew)[j];
            if (pho_et > 30 && !heavyion && apply_er) pho_et = (*p->phoEtEr)[j];
            if (filter && pho_et/(*p->phoEt)[j] > 1.2) { continue; }

            if (pho_et > leading_pt) {
                leading = j;
                leading_pt = pho_et;
            }
        }

        /* require leading photon */
        if (leading < 0) { continue; }

        if ((*p->phoSigmaIEtaIEta_2012)[leading] > see_max
                || (*p->phoSigmaIEtaIEta_2012)[leading] < see_min)
            continue;

        /* hem failure region exclusion */
        if (heavyion && in_pho_failure_region(p, leading)) { continue; }

        /* isolation requirement */
        float isolation = (*p->pho_ecalClusterIsoR3)[leading]
            + (*p->pho_hcalRechitIsoR3)[leading]
            + (*p->pho_trackIsoR3PtCut20)[leading];
        if (isolation > iso_max) { continue; }

        /* leading photon axis */
        auto photon_eta = (*p->phoEta)[leading];
        auto photon_phi = convert_radian((*p->phoPhi)[leading]);

        /* electron rejection */
        if (ele_rej) {
            bool electron = false;
            for (int64_t j = 0; j < p->nEle; ++j) {
                if (std::abs((*p->eleSCEta)[j]) > 1.4442) { continue; }

                auto deta = photon_eta - (*p->eleEta)[j];
                if (deta > 0.1) { continue; }

                auto ele_phi = convert_radian((*p->elePhi)[j]);
                auto dphi = revert_radian(photon_phi - ele_phi);
                auto dr2 = deta * deta + dphi * dphi;

                if (dr2 < 0.01 && passes_electron_id<
                            det::barrel, wp::loose, pjtree
                        >(p, j, heavyion)) {
                    electron = true; break; }
            }

            if (electron) { continue; }
        }

        if (mc_branches) {
            (*counts)[0]->Fill(leading_pt, p->weight);
            if ((*p->accepts)[0] == 1) {
                (*counts)[1]->Fill(leading_pt, p->weight);
            }
        }
        else {
            (*counts)[0]->Fill(leading_pt);
            if ((*p->accepts)[0] == 1) {
                (*counts)[1]->Fill(leading_pt);
            }
        }
    }

    /* calculate efficiency */
    auto hframe = frame((*counts)[0]->GetXaxis(), (*counts)[0]->GetYaxis());
    hframe->GetYaxis()->SetTitle("trigger efficiency");
    hframe->GetXaxis()->SetTitle("photon p_{T}");

    auto eff = new TGraphAsymmErrors((*counts)[1], (*counts)[0], "cl=0.683 b(1,1) mode");
    
    /* draw efficiency */
    auto hb = new pencil();
    hb->category("sample", "pp", "PbPb");

    auto c1 = new paper(tag + "_efficiency", hb);
    apply_style(c1, system + " #sqrt{s} = 5.02 TeV"s, 0., 1.2);
    c1->accessory(std::bind(line_at, _1, 1., rpt.front(), rpt.back()));

    c1->add(hframe);
    c1->stack(eff, system);

    hb->sketch();
    c1->draw("pdf");

    /* save output */
    in(output, [&]() { counts->save(tag); });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return speculate(argv[1], argv[2]);

    return 0;
}
