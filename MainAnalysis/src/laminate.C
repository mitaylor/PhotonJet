#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

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

// template <typename... T>
// void normalise_to_unity(T*&... args) {
//     (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
//         obj->Scale(1. / obj->Integral("width")); }), 0)... };
// }

void fill_axes(pjtree* pjt, float weight, float photon_eta, int64_t photon_phi, 
               multival* mpthf, bool exclude, bool jet_cor, float jet_pt_min,
               float max_dphi, memory<TH1F>* pjet_lead_jet_deta, bool test) {

    float leading_jet_pt = 0;
    float leading_jet_eta = -999;
    int64_t leading_jet_x = -1;

    std::vector<float> accepted_jet_pt;
    std::vector<float> accepted_jet_eta;
    std::vector<int64_t> accepted_jet_x;

    for (int64_t j = 0; j < pjt->nref; ++j) {
        auto jet_pt = (*pjt->jtpt)[j];
        if (jet_cor) jet_pt = (*pjt->jtptCor)[j];
        
        if (jet_pt <= jet_pt_min) { continue; }

        auto jet_eta = (*pjt->jteta)[j];
        if (std::abs(jet_eta) >= 1.6) { continue; }

        auto jet_phi = convert_radian((*pjt->jtphi)[j]);

        auto pj_deta = photon_eta - jet_eta;
        auto pj_dphi = revert_radian(std::abs(photon_phi - jet_phi));
        auto pj_dr = std::sqrt(pj_deta * pj_deta + pj_dphi * pj_dphi);

        if (pj_dr < 0.4) { continue; }

        /* hem failure region exclusion */
        if (exclude && in_jet_failure_region(pjt,j)) { continue; }

        accepted_jet_pt.push_back(jet_pt);
        accepted_jet_eta.push_back(jet_eta);
        accepted_jet_x.push_back(j);
        
        if (pj_dphi > max_dphi) { continue; }

        if (jet_pt > leading_jet_pt) {
            leading_jet_pt = jet_pt;
            leading_jet_eta = jet_eta;
            leading_jet_x = j;
        }
    }

    if (leading_jet_x < 0) { return; }

    if (test) std::cout << "here" << std::endl;

    for (int64_t i = 0; i < (int64_t) accepted_jet_x.size(); ++i) {
        if (i == leading_jet_x) { continue; }

        auto pthf_x = mpthf->index_for(v{accepted_jet_pt[i], pjt->hiHF});
        auto deta = std::abs(leading_jet_eta - accepted_jet_eta[i]);

        if (test) std::cout << pthf_x << " " << deta << " " << accepted_jet_pt[i] << " " << pjt->hiHF << std::endl;

        (*pjet_lead_jet_deta)[pthf_x]->Fill(deta, weight);
    }
}

