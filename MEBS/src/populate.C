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

static float dr2(float eta1, float eta2, float phi1, float phi2) {
    auto deta = eta1 - eta2;
    auto dphi = revert_radian(convert_radian(phi1) - convert_radian(phi2));

    return deta * deta + dphi * dphi;
}

float back_to_back(float photon_phi, float jet_phi, float threshold) {
    return std::abs(convert_radian(photon_phi) - convert_radian(jet_phi)) > convert_pi(threshold);
}

void fill_axes(pjtree* pjt, multival* mpthf, multival* mpthfjpt, interval *ijpt,
               int64_t pt_x, int64_t hf_x, float weight, int64_t photon_phi, bool exclude, 
               float jet_eta_abs, float jet_dr_max, float dphi_min_numerator, float dphi_min_denominator,
               memory<TH1F>* nevt, memory<TH1F>* pjet_f_dr, memory<TH1F>* pjet_f_jpt,
               memory<TH1F>* pjet_f_dphi, memory<TH1F>* pjet_f_dr_jpt
            ) {
    
    auto pthf_x = mpthf->index_for(x{pt_x, hf_x});
    
    (*nevt)[pthf_x]->Fill(1., weight);

    for (int64_t j = 0; j < pjt->nref; ++j) {
        auto jet_pt = (*pjt->jtpt)[j];
        auto jet_eta = (*pjt->jteta)[j];
        auto jet_phi = (*pjt->jtphi)[j];

        if (std::abs(jet_eta) >= jet_eta_abs) { continue; }
        if (exclude && in_jet_failure_region(pjt, j)) { continue; }
        if (!back_to_back(photon_phi, jet_phi, dphi_min_numerator/dphi_min_denominator)) { continue; }

        auto jet_dr = std::sqrt(dr2(jet_eta, (*pjt->WTAeta)[j], jet_phi, (*pjt->WTAphi)[j]));
        auto photon_jet_dphi = std::sqrt(dr2(0, 0, jet_phi, photon_phi)) / TMath::Pi();

        auto jpt_x =  ijpt->index_for(jet_pt);
        auto mpthfjpt_x = mpthfjpt->index_for(x{pt_x, jpt_x, hf_x});

        if (mpthfjpt_x > -1 && mpthfjpt_x < mpthfjpt->size() && jet_dr < jet_dr_max) {
            (*pjet_f_dr_jpt)[mpthfjpt_x]->Fill(jet_dr, weight);
            (*pjet_f_jpt)[pthf_x]->Fill(jet_pt, weight);
            (*pjet_f_dr)[pthf_x]->Fill(jet_dr, weight);
            (*pjet_f_dphi)[pthf_x]->Fill(photon_jet_dphi, weight);
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
    auto exclude = conf->get<bool>("exclude");

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
    auto const jet_dr_max = sel->get<float>("jet_dr_max");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto rjpt = sel->get<std::vector<float>>("jpt_range");
    auto rdr = sel->get<std::vector<float>>("dr_range");
    auto rdphi = sel->get<std::vector<float>>("dphi_range");

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    auto alter_base = conf->get<std::string>("alter_base"); // get offset to paths from the original configuration file

    /* fix sigma eta eta range for background distributions */
    if (tag == "bkg") see_min = 0.012;
    if (tag == "bkg") see_max = 0.02;

    /* make histograms */
    auto ipt = new interval(dpt);
    auto ihf = new interval(dhf);
    auto idr = new interval("#deltaj"s, rdr);
    auto ijpt = new interval("p_{T}^{j}"s, rjpt);
    auto idphi = new interval("#Delta#phi"s, rdphi);
    auto incl = new interval(""s, 1, 0.f, 9999.f);

    auto mpthf = new multival(dpt, dhf);
    auto mpthfjpt = new multival(dpt, rjpt, dhf);

    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto fdr = std::bind(&interval::book<TH1F>, idr, _1, _2, _3);
    auto fjpt = std::bind(&interval::book<TH1F>, ijpt, _1, _2, _3);
    auto fdphi = std::bind(&interval::book<TH1F>, idphi, _1, _2, _3);

    auto nevt = new memory<TH1F>("nevt"s, "", fincl, mpthf);
    auto nmix = new memory<TH1F>("nmix"s, "", fincl, mpthf);

    auto pjet_f_dr = new memory<TH1F>("pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto pjet_f_jpt = new memory<TH1F>("pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto pjet_f_dphi = new memory<TH1F>("pjet_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi", fdphi, mpthf);
    auto pjet_f_dr_jpt = new memory<TH1F>("pjet_f_dr_jpt"s, 
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthfjpt);

    auto mix_pjet_f_dr = new memory<TH1F>("mix_pjet_f_dr",
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthf);
    auto mix_pjet_f_jpt = new memory<TH1F>("mix_pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mpthf);
    auto mix_pjet_f_dphi = new memory<TH1F>("mix_pjet_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi", fdphi, mpthf);
    auto mix_pjet_f_dr_jpt = new memory<TH1F>("mix_pjet_f_dr_jpt"s, 
        "1/N^{#gamma} dN/d#deltaj", fdr, mpthfjpt);

    /* random number for smearing and mb selection */
    auto rng = new TRandom3(144);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    int index_m = rng->Integer(mb.size());
    TFile* fm = new TFile(mb[index_m].data(), "read");
    TTree* tm = (TTree*)fm->Get("pj");
    auto pjtm = new pjtree(false, false, heavyion, tm, { 1, 1, 1, 1, 1, 0, heavyion, 0, 0 });
    int64_t mentries = static_cast<int64_t>(tm->GetEntries()); std::cout << mentries << std::endl;

    printf("iterate..\n");

    /* load input */
    for (auto const& file : input) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(false, false, heavyion, t, { 1, 1, 1, 1, 1, 0, heavyion, 0, !heavyion });
        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        for (int64_t i = 0, m = 0; i < nentries; ++i) {
            if (i % frequency == 0) { printf("entry: %li/%li\n", i, nentries); }

            t->GetEntry(i);

            double hf = pjt->hiHF; 

            if (hf <= dhf.front() || hf >= dhf.back()) { continue; }
            if (std::abs(pjt->vz) > 15) { continue; }

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
            if (exclude && in_pho_failure_region(pjt, photon_index)) { continue; }

            /* isolation requirement */
            float isolation = (*pjt->pho_ecalClusterIsoR3)[photon_index] + (*pjt->pho_hcalRechitIsoR3)[photon_index] + (*pjt->pho_trackIsoR3PtCut20)[photon_index];
            if (isolation > iso_max) { continue; }

            /* leading photon axis */
            auto photon_eta = (*pjt->phoEta)[photon_index];
            auto photon_phi = convert_radian((*pjt->phoPhi)[photon_index]);

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

            auto pt_x = ipt->index_for(photon_pt);
            auto hf_x = ihf->index_for(hf);
            auto weight = pjt->w;

            /* fill histograms */
            fill_axes(pjt, mpthf, mpthfjpt, ijpt, pt_x, hf_x, weight,
                photon_phi, exclude, jet_eta_abs, jet_dr_max, 
                dphi_min_numerator, dphi_min_denominator,
                nevt, pjet_f_dr, pjet_f_jpt, pjet_f_dphi, pjet_f_dr_jpt);

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
                        pjtm = new pjtree(false, false, heavyion, tm, { 1, 1, 1, 1, 1, 0, heavyion, 0, 0 });

                        mentries = static_cast<int64_t>(tm->GetEntries()); std::cout << mentries << std::endl;
                    }

                    tm->GetEntry(m);

                    if (std::abs(pjtm->hiHF / hf - 1.) > 0.1) { continue; }

                    fill_axes(pjtm, mpthf, mpthfjpt, ijpt, pt_x, hf_x, weight,
                        photon_phi, exclude, jet_eta_abs, jet_dr_max, 
                        dphi_min_numerator, dphi_min_denominator,
                        nmix, mix_pjet_f_dr, mix_pjet_f_jpt, mix_pjet_f_dphi, mix_pjet_f_dr_jpt);

                    ++k;
                }
            }
        }

        f->Close();
    }

    /* scale histograms */
    if (mix > 0) {
        scale(1. / mix, mix_pjet_f_dr, mix_pjet_f_jpt, mix_pjet_f_dphi, mix_pjet_f_dr_jpt);
    }

    /* subtract histograms */
    auto sub_pjet_f_dr = new memory<TH1F>(*pjet_f_dr, "sub");
    auto sub_pjet_f_jpt = new memory<TH1F>(*pjet_f_jpt, "sub");
    auto sub_pjet_f_dphi = new memory<TH1F>(*pjet_f_dphi, "sub");
    auto sub_pjet_f_dr_jpt = new memory<TH1F>(*pjet_f_dr_jpt, "sub");

    *sub_pjet_f_dr -= *mix_pjet_f_dr;
    *sub_pjet_f_jpt -= *mix_pjet_f_jpt;
    *sub_pjet_f_dphi -= *mix_pjet_f_dphi;
    *sub_pjet_f_dr_jpt -= *mix_pjet_f_dr_jpt;

    /* save histograms */
    in(output, [&]() {
        nevt->save(tag);
        nmix->save(tag);

        pjet_f_dr->save(tag);
        pjet_f_jpt->save(tag);
        pjet_f_dphi->save(tag);
        pjet_f_dr_jpt->save(tag);

        mix_pjet_f_dr->save(tag);
        mix_pjet_f_jpt->save(tag);
        mix_pjet_f_dphi->save(tag);
        mix_pjet_f_dr_jpt->save(tag);

        sub_pjet_f_dr->save(tag);
        sub_pjet_f_jpt->save(tag);
        sub_pjet_f_dphi->save(tag);
        sub_pjet_f_dr_jpt->save(tag);
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
