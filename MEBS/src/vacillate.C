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
#include "TMath.h"

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

float back_to_back(float photon_phi, float jet_phi, float threshold) {
    float dphi = std::abs(photon_phi - jet_phi);
    if (dphi > TMath::Pi()) dphi = std::abs(dphi - 2*TMath::Pi());

    return dphi > threshold * TMath::Pi();
}

int vacillate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::vector<std::string>>("input");
    auto tag = conf->get<std::string>("tag");

    auto dhf = conf->get<std::vector<float>>("hf_diff");

    /* selections */
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto heavyion = sel->get<bool>("heavyion");

    auto const photon_pt_min = sel->get<float>("photon_pt_min");
    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");
    auto const hovere_max = sel->get<float>("hovere_max");
    auto const see_min = sel->get<float>("see_min");
    auto const see_max = sel->get<float>("see_max");
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

    // jpt scan
    auto jpt_nevt = new memory<TH1F>("reco_jpt_nevt"s, "", fincl, mpthf);
    auto jpt_pjet_f_dr = new memory<TH1F>("reco_jpt_pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto jpt_pjet_f_jpt = new memory<TH1F>("reco_jpt_pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto jpt_pjet_f_dphi = new memory<TH1F>("reco_jpt_pjet_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi", fdphi, mpthf);
    auto jpt_pjet_f_dr_jpt = new memory<TH1F>("reco_jpt_pjet_f_dr_jpt"s, 
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthfjpt);

    // eta scan
    auto eta_nevt = new memory<TH1F>("reco_eta_nevt"s, "", fincl, mpthf);
    auto eta_pjet_f_dr = new memory<TH1F>("reco_eta_pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto eta_pjet_f_jpt = new memory<TH1F>("reco_eta_pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto eta_pjet_f_dphi = new memory<TH1F>("reco_eta_pjet_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi", fdphi, mpthf);
    auto eta_pjet_f_dr_eta = new memory<TH1F>("reco_eta_pjet_f_dr_eta"s, 
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthfeta);

    // dphi scan
    auto dphi_nevt = new memory<TH1F>("reco_dphi_nevt"s, "", fincl, mpthf);
    auto dphi_pjet_f_dr = new memory<TH1F>("reco_dphi_pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto dphi_pjet_f_jpt = new memory<TH1F>("reco_dphi_pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto dphi_pjet_f_dphi = new memory<TH1F>("reco_dphi_pjet_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi", fdphi, mpthf);
    auto dphi_pjet_f_dr_dphi = new memory<TH1F>("reco_dphi_pjet_f_dr_dphi"s, 
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthfdphi);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    for (auto const& file : input) {
        std::cout << file << std::endl;
        
        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(true, false, heavyion, t, { 1, 1, 1, 1, 1, 0, heavyion, 0, !heavyion });
        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        /* fill histograms */
        for (int64_t i = 0; i < nentries; ++i) {
            if (i % 100000 == 0) { printf("entry: %li/%li\n", i, nentries); }

            t->GetEntry(i);

            double hf = pjt->hiHF; 

            if (hf <= dhf.front() || hf >= dhf.back()) { continue; }
            if (std::abs(pjt->vz) > 15) { continue; }

            /* look for reco photons */
            int64_t photon_index = -1;
            float photon_pt = 0;

            for (int64_t j = 0; j < pjt->nPho; ++j) {
                if (std::abs((*pjt->phoEta)[j]) >= photon_eta_abs) { continue; }
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

            auto pthf_x = mpthf->index_for(x{pt_x, hf_x});
            (*jpt_nevt)[pthf_x]->Fill(1., weight);
            (*eta_nevt)[pthf_x]->Fill(1., weight);
            (*dphi_nevt)[pthf_x]->Fill(1., weight);

            // go through reco jets and fill histograms for hits and fakes
            for (int64_t j = 0; j < pjt->nref; ++j) {
                auto jet_pt = (*pjt->jtpt)[j];
                auto jet_eta = (*pjt->jteta)[j];
                auto jet_phi = (*pjt->jtphi)[j];
                
                auto gen_jet_pt = (*pjt->refpt)[j];
                
                if (std::abs(jet_eta) >= jet_eta_abs) { continue; }
                if (heavyion && in_jet_failure_region(pjt, j)) { continue; }

                // no matching gen jet => fake, already accounted for by mixed-event background subtraction
                if (gen_jet_pt <= 5 || dr2((*pjt->refeta)[j], (*pjt->jteta)[j], (*pjt->refphi)[j], (*pjt->jtphi)[j]) > 0.0225) { 
                    continue;
                }

                auto jet_dr = std::sqrt(dr2(jet_eta, (*pjt->WTAeta)[j], jet_phi, (*pjt->WTAphi)[j]));
                auto photon_jet_dphi = std::sqrt(dr2(0, 0, jet_phi, photon_phi)) / TMath::Pi();
                auto photon_jet_dr = std::sqrt(dr2(jet_eta, photon_eta, jet_phi, photon_phi));

                if (photon_jet_dr < 0.4) { continue; }

                // get the indices
                auto jpt_x =  ijpt->index_for(jet_pt);
                auto phi_x = idphi->index_for(photon_jet_dphi);
                auto eta_x = ieta->index_for(jet_eta);

                auto mpthfjpt_x = mpthfjpt->index_for(x{pt_x, jpt_x, hf_x});
                auto mpthfdphi_x = mpthfdphi->index_for(x{pt_x, phi_x, hf_x});
                auto mpthfeta_x = mpthfeta->index_for(x{pt_x, eta_x, hf_x});

                // fill the dphi scan comparison histograms
                if (mpthfdphi_x > -1 && mpthfdphi_x < mpthfdphi->size() && jet_dr < jet_dr_max) {
                    (*dphi_pjet_f_jpt)[pthf_x]->Fill(jet_pt, weight);

                    if (jet_pt > jet_pt_min && jet_pt < jet_pt_max) {
                        (*dphi_pjet_f_dr_dphi)[mpthfdphi_x]->Fill(jet_dr, weight);
                        (*dphi_pjet_f_dr)[pthf_x]->Fill(jet_dr, weight);
                        (*dphi_pjet_f_dphi)[pthf_x]->Fill(photon_jet_dphi, weight);
                    }
                }

                if (!back_to_back(photon_phi, jet_phi, dphi_min_numerator/dphi_min_denominator)) { continue; }

                // fill the other histograms
                if (mpthfeta_x > -1 && mpthfeta_x < mpthfeta->size() && jet_dr < jet_dr_max) {
                    (*eta_pjet_f_jpt)[pthf_x]->Fill(jet_pt, weight);

                    if (jet_pt > jet_pt_min && jet_pt < jet_pt_max) {
                        (*eta_pjet_f_dr_eta)[mpthfeta_x]->Fill(jet_dr, weight);
                        (*eta_pjet_f_dr)[pthf_x]->Fill(jet_dr, weight);
                        (*eta_pjet_f_dphi)[pthf_x]->Fill(photon_jet_dphi, weight);
                    }
                }

                if (mpthfjpt_x > -1 && mpthfjpt_x < mpthfjpt->size() && jet_dr < jet_dr_max) {
                    (*jpt_pjet_f_jpt)[pthf_x]->Fill(jet_pt, weight);
                    (*jpt_pjet_f_dr_jpt)[mpthfjpt_x]->Fill(jet_dr, weight);

                    if (jet_pt > jet_pt_min && jet_pt < jet_pt_max) {
                        (*jpt_pjet_f_dr)[pthf_x]->Fill(jet_dr, weight);
                        (*jpt_pjet_f_dphi)[pthf_x]->Fill(photon_jet_dphi, weight);
                    }
                }
            }
        }
    }
   
    /* save output */
    in(output, [&]() {
        jpt_nevt->save(tag);
        jpt_pjet_f_dr->save(tag);
        jpt_pjet_f_jpt->save(tag);
        jpt_pjet_f_dphi->save(tag);
        jpt_pjet_f_dr_jpt->save(tag);

        eta_nevt->save(tag);
        eta_pjet_f_dr->save(tag);
        eta_pjet_f_jpt->save(tag);
        eta_pjet_f_dphi->save(tag);
        eta_pjet_f_dr_eta->save(tag);

        dphi_nevt->save(tag);
        dphi_pjet_f_dr->save(tag);
        dphi_pjet_f_jpt->save(tag);
        dphi_pjet_f_dphi->save(tag);
        dphi_pjet_f_dr_dphi->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return vacillate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}