int populate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto mb = conf->get<std::string>("mb");
    
    auto eff = conf->get<std::string>("eff");
    auto eff_label = conf->get<std::string>("eff_label");

    auto rho = conf->get<std::string>("rho");
    auto rho_label = conf->get<std::string>("rho_label");
    auto index = conf->get<int64_t>("index");

    auto entries = conf->get<int64_t>("entries");
    auto mix = conf->get<int64_t>("mix");
    auto frequency = conf->get<int64_t>("frequency");
    auto tag = conf->get<std::string>("tag");

    /* options */
    auto heavyion = conf->get<bool>("heavyion");
    auto gen_iso = conf->get<bool>("generator_isolation");
    auto ele_rej = conf->get<bool>("electron_rejection");
    auto exclude = conf->get<bool>("exclude");
    auto apply_er = conf->get<bool>("apply_er");
    auto no_jes = conf->get<bool>("no_jes");

    /* selections */
    auto const photon_pt_min = conf->get<float>("photon_pt_min");
    auto const photon_eta_abs = conf->get<float>("photon_eta_abs");
    auto const hovere_max = conf->get<float>("hovere_max");
    auto const see_min = conf->get<float>("see_min");
    auto const see_max = conf->get<float>("see_max");
    auto const iso_max = conf->get<float>("iso_max");
    auto const gen_iso_max = conf->get<float>("gen_iso_max");
    auto const jet_pt_min = conf->get<float>("jet_pt_min");
    auto const max_dphi = conf->get<float>("max_dphi");

    auto rdeta = conf->get<std::vector<float>>("deta_range");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");

    auto mpthf = new multival(dpt, dhf);

    auto ideta = new interval("#Delta#eta^{jj}"s, rdeta);
    auto fdeta = std::bind(&interval::book<TH1F>, ideta, _1, _2, _3);

    auto pjet_lead_jet_deta = new memory<TH1F>("pjet_lead_jet_deta"s,
        "1/N^{#gamma} dN/d#Delta#eta^{jj}", fdeta, mpthf);
    auto mix_pjet_lead_jet_deta = new memory<TH1F>("mix_pjet_lead_jet_deta"s,
        "1/N^{#gamma} dN/d#Delta#eta^{jj}", fdeta, mpthf);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    TFile* f = new TFile(input.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto pjt = new pjtree(gen_iso, false, heavyion, t, { 1, 1, 1, 1, 1, 0, heavyion });

    TFile* fm = new TFile(mb.data(), "read");
    TTree* tm = (TTree*)fm->Get("pj");
    auto pjtm = new pjtree(gen_iso, false, heavyion, tm, { 1, 1, 1, 1, 1, 0, heavyion });

    printf("iterate..\n");

    /* load efficiency correction */
    TFile* fe;
    history<TH1F>* efficiency = nullptr;

    if (!eff.empty()) {
        fe = new TFile(eff.data(), "read");
        efficiency = new history<TH1F>(fe, eff_label);
    }

    /* load centrality weighting for MC */
    TFile* fr;
    history<TH1F>* rho_weighting = nullptr;

    if (!rho.empty()) {
        fr = new TFile(rho.data(), "read");
        rho_weighting = new history<TH1F>(fr, rho_label);
    }

    int64_t nentries = static_cast<int64_t>(t->GetEntries());
    int64_t mod = 1;
    if (entries) { mod = nentries / entries; }
    if (mod !=1) { std::cout << "mod: " << mod << std::endl; }

    int64_t mentries = static_cast<int64_t>(tm->GetEntries());
    
    int64_t tentries = 0;
    clock_t time = clock();
    clock_t duration = 0;

    for (int64_t i = 0, m = 0; i < nentries; ++i) {
        if (i % frequency == 0) { printf("entry: %li/%li\n", i, nentries); }
        if (i % frequency == 0) { 
            if (tentries != 0) {
                duration = clock() - time;
                std::cout << "Time per " << frequency/mod << " entries: " << (double)(duration)/CLOCKS_PER_SEC << " seconds" << std::endl;
                std::cout << "Entries: " << tentries << std::endl;
                tentries = 0;
                time = clock();
            }
        }

        if (i % mod != 0) { continue; }

        t->GetEntry(i);
        
        if (rho.empty() && pjt->hiHF <= dhf.front()) { std::cout << dhf.front() << std::endl; continue; }
        if (rho.empty() && pjt->hiHF >= dhf.back()) { std::cout << dhf.back() << std::endl; continue; }
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

        auto weight = pjt->w;

        if (!eff.empty() && leading_pt < 70) {
            auto bin = (*efficiency)[1]->FindBin(leading_pt);
            auto corr = (*efficiency)[0]->GetBinContent(bin) / (*efficiency)[1]->GetBinContent(bin);
            if (corr < 1) { std::cout << "error" << std::endl; return -1; }
            weight *= corr;
        }

        if (!rho.empty()) {
            auto avg_rho = get_avg_rho(pjt,-photon_eta_abs,photon_eta_abs);
            auto bin = (*rho_weighting)[index]->FindBin(avg_rho);
            auto corr = (*rho_weighting)[index]->GetBinContent(bin);
            weight *= corr;
        }
        bool test = false;
        if (i > 520000) test = true; if (test) { std::cout << pjt->hiHF << " " << i << std::endl; }
        fill_axes(pjt, weight, photon_eta, photon_phi, mpthf, 
                  exclude, heavyion && !no_jes, jet_pt_min, 
                  max_dphi, pjet_lead_jet_deta, test);

        /* mixing events in minimum bias */
        for (int64_t k = 0; k < mix; m = (m + 1) % mentries) {
            tm->GetEntry(m);

            /* hf within +/- 10% */
            if (std::abs(pjtm->rho / pjt->rho - 1.) > 0.1) { continue; }

            fill_axes(pjtm, weight, photon_eta, photon_phi, mpthf,
                      exclude, heavyion && !no_jes, jet_pt_min, 
                      max_dphi, mix_pjet_lead_jet_deta, test);

            ++k;
        }

        tentries++;
    }

    /* normalise histograms */
    if (mix > 0)
        scale(1. / mix,
            mix_pjet_lead_jet_deta);

    /* scale by bin width */
    scale_bin_width(
        pjet_lead_jet_deta,
        mix_pjet_lead_jet_deta);

    /* normalise */
    // figure out how to normalize well

    /* subtract histograms */
    auto sub_pjet_lead_jet_deta = new memory<TH1F>(*pjet_lead_jet_deta, "sub");

    *sub_pjet_lead_jet_deta -= *mix_pjet_lead_jet_deta;

    /* save histograms */
    in(output, [&]() {
        pjet_lead_jet_deta->save(tag);
        mix_pjet_lead_jet_deta->save(tag);
        sub_pjet_lead_jet_deta->save(tag);
    });

    printf("destroying objects..\n");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return populate(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
