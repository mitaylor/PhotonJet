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
               interval* idphi, float pho_cor, float photon_eta, float photon_phi, 
               bool jet_cor, float jet_eta_abs, float dphi_min_numerator, float dphi_min_denominator,
               memory<TH1F>* nevt, memory<TH1F>* njet20, memory<TH1F>* njet25, memory<TH1F>* njet30, 
               memory<TH1F>* njet35, memory<TH1F>* njet40, memory<TH1F>* njet50,
               history<TH2D>* acceptance, history<TH2D>* total) {
    
    zip([&](auto const& index, auto const& weight) {
        (*nevt)[index]->Fill(1., weight * pho_cor);
    }, pthf_x, weights);

    for (int64_t j = 0; j < pjt->nref; ++j) {
        auto jet_pt = (jet_cor) ? (*pjt->jtptCor)[j] : (*pjt->jtpt)[j];
        auto jet_eta = (*pjt->jteta)[j];
        auto jet_phi = (*pjt->jtphi)[j];

        if (std::abs(jet_eta) >= jet_eta_abs) { continue; }
        if (exclude && in_jet_failure_region(pjt, j)) { continue; }

        auto photon_jet_dphi = std::sqrt(dr2(0, 0, jet_phi, photon_phi)) / TMath::Pi();
        auto photon_jet_dr = std::sqrt(dr2(jet_eta, photon_eta, jet_phi, photon_phi));

        if (photon_jet_dr < 0.4) { continue; }
        if (!back_to_back(photon_phi, jet_phi, dphi_min_numerator/dphi_min_denominator)) { continue; }

        /* do acceptance weighting */
        double cor = 1;

        if (exclude) {
            auto dphi_x = idphi->index_for(photon_jet_dphi);
            auto bin = (*total)[dphi_x]->FindBin(jet_eta, photon_eta);

            cor = (*total)[dphi_x]->GetBinContent(bin) / (*acceptance)[dphi_x]->GetBinContent(bin);
        }

        zip([&](auto const& index, auto const& weight) {
            if (jet_pt > 20) {
                (*njet20)[pthf_x]->Fill(1, weight*cor);
            }
            if (jet_pt > 25) {
                (*njet25)[pthf_x]->Fill(1, weight*cor);
            }
            if (jet_pt > 30) {
                (*njet30)[pthf_x]->Fill(1, weight*cor);
            }
            if (jet_pt > 35) {
                (*njet35)[pthf_x]->Fill(1, weight*cor);
            }
            if (jet_pt > 40) {
                (*njet40)[pthf_x]->Fill(1, weight*cor);
            }
            if (jet_pt > 50) {
                (*njet50)[pthf_x]->Fill(1, weight*cor);
            }
        }, pthf_x, weights);
    }
}

/* version for background events */
void fill_axes(std::vector<std::map<std::string,float>> pjt, std::vector<int64_t>& pthf_x, std::vector<float>& weights, 
               bool exclude, interval* idphi, float pho_cor, float photon_eta, float photon_phi, 
               float jet_eta_abs, float dphi_min_numerator, float dphi_min_denominator,
               memory<TH1F>* nevt, memory<TH1F>* njet20, memory<TH1F>* njet25, memory<TH1F>* njet30, 
               memory<TH1F>* njet35, memory<TH1F>* njet40, memory<TH1F>* njet50, history<TH2D>* acceptance, history<TH2D>* total) {
    
    zip([&](auto const& index, auto const& weight) {
        (*nevt)[index]->Fill(1., weight * pho_cor);
    }, pthf_x, weights);

    for (size_t j = 0; j < pjt.size(); ++j) {
        auto jet_pt = pjt[j]["jet_pt"];
        auto jet_eta = pjt[j]["jet_eta"];
        auto jet_phi = pjt[j]["jet_phi"];

        if (std::abs(jet_eta) >= jet_eta_abs) { continue; }

        auto photon_jet_dphi = std::sqrt(dr2(0, 0, jet_phi, photon_phi)) / TMath::Pi();
        auto photon_jet_dr = std::sqrt(dr2(jet_eta, photon_eta, jet_phi, photon_phi));

        if (photon_jet_dr < 0.4) { continue; }
        if (!back_to_back(photon_phi, jet_phi, dphi_min_numerator/dphi_min_denominator)) { continue; }

        /* do acceptance weighting */
        double cor = 1;

        if (exclude) {
            auto dphi_x = idphi->index_for(photon_jet_dphi);
            auto bin = (*total)[dphi_x]->FindBin(jet_eta, photon_eta);
            
            cor = (*total)[dphi_x]->GetBinContent(bin) / (*acceptance)[dphi_x]->GetBinContent(bin);
        }

        zip([&](auto const& index, auto const& weight) {
            if (jet_pt > 20) {
                (*njet20)[pthf_x]->Fill(1, weight*cor);
            }
            if (jet_pt > 25) {
                (*njet25)[pthf_x]->Fill(1, weight*cor);
            }
            if (jet_pt > 30) {
                (*njet30)[pthf_x]->Fill(1, weight*cor);
            }
            if (jet_pt > 35) {
                (*njet35)[pthf_x]->Fill(1, weight*cor);
            }
            if (jet_pt > 40) {
                (*njet40)[pthf_x]->Fill(1, weight*cor);
            }
            if (jet_pt > 50) {
                (*njet50)[pthf_x]->Fill(1, weight*cor);
            }
        }, pthf_x, weights);
    }
}

