#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"
#include "../git/history/include/history.h"

#include "../git/tricks-and-treats/include/trunk.h"

#include "../include/JetCorrector.h"
#include "../include/JetUncertainty.h"

#include "TFile.h"
#include "TRandom3.h"
#include "TH1.h"
#include "TH2.h"
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

float res(float c, float s, float n, float pt) {
    return std::sqrt(c*c + s*s / pt + n*n / (pt * pt));
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

int vacillate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto inputs = conf->get<std::vector<std::string>>("inputs");
    auto tag = conf->get<std::string>("tag");

    auto rho_file = conf->get<std::string>("rho_file");
    auto rho_label = conf->get<std::string>("rho_label");

    auto acc_file = conf->get<std::string>("acc_file");
    auto acc_label_ref = conf->get<std::string>("acc_label_ref");
    auto acc_label_acc = conf->get<std::string>("acc_label_acc");

    auto jer_file = conf->get<std::string>("jer_file");
    auto jer_label_hist = conf->get<std::string>("jer_label_hist");
    auto jer_label_func = conf->get<std::string>("jer_label_func");
    
    auto jersf = conf->get<std::string>("jersf");
    auto jeu = conf->get<std::string>("jeu");
    auto direction = conf->get<bool>("direction");

    auto mod = conf->get<bool>("mod");
    auto parity = conf->get<bool>("parity");

    auto apply_er = conf->get<bool>("apply_er");
    auto no_jes = conf->get<bool>("no_jes");
    auto ele_rej = conf->get<bool>("ele_rej");
    auto gen_iso = conf->get<bool>("gen_iso");
    auto jer_up = conf->get<bool>("jer_up");
    auto mc = conf->get<bool>("mc");

    auto photon_pt_weight = conf->get<std::vector<float>>("photon_pt_weight");

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
    auto const see_min = sel->get<float>("see_min");
    auto const see_max = sel->get<float>("see_max");
    auto const iso_max = sel->get<float>("iso_max");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto rdrr = sel->get<std::vector<float>>("drr_range");
    auto rdrg = sel->get<std::vector<float>>("drg_range");
    auto rptr = sel->get<std::vector<float>>("ptr_range");
    auto rptg = sel->get<std::vector<float>>("ptg_range");

    auto rdphi = sel->get<std::vector<float>>("dphi_range"); // used for the acceptance weighting

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* prepare histograms */
    auto incl = new interval(""s, 1, 0.f, 9999.f);
    auto ihf = new interval(dhf);
    auto idphi = new interval("#Delta#phi^{#gammaj}"s, rdphi);
    auto ippt = new interval("Photon p_{T}"s, 50, 0.f, 250.f);

    auto mcdr = new multival(rdrr, rdrg);
    auto mcpt = new multival(rptr, rptg);
    auto mppt = new multival (*ippt, *ippt);

    auto mr = new multival(rdrr, rptr);
    auto mg = new multival(rdrg, rptg);

    auto fn = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto fcdr = std::bind(&multival::book<TH2F>, mcdr, _1, _2, _3);
    auto fcpt = std::bind(&multival::book<TH2F>, mcpt, _1, _2, _3);
    auto fppt = std::bind(&multival::book<TH2F>, mppt, _1, _2, _3);

    auto fr = [&](int64_t, std::string const& name, std::string const& label) {
        return new TH1F(name.data(), (";Reconstructed Bin;"s + label).data(),
            mr->size(), 0, mr->size()); };

    auto fg = [&](int64_t, std::string const& name, std::string const& label) {
        return new TH1F(name.data(), (";Generator Bin;"s + label).data(),
            mg->size(), 0, mg->size()); };

    auto fc = [&](int64_t, std::string const& name, std::string const& label) {
        return new TH2F(name.data(), (";Reconstructed Bin;Generator Bin;"s + label).data(),
            mr->size(), 0, mr->size(), mg->size(), 0, mg->size()); };

    auto r = new history<TH1F>("r"s, "counts", fr, ihf->size());
    auto g = new history<TH1F>("g"s, "counts", fg, ihf->size());
    auto g_r = new history<TH1F>("g_r"s, "counts", fg, ihf->size());
    auto c = new history<TH2F>("c"s, "counts", fc, ihf->size());

    auto r_n = new history<TH1F>("r_n"s, "events", fn, ihf->size());
    auto g_n = new history<TH1F>("g_n"s, "events", fn, ihf->size());
    auto g_r_n = new history<TH1F>("g_r_n"s, "events", fn, ihf->size());
    auto c_n = new history<TH1F>("c_n"s, "events", fn, ihf->size());

    auto cdr = new history<TH2F>("cdr"s, "counts", fcdr, ihf->size());
    auto cpt = new history<TH2F>("cpt"s, "counts", fcpt, ihf->size());

    auto r_merge = new history<TH1F>("r_merge"s, "counts", fr, 1);
    auto g_merge = new history<TH1F>("g_merge"s, "counts", fg, 1);
    auto g_r_merge = new history<TH1F>("g_r_merge"s, "counts", fg, 1);
    auto c_merge = new history<TH2F>("c_merge"s, "counts", fc, 1);

    auto r_n_merge = new history<TH1F>("r_n_merge"s, "events", fn, 1);
    auto g_n_merge = new history<TH2F>("g_n_merge"s, "events", fn, 1);
    auto g_r_n_merge = new history<TH2F>("g_r_n_merge"s, "events", fn, 1);
    auto c_n_merge = new history<TH2F>("c_n_merge"s, "events", fn, 1);

    auto cdr_merge = new history<TH2F>("cdr_merge"s, "counts", fcdr, 1);
    auto cpt_merge = new history<TH2F>("cpt_merge"s, "counts", fcpt, 1);

    auto ppt = new history<TH2F>("ppt"s, "counts", fppt, ihf->size());

    /* load centrality weighting for MC */
    TFile* frho;
    history<TH1F>* rho_weighting = nullptr;
    history<TH1F>* rho_weighting_merge = nullptr;

    if (!rho_file.empty()) {
        frho = new TFile((base + rho_file).data(), "read");
        rho_weighting = new history<TH1F>(frho, rho_label);
        rho_weighting_merge = new history<TH1F>(frho, rho_label + "_merge"s);
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

    // /* load derived JER based on reco pT */
    // TFile* fj;
    // history<TH1F>* jer_histogram = nullptr;
    // history<TF1>* jer_function = nullptr;

    // if (!jer_file.empty()) {
    //     fj = new TFile(jer_file.data(), "read");
    //     jer_histogram = new history<TH1F>(fj, jer_label_hist);
    //     jer_function = new history<TF1>("jer_function"s, "", jer_histogram->shape());

    //     for (int64_t i = 0; i < jer_function->size(); ++i) {
    //         auto name = jer_label_func + "_" + std::to_string(i);
    //         (*jer_function)[i] = (*jer_histogram)[i]->GetFunction(name.c_str());
    //     }

    //     jer_function->rename(jer_label_func);

    //     for (int64_t i = 0; i < jer_function->size(); ++i) {
    //         std::cout << (*jer_function)[i]->Eval(20) << " " << (*jer_function)[i]->Eval(100) << std::endl;
    //     }
    // }

    /* get data/MC resolution correction */
    auto JERSF = new SingleJetCorrector(jersf);
    auto JEU = new JetUncertainty(jeu);

    /* load input */
    for (auto const& input : inputs) {
        std::cout << input << std::endl;
        
        TFile* f = new TFile(input.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto p = new pjtree(true, false, heavyion, t, { 1, 1, 1, 1, 1, 0, heavyion, 0, 0 });
        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        /* fill histograms */
        for (int64_t i = 0; i < nentries; ++i) {
            if (i % 100000 == 0) { printf("%li/%li\n", i, nentries); }

            if (mod) {
                if ((i + parity) % 2 == 0) { continue; }
            }
            
            t->GetEntry(i);

            /* look for reco photons */
            bool reco_photon = true;
            int64_t reco_photon_index = -1;
            float reco_photon_pt = 0;
            float reco_photon_phi = -1000;
            float reco_photon_eta = -1000;

            for (int64_t j = 0; j < p->nPho; ++j) {
                auto temp_photon_pt = (*p->phoEt)[j];

                if (temp_photon_pt <= 30) { continue; }
                if (std::abs((*p->phoEta)[j]) >= photon_eta_abs) { continue; }
                if ((*p->phoHoverE)[j] > hovere_max) { continue; }
                if (apply_er) temp_photon_pt = (heavyion) ? (*p->phoEtErNew)[j] : (*p->phoEtEr)[j];

                if (temp_photon_pt < photon_pt_min || temp_photon_pt > photon_pt_max) { continue; }

                if (temp_photon_pt > reco_photon_pt) {
                    reco_photon_index = j;
                    reco_photon_pt = temp_photon_pt;
                }
            }
           
            // potential miss if there is no reco photon in the right pT range
            if (reco_photon_index < 0) { 
                reco_photon = false; 
            }
            else {
                // potential miss if the reco photon does not pass sigma eta eta selections
                if ((*p->phoSigmaIEtaIEta_2012)[reco_photon_index] > see_max || (*p->phoSigmaIEtaIEta_2012)[reco_photon_index] < see_min) { reco_photon = false; }

                // potential miss if the reco photon is in the HEM failure region
                if (heavyion && in_pho_failure_region(p, reco_photon_index)) { reco_photon = false; }

                // potential miss if the reco photon is not isolated
                if (!gen_iso) {
                    float isolation = (*p->pho_ecalClusterIsoR3)[reco_photon_index] + (*p->pho_hcalRechitIsoR3)[reco_photon_index] + (*p->pho_trackIsoR3PtCut20)[reco_photon_index];

                    if (isolation > iso_max) { reco_photon = false; }
                }
                
                // potential miss if there is a nearby electron
                reco_photon_eta = (*p->phoEta)[reco_photon_index];
                reco_photon_phi = (*p->phoPhi)[reco_photon_index];

                if (ele_rej) {
                    bool electron = false;
                    for (int64_t j = 0; j < p->nEle; ++j) {
                        if (std::abs((*p->eleEta)[j]) > 1.4442) { continue; }

                        auto dr = std::sqrt(dr2(reco_photon_eta, (*p->eleEta)[j], reco_photon_phi, (*p->elePhi)[j]));

                        if (dr < 0.1 && passes_electron_id<det::barrel, wp::loose, pjtree>(p, j, heavyion)) {
                            electron = true; break; 
                        }
                    }

                    if (electron) { reco_photon = false; }
                }
            }
           
            /* look for gen photons */
            bool gen_photon = true;
            int64_t gen_photon_index = -1;
            float gen_photon_pt = 0;
            float gen_photon_phi = -1000;
            float gen_photon_eta = -1000;

            // find if there is a gen photon, or if it is a fake
            if (reco_photon) {
                gen_photon_index = (*p->pho_genMatchedIndex)[reco_photon_index];
            }
            // find if there is a gen photon (outside the HEM failure region), and it is a miss
            else {
                for (int64_t j = 0; j < p->nMC; ++j) {
                    auto pho_et = (*p->mcPt)[j];
                    auto pid = (*p->mcPID)[j];
                    auto mother_pid = (*p->mcMomPID)[j];

                    if (std::abs((*p->mcEta)[j]) >= photon_eta_abs) { continue; }
                    if (pid != 22 || (std::abs(mother_pid) > 22 && mother_pid != -999)) { continue; }

                    if (pho_et > gen_photon_pt) {
                        gen_photon_index = j;
                        gen_photon_pt = pho_et;
                    }
                }
            }
           
            // potential miss if there is no reco photon in the right pT range
            if (gen_photon_index < 0) { 
                gen_photon = false; 
            }
            else {
                auto pid = (*p->mcPID)[gen_photon_index];
                auto mother_pid = (*p->mcMomPID)[gen_photon_index];

                gen_photon_eta = (*p->mcEta)[gen_photon_index];
                gen_photon_phi = (*p->mcPhi)[gen_photon_index];
                gen_photon_pt = (*p->mcPt)[gen_photon_index];
                
                // fake if the gen particle is not isolated
                if ((*p->mcCalIsoDR04)[gen_photon_index] > 5) { gen_photon = false; }
                if (std::abs(gen_photon_eta) >= photon_eta_abs) { continue; }

                // fake if the gen particle is not actualy a photon
                if (pid != 22 || (std::abs(mother_pid) > 22 && mother_pid != -999)) { gen_photon = false; }

                // fake if the gen particle is not in the kinematic range
                if (gen_photon_pt < photon_pt_min || gen_photon_pt > photon_pt_max) { gen_photon = false; }
               
                // fake if the gen particle is in the HEM failure region
                if (heavyion && in_pho_failure_region(gen_photon_eta, gen_photon_phi)) { gen_photon = false; }
            }
           
            /* skip useless events */
            // fakes are handled already by the purity subtraction
            if (!gen_photon) {
                continue;
            } 

            /* declare weights */
            auto weight = p->w;
            std::vector<float> weights(ihf->size(), weight);
            float weights_merge = weight;
           
            if (heavyion) {
                auto avg_rho = get_avg_rho(p, -photon_eta_abs, photon_eta_abs);

                for (int64_t j = 0; j < ihf->size(); ++j) {
                    auto bin = (*rho_weighting)[j]->FindBin(avg_rho);
                    auto cor = (*rho_weighting)[j]->GetBinContent(bin);
                    
                    weights[j] *= cor;
                }

                auto bin = (*rho_weighting_merge)[0]->FindBin(avg_rho);
                auto cor = (*rho_weighting_merge)[0]->GetBinContent(bin);
                weights_merge *= cor;
            }
           
            if (reco_photon && photon_pt_weight.size() > 0) {
                for (int64_t j = 0; j < ihf->size(); ++j) {
                    weights[j] *= reco_photon_pt * photon_pt_weight[1] + photon_pt_weight[0];
                }

                weights_merge *= reco_photon_pt * photon_pt_weight[1] + photon_pt_weight[0];
            }

            double pho_cor = (heavyion) ? 1 / (1 - pho_failure_region_fraction(photon_eta_abs)) : 1;
           
            /* handle hits and misses for photons */
            // miss, fill the truth histogram
            if (gen_photon && !reco_photon) {
                for (int64_t k = 0; k < ihf->size(); ++k) {
                    (*g_n)[k]->Fill(1., weights[k] * pho_cor); 
                }
                
                (*g_n_merge)[0]->Fill(1., weights_merge * pho_cor);

                for (int64_t j = 0; j < p->ngen; ++j) {
                    auto gen_jet_pt = (*p->genpt)[j];
                    auto gen_jet_eta = (*p->geneta)[j];
                    auto gen_jet_phi = (*p->genphi)[j];
                   
                    if (std::abs(gen_jet_eta) >= jet_eta_abs) { continue; }
                    if (heavyion && in_jet_failure_region(gen_jet_eta, gen_jet_phi)) { continue; }
                    if (!back_to_back(gen_photon_phi, gen_jet_phi, dphi_min_numerator/dphi_min_denominator)) { continue; }
                    
                    auto gen_jet_dr = std::sqrt(dr2(gen_jet_eta, (*p->WTAgeneta)[j], gen_jet_phi, (*p->WTAgenphi)[j]));
                    auto jet_cor = acceptance_weight(heavyion, idphi, total, acceptance, gen_photon_phi, gen_jet_phi, gen_photon_eta, gen_jet_eta);
                    
                    // fill histograms
                    for (int64_t k = 0; k < ihf->size(); ++k) { 
                        (*g)[k]->Fill(mg->index_for(v{gen_jet_dr, gen_jet_pt}), weights[k] * jet_cor); 
                    }
                   
                    (*g_merge)[0]->Fill(mg->index_for(v{gen_jet_dr, gen_jet_pt}), weights_merge * jet_cor);
                }
            }
            // real, fill all histograms
            else if (gen_photon && reco_photon) {
                // look at photon correlations
                for (int64_t k = 0; k < ihf->size(); ++k) {
                    (*ppt)[k]->Fill(reco_photon_pt, gen_photon_pt, weights[k] * pho_cor);
                }

                /* fill histogram */
                for (int64_t k = 0; k < ihf->size(); ++k) {
                    (*r_n)[k]->Fill(1., weights[k] * pho_cor); 
                    (*g_n)[k]->Fill(1., weights[k] * pho_cor); 
                    (*g_r_n)[k]->Fill(1., weights[k] * pho_cor); 
                    (*c_n)[k]->Fill(1., weights[k] * pho_cor); 
                }
                
                (*r_n_merge)[0]->Fill(1., weights_merge * pho_cor);
                (*g_n_merge)[0]->Fill(1., weights_merge * pho_cor);
                (*g_r_n_merge)[0]->Fill(1., weights_merge * pho_cor);
                (*c_n_merge)[0]->Fill(1., weights_merge * pho_cor);
               
                // map reco jet to gen jet
                std::unordered_map<float, int64_t> gen_jet_id;
                for (int64_t j = 0; j < p->ngen; ++j)
                    gen_jet_id[(*p->genpt)[j]] = j;

                std::vector<int64_t> exclude;

                // go through reco jets and fill histograms for hits and fakes
                for (int64_t j = 0; j < p->nref; ++j) {
                    auto reco_jet_pt = (!no_jes && heavyion) ? (*p->jtptCor)[j] : (*p->jtpt)[j];
                    auto reco_jet_eta = (*p->jteta)[j];
                    auto reco_jet_phi = (*p->jtphi)[j];
                   
                    auto gen_jet_pt = (*p->refpt)[j];
                    auto gen_jet_eta = (*p->refeta)[j];
                    auto gen_jet_phi = (*p->refphi)[j];
                   
                    if (std::abs(reco_jet_eta) >= jet_eta_abs) { continue; }
                    if (heavyion && in_jet_failure_region(p, j)) { continue; }
                    if (!back_to_back(reco_photon_phi, reco_jet_phi, dphi_min_numerator/dphi_min_denominator)) { continue; }

                    // no matching gen jet => fake, already accounted for by mixed-event background subtraction
                    if (gen_jet_pt <= 5 || dr2((*p->refeta)[j], (*p->jteta)[j], (*p->refphi)[j], (*p->jtphi)[j]) > 0.0225) { 
                        continue;
                    }
                    
                    auto reco_jet_dr = std::sqrt(dr2(reco_jet_eta, (*p->WTAeta)[j], reco_jet_phi, (*p->WTAphi)[j]));
                    auto jet_cor = acceptance_weight(heavyion, idphi, total, acceptance, reco_photon_phi, reco_jet_phi, reco_photon_eta, reco_jet_eta);
                    
                    // jet energy scale uncertainty
                    if (!jeu.empty()) {
                        JEU->SetJetPT(reco_jet_pt);
                        JEU->SetJetEta(reco_jet_eta);
                        JEU->SetJetPhi(reco_jet_phi);

                        auto jes_uncertainty = JEU->GetUncertainty();
                        reco_jet_pt *= direction ? (1. + jes_uncertainty.second) : (1. - jes_uncertainty.first);
                    }
                   
                    // matching gen jet, hit
                    exclude.push_back(gen_jet_id[gen_jet_pt]);

                    // add in data/MC JER difference correction
                    JERSF->SetJetPT(reco_jet_pt);
                    JERSF->SetJetEta(reco_jet_eta);
                    JERSF->SetJetPhi(reco_jet_phi);
                    
                    auto jer_scale_factors = JERSF->GetParameters();
                    auto jer_scale = jer_scale_factors[0];
                    if (jer_up && heavyion) jer_scale += (jer_scale_factors[2] - jer_scale_factors[0]) * 1.5;
                    else if  (jer_up && !heavyion) jer_scale += (jer_scale_factors[2] - jer_scale_factors[0]);
                    if (!mc) { reco_jet_pt *= 1 + (jer_scale - 1) * (reco_jet_pt - gen_jet_pt) / reco_jet_pt; }

                    // fill histograms
                    auto id = gen_jet_id[gen_jet_pt];
                    auto gen_jet_dr = std::sqrt(dr2(gen_jet_eta, (*p->WTAgeneta)[id], gen_jet_phi, (*p->WTAgenphi)[id]));

                    for (int64_t k = 0; k < ihf->size(); ++k) { 
                        (*g)[k]->Fill(mg->index_for(v{gen_jet_dr, gen_jet_pt}), weights[k] * jet_cor);
                        (*g_r)[k]->Fill(mg->index_for(v{gen_jet_dr, gen_jet_pt}), weights[k] * jet_cor);
                        (*r)[k]->Fill(mr->index_for(v{reco_jet_dr, reco_jet_pt}), weights[k] * jet_cor);
                        (*c)[k]->Fill(mr->index_for(v{reco_jet_dr, reco_jet_pt}), mg->index_for(v{gen_jet_dr, gen_jet_pt}), weights[k] * jet_cor);
                    }
                    
                    (*g_merge)[0]->Fill(mg->index_for(v{gen_jet_dr, gen_jet_pt}), weights_merge * jet_cor);
                    (*g_r_merge)[0]->Fill(mg->index_for(v{gen_jet_dr, gen_jet_pt}), weights_merge * jet_cor);
                    (*r_merge)[0]->Fill(mr->index_for(v{reco_jet_dr, reco_jet_pt}), weights_merge * jet_cor);
                    (*c_merge)[0]->Fill(mr->index_for(v{reco_jet_dr, reco_jet_pt}), mg->index_for(v{gen_jet_dr, gen_jet_pt}), weights_merge * jet_cor);
                    
                    // if both gen and reco jet are within bounds, fill the collapsed response matrices
                    if (mg->index_for(v{gen_jet_dr, gen_jet_pt}) > -1 && mr->index_for(v{reco_jet_dr, reco_jet_pt}) > -1) {
                        if (mg->index_for(v{gen_jet_dr, gen_jet_pt}) <= mg->size() && mr->index_for(v{reco_jet_dr, reco_jet_pt}) <= mr->size()) {
                            for (int64_t k = 0; k < ihf->size(); ++k) { 
                                (*cdr)[k]->Fill(reco_jet_dr, gen_jet_dr, weights[k] * jet_cor);
                                (*cpt)[k]->Fill(reco_jet_pt, gen_jet_pt, weights[k] * jet_cor);
                            }

                            (*cdr_merge)[0]->Fill(reco_jet_dr, gen_jet_dr, weights_merge * jet_cor);
                            (*cdr_merge)[0]->Fill(reco_jet_pt, gen_jet_pt, weights_merge * jet_cor);
                        }
                    }
                }
               
                // go through gen jets and fill histograms with misses
                for (int64_t j = 0; j < p->ngen; ++j) {
                    // skip jets that were matched with a reco photon
                    bool hit = false;
                   
                    for (auto const& index : exclude) {
                        if (j == index) {
                            hit = true; break; 
                        }
                    }

                    if (hit) continue;

                    // look at remaining jets
                    auto gen_jet_pt = (*p->genpt)[j];
                    auto gen_jet_eta = (*p->geneta)[j];
                    auto gen_jet_phi = (*p->genphi)[j];

                    if (std::abs(gen_jet_eta) >= jet_eta_abs) { continue; }
                    if (heavyion && in_jet_failure_region(gen_jet_eta, gen_jet_phi)) { continue; }
                    if (!back_to_back(gen_photon_phi, gen_jet_phi, dphi_min_numerator/dphi_min_denominator)) { continue; }
                    
                    auto id = gen_jet_id[gen_jet_pt];
                    auto gen_jet_dr = std::sqrt(dr2(gen_jet_eta, (*p->WTAgeneta)[id], gen_jet_phi, (*p->WTAgenphi)[id]));
                    auto jet_cor = acceptance_weight(heavyion, idphi, total, acceptance, gen_photon_phi, gen_jet_phi, gen_photon_eta, gen_jet_eta);
                    
                    // fill histograms
                    for (int64_t k = 0; k < ihf->size(); ++k) { 
                        (*g)[k]->Fill(mg->index_for(v{gen_jet_dr, gen_jet_pt}), weights[k] * jet_cor); 
                    }

                    (*g_merge)[0]->Fill(mg->index_for(v{gen_jet_dr, gen_jet_pt}), weights_merge * jet_cor);
                }
            }
        }

        f->Close();
        delete f; delete p;
    }

    r->divide(*r_n);
    g->divide(*g_n);
    g_r->divide(*g_r_n);
    // c->divide(*c_n);

    r_merge->divide(*r_n_merge);
    g_merge->divide(*g_n_merge);
    g_r_merge->divide(*g_r_n_merge);
    // c_merge->divide(*c_n_merge);
   
    /* save output */
    in(output, [&]() {
        r->save(tag);
        g->save(tag);
        g_r->save(tag);
        c->save(tag);

        r_n->save(tag);
        g_n->save(tag);
        g_r_n->save(tag);
        c_n->save(tag);

        cdr->save(tag);
        cpt->save(tag);

        r_merge->save(tag);
        g_merge->save(tag);
        g_r_merge->save(tag);
        c_merge->save(tag);

        r_n_merge->save(tag);
        g_n_merge->save(tag);
        g_r_n_merge->save(tag);
        c_n_merge->save(tag);

        cdr_merge->save(tag);
        cpt_merge->save(tag);

        ppt->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return vacillate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}