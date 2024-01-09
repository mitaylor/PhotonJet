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
void fill_axes(pjtree* pjt, multival* mpthf, multival* mpthfjpt, multival* mpthfeta, multival* mpthfdphi, 
               interval *ijpt, interval *ieta, interval *idphi, int64_t pt_x, int64_t hf_x,
               float weight, float photon_eta, float photon_phi, bool heavyion, 
               float jet_eta_abs, float jet_dr_max, float jet_pt_min, float jet_pt_max,
               float dphi_min_numerator, float dphi_min_denominator, memory<TH1F>* nevt, 
               memory<TH1F>* jpt_pjet_f_dr, memory<TH1F>* jpt_pjet_f_jpt,
               memory<TH1F>* jpt_pjet_f_dphi, memory<TH1F>* jpt_pjet_f_dr_jpt,
               memory<TH1F>* eta_pjet_f_dr, memory<TH1F>* eta_pjet_f_jpt,
               memory<TH1F>* eta_pjet_f_dphi, memory<TH1F>* eta_pjet_f_dr_eta,
               memory<TH1F>* dphi_pjet_f_dr, memory<TH1F>* dphi_pjet_f_jpt,
               memory<TH1F>* dphi_pjet_f_dphi, memory<TH1F>* dphi_pjet_f_dr_dphi,
               history<TH2F>* acceptance, history<TH2F>* total
            ) {
    
    auto pthf_x = mpthf->index_for(x{pt_x, hf_x});
    
    (*nevt)[pthf_x]->Fill(1., weight);

    for (int64_t j = 0; j < pjt->nref; ++j) {
        auto jet_pt = (*pjt->jtpt)[j];
        auto jet_eta = (*pjt->jteta)[j];
        auto jet_phi = (*pjt->jtphi)[j];

        if (std::abs(jet_eta) >= jet_eta_abs) { continue; }
        if (heavyion && in_jet_failure_region(pjt, j)) { continue; }

        auto jet_dr = std::sqrt(dr2(jet_eta, (*pjt->WTAeta)[j], jet_phi, (*pjt->WTAphi)[j]));
        auto photon_jet_dphi = std::sqrt(dr2(0, 0, jet_phi, photon_phi)) / TMath::Pi();

        // do acceptance weighting
        auto dphi_x = idphi->index_for(photon_jet_dphi);
        auto bin = (*total)[dphi_x]->FindBin(jet_eta, photon_eta);
        auto cor = (*total)[dphi_x]->GetBinContent(bin) / (*acceptance)[dphi_x]->GetBinContent(bin);

        if (cor < 1) { std::cout << "error" << std::endl; }

        // get the indices
        auto jpt_x =  ijpt->index_for(jet_pt);
        auto phi_x = idphi->index_for(photon_jet_dphi);
        auto eta_x = ieta->index_for(jet_eta);

        auto mpthfjpt_x = mpthfjpt->index_for(x{pt_x, jpt_x, hf_x});
        auto mpthfdphi_x = mpthfdphi->index_for(x{pt_x, phi_x, hf_x});
        auto mpthfeta_x = mpthfeta->index_for(x{pt_x, eta_x, hf_x});

        // fill the dphi scan comparison histograms
        if (mpthfdphi_x > -1 && mpthfdphi_x < mpthfdphi->size() && jet_dr < jet_dr_max) {
            (*dphi_pjet_f_jpt)[pthf_x]->Fill(jet_pt, weight*cor);

            if (jet_pt > jet_pt_min && jet_pt < jet_pt_max) {
                (*dphi_pjet_f_dr_dphi)[mpthfdphi_x]->Fill(jet_dr, weight*cor);
                (*dphi_pjet_f_dr)[pthf_x]->Fill(jet_dr, weight*cor);
                (*dphi_pjet_f_dphi)[pthf_x]->Fill(photon_jet_dphi, weight*cor);
            }
        }

        if (!back_to_back(photon_phi, jet_phi, dphi_min_numerator/dphi_min_denominator)) { continue; }

        // fill the other histograms
        if (mpthfeta_x > -1 && mpthfeta_x < mpthfeta->size() && jet_dr < jet_dr_max) {
            (*eta_pjet_f_jpt)[pthf_x]->Fill(jet_pt, weight*cor);

            if (jet_pt > jet_pt_min && jet_pt < jet_pt_max) {
                (*eta_pjet_f_dr_eta)[mpthfeta_x]->Fill(jet_dr, weight*cor);
                (*eta_pjet_f_dr)[pthf_x]->Fill(jet_dr, weight*cor);
                (*eta_pjet_f_dphi)[pthf_x]->Fill(photon_jet_dphi, weight*cor);
            }
        }

        if (mpthfjpt_x > -1 && mpthfjpt_x < mpthfjpt->size() && jet_dr < jet_dr_max) {
            (*jpt_pjet_f_jpt)[pthf_x]->Fill(jet_pt, weight*cor);
            (*jpt_pjet_f_dr_jpt)[mpthfjpt_x]->Fill(jet_dr, weight*cor);

            if (jet_pt > jet_pt_min && jet_pt < jet_pt_max) {
                (*jpt_pjet_f_dr)[pthf_x]->Fill(jet_dr, weight*cor);
                (*jpt_pjet_f_dphi)[pthf_x]->Fill(photon_jet_dphi, weight*cor);
            }
        }
    }
}

