#include "../include/lambdas.h"
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

#include <ctime>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename... T>
void scale(double factor, T*... args) {
    (void)(int [sizeof...(T)]) { (args->scale(factor), 0)... };
}

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

/* version for signal events */
void fill_axes(pjtree* pjt, std::vector<int64_t>& pthf_x, std::vector<float>& weights, bool exclude, 
               multival* mdrjpt, interval* idphi, float photon_pt, float pho_cor, float photon_eta, float photon_phi, 
               bool jet_cor, float jet_eta_abs, float dphi_min_numerator, float dphi_min_denominator,
               memory<TH1F>* nevt, memory<TH1F>* pjet_f_dr, memory<TH1F>* pjet_f_jpt, memory<TH1F>* pjet_f_xjg, 
               memory<TH1F>* pjet_u_dr_jpt, history<TH2D>* acceptance, history<TH2D>* total) {
    
    zip([&](auto const& index, auto const& weight) {
        (*nevt)[index]->Fill(1., weight * pho_cor);
    }, pthf_x, weights);

    for (int64_t j = 0; j < pjt->nref; ++j) {
        auto jet_pt = (jet_cor) ? (*pjt->jtptCor)[j] : (*pjt->jtpt)[j];
        auto jet_eta = (*pjt->jteta)[j];
        auto jet_phi = (*pjt->jtphi)[j];

        if (std::abs(jet_eta) >= jet_eta_abs) { continue; }
        if (exclude && in_jet_failure_region(pjt, j)) { continue; }

        auto jet_dr = std::sqrt(dr2(jet_eta, (*pjt->WTAeta)[j], jet_phi, (*pjt->WTAphi)[j]));
        auto photon_jet_dphi = std::sqrt(dr2(0, 0, jet_phi, photon_phi)) / TMath::Pi();
        auto photon_jet_dr = std::sqrt(dr2(jet_eta, photon_eta, jet_phi, photon_phi));

        if (photon_jet_dr < 0.4) { continue; }
        if (!back_to_back(photon_phi, jet_phi, dphi_min_numerator/dphi_min_denominator)) { continue; }

        auto drjpt_x = mdrjpt->index_for(v{jet_dr, jet_pt});

        /* do acceptance weighting */
        double cor = 1;

        if (exclude) {
            auto dphi_x = idphi->index_for(photon_jet_dphi);
            auto bin = (*total)[dphi_x]->FindBin(jet_eta, photon_eta);

            cor = (*total)[dphi_x]->GetBinContent(bin) / (*acceptance)[dphi_x]->GetBinContent(bin);
        }

        zip([&](auto const& index, auto const& weight) {
            if (drjpt_x > -1 && drjpt_x < mdrjpt->size()) {
                (*pjet_u_dr_jpt)[index]->Fill(drjpt_x, cor * weight);
                (*pjet_f_jpt)[index]->Fill(jet_pt, cor * weight);
                (*pjet_f_xjg)[index]->Fill(jet_pt/photon_pt, cor * weight);
                (*pjet_f_dr)[index]->Fill(jet_dr, cor * weight);
            }
        }, pthf_x, weights);
    }
}

