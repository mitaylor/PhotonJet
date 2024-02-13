#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TTree.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

static float dr2(float eta1, float eta2, float phi1, float phi2) {
    auto deta = eta1 - eta2;
    float dphi = std::abs(phi1 - phi2);
    if (dphi > TMath::Pi()) dphi = std::abs(dphi - 2*TMath::Pi());

    return deta * deta + dphi * dphi;
}

template <typename... T>
void scale_bin_width(T*... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1., "width"); }), 0)... };
}

float acceptance_weight(bool heavyion, interval* idphi, 
    history<TH2D>* total, history<TH2D>* acceptance, 
    float photon_phi, float jet_phi, float photon_eta, float jet_eta) {
   
    double cor = 1;
   
    if (heavyion) {
        auto photon_jet_dphi = std::sqrt(dr2(0, 0, jet_phi, photon_phi)) / TMath::Pi();
        auto dphi_x = idphi->index_for(photon_jet_dphi);
        auto bin = (*total)[dphi_x]->FindBin(jet_eta, photon_eta);
        
        cor = (*total)[dphi_x]->GetBinContent(bin) / (*acceptance)[dphi_x]->GetBinContent(bin);
    }
   
    return cor;
}

float back_to_back(float photon_phi, float jet_phi, float threshold) {
    float dphi = std::abs(photon_phi - jet_phi);
    if (dphi > TMath::Pi()) dphi = std::abs(dphi - 2*TMath::Pi());

    return dphi > threshold * TMath::Pi();
}

