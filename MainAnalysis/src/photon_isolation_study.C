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

template <typename... T>
void scale(double factor, T*... args) {
    (void)(int [sizeof...(T)]) { (args->scale(factor), 0)... };
}

int populate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::vector<std::string>>("input");

    auto eff_file = conf->get<std::string>("eff_file");
    auto eff_label = conf->get<std::string>("eff_label");

    auto rho_file = conf->get<std::string>("rho_file");
    auto rho_label = conf->get<std::string>("rho_label");

    auto frequency = conf->get<int64_t>("frequency");
    auto tag = conf->get<std::string>("tag");

    /* options */
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

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    /* fix sigma eta eta range for background distributions */
    if (tag == "bkg") see_min = 0.012;
    if (tag == "bkg") see_max = 0.02;

    /* make histograms */
    auto ipt = new interval(dpt);
    auto ihf = new interval(dhf);

    auto mpthf = new multival(dpt, dhf);

    auto incl = new interval(""s, 1, 0.f, 9999.f);

    float range_min;
    float range_max;
    int nbins;

    if (heayvion) {
        range_min = -12*(iso_max+0.00001);
        range_max = 29*(iso_max+0.00001);
        nbins = 205;
    } else {
        range_min = -63*(iso_max+0.00001);
        range_max = 151*(iso_max+0.00001);
        nbins = 214;
    }

    auto isumiso = new interval("SumIso"s, nbins, range_min, range_max);

    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto fsumiso = std::bind(&interval::book<TH1F>, isumiso, _1, _2, _3);

    auto pjet_f_dsumiso = new memory<TH1F>("pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fsumiso, mpthf);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    printf("iterate..\n");

    /* load efficiency correction */
    TFile* fe;
    history<TH1F>* efficiency = nullptr;

    if (!eff_file.empty()) {
        fe = new TFile((base + eff_file).data(), "read");
        efficiency = new history<TH1F>(fe, eff_label);
    }

    /* load centrality weighting for MC */
    TFile* fr;
    history<TH1F>* rho_weighting = nullptr;

    if (!rho_file.empty()) {
        fr = new TFile((base + rho_file).data(), "read");
        rho_weighting = new history<TH1F>(fr, rho_label);
    }

    /* add weight for the number of photons, based on the fraction that are excluded by area */
    auto pho_cor = (exclude) ? 1 / (1 - pho_failure_region_fraction(photon_eta_abs)) : 1;
    
    /* load input */
    for (auto const& file : input) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(false, false, heavyion, t, { 1, 1, 1, 1, 1, 0, heavyion, 0, !heavyion });
        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        for (int64_t i = 0; i < nentries; ++i) {
            if (i % frequency == 0) { printf("entry: %li/%li\n", i, nentries); }

            t->GetEntry(i);

            double hf = pjt->hiHF;
            auto hf_x = ihf->index_for(hf);

            if (rho_file.empty() && hf <= dhf.front()) { continue; }
            if (rho_file.empty() && hf >= dhf.back()) { continue; }
            if (std::abs(pjt->vz) > 15) { continue; }

            int64_t leading = -1;
            float leading_pt = 0;
            for (int64_t j = 0; j < pjt->nPho; ++j) {
                if ((*pjt->phoEt)[j] <= 30) { continue; }
                if (std::abs((*pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
                if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }

                auto pho_et = (*pjt->phoEt)[j];
                if (heavyion && apply_er) pho_et = (*pjt->phoEtErNew)[j];
                if (!heavyion && apply_er) pho_et = (*pjt->phoEtEr)[j];

                if (pho_et < photon_pt_min) { continue; }

                if (pho_et > leading_pt) {
                    leading = j;
                    leading_pt = pho_et;
                }
            }

            /* require leading photon */
            if (leading < 0) { continue; }

            if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > see_max
                    || (*pjt->phoSigmaIEtaIEta_2012)[leading] < see_min)
                continue;

            /* hem failure region exclusion */
            if (exclude && in_pho_failure_region(pjt, leading)) { continue; }

            /* isolation requirement */
            float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
                + (*pjt->pho_hcalRechitIsoR3)[leading]
                + (*pjt->pho_trackIsoR3PtCut20)[leading];
            // if (isolation > iso_max) { continue; }

            /* leading photon axis */
            auto photon_eta = (*pjt->phoEta)[leading];
            auto photon_phi = convert_radian((*pjt->phoPhi)[leading]);

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

            if (electron) { continue; }

            auto pt_x = ipt->index_for(leading_pt);

            std::vector<int64_t> pthf_x;
            if (!rho_file.empty()) {
                for (int64_t k = 0; k < ihf->size(); ++k) {
                    pthf_x.push_back(mpthf->index_for(x{pt_x, k}));
                }
            } else {
                pthf_x.push_back(mpthf->index_for(x{pt_x, hf_x}));
            }

            auto weight = pjt->w;

            if (!eff_file.empty() && leading_pt < 70) {
                auto bin = (*efficiency)[1]->FindBin(leading_pt);
                auto cor = (*efficiency)[0]->GetBinContent(bin) / (*efficiency)[1]->GetBinContent(bin);
                if (cor < 1) { std::cout << "error" << std::endl; return -1; }
                weight *= cor;
            }

            std::vector<float> weights;
            if (!rho_file.empty()) {
                auto avg_rho = get_avg_rho(pjt, -photon_eta_abs, photon_eta_abs);

                for (int64_t k = 0; k < ihf->size(); ++k) {
                    auto bin = (*rho_weighting)[k]->FindBin(avg_rho);
                    auto cor = (*rho_weighting)[k]->GetBinContent(bin);
                    weights.push_back(weight * cor);
                }
            } else {
                weights.push_back(weight);
            }

            zip([&](auto const& index, auto const& weight) {
                (*pjet_f_dsumiso)[index]->Fill(isolation, weight * pho_cor);
            }, pthf_x, weights);
        }

        f->Close();
    }

    /* normalise by number of photons (events) */
    pjet_f_dsumiso->apply([&](TH1* h) { 
        pjet_f_dsumiso->Scale(1. / pjet_f_dsumiso->Integral(1, pjet_f_dsumiso->FindBin(iso_max)));
    });

    /* save histograms */
    in(output, [&]() {
        pjet_f_dsumiso->save(tag);
    });

    printf("destroying objects..\n");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return populate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