/* version for background events */
void fill_axes(std::vector<std::map<std::string,float>> pjt, multival* mpthf, 
               multival* mpthfjpt, multival* mpthfeta, multival* mpthfdphi, 
               interval *ijpt, interval *ieta, interval *idphi, 
               int64_t pt_x, int64_t hf_x, float weight, float photon_eta, float photon_phi,
               float jet_eta_abs, float jet_dr_max, float jet_pt_min, float jet_pt_max,
               float dphi_min_numerator, float dphi_min_denominator, memory<TH1F>* nevt, 
               memory<TH1F>* jpt_pjet_f_dr, memory<TH1F>* jpt_pjet_f_jpt,
               memory<TH1F>* jpt_pjet_f_dphi, memory<TH1F>* jpt_pjet_f_dr_jpt,
               memory<TH1F>* eta_pjet_f_dr, memory<TH1F>* eta_pjet_f_jpt,
               memory<TH1F>* eta_pjet_f_dphi, memory<TH1F>* eta_pjet_f_dr_eta,
               memory<TH1F>* dphi_pjet_f_dr, memory<TH1F>* dphi_pjet_f_jpt,
               memory<TH1F>* dphi_pjet_f_dphi, memory<TH1F>* dphi_pjet_f_dr_dphi,
               history<TH2F>* acceptance, history<TH2F>* total
            ) {
    
    auto pthf_x = mpthf->index_for(x{pt_x, hf_x});
    
    (*nevt)[pthf_x]->Fill(1., weight);

    for (size_t j = 0; j < pjt.size(); ++j) {
        auto jet_pt = pjt[j]["jet_pt"];
        auto jet_eta = pjt[j]["jet_eta"];
        auto jet_phi = pjt[j]["jet_phi"];
        auto jet_wta_eta = pjt[j]["jet_wta_eta"];
        auto jet_wta_phi = pjt[j]["jet_wta_phi"];

        if (std::abs(jet_eta) >= jet_eta_abs) { continue; }

        auto jet_dr = std::sqrt(dr2(jet_eta, jet_wta_eta, jet_phi, jet_wta_phi));
        auto photon_jet_dphi = std::sqrt(dr2(0, 0, jet_phi, photon_phi)) / TMath::Pi();
        
        // do acceptance weighting
        auto dphi_x = idphi->index_for(photon_jet_dphi);
        auto bin = (*total)[dphi_x]->FindBin(jet_eta, photon_eta);
        auto cor = (*total)[dphi_x]->GetBinContent(bin) / (*acceptance)[dphi_x]->GetBinContent(bin);
        
        if (cor < 1) { std::cout << "error" << std::endl; }

        // get the indices
        auto jpt_x =  ijpt->index_for(jet_pt);
        auto phi_x = idphi->index_for(photon_jet_dphi);
        auto eta_x = ieta->index_for(jet_eta);

        auto mpthfjpt_x = mpthfjpt->index_for(x{pt_x, jpt_x, hf_x});
        auto mpthfdphi_x = mpthfdphi->index_for(x{pt_x, phi_x, hf_x});
        auto mpthfeta_x = mpthfeta->index_for(x{pt_x, eta_x, hf_x});

        // fill the dphi scan comparison histograms
        if (mpthfdphi_x > -1 && mpthfdphi_x < mpthfdphi->size() && jet_dr < jet_dr_max) {
            (*dphi_pjet_f_jpt)[pthf_x]->Fill(jet_pt, weight*cor);

            if (jet_pt > jet_pt_min && jet_pt < jet_pt_max) {
                (*dphi_pjet_f_dr_dphi)[mpthfdphi_x]->Fill(jet_dr, weight*cor);
                (*dphi_pjet_f_dr)[pthf_x]->Fill(jet_dr, weight*cor);
                (*dphi_pjet_f_dphi)[pthf_x]->Fill(photon_jet_dphi, weight*cor);
            }
        }

        if (!back_to_back(photon_phi, jet_phi, dphi_min_numerator/dphi_min_denominator)) { continue; }

        // fill the other histograms
        if (mpthfeta_x > -1 && mpthfeta_x < mpthfeta->size() && jet_dr < jet_dr_max) {
            (*eta_pjet_f_jpt)[pthf_x]->Fill(jet_pt, weight*cor);

            if (jet_pt > jet_pt_min && jet_pt < jet_pt_max) {
                (*eta_pjet_f_dr_eta)[mpthfeta_x]->Fill(jet_dr, weight*cor);
                (*eta_pjet_f_dr)[pthf_x]->Fill(jet_dr, weight*cor);
                (*eta_pjet_f_dphi)[pthf_x]->Fill(photon_jet_dphi, weight*cor);
            }
        }

        if (mpthfjpt_x > -1 && mpthfjpt_x < mpthfjpt->size() && jet_dr < jet_dr_max) {
            (*jpt_pjet_f_jpt)[pthf_x]->Fill(jet_pt, weight*cor);
            (*jpt_pjet_f_dr_jpt)[mpthfjpt_x]->Fill(jet_dr, weight*cor);

            if (jet_pt > jet_pt_min && jet_pt < jet_pt_max) {
                (*jpt_pjet_f_dr)[pthf_x]->Fill(jet_dr, weight*cor);
                (*jpt_pjet_f_dphi)[pthf_x]->Fill(photon_jet_dphi, weight*cor);
            }
        }
    }
}