int populate(char const* config, char const* selections, char const* output) {
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

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto rjpt = sel->get<std::vector<float>>("jpt_range");
    auto rdr = sel->get<std::vector<float>>("dr_range");
    auto rdphi = sel->get<std::vector<float>>("dphi_range");

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    auto photon_pt_es = sel->get<std::vector<float>>("photon_pt_es");

    auto alter_base = conf->get<std::string>("alter_base"); // get offset to paths from the original configuration file

    /* fix sigma eta eta range for background distributions */
    if (tag == "bkg") see_min = 0.012;
    if (tag == "bkg") see_max = 0.02;

    auto jet_cor = heavyion && !no_jes;

    /* prepare histograms */
    auto ipt = new interval(dpt);
    auto ihf = new interval(dhf);
    auto idphi = new interval("#Delta#phi^{#gammaj}"s, rdphi);
    auto incl = new interval(""s, 1, 0.f, 9999.f);
    auto mpthf = new multival(dpt, dhf);

    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);

    /* create histograms */
    auto nevt = new memory<TH1F>("nevt"s, "", fincl, mpthf);
    auto nmix = new memory<TH1F>("nmix"s, "", fincl, mpthf);

    auto njet20 = new memory<TH1F>("njet20"s, "", fincl, mpthf);
    auto njet25 = new memory<TH1F>("njet25"s, "", fincl, mpthf);
    auto njet30 = new memory<TH1F>("njet30"s, "", fincl, mpthf);
    auto njet35 = new memory<TH1F>("njet35"s, "", fincl, mpthf);
    auto njet40 = new memory<TH1F>("njet40"s, "", fincl, mpthf);
    auto njet50 = new memory<TH1F>("njet50"s, "", fincl, mpthf);

    auto mix_njet20 = new memory<TH1F>("mix_njet20"s, "", fincl, mpthf);
    auto mix_njet25 = new memory<TH1F>("mix_njet25"s, "", fincl, mpthf);
    auto mix_njet30 = new memory<TH1F>("mix_njet30"s, "", fincl, mpthf);
    auto mix_njet35 = new memory<TH1F>("mix_njet35"s, "", fincl, mpthf);
    auto mix_njet40 = new memory<TH1F>("mix_njet40"s, "", fincl, mpthf);
    auto mix_njet50 = new memory<TH1F>("mix_njet50"s, "", fincl, mpthf);

    /* random number for mb selection */
    auto rng = new TRandom3(0);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* set up mixed event background subtraction */
    // bin construction, 10% intervals
    std::vector<float> hf_bins = {0, dhf.front()};
    float bin_factor = 1.1;
    int64_t tentries = 0;

    if (mix > 0) while (hf_bins.back() < dhf.back()) {
        hf_bins.push_back(hf_bins.back()*bin_factor);
    }

    if (rho_file.empty()) {
        hf_bins.pop_back(); hf_bins.pop_back();
    }
    
    hf_bins.push_back(10000);

    auto ihfm = new interval(hf_bins);

    // map: tag, events, jets, jet kinematics
    std::map<int64_t, std::vector<std::vector<std::map<std::string,float>>>> hf_map;
    for (size_t i = 0; i < hf_bins.size() - 1; ++i) { hf_map[i] = {}; }

    while (tentries < 952000 && mix > 0) {
        int index_m = rng->Integer(mb.size());
        TFile* fm = new TFile(mb[index_m].data(), "read");
        TTree* tm = (TTree*)fm->Get("pj");
        auto pjtm = new pjtree(gen_iso, false, heavyion, tm, { 1, 1, 1, 1, 1, 0, heavyion, 0, 0 });
        
        int64_t mentries = static_cast<int64_t>(tm->GetEntries());
        
        tentries += mentries;

        for (int64_t i = 0; i < mentries; ++i){
            tm->GetEntry(i);

            double hf = pjtm->hiHF; 
            auto hfm_x = ihfm->index_for(hf);

            if (std::abs(pjtm->vz) > 15) { continue; }
            if (rho_file.empty() && hf <= dhf.front()) { continue; }
            if (rho_file.empty() && hf >= dhf.back()) { continue; }
        
            std::vector<std::map<std::string, float>> jet_vector;

            for (int64_t j = 0; j < pjtm->nref; ++j) {
                if (exclude && in_jet_failure_region(pjtm, j)) { continue; }

                auto jet_pt = (jet_cor) ? (*pjtm->jtptCor)[j] : (*pjtm->jtpt)[j];
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
    }

    std::cout << "Bin: Events" << std::endl;

    for (size_t i = 0; i < hf_bins.size() - 1; ++i) {
        std::cout << " " << i << ", " << hf_bins[i] << "-" << hf_bins[i+1] << ": " << hf_map[i].size() << std::endl;
    }

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
        auto pjt = new pjtree(gen_iso, false, heavyion, t, { 1, 1, 1, 1, 1, 0, heavyion, 0, !heavyion });
        int64_t nentries = static_cast<int64_t>(t->GetEntries());

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
                auto bin = (*efficiency)[1]->FindBin(photon_pt / photon_es);
                auto cor = (*efficiency)[0]->GetBinContent(bin) / (*efficiency)[1]->GetBinContent(bin);

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
            fill_axes(pjt, pthf_x, weights, exclude, idphi,
                pho_cor, photon_eta, photon_phi, jet_cor, jet_eta_abs, 
                dphi_min_numerator, dphi_min_denominator, nevt, 
                njet20, njet25, njet30, njet35, njet40, njet50,
                acceptance, total);

            if (mix > 0) {
                auto hfm_x = ihfm->index_for(hf);
                size_t map_x = rng->Integer(hf_map[hfm_x].size());

                /* mixing events in minimum bias */
                for (int64_t k = 0; k < mix; k++) {
                    std::vector<std::map<std::string, float>> jet_vector = hf_map[hfm_x][map_x];

                    fill_axes(jet_vector, pthf_x, weights, exclude, idphi,
                        pho_cor, photon_eta, photon_phi, jet_eta_abs, 
                        dphi_min_numerator, dphi_min_denominator, nmix, 
                        mix_njet20, mix_njet25, mix_njet30, mix_njet35, 
                        mix_njet40, mix_njet50, acceptance, total);

                    map_x++;
                    if (map_x >= hf_map[hfm_x].size()) { map_x = 0; }
                }
            }
        }

        f->Close();
    }

    /* normalise histograms */
    if (mix > 0) {
        scale(1. / mix, mix_njet20, mix_njet25, mix_njet30, mix_njet35, mix_njet40, mix_njet50);
    }

    float area_fraction = TMath::Pi()*0.09 / (2*jet_eta_abs*2*TMath::Pi()/3);

    scale(area_fraction, njet20, njet25, njet30, njet35, njet40, njet50);
    scale(area_fraction, mix_njet20, mix_njet25, mix_njet30, mix_njet35, mix_njet40, mix_njet50);

    njet20->divide(*nevt);
    njet25->divide(*nevt);
    njet30->divide(*nevt);
    njet35->divide(*nevt);
    njet40->divide(*nevt);
    njet50->divide(*nevt);

    mix_njet20->divide(*nevt);
    mix_njet25->divide(*nevt);
    mix_njet30->divide(*nevt);
    mix_njet35->divide(*nevt);
    mix_njet40->divide(*nevt);
    mix_njet50->divide(*nevt);

    /* save histograms */
    in(output, [&]() {
        nevt->save(tag);
        nmix->save(tag);

        njet20->save(tag);
        njet25->save(tag);
        njet30->save(tag);
        njet35->save(tag);
        njet40->save(tag);
        njet50->save(tag);

        mix_njet20->save(tag);
        mix_njet25->save(tag);
        mix_njet30->save(tag);
        mix_njet35->save(tag);
        mix_njet40->save(tag);
        mix_njet50->save(tag);
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
