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

template <typename... T>
void scale_bin_width(T*... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1., "width"); }), 0)... };
}

void fill_axes(pjtree* pjt, 
               std::vector<int64_t>& hf_x, std::vector<float>& weights, 
               float pho_cor, float photon_pt, float photon_eta, int64_t photon_phi, 
               bool exclude, 
               bool jet_cor, float jet_pt_min, float jet_eta_abs,
               float dphi_min_numerator, float dphi_min_denominator,
               interval* idphi,
               history<TH1F>* photon_pt_spectrum, history<TH1F>* photon_jet_pt_spectrum,
               history<TH2F>* acceptance, history<TH2F>* total) {
    
    zip([&](auto const& index, auto const& weight) {
        (*photon_pt_spectrum)[index]->Fill(photon_pt, weight * pho_cor);
    }, hf_x, weights);

    for (int64_t j = 0; j < pjt->nref; ++j) {
        auto jet_pt = (*pjt->jtpt)[j];
        if (jet_cor) jet_pt = (*pjt->jtptCor)[j];
        
        if (jet_pt <= jet_pt_min) { continue; }

        auto jet_eta = (*pjt->jteta)[j];
        if (std::abs(jet_eta) >= jet_eta_abs) { continue; }

        auto jet_phi = convert_radian((*pjt->jtphi)[j]);

        /* hem failure region exclusion */
        if (exclude && in_jet_failure_region(pjt,j)) { continue; }

        auto jet_wta_eta = (*pjt->WTAeta)[j];
        auto jet_wta_phi = convert_radian((*pjt->WTAphi)[j]);

        auto photon_jet_dphi = std::abs(photon_phi - jet_phi);

        /* require back-to-back jets */
        if (photon_jet_dphi < convert_pi(dphi_min_numerator/dphi_min_denominator)) { continue; }

        /* do acceptance weighting */
        double cor = 1;
        if (exclude) {
            auto dphi_x = idphi->index_for(revert_pi(photon_jet_dphi));
            auto bin = (*total)[dphi_x]->FindBin(jet_eta, photon_eta);
            cor = (*total)[dphi_x]->GetBinContent(bin) / (*acceptance)[dphi_x]->GetBinContent(bin);
            if (cor < 1) { std::cout << "error" << std::endl; }
        }

        double jt_deta = jet_eta - jet_wta_eta;
        double jt_dphi = revert_radian(jet_phi - jet_wta_phi);
        double jt_dr = std::sqrt(jt_deta * jt_deta + jt_dphi * jt_dphi);

        if (jt_dr > 0.3) { continue; }

        zip([&](auto const& index, auto const& weight) {
            if (jet_pt < 200 && jet_pt > jet_pt_min) {
                (*photon_jet_pt_spectrum)[index]->Fill(photon_pt, weight * cor * pho_cor);
            }
        }, hf_x, weights);
    }
}

int populate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::vector<std::string>>("input");

    auto acc_file = conf->get<std::string>("acc_file");
    auto acc_label_ref = conf->get<std::string>("acc_label_ref");
    auto acc_label_acc = conf->get<std::string>("acc_label_acc");

    auto eff_file = conf->get<std::string>("eff_file");
    auto eff_label = conf->get<std::string>("eff_label");

    auto rho_file = conf->get<std::string>("rho_file");
    auto rho_label = conf->get<std::string>("rho_label");

    auto frequency = conf->get<int64_t>("frequency");
    auto tag = conf->get<std::string>("tag");

    /* options */
    auto gen_iso = conf->get<bool>("generator_isolation");
    auto ele_rej = conf->get<bool>("electron_rejection");
    auto exclude = conf->get<bool>("exclude");
    auto apply_er = conf->get<bool>("apply_er");
    auto no_jes = conf->get<bool>("no_jes");

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

    auto const jet_pt_min = sel->get<float>("jet_pt_min");
    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto rdphi = sel->get<std::vector<float>>("dphi_range"); // used for the acceptance weighting

    std::vector<float> dpt { 40, 50, 60, 80, 100, 200 };

    /* fix sigma eta eta range for background distributions */
    if (tag == "bkg") see_min = 0.012;
    if (tag == "bkg") see_max = 0.02;

    /* make histograms */
    auto ipt = new interval("photon p_{T}", dpt);
    auto ihf = new interval(dhf);
    auto idphi = new interval("#Delta#phi^{#gammaj}"s, rdphi);

    auto fpt = std::bind(&interval::book<TH1F>, ipt, _1, _2, _3);

    auto photon_pt_spectrum = new history<TH1F>("pt_spectrum"s, "", fpt, ihf->size());
    auto photon_jet_pt_spectrum = new history<TH1F>("jet_pt_spectrum"s, "", fpt, ihf->size());

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

    /* load acceptance weighting for HI */
    TFile* fa;
    history<TH2F>* acceptance = nullptr;
    history<TH2F>* total = nullptr;

    if (!acc_file.empty()) {
        fa = new TFile(acc_file.data(), "read");
        acceptance = new history<TH2F>(fa, acc_label_acc);
        total = new history<TH2F>(fa, acc_label_ref);
    }

    /* add weight for the number of photons, based on the fraction that are excluded by area */
    auto pho_cor = (exclude) ? 1 / (1 - pho_failure_region_fraction(photon_eta_abs)) : 1;

    /* load input */
    for (auto const& file : input) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(gen_iso, false, heavyion, t, { 1, 1, 1, 1, 1, 0, heavyion, 1, !heavyion });
        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        for (int64_t i = 0; i < nentries; ++i) {
            if (i % frequency == 0) { printf("entry: %li/%li\n", i, nentries); }

            t->GetEntry(i);

            if (rho_file.empty() && pjt->hiHF <= dhf.front()) { continue; }
            if (rho_file.empty() && pjt->hiHF >= dhf.back()) { continue; }
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
            if (gen_iso) {
                auto gen_index = (*pjt->pho_genMatchedIndex)[leading];
                if (gen_index == -1) { continue; }

                float isolation = (*pjt->mcCalIsoDR04)[gen_index];
                if (isolation > gen_iso_max) { continue; }
            } else {
                float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
                    + (*pjt->pho_hcalRechitIsoR3)[leading]
                    + (*pjt->pho_trackIsoR3PtCut20)[leading];
                if (isolation > iso_max) { continue; }
            }

            /* leading photon axis */
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

            double hf = pjt->hiHF;

            std::vector<int64_t> hf_x;
            if (!rho_file.empty()) {
                for (int64_t k = 0; k < ihf->size(); ++k) {
                    hf_x.push_back(k);
                }
            } else {
                hf_x.push_back(ihf->index_for(hf));
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

            fill_axes(pjt, hf_x, weights, pho_cor, leading_pt,
                photon_eta, photon_phi, exclude, heavyion && !no_jes,
                jet_pt_min, jet_eta_abs, dphi_min_numerator, dphi_min_denominator,
                idphi, photon_pt_spectrum, photon_jet_pt_spectrum,
                acceptance, total);
            
        }

        f->Close();
    }

    /* save histograms */
    in(output, [&]() {
        photon_pt_spectrum->save(tag);
        photon_jet_pt_spectrum->save(tag);
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