/* version for background events */
void fill_axes(std::vector<std::map<std::string,float>> pjt, std::vector<int64_t>& pthf_x, std::vector<float>& weights, bool exclude, 
               multival* mdrjpt, interval* idphi, float photon_pt, float pho_cor, float photon_eta, float photon_phi, 
               float jet_eta_abs, float dphi_min_numerator, float dphi_min_denominator,
               memory<TH1F>* nevt, memory<TH1F>* pjet_f_dr, memory<TH1F>* pjet_f_jpt, memory<TH1F>* pjet_f_xjg, 
               memory<TH1F>* pjet_u_dr_jpt, history<TH2D>* acceptance, history<TH2D>* total) {
    
    zip([&](auto const& index, auto const& weight) {
        (*nevt)[index]->Fill(1., weight * pho_cor);
    }, pthf_x, weights);

    for (size_t j = 0; j < pjt.size(); ++j) {
        auto jet_pt = pjt[j]["jet_pt"];
        auto jet_eta = pjt[j]["jet_eta"];
        auto jet_phi = pjt[j]["jet_phi"];
        auto jet_wta_eta = pjt[j]["jet_wta_eta"];
        auto jet_wta_phi = pjt[j]["jet_wta_phi"];

        if (std::abs(jet_eta) >= jet_eta_abs) { continue; }

        auto jet_dr = std::sqrt(dr2(jet_eta, jet_wta_eta, jet_phi, jet_wta_phi));
        auto photon_jet_dphi = std::sqrt(dr2(0, 0, jet_phi, photon_phi)) / TMath::Pi();
        auto photon_jet_dr = std::sqrt(dr2(jet_eta, photon_eta, jet_phi, photon_phi));

        if (photon_jet_dr < 0.4) { continue; }
        if (!back_to_back(photon_phi, jet_phi, dphi_min_numerator/dphi_min_denominator)) { continue; }

        auto drjpt_x = mdrjpt->index_for(v{jet_dr, jet_pt});

        /* do acceptance weighting */
        double cor = 1;

        if (exclude) {
            auto dphi_x = idphi->index_for(photon_jet_dphi);
            auto bin = (*total)[dphi_x]->FindBin(jet_eta, photon_eta);
            
            cor = (*total)[dphi_x]->GetBinContent(bin) / (*acceptance)[dphi_x]->GetBinContent(bin);
        }

        zip([&](auto const& index, auto const& weight) {
            if (drjpt_x > -1 && drjpt_x < mdrjpt->size()) {
                (*pjet_u_dr_jpt)[index]->Fill(drjpt_x, cor * weight);
                (*pjet_f_jpt)[index]->Fill(jet_pt, cor * weight);
                (*pjet_f_xjg)[index]->Fill(jet_pt/photon_pt, cor * weight);
                (*pjet_f_dr)[index]->Fill(jet_dr, cor * weight);
            }
        }, pthf_x, weights);
    }
}