int populate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::vector<std::string>>("input");
    auto mb = conf->get<std::vector<std::string>>("mb");

    auto mix = conf->get<int64_t>("mix");
    auto frequency = conf->get<int64_t>("frequency");
    auto tag = conf->get<std::string>("tag");

    auto dhf = conf->get<std::vector<float>>("hf_diff");

    auto acc_file = conf->get<std::string>("acc_file");
    auto acc_label_ref = conf->get<std::string>("acc_label_ref");
    auto acc_label_acc = conf->get<std::string>("acc_label_acc");

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
    auto const jet_dr_max = sel->get<float>("jet_dr_max");

    auto const jet_pt_min = sel->get<float>("jet_pt_min");
    auto const jet_pt_max = sel->get<float>("jet_pt_max");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto rjpt = sel->get<std::vector<float>>("jpt_range");
    auto rdr = sel->get<std::vector<float>>("dr_range");
    auto rdphi = sel->get<std::vector<float>>("dphi_range");
    auto reta = sel->get<std::vector<float>>("eta_range");

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    auto alter_base = conf->get<std::string>("alter_base"); // get offset to paths from the original configuration file

    /* fix sigma eta eta range for background distributions */
    if (tag == "bkg") see_min = 0.012;
    if (tag == "bkg") see_max = 0.02;

    /* load acceptance weighting for HI */
    TFile* fa;
    history<TH2F>* acceptance = nullptr;
    history<TH2F>* total = nullptr;

    if (!acc_file.empty()) {
        fa = new TFile(acc_file.data(), "read");
        acceptance = new history<TH2F>(fa, acc_label_acc);
        total = new history<TH2F>(fa, acc_label_ref);
    }

    /* prepare histograms */
    auto ipt = new interval(dpt);
    auto ihf = new interval(dhf);
    auto idr = new interval("#deltaj"s, rdr);
    auto ijpt = new interval("p_{T}^{j}"s, rjpt);
    auto idphi = new interval("#Delta#phi"s, rdphi);
    auto ieta = new interval(reta);
    auto incl = new interval(""s, 1, 0.f, 9999.f);

    auto mpthf = new multival(dpt, dhf);
    auto mpthfeta = new multival(dpt, reta, dhf);
    auto mpthfdphi = new multival(dpt, rdphi, dhf);
    auto mpthfjpt = new multival(dpt, rjpt, dhf);

    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto fdr = std::bind(&interval::book<TH1F>, idr, _1, _2, _3);
    auto fjpt = std::bind(&interval::book<TH1F>, ijpt, _1, _2, _3);
    auto fdphi = std::bind(&interval::book<TH1F>, idphi, _1, _2, _3);

    /* create histograms */
    auto nevt = new memory<TH1F>("nevt"s, "", fincl, mpthf);
    auto nmix = new memory<TH1F>("nmix"s, "", fincl, mpthf);

    // jpt scan
    auto jpt_pjet_f_dr = new memory<TH1F>("jpt_pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto jpt_pjet_f_jpt = new memory<TH1F>("jpt_pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto jpt_pjet_f_dphi = new memory<TH1F>("jpt_pjet_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi", fdphi, mpthf);
    auto jpt_pjet_f_dr_jpt = new memory<TH1F>("jpt_pjet_f_dr_jpt"s, 
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthfjpt);

    auto jpt_mix_pjet_f_dr = new memory<TH1F>("mix_jpt_pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto jpt_mix_pjet_f_jpt = new memory<TH1F>("mix_jpt_pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto jpt_mix_pjet_f_dphi = new memory<TH1F>("mix_jpt_pjet_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi", fdphi, mpthf);
    auto jpt_mix_pjet_f_dr_jpt = new memory<TH1F>("mix_jpt_pjet_f_dr_jpt"s, 
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthfjpt);

    // eta scan
    auto eta_pjet_f_dr = new memory<TH1F>("eta_pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto eta_pjet_f_jpt = new memory<TH1F>("eta_pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto eta_pjet_f_dphi = new memory<TH1F>("eta_pjet_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi", fdphi, mpthf);
    auto eta_pjet_f_dr_eta = new memory<TH1F>("eta_pjet_f_dr_eta"s, 
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthfeta);

    auto eta_mix_pjet_f_dr = new memory<TH1F>("mix_eta_pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto eta_mix_pjet_f_jpt = new memory<TH1F>("mix_eta_pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto eta_mix_pjet_f_dphi = new memory<TH1F>("mix_eta_pjet_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi", fdphi, mpthf);
    auto eta_mix_pjet_f_dr_eta = new memory<TH1F>("mix_eta_pjet_f_dr_eta"s, 
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthfeta);

    // dphi scan
    auto dphi_pjet_f_dr = new memory<TH1F>("dphi_pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto dphi_pjet_f_jpt = new memory<TH1F>("dphi_pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto dphi_pjet_f_dphi = new memory<TH1F>("dphi_pjet_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi", fdphi, mpthf);
    auto dphi_pjet_f_dr_dphi = new memory<TH1F>("dphi_pjet_f_dr_dphi"s, 
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthfdphi);

    auto dphi_mix_pjet_f_dr = new memory<TH1F>("mix_dphi_pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto dphi_mix_pjet_f_jpt = new memory<TH1F>("mix_dphi_pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto dphi_mix_pjet_f_dphi = new memory<TH1F>("mix_dphi_pjet_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi", fdphi, mpthf);
    auto dphi_mix_pjet_f_dr_dphi = new memory<TH1F>("mix_dphi_pjet_f_dr_dphi"s, 
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthfdphi);

    /* random number for mb selection */
    auto rng = new TRandom3(0);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* set up mixed event background subtraction */
    TFile* fm = new TFile(mb[0].data(), "read");
    TTree* tm = (TTree*)fm->Get("pj");
    auto pjtm = new pjtree(false, false, heavyion, tm, { 1, 1, 1, 1, 1, 0, heavyion, 0, 0 });
    int64_t mentries = static_cast<int64_t>(tm->GetEntries());

    // bin construction, 10% intervals
    std::vector<float> hf_bins = {dhf.front()};
    float bin_factor = 1.1;

    while (hf_bins.back() < dhf.back()) {
      hf_bins.push_back(hf_bins.back()*bin_factor);
    }

    hf_bins.push_back(10000);

    auto ihfm = new interval(hf_bins);

    // map: tag, events, jets, jet kinematics
    std::map<int64_t, std::vector<std::vector<std::map<std::string,float>>>> hf_map;
    for (size_t i = 0; i < hf_bins.size() - 1; ++i) { hf_map[i] = {}; }

    // fill map
    for (int64_t i = 0; i < mentries; ++i){
        tm->GetEntry(i);

        double hf = pjtm->hiHF; 

        if (hf <= dhf.front() || hf >= dhf.back()) { continue; }
        if (std::abs(pjtm->vz) > 15) { continue; }
      
        auto hfm_x = ihfm->index_for(hf);
      
        std::vector<std::map<std::string, float>> jet_vector;

        for (int64_t j = 0; j < pjtm->nref; ++j) {
            if (heavyion && in_jet_failure_region(pjtm, j)) { continue; }

            auto jet_pt = (*pjtm->jtpt)[j];
            auto jet_eta = (*pjtm->jteta)[j];
            auto jet_phi = (*pjtm->jtphi)[j];
            auto jet_wta_eta = (*pjtm->WTAeta)[j];
            auto jet_wta_phi = (*pjtm->WTAphi)[j];

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

    std::cout << "Bin: Events" << std::endl;

    for(size_t i = 0; i < hf_bins.size() - 1; ++i) {
        std::cout << " " << i << ", " << hf_bins[i] << "-" << hf_bins[i+1] << ": " << hf_map[i].size() << std::endl;
    }

    /* load input */
    for (auto const& file : input) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(false, false, heavyion, t, { 1, 1, 1, 1, 1, 0, heavyion, 0, 0 });
        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        for (int64_t i = 0; i < nentries; ++i) {
            if (i % frequency == 0) { printf("entry: %li/%li\n", i, nentries); }

            t->GetEntry(i);

            double hf = pjt->hiHF; 

            if (hf <= dhf.front() || hf >= dhf.back()) { continue; }
            if (std::abs(pjt->vz) > 15) { continue; }

            /* look for reco photons */
            int64_t photon_index = -1;
            float photon_pt = 0;

            for (int64_t j = 0; j < pjt->nPho; ++j) {
                if (std::abs((*pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
                if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }
                if ((*pjt->phoEt)[j] < photon_pt_min) { continue; }

                if ((*pjt->phoEt)[j] > photon_pt) {
                    photon_index = j;
                    photon_pt = (*pjt->phoEt)[j];
                }
            }

            /* require leading photon */
            if (photon_index < 0) { continue; }
            if ((*pjt->phoSigmaIEtaIEta_2012)[photon_index] > see_max || (*pjt->phoSigmaIEtaIEta_2012)[photon_index] < see_min) { continue; }

            /* hem failure region exclusion */
            if (heavyion && in_pho_failure_region(pjt, photon_index)) { continue; }

            /* isolation requirement */
            float isolation = (*pjt->pho_ecalClusterIsoR3)[photon_index] + (*pjt->pho_hcalRechitIsoR3)[photon_index] + (*pjt->pho_trackIsoR3PtCut20)[photon_index];
            if (isolation > iso_max) { continue; }

            /* leading photon axis */
            auto photon_eta = (*pjt->phoEta)[photon_index];
            auto photon_phi = (*pjt->phoPhi)[photon_index];

            /* electron rejection */
            bool electron = false;
            for (int64_t j = 0; j < pjt->nEle; ++j) {
                if (std::abs((*pjt->eleSCEta)[j]) > 1.4442) { continue; }

                auto dr = std::sqrt(dr2(photon_eta, (*pjt->eleEta)[j], photon_phi, (*pjt->elePhi)[j]));

                if (dr < 0.1 && passes_electron_id<det::barrel, wp::loose, pjtree>(pjt, j, heavyion)) {
                    electron = true; break;
                }
            }

            if (electron) { continue; }

            /* declare weights */
            auto pt_x = ipt->index_for(photon_pt);
            auto hf_x = ihf->index_for(hf);
            auto weight = pjt->w;

            /* fill histograms */
            fill_axes(pjt, mpthf, mpthfjpt, mpthfeta, mpthfdphi,
                ijpt, ieta, idphi, pt_x, hf_x, weight,
                photon_eta, photon_phi, heavyion, jet_eta_abs, 
                jet_dr_max, jet_pt_min, jet_pt_max,
                dphi_min_numerator, dphi_min_denominator, nevt, 
                jpt_pjet_f_dr, jpt_pjet_f_jpt, 
                jpt_pjet_f_dphi, jpt_pjet_f_dr_jpt,
                eta_pjet_f_dr, eta_pjet_f_jpt, 
                eta_pjet_f_dphi, eta_pjet_f_dr_eta,
                dphi_pjet_f_dr, dphi_pjet_f_jpt, 
                dphi_pjet_f_dphi, dphi_pjet_f_dr_dphi, 
                acceptance, total);

            if (mix > 0) {
                auto hfm_x = ihfm->index_for(hf);
                size_t map_x = rng->Integer(hf_map[hfm_x].size());

                /* mixing events in minimum bias */
                for (int64_t k = 0; k < mix; k++) {
                    std::vector<std::map<std::string, float>> jet_vector = hf_map[hfm_x][map_x];

                    fill_axes(jet_vector, mpthf, mpthfjpt, mpthfeta, mpthfdphi,
                        ijpt, ieta, idphi, pt_x, hf_x, weight,
                        photon_eta, photon_phi, jet_eta_abs, 
                        jet_dr_max, jet_pt_min, jet_pt_max, 
                        dphi_min_numerator, dphi_min_denominator, nmix, 
                        jpt_mix_pjet_f_dr, jpt_mix_pjet_f_jpt, 
                        jpt_mix_pjet_f_dphi, jpt_mix_pjet_f_dr_jpt,
                        eta_mix_pjet_f_dr, eta_mix_pjet_f_jpt, 
                        eta_mix_pjet_f_dphi, eta_mix_pjet_f_dr_eta,
                        dphi_mix_pjet_f_dr, dphi_mix_pjet_f_jpt, 
                        dphi_mix_pjet_f_dphi, dphi_mix_pjet_f_dr_dphi, 
                        acceptance, total);

                    map_x++;
                    if (map_x >= hf_map[hfm_x].size()) { map_x = 0; }
                }
            }
        }

        f->Close();
    }

    /* scale histograms */
    if (mix > 0) {
        scale(1. / mix, jpt_mix_pjet_f_dr, jpt_mix_pjet_f_jpt, jpt_mix_pjet_f_dphi, jpt_mix_pjet_f_dr_jpt);
        scale(1. / mix, eta_mix_pjet_f_dr, eta_mix_pjet_f_jpt, eta_mix_pjet_f_dphi, eta_mix_pjet_f_dr_eta);
        scale(1. / mix, dphi_mix_pjet_f_dr, dphi_mix_pjet_f_jpt, dphi_mix_pjet_f_dphi, dphi_mix_pjet_f_dr_dphi);
    }

    /* subtract histograms */
    auto jpt_sub_pjet_f_dr = new memory<TH1F>(*jpt_pjet_f_dr, "sub");
    auto jpt_sub_pjet_f_jpt = new memory<TH1F>(*jpt_pjet_f_jpt, "sub");
    auto jpt_sub_pjet_f_dphi = new memory<TH1F>(*jpt_pjet_f_dphi, "sub");
    auto jpt_sub_pjet_f_dr_jpt = new memory<TH1F>(*jpt_pjet_f_dr_jpt, "sub");

    auto eta_sub_pjet_f_dr = new memory<TH1F>(*eta_pjet_f_dr, "sub");
    auto eta_sub_pjet_f_jpt = new memory<TH1F>(*eta_pjet_f_jpt, "sub");
    auto eta_sub_pjet_f_dphi = new memory<TH1F>(*eta_pjet_f_dphi, "sub");
    auto eta_sub_pjet_f_dr_eta = new memory<TH1F>(*eta_pjet_f_dr_eta, "sub");

    auto dphi_sub_pjet_f_dr = new memory<TH1F>(*dphi_pjet_f_dr, "sub");
    auto dphi_sub_pjet_f_jpt = new memory<TH1F>(*dphi_pjet_f_jpt, "sub");
    auto dphi_sub_pjet_f_dphi = new memory<TH1F>(*dphi_pjet_f_dphi, "sub");
    auto dphi_sub_pjet_f_dr_dphi = new memory<TH1F>(*dphi_pjet_f_dr_dphi, "sub");

    *jpt_sub_pjet_f_dr -= *jpt_mix_pjet_f_dr;
    *jpt_sub_pjet_f_jpt -= *jpt_mix_pjet_f_jpt;
    *jpt_sub_pjet_f_dphi -= *jpt_mix_pjet_f_dphi;
    *jpt_sub_pjet_f_dr_jpt -= *jpt_mix_pjet_f_dr_jpt;

    *eta_sub_pjet_f_dr -= *eta_mix_pjet_f_dr;
    *eta_sub_pjet_f_jpt -= *eta_mix_pjet_f_jpt;
    *eta_sub_pjet_f_dphi -= *eta_mix_pjet_f_dphi;
    *eta_sub_pjet_f_dr_eta -= *eta_mix_pjet_f_dr_eta;

    *dphi_sub_pjet_f_dr -= *dphi_mix_pjet_f_dr;
    *dphi_sub_pjet_f_jpt -= *dphi_mix_pjet_f_jpt;
    *dphi_sub_pjet_f_dphi -= *dphi_mix_pjet_f_dphi;
    *dphi_sub_pjet_f_dr_dphi -= *dphi_mix_pjet_f_dr_dphi;
    
    /* save histograms */
    in(output, [&]() {
        nevt->save(tag);
        nmix->save(tag);

        jpt_pjet_f_dr->save(tag);
        jpt_pjet_f_jpt->save(tag);
        jpt_pjet_f_dphi->save(tag);
        jpt_pjet_f_dr_jpt->save(tag);

        jpt_mix_pjet_f_dr->save(tag);
        jpt_mix_pjet_f_jpt->save(tag);
        jpt_mix_pjet_f_dphi->save(tag);
        jpt_mix_pjet_f_dr_jpt->save(tag);

        jpt_sub_pjet_f_dr->save(tag);
        jpt_sub_pjet_f_jpt->save(tag);
        jpt_sub_pjet_f_dphi->save(tag);
        jpt_sub_pjet_f_dr_jpt->save(tag);

        eta_pjet_f_dr->save(tag);
        eta_pjet_f_jpt->save(tag);
        eta_pjet_f_dphi->save(tag);
        eta_pjet_f_dr_eta->save(tag);

        eta_mix_pjet_f_dr->save(tag);
        eta_mix_pjet_f_jpt->save(tag);
        eta_mix_pjet_f_dphi->save(tag);
        eta_mix_pjet_f_dr_eta->save(tag);

        eta_sub_pjet_f_dr->save(tag);
        eta_sub_pjet_f_jpt->save(tag);
        eta_sub_pjet_f_dphi->save(tag);
        eta_sub_pjet_f_dr_eta->save(tag);

        dphi_pjet_f_dr->save(tag);
        dphi_pjet_f_jpt->save(tag);
        dphi_pjet_f_dphi->save(tag);
        dphi_pjet_f_dr_dphi->save(tag);

        dphi_mix_pjet_f_dr->save(tag);
        dphi_mix_pjet_f_jpt->save(tag);
        dphi_mix_pjet_f_dphi->save(tag);
        dphi_mix_pjet_f_dr_dphi->save(tag);

        dphi_sub_pjet_f_dr->save(tag);
        dphi_sub_pjet_f_jpt->save(tag);
        dphi_sub_pjet_f_dphi->save(tag);
        dphi_sub_pjet_f_dr_dphi->save(tag);
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