int mebs_nonclosure(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::vector<std::string>>("input");
    auto tag = conf->get<std::string>("tag");

    auto acc_file = conf->get<std::string>("acc_file");
    auto acc_label_ref = conf->get<std::string>("acc_label_ref");
    auto acc_label_acc = conf->get<std::string>("acc_label_acc");

    auto rho_file = conf->get<std::string>("rho_file");
    auto rho_label = conf->get<std::string>("rho_label");

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

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto rjpt = sel->get<std::vector<float>>("jpt_range");
    auto rdr = sel->get<std::vector<float>>("dr_range");
    auto rdphi = sel->get<std::vector<float>>("dphi_range");

    auto rdrr = sel->get<std::vector<float>>("drr_range");
    auto rptr = sel->get<std::vector<float>>("ptr_range");

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    /* fix sigma eta eta range for background distributions */
    if (tag == "bkg") see_min = 0.012;
    if (tag == "bkg") see_max = 0.02;

    auto jet_cor = heavyion && !no_jes;

    /* prepare histograms */
    auto ipt = new interval(dpt);
    auto ihf = new interval(dhf);
    auto idr = new interval("#deltaj"s, rdr);
    auto ijpt = new interval("p_{T}^{j}"s, rjpt);
    auto idphi = new interval("#Delta#phi^{#gammaj}"s, rdphi);
    auto incl = new interval(""s, 1, 0.f, 9999.f);

    auto mpthf = new multival(dpt, dhf);
    auto mdrjpt = new multival(rdrr, rptr);

    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto fdr = std::bind(&interval::book<TH1F>, idr, _1, _2, _3);
    auto fjpt = std::bind(&interval::book<TH1F>, ijpt, _1, _2, _3);

    auto frdr = [&](int64_t, std::string const& name, std::string const&) {
        return new TH1F(name.data(), ";index;", mdrjpt->size(), 0, mdrjpt->size()); };

    /* create histograms */
    auto nevt = new memory<TH1F>("nevt"s, "", fincl, mpthf);

    auto reco_pjet_f_dr = new memory<TH1F>("reco_pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto reco_pjet_f_jpt = new memory<TH1F>("reco_pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto reco_pjet_u_dr_jpt = new memory<TH1F>("reco_pjet_u_dr_jpt"s, "", frdr, mpthf);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load centrality weighting for MC */
    TFile* fr;
    history<TH1F>* rho_weighting = nullptr;

    if (!rho_file.empty()) {
        fr = new TFile((base + rho_file).data(), "read");
        rho_weighting = new history<TH1F>(fr, rho_label);
    }

    /* load acceptance weighting for HI */
    TFile* fa;
    history<TH2D>* acceptance = nullptr;
    history<TH2D>* total = nullptr;

    if (!acc_file.empty()) {
        fa = new TFile(acc_file.data(), "read");
        acceptance = new history<TH2D>(fa, acc_label_acc);
        total = new history<TH2D>(fa, acc_label_ref);
    }

    /* add weight for the number of photons, based on the fraction that are excluded by area */
    auto pho_cor = (exclude) ? 1 / (1 - pho_failure_region_fraction(photon_eta_abs)) : 1;

    /* load input */
    for (auto const& file : input) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(true, false, heavyion, t, { 1, 1, 1, 1, 1, 0, heavyion, 0, 0 });
        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        for (int64_t i = 0; i < nentries; ++i) {
            if (i % 100000 == 0) { printf("%li/%li\n", i, nentries); }

            t->GetEntry(i);

            if (std::abs(pjt->vz) > 15) { continue; }

            /* look for reco photons */
            int64_t photon_index = -1;
            float photon_pt = 0;

            for (int64_t j = 0; j < pjt->nPho; ++j) {
                auto temp_photon_pt = (*pjt->phoEt)[j];

                if (temp_photon_pt <= 30) { continue; }
                if (std::abs((*pjt->phoEta)[j]) >= photon_eta_abs) { continue; }
                if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }
                if (apply_er) temp_photon_pt = (heavyion) ? (*pjt->phoEtErNew)[j] : (*pjt->phoEtEr)[j];

                if (temp_photon_pt < photon_pt_min) { continue; }

                if (temp_photon_pt > photon_pt) {
                    photon_index = j;
                    photon_pt = temp_photon_pt;
                }
            }

            /* require leading photon */
            if (photon_index < 0) { continue; }
            if ((*pjt->phoSigmaIEtaIEta_2012)[photon_index] > see_max || (*pjt->phoSigmaIEtaIEta_2012)[photon_index] < see_min) { continue; }

            /* hem failure region exclusion */
            if (exclude && in_pho_failure_region(pjt, photon_index)) { continue; }

            /* isolation requirement */
            if ((*pjt->pho_ecalClusterIsoR3)[photon_index] + (*pjt->pho_hcalRechitIsoR3)[photon_index] + (*pjt->pho_trackIsoR3PtCut20)[photon_index] > iso_max) { continue; }

            /* leading photon axis */
            auto photon_eta = (*pjt->phoEta)[photon_index];
            auto photon_phi = (*pjt->phoPhi)[photon_index];

            /* electron rejection */
            bool electron = false;

            for (int64_t j = 0; j < pjt->nEle; ++j) {
                if (std::abs((*pjt->eleEta)[j]) > 1.4442) { continue; }

                auto dr = std::sqrt(dr2(photon_eta, (*pjt->eleEta)[j], photon_phi, (*pjt->elePhi)[j]));

                if (dr < 0.1 && passes_electron_id<det::barrel, wp::loose, pjtree>(pjt, j, heavyion)) {
                    electron = true; break;
                }
            }

            if (electron) { continue; }

            /* declare weights */
            auto pt_x = ipt->index_for(photon_pt);
            std::vector<int64_t> pthf_x;

            for (int64_t k = 0; k < ihf->size(); ++k) {
                pthf_x.push_back(mpthf->index_for(x{pt_x, k}));
            }

            auto weight = pjt->w;
            std::vector<float> weights;
            auto avg_rho = get_avg_rho(pjt, -photon_eta_abs, photon_eta_abs);

            for (int64_t k = 0; k < ihf->size(); ++k) {
                auto bin = (*rho_weighting)[k]->FindBin(avg_rho);
                auto cor = (*rho_weighting)[k]->GetBinContent(bin);

                weights.push_back(weight * cor);
            }

            zip([&](auto const& index, auto const& weight) {
                (*nevt)[index]->Fill(1., weight * pho_cor);
            }, pthf_x, weights);

            for (int64_t j = 0; j < pjt->nref; ++j) {
                auto reco_jet_pt = (jet_cor) ? (*pjt->jtptCor)[j] : (*pjt->jtpt)[j];
                auto reco_jet_eta = (*pjt->jteta)[j];
                auto reco_jet_phi = (*pjt->jtphi)[j];
                
                auto gen_jet_pt = (*pjt->refpt)[j];
                auto gen_jet_eta = (*pjt->refeta)[j];
                auto gen_jet_phi = (*pjt->refphi)[j];
                
                if (std::abs(reco_jet_eta) >= jet_eta_abs) { continue; }
                if (exclude && in_jet_failure_region(pjt, j)) { continue; }

                if (!back_to_back(photon_phi, reco_jet_phi, dphi_min_numerator/dphi_min_denominator)) { continue; }

                // no matching gen jet => fake
                if (gen_jet_pt <= 5 || dr2(gen_jet_eta, reco_jet_eta, gen_jet_phi, reco_jet_phi) > 0.0225) { 
                    continue;
                }
                
                auto jet_dr = std::sqrt(dr2(reco_jet_eta, (*pjt->WTAeta)[j], reco_jet_phi, (*pjt->WTAphi)[j]));
                auto jet_cor = acceptance_weight(heavyion, idphi, total, acceptance, photon_phi, reco_jet_phi, photon_eta, reco_jet_eta);
                
                auto drjpt_x = mdrjpt->index_for(v{jet_dr, reco_jet_pt});
                
                zip([&](auto const& index, auto const& weight) {
                    if (drjpt_x > -1 && drjpt_x < mdrjpt->size()) {
                        (*reco_pjet_u_dr_jpt)[index]->Fill(drjpt_x, jet_cor * weight);
                        (*reco_pjet_f_jpt)[index]->Fill(reco_jet_pt, jet_cor * weight);
                        (*reco_pjet_f_dr)[index]->Fill(jet_dr, jet_cor * weight);
                    }
                }, pthf_x, weights);
            }
        }

        f->Close();
        delete f;
    }

    scale_bin_width(reco_pjet_f_dr, reco_pjet_f_jpt, reco_pjet_u_dr_jpt);
    
    reco_pjet_f_dr->divide(*nevt);
    reco_pjet_f_jpt->divide(*nevt);
    reco_pjet_u_dr_jpt->divide(*nevt);

    /* save output */
    in(output, [&]() {
        nevt->save(tag);

        reco_pjet_f_dr->save(tag);
        reco_pjet_f_jpt->save(tag);
        reco_pjet_u_dr_jpt->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return mebs_nonclosure(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}