int populate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::vector<std::string>>("input");
    auto mb = conf->get<std::vector<std::string>>("mb");
    auto mb_sum = conf->get<std::vector<std::string>>("mb_sum");

    auto acc_file = conf->get<std::string>("acc_file");
    auto acc_label_ref = conf->get<std::string>("acc_label_ref");
    auto acc_label_acc = conf->get<std::string>("acc_label_acc");

    auto eff_file = conf->get<std::string>("eff_file");
    auto eff_label = conf->get<std::string>("eff_label");

    auto rho_file = conf->get<std::string>("rho_file");
    auto rho_label = conf->get<std::string>("rho_label");

    auto modulo = conf->get<int64_t>("modulo");
    auto parity = conf->get<bool>("parity");

    auto mix = conf->get<int64_t>("mix");
    auto frequency = conf->get<int64_t>("frequency");
    auto tag = conf->get<std::string>("tag");

    /* options */
    auto gen_iso = conf->get<bool>("generator_isolation");
    auto ele_rej = conf->get<bool>("electron_rejection");
    auto exclude = conf->get<bool>("exclude");
    auto apply_er = conf->get<bool>("apply_er");
    auto no_jes = conf->get<bool>("no_jes");
    auto apply_es = conf->get<bool>("apply_es");
    auto condor = conf->get<bool>("condor");

    auto dhf = conf->get<std::vector<float>>("hf_diff");

    auto const offset = conf->get<float>("hf_offset");

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

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto rjpt = sel->get<std::vector<float>>("jpt_range");
    auto rdr = sel->get<std::vector<float>>("dr_range");
    auto rdphi = sel->get<std::vector<float>>("dphi_range");

    auto rdrr = sel->get<std::vector<float>>("drr_range");
    auto rptr = sel->get<std::vector<float>>("ptr_range");

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    auto photon_pt_es = sel->get<std::vector<float>>("photon_pt_es");

    auto alter_base = conf->get<std::string>("alter_base"); // get offset to paths from the original configuration file

    /* fix sigma eta eta range for background distributions */
    if (tag == "bkg") see_min = 0.012;
    if (tag == "bkg") see_max = 0.02;

    auto jet_cor = heavyion && !no_jes;

    std::vector<float> rxjg = {0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.66, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.0, 1.05, 1.1, 1.15, 1.2, 1.25, 1.3, 1.35, 1.4, 1.45, 1.5, 1.55, 1.6, 1.65, 1.7, 1.75, 1.8, 1.85, 1.9, 1.95, 2.0};

    /* prepare histograms */
    auto ipt = new interval(dpt);
    auto ihf = new interval(dhf);
    auto idr = new interval("#deltaj"s, rdr);
    auto ixjg = new interval("x_{j#gamma}"s, rxjg);
    auto ijpt = new interval("p_{T}^{j}"s, rjpt);
    auto idphi = new interval("#Delta#phi^{#gammaj}"s, rdphi);
    auto incl = new interval(""s, 1, 0.f, 9999.f);

    auto mpthf = new multival(dpt, dhf);
    auto mdrjpt = new multival(rdrr, rptr);

    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto fdr = std::bind(&interval::book<TH1F>, idr, _1, _2, _3);
    auto fxjg = std::bind(&interval::book<TH1F>, ixjg, _1, _2, _3);
    auto fjpt = std::bind(&interval::book<TH1F>, ijpt, _1, _2, _3);

    auto frdr = [&](int64_t, std::string const& name, std::string const&) {
        return new TH1F(name.data(), ";index;", mdrjpt->size(), 0, mdrjpt->size()); };

    /* create histograms */
    auto nevt = new memory<TH1F>("nevt"s, "", fincl, mpthf);
    auto nmix = new memory<TH1F>("nmix"s, "", fincl, mpthf);

    auto pjet_f_dr = new memory<TH1F>("pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto pjet_f_jpt = new memory<TH1F>("pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto pjet_f_xjg = new memory<TH1F>("pjet_f_xjg"s,
        "1/N^{#gamma} dN/dx_{j#gamma}", fxjg, mpthf);
    auto pjet_u_dr_jpt = new memory<TH1F>("pjet_u_dr_jpt"s, "", frdr, mpthf);

    auto mix_pjet_f_dr = new memory<TH1F>("mix_pjet_f_dr",
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto mix_pjet_f_jpt = new memory<TH1F>("mix_pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto mix_pjet_f_xjg = new memory<TH1F>("mix_pjet_f_xjg"s,
        "1/N^{#gamma} dN/dx_{j#gamma}", fxjg, mpthf);
    auto mix_pjet_u_dr_jpt = new memory<TH1F>("mix_pjet_u_dr_jpt"s, "", frdr, mpthf);

    /* random number for mb selection */
    auto rng = new TRandom3(0);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* set up mixed event background subtraction */
    std::vector<float> hf_bins = {-1000, 100, 150, 200, 250, 300, 400, 500, 600, 700, 
                                  800, 900, 1050, 1200, 1350, 1500, 1700, 1900, 2100, 
                                  2350, 2600, 2850, 3150, 3450, 3800, 4150, 4550, 4950, 
                                  5350, 5800, 6300, 6800, 7350, 7900, 8500, 9150, 9850, 
                                  10550, 11300, 12050, 12850, 13700, 14600, 15500, 16500, 
                                  17500, 18550, 19650, 20800, 22000, 23200, 24500, 25850, 
                                  27250, 28700, 30150, 31650, 33300, 34950, 36650, 38450, 
                                  40300, 42200, 44150, 46200, 48350, 50550, 52750, 55100, 
                                  60100, 62700, 65400, 68250, 71150, 74200, 77400, 80700, 
                                  84100, 87650, 91400, 95300, 99350, 103600, 108100, 
                                  112800, 117700, 123050, 129600, 160000};
    auto ihfm = new interval(hf_bins);

    // map: tag, events, jets, jet kinematics
    std::map<int64_t, std::vector<std::vector<std::map<std::string,float>>>> hf_map;
    for (size_t i = 0; i < hf_bins.size() - 1; ++i) { hf_map[i] = {}; }

    if (mix > 0) {
        // read in variables
        int index_m = rng->Integer(mb.size());
        TFile* fm = new TFile(mb[index_m].data(), "read");
        TTree* tm = (TTree*) fm->Get("pj");

        // variables used for mixing
        float vz;
        float pfSum;

        std::vector<float> *jtptCor = 0;
        std::vector<float> *jtpt = 0;
        std::vector<float> *jteta = 0;
        std::vector<float> *jtphi = 0;
        std::vector<float> *WTAeta = 0;
        std::vector<float> *WTAphi = 0;

        tm->SetBranchAddress("vz", &vz);
        tm->SetBranchAddress("pfSum", &pfSum);
        tm->SetBranchAddress("jtptCor", &jtptCor);
        tm->SetBranchAddress("jtpt", &jtpt);
        tm->SetBranchAddress("jteta", &jteta);
        tm->SetBranchAddress("jtphi", &jtphi);
        tm->SetBranchAddress("WTAeta", &WTAeta);
        tm->SetBranchAddress("WTAphi", &WTAphi);

        int64_t mentries = static_cast<int64_t>(tm->GetEntries());
        if (mentries > 900000) mentries = 900000;

        for (int64_t i = 0; i < mentries; ++i){
            tm->GetEntry(i);

            auto hfm_x = ihfm->index_for(pfSum);
            int64_t nref = jtpt->size();

            if (std::abs(vz) > 15) { continue; }

            std::vector<std::map<std::string, float>> jet_vector;

            for (int64_t j = 0; j < nref; ++j) {
                auto jet_pt = (jet_cor) ? (*jtptCor)[j] : (*jtpt)[j];
                auto jet_eta = (*jteta)[j];
                auto jet_phi = (*jtphi)[j];
                auto jet_wta_eta = (*WTAeta)[j];
                auto jet_wta_phi = (*WTAphi)[j];

                if (exclude && in_jet_failure_region(jet_eta, jet_phi)) { continue; }
                if (std::abs(jet_eta) >= jet_eta_abs) { continue; }

                std::map<std::string, float> jet_map;
                
                jet_map["jet_pt"] = jet_pt;
                jet_map["jet_eta"] = jet_eta;
                jet_map["jet_phi"] = jet_phi;
                jet_map["jet_wta_eta"] = jet_wta_eta;
                jet_map["jet_wta_phi"] = jet_wta_phi;

                jet_vector.push_back(jet_map);
            }
        
            hf_map[hfm_x].push_back(jet_vector);
        }
    }

    std::cout << "Bin: Events" << std::endl;

    for (size_t i = 0; i < hf_bins.size() - 1; ++i) {
        std::cout << " " << i << ", " << hf_bins[i] << "-" << hf_bins[i+1] << ": " << hf_map[i].size() << std::endl;
    }

    /* load efficiency correction */
    TFile* fe;
    history<TH1F>* eff_numerator = nullptr;
    history<TH1F>* eff_denominator = nullptr;

    if (!eff_file.empty()) {
        if (!condor) fe = new TFile((alter_base + base + eff_file).data(), "read");
        else         fe = new TFile(eff_file.data(), "read");
        
        eff_numerator = new history<TH1F>(fe, eff_label + "_numerator"s);
        eff_denominator = new history<TH1F>(fe, eff_label + "_denominator"s);
    }

    /* load centrality weighting for MC */
    TFile* fr;
    history<TH1F>* rho_weighting = nullptr;

    if (!rho_file.empty()) {
        if (!condor) fr = new TFile((alter_base + base + rho_file).data(), "read");
        else         fr = new TFile(rho_file.data(), "read");

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

    if (modulo != 1) { std::cout << "modulo: " << modulo << std::endl; }

    /* load input */
    for (auto const& file : input) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(gen_iso, false, heavyion, t, { 1, 1, 1, 1, 1, 0, heavyion, 1, !heavyion });
        int64_t nentries = static_cast<int64_t>(t->GetEntries());
        int64_t pfSumType = -1;

        for (int64_t i = 0; i < nentries; ++i) {
            if (i % frequency == 0) { printf("entry: %li/%li\n", i, nentries); }
            if ((i + parity) % modulo != 0) { continue; }

            t->GetEntry(i);

            double hf = pjt->hiHF;
            auto hf_x = ihf->index_for(hf);

            if (rho_file.empty() && hf <= dhf.front()) { continue; }
            if (rho_file.empty() && hf >= dhf.back()) { continue; }
            if (std::abs(pjt->vz) > 15) { continue; }

            auto photon_es = (apply_es) ? photon_pt_es[hf_x] : 1;

            /* look for reco photons */
            int64_t photon_index = -1;
            float photon_pt = 0;

            for (int64_t j = 0; j < pjt->nPho; ++j) {
                auto temp_photon_pt = (*pjt->phoEt)[j];

                if (temp_photon_pt <= 30) { continue; }
                if (std::abs((*pjt->phoEta)[j]) >= photon_eta_abs) { continue; }
                if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }
                if (apply_er) temp_photon_pt = (heavyion) ? (*pjt->phoEtErNew)[j] : (*pjt->phoEtEr)[j];

                temp_photon_pt *= photon_es;

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
            if (gen_iso) {
                auto gen_index = (*pjt->pho_genMatchedIndex)[photon_index];

                if (gen_index == -1) { continue; }
                if ((*pjt->mcCalIsoDR04)[gen_index] > gen_iso_max) { continue; }
            } else {
                if ((*pjt->pho_ecalClusterIsoR3)[photon_index] + (*pjt->pho_hcalRechitIsoR3)[photon_index] + (*pjt->pho_trackIsoR3PtCut20)[photon_index] > iso_max) { continue; }
            }

            /* leading photon axis */
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

            /* declare weights */
            auto pt_x = ipt->index_for(photon_pt);
            std::vector<int64_t> pthf_x;

            if (!rho_file.empty()) {
                for (int64_t k = 0; k < ihf->size(); ++k) {
                    pthf_x.push_back(mpthf->index_for(x{pt_x, k}));
                }
            } else {
                pthf_x.push_back(mpthf->index_for(x{pt_x, hf_x}));
            }

            auto weight = pjt->w;

            if (!eff_file.empty() && photon_pt / photon_es < 70) {
                auto bin = (*eff_numerator)[hf_x]->FindBin(photon_pt / photon_es);
                auto cor = (*eff_denominator)[hf_x]->GetBinContent(bin) / (*eff_numerator)[hf_x]->GetBinContent(bin);

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

            /* fill histograms */
            fill_axes(pjt, pthf_x, weights, exclude, mdrjpt, idphi, photon_pt,
                pho_cor, photon_eta, photon_phi, jet_cor, jet_eta_abs, 
                dphi_min_numerator, dphi_min_denominator, nevt, 
                pjet_f_dr, pjet_f_jpt, pjet_f_xjg, pjet_u_dr_jpt, acceptance, total);

            if (mix > 0) {
                if (pfSumType < 0) {
                    if (pjt->pfE->size() > 0)       pfSumType = 1;
                    if (pjt->pfEnergy->size() > 0)  pfSumType = 2;
                }

                float pfSum = 0;

                for (size_t j = 0; j < pjt->pfEta->size(); ++j) {
                    if (std::abs((*pjt->pfEta)[j]) > 3 && std::abs((*pjt->pfEta)[j]) < 5) {
                        if (pfSumType == 1) pfSum += (*pjt->pfE)[j];
                        if (pfSumType == 2) pfSum += (*pjt->pfEnergy)[j];
                    }
                }

                auto hfm_x = ihfm->index_for(pfSum - offset);
                size_t map_x = rng->Integer(hf_map[hfm_x].size());

                /* mixing events in minimum bias */
                for (int64_t k = 0; k < mix; k++) {
                    std::vector<std::map<std::string, float>> jet_vector = hf_map[hfm_x][map_x];

                    fill_axes(jet_vector, pthf_x, weights, exclude, mdrjpt, idphi, photon_pt,
                        pho_cor, photon_eta, photon_phi, jet_eta_abs, 
                        dphi_min_numerator, dphi_min_denominator, nmix, mix_pjet_f_dr, 
                        mix_pjet_f_jpt, mix_pjet_f_xjg, mix_pjet_u_dr_jpt, acceptance, total);

                    map_x++;
                    if (map_x >= hf_map[hfm_x].size()) { map_x = 0; }
                }
            }
        }

        f->Close();
    }

    /* normalise histograms */
    if (mix > 0) { scale(1. / mix, mix_pjet_f_dr, mix_pjet_f_jpt, mix_pjet_f_xjg, mix_pjet_u_dr_jpt); }

    /* normalise by number of photons (events) */
    pjet_f_dr->divide(*nevt);
    pjet_f_jpt->divide(*nevt);
    pjet_f_xjg->divide(*nevt);
    pjet_u_dr_jpt->divide(*nevt);

    mix_pjet_f_dr->divide(*nevt);
    mix_pjet_f_jpt->divide(*nevt);
    mix_pjet_f_xjg->divide(*nevt);
    mix_pjet_u_dr_jpt->divide(*nevt);

    /* subtract histograms */
    auto sub_pjet_f_dr = new memory<TH1F>(*pjet_f_dr, "sub");
    auto sub_pjet_f_jpt = new memory<TH1F>(*pjet_f_jpt, "sub");
    auto sub_pjet_f_xjg = new memory<TH1F>(*pjet_f_xjg, "sub");
    auto sub_pjet_u_dr_jpt = new memory<TH1F>(*pjet_u_dr_jpt, "sub");

    *sub_pjet_f_dr -= *mix_pjet_f_dr;
    *sub_pjet_f_jpt -= *mix_pjet_f_jpt;
    *sub_pjet_f_xjg -= *mix_pjet_f_xjg;
    *sub_pjet_u_dr_jpt -= *mix_pjet_u_dr_jpt;

    /* save histograms */
    in(output, [&]() {
        nevt->save(tag);
        nmix->save(tag);

        pjet_f_dr->save(tag);
        pjet_f_jpt->save(tag);
        pjet_f_xjg->save(tag);
        pjet_u_dr_jpt->save(tag);

        mix_pjet_f_dr->save(tag);
        mix_pjet_f_jpt->save(tag);
        mix_pjet_f_xjg->save(tag);
        mix_pjet_u_dr_jpt->save(tag);

        sub_pjet_f_dr->save(tag);
        sub_pjet_f_jpt->save(tag);
        sub_pjet_f_xjg->save(tag);
        sub_pjet_u_dr_jpt->save(tag);
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
