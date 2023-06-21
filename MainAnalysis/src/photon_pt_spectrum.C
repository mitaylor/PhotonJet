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

float res(float c, float s, float n, float pt) {
    return std::sqrt(c*c + s*s / pt + n*n / (pt * pt));
}

void fill_axes(pjtree* pjt, 
               std::vector<int64_t>& hf_x, std::vector<float>& weights, 
               float photon_eta, int64_t photon_phi, float photon_pt,
               bool exclude, bool jet_cor, float jet_pt_min, float jet_eta_abs,
               float dphi_min_numerator, float dphi_min_denominator,
               interval* idphi, history<TH1F>* spectrum_photon_jet,
               history<TH2F>* acceptance, history<TH2F>* total) {

    for (int64_t j = 0; j < pjt->nref; ++j) {
        auto jet_pt = (*pjt->jtpt)[j];
        if (jet_cor) jet_pt = (*pjt->jtptCor)[j];
        
        if (jet_pt <= jet_pt_min && jet_pt >= 200) { continue; }

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
            (*spectrum_photon_jet)[index]->Fill(photon_pt, cor * weight);
        }, hf_x, weights);
    }
}

int photon_pt_spectrum(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::vector<std::string>>("input");
    auto mb = conf->get<std::vector<std::string>>("mb");

    auto acc_file = conf->get<std::string>("acc_file");
    auto acc_label_ref = conf->get<std::string>("acc_label_ref");
    auto acc_label_acc = conf->get<std::string>("acc_label_acc");

    auto eff_file = conf->get<std::string>("eff_file");
    auto eff_label = conf->get<std::string>("eff_label");

    auto rho_file = conf->get<std::string>("rho_file");
    auto rho_label = conf->get<std::string>("rho_label");

    auto mix = conf->get<int64_t>("mix");
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
    auto const photon_pt_max = sel->get<float>("photon_pt_max");
    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");
    auto const hovere_max = sel->get<float>("hovere_max");
    auto see_min = sel->get<float>("see_min");
    auto see_max = sel->get<float>("see_max");
    auto const iso_max = sel->get<float>("iso_max");

    auto const jet_pt_min = sel->get<float>("jet_pt_min");
    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto rjpt = sel->get<std::vector<float>>("jpt_range");
    auto rdphi = sel->get<std::vector<float>>("dphi_range"); // used for the acceptance weighting

    auto rdrr = sel->get<std::vector<float>>("drr_range");
    auto rptr = sel->get<std::vector<float>>("ptr_range");

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    auto alter_base = conf->get<std::string>("alter_base"); // get offset to paths from the original configuration file

    /* make histograms */
    dpt.pop_back();

    auto ipt = new interval("p_{T}^{#gamma}"s, dpt);
    auto ihf = new interval(dhf);

    auto fpt = std::bind(&interval::book<TH1F>, ipt, _1, _2, _3);

    auto spectrum_photon = new history<TH1F>("spectrum_photon"s, "", fpt, ihf->size());
    auto spectrum_photon_jet = new history<TH1F>("spectrum_photon_jet"s, "", fpt, ihf->size());
    auto mix_spectrum_photon_jet = new history<TH1F>("mix_spectrum_photon_jet"s, "", fpt, ihf->size());

    /* random number for mb selection */
    auto rng = new TRandom3(144);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    int index_m = rng->Integer(mb.size());
    TFile* fm = new TFile(mb[index_m].data(), "read");
    TTree* tm = (TTree*)fm->Get("pj");
    auto pjtm = new pjtree(gen_iso, false, heavyion, tm, { 1, 1, 1, 1, 1, 0, heavyion, 0, 0 });
    int64_t mentries = static_cast<int64_t>(tm->GetEntries()); std::cout << mentries << std::endl;

    printf("iterate..\n");

    /* load efficiency correction */
    TFile* fe;
    history<TH1F>* efficiency = nullptr;

    if (!eff_file.empty()) {
        fe = new TFile((alter_base + base + eff_file).data(), "read");
        efficiency = new history<TH1F>(fe, eff_label);
    }

    /* load centrality weighting for MC */
    TFile* fr;
    history<TH1F>* rho_weighting = nullptr;

    if (!rho_file.empty()) {
        fr = new TFile((alter_base + base + rho_file).data(), "read");
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
        auto pjt = new pjtree(gen_iso, false, heavyion, t, { 1, 1, 1, 1, 1, 0, heavyion, 0, !heavyion });
        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        for (int64_t i = 0, m = 0; i < nentries; ++i) {
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

                if (pho_et < photon_pt_min || pho_et > photon_pt_max) { continue; }

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
            if (isolation > iso_max) { continue; }

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

            zip([&](auto const& index, auto const& weight) {
                (*spectrum_photon)[index]->Fill(leading_pt, weight * pho_cor);
            }, hf_x, weights);

            fill_axes(pjt, hf_x, weights, photon_eta, photon_phi, 
                    leading_pt, exclude, heavyion && !no_jes,
                    jet_pt_min, jet_eta_abs, dphi_min_numerator, 
                    dphi_min_denominator, idphi,
                    spectrum_photon_jet, acceptance, total);

            if (mix > 0) {
                /* mixing events in minimum bias */
                for (int64_t k = 0; k < mix; m++) {
                    if ((m + 1) % mentries == 0) {
                        std::cout << "Switch MB file" << std::endl;
                        m = -1;

                        fm->Close();

                        delete fm; delete pjtm;
                        
                        index_m = rng->Integer(mb.size());
                        fm = new TFile(mb[index_m].data(), "read");
                        tm = (TTree*)fm->Get("pj");
                        pjtm = new pjtree(gen_iso, false, heavyion, tm, { 1, 1, 1, 1, 1, 0, heavyion, 0, 0 });

                        mentries = static_cast<int64_t>(tm->GetEntries()); std::cout << mentries << std::endl;
                    }

                    tm->GetEntry(m);

                    if (std::abs(pjtm->hiHF / hf - 1.) > 0.1) { continue; }

                    fill_axes(pjtm, hf_x, weights, photon_eta, photon_phi, 
                            leading_pt, exclude, heavyion && !no_jes,
                            jet_pt_min, jet_eta_abs, dphi_min_numerator, 
                            dphi_min_denominator, idphi, 
                            mix_spectrum_photon_jet, acceptance, total);

                    ++k;
                }

                tentries++;
            }
        }

        f->Close();
    }

    /* normalise histograms */
    if (mix > 0)    scale(1. / mix, mix_spectrum_photon_jet);

    // /* scale by bin width */
    // scale_bin_width(
    //     pjet_f_dr,
    //     pjet_f_jpt,
    //     mix_pjet_f_dr,
    //     mix_pjet_f_jpt);

    /* subtract histograms */
    auto sub_spectrum_photon_jet = new history<TH1F>(*spectrum_photon_jet, "sub");

    *sub_spectrum_photon_jet -= *mix_spectrum_photon_jet;

    /* save histograms */
    in(output, [&]() {
        spectrum_photon->save(tag);
        spectrum_photon_jet->save(tag);
        mix_spectrum_photon_jet->save(tag);
        sub_spectrum_photon_jet->save(tag);
    });

    printf("destroying objects..\n");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return photon_pt_spectrum(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
