#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"

#include <memory>
#include <string>
#include <vector>
#include <ctime>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int populate(char const* config, char const* selections) {
    auto conf = new configurer(config);

    auto input = conf->get<std::vector<std::string>>("input");
    auto frequency = conf->get<int64_t>("frequency");
    auto tag = conf->get<std::string>("tag");

    /* options */
    auto gen_iso = conf->get<bool>("generator_isolation");
    auto ele_rej = conf->get<bool>("electron_rejection");
    auto exclude = conf->get<bool>("exclude");
    auto apply_er = conf->get<bool>("apply_er");

    auto dhf = conf->get<std::vector<float>>("hf_diff");

    /* selections */
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto heavyion = sel->get<bool>("heavyion");

    auto const photon_pt_min = sel->get<float>("photon_pt_min");
    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");
    auto const hovere_max = sel->get<float>("hovere_max");
    auto see_min = sel->get<float>("see_min");
    auto see_max = sel->get<float>("see_max");
    auto const iso_max = sel->get<float>("iso_max");
    auto const gen_iso_max = sel->get<float>("gen_iso_max");

    auto photon_pt_es = sel->get<std::vector<float>>("photon_pt_es");

    auto alter_base = conf->get<std::string>("alter_base"); // get offset to paths from the original configuration file

    /* fix sigma eta eta range for background distributions */
    if (tag == "bkg") see_min = 0.012;
    if (tag == "bkg") see_max = 0.02;

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    int type1_type2_match = 0;
    int type1_type2_mismatch = 0;
    int type1_type3_match = 0;
    int type1_type3_mismatch = 0;

    /* load input */
    for (auto const& file : input) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(gen_iso, false, heavyion, t, { 1, 1, 1, 1, 1, 0, heavyion, 0, !heavyion });
        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        for (int64_t i = 0; i < nentries; ++i) {
            if (i % frequency == 0) { printf("entry: %li/%li\n", i, nentries); }

            t->GetEntry(i);

            double hf = pjt->hiHF;

            if (hf <= dhf.front()) { continue; }
            if (hf >= dhf.back()) { continue; }
            if (std::abs(pjt->vz) > 15) { continue; }

            int64_t type1_leading = -1;
            float type1_leading_pt = 0;
            bool type1 = false;

            // find the leading photon that then passes the remaining selections
            for (int64_t j = 0; j < pjt->nPho; ++j) {
                if ((*pjt->phoEt)[j] <= 30) { continue; }
                if (std::abs((*pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
                if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }

                auto pho_et = (*pjt->phoEt)[j];
                if (heavyion && apply_er) pho_et = (*pjt->phoEtErNew)[j];
                if (!heavyion && apply_er) pho_et = (*pjt->phoEtEr)[j];

                if (pho_et < photon_pt_min) { continue; }

                if (pho_et > type1_leading_pt) {
                    type1_leading = j;
                    type1_leading_pt = pho_et;
                }
            }

            /* require leading photon */
            if (type1_leading > -1) { 
                /* hem failure region exclusion */
                if (!exclude || !in_pho_failure_region(pjt, type1_leading)) { 
                    if ((*pjt->phoSigmaIEtaIEta_2012)[type1_leading] <= see_max && (*pjt->phoSigmaIEtaIEta_2012)[type1_leading] >= see_min) {
                        /* isolation requirement */
                        float isolation = (*pjt->pho_ecalClusterIsoR3)[type1_leading]
                            + (*pjt->pho_hcalRechitIsoR3)[type1_leading]
                            + (*pjt->pho_trackIsoR3PtCut20)[type1_leading];

                        if (isolation <= iso_max) { 
                            /* leading photon axis */
                            auto photon_eta = (*pjt->phoEta)[type1_leading];
                            auto photon_phi = convert_radian((*pjt->phoPhi)[type1_leading]);

                            /* electron rejection */
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

                            if (!electron) { type1 = true; }
                        }
                    }
                }
            }

            int64_t type2_leading = -1;
            float type2_leading_pt = 0;
            bool type2 = false;

            // find the leading photon that then passes the remaining selections
            for (int64_t j = 0; j < pjt->nPho; ++j) {
                if ((*pjt->phoEt)[j] <= 30) { continue; }
                if (std::abs((*pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
                if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }

                auto pho_et = (*pjt->phoEt)[j];
                if (heavyion && apply_er) pho_et = (*pjt->phoEtErNew)[j];
                if (!heavyion && apply_er) pho_et = (*pjt->phoEtEr)[j];

                if (pho_et < photon_pt_min) { continue; }

                /* isolation requirement */
                float isolation = (*pjt->pho_ecalClusterIsoR3)[j]
                    + (*pjt->pho_hcalRechitIsoR3)[j]
                    + (*pjt->pho_trackIsoR3PtCut20)[j];

                if (isolation > iso_max) { continue; }

                if (pho_et > type2_leading_pt) {
                    type2_leading = j;
                    type2_leading_pt = pho_et;
                }
            }

            /* require leading photon */
            if (type2_leading > -1) { 
                /* hem failure region exclusion */
                if (!exclude || !in_pho_failure_region(pjt, type2_leading)) { 
                    if ((*pjt->phoSigmaIEtaIEta_2012)[type2_leading] <= see_max && (*pjt->phoSigmaIEtaIEta_2012)[type2_leading] >= see_min) {
                        /* leading photon axis */
                        auto photon_eta = (*pjt->phoEta)[type2_leading];
                        auto photon_phi = convert_radian((*pjt->phoPhi)[type2_leading]);

                        /* electron rejection */
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

                        if (!electron) { type2 = true; }
                    }
                }
            }


            int64_t type3_leading = -1;
            float type3_leading_pt = 0;
            bool type3 = false;

            // find the leading photon that then passes the remaining selections
            for (int64_t j = 0; j < pjt->nPho; ++j) {
                if ((*pjt->phoEt)[j] <= 30) { continue; }
                if (std::abs((*pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
                if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }

                auto pho_et = (*pjt->phoEt)[j];
                if (heavyion && apply_er) pho_et = (*pjt->phoEtErNew)[j];
                if (!heavyion && apply_er) pho_et = (*pjt->phoEtEr)[j];

                if (pho_et < photon_pt_min) { continue; }

                /* isolation requirement */
                float isolation = (*pjt->pho_ecalClusterIsoR3)[j]
                    + (*pjt->pho_hcalRechitIsoR3)[j]
                    + (*pjt->pho_trackIsoR3PtCut20)[j];

                if (isolation > iso_max) { continue; }

                /* leading photon axis */
                auto photon_eta = (*pjt->phoEta)[j];
                auto photon_phi = convert_radian((*pjt->phoPhi)[j]);

                /* electron rejection */
                bool electron = false;
                for (int64_t k = 0; k < pjt->nEle; ++k) {
                    if (std::abs((*pjt->eleSCEta)[k]) > 1.4442) { continue; }

                    auto deta = photon_eta - (*pjt->eleEta)[k];
                    if (deta > 0.1) { continue; }

                    auto ele_phi = convert_radian((*pjt->elePhi)[k]);
                    auto dphi = revert_radian(photon_phi - ele_phi);
                    auto dr2 = deta * deta + dphi * dphi;

                    if (dr2 < 0.01 && passes_electron_id<
                                det::barrel, wp::loose, pjtree
                            >(pjt, k, heavyion)) {
                        electron = true; break; }
                }

                if (electron) { continue; }

                if (pho_et > type3_leading_pt) {
                    type3_leading = j;
                    type3_leading_pt = pho_et;
                }
            }

            /* require leading photon */
            if (type3_leading > -1) { 
                /* hem failure region exclusion */
                if (!exclude || !in_pho_failure_region(pjt, type3_leading)) { 
                    if ((*pjt->phoSigmaIEtaIEta_2012)[type3_leading] <= see_max && (*pjt->phoSigmaIEtaIEta_2012)[type3_leading] >= see_min) {
                        type3 = true;
                    }
                }
            }

            /* compare all three types and find when they match and when they mismatch */
            if (!type1 && !type2 && !type3) { continue; }

            if (type1 & type2) {
                if (type1_leading == type2_leading) { type1_type2_match++; }
                else { type1_type2_mismatch++; }
            }
            else if ((type1 && !type2) || (!type1 && type2)) {
                type1_type2_mismatch++;
            }

            if (type1 & type3) {
                if (type1_leading == type3_leading) { type1_type3_match++; }
                else { type1_type3_mismatch++; }
            }
            else if ((type1 && !type3) || (!type1 && type3)) {
                type1_type3_mismatch++;
            }
        }

        f->Close();
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return populate(argv[1], argv[2]);

    printf("usage: %s [config] [selections]\n", argv[0]);
    return 1;
}
