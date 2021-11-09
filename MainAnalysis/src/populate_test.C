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

#include <memory>
#include <string>
#include <vector>

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

template <typename... T>
void scale_ia_bin_width(T*... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        for (int64_t i = 1; i <= obj->GetNbinsX(); ++i) {
            auto width = revert_radian(obj->GetBinWidth(i));
            obj->SetBinContent(i, obj->GetBinContent(i) / width);
        }
    }), 0)... };
}

template <typename... T>
void title(std::function<void(TH1*)> f, T*... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

void fill_axes(pjtree* pjt, int64_t pthf_x, multival* mr,
               double photon_pt, float photon_eta, int64_t photon_phi,
               memory<TH1F>* nevt,
               memory<TH1F>* pjet_es_f_dphi,
               memory<TH1F>* pjet_wta_f_dphi,
               memory<TH1F>* pjet_f_x,
               memory<TH1F>* pjet_f_ddr,
               memory<TH1F>* pjet_f_jpt,
               memory<TH1F>* pjet_f_r) {
    (*nevt)[pthf_x]->Fill(1., pjt->w);

    for (int64_t j = 0; j < pjt->nref; ++j) {
        auto jet_pt = (*pjt->jtpt)[j];
        if (jet_pt <= 30) { continue; }

        auto jet_eta = (*pjt->jteta)[j];
        if (std::abs(jet_eta) >= 1.6) { continue; }

        auto jet_phi = convert_radian((*pjt->jtphi)[j]);

        auto pj_deta = photon_eta - jet_eta;
        auto pj_dphi = revert_radian(photon_phi - jet_phi);
        auto pj_dr = pj_deta * pj_deta + pj_dphi * pj_dphi;

        if (pj_dr < 0.16) { continue; }

        auto jet_wta_eta = (*pjt->WTAeta)[j];
        auto jet_wta_phi = convert_radian((*pjt->WTAphi)[j]);

        auto photon_jet_dphi = std::abs(photon_phi - jet_phi);
        auto photon_wta_dphi = std::abs(photon_phi - jet_wta_phi);

        (*pjet_es_f_dphi)[pthf_x]->Fill(photon_jet_dphi, pjt->w);
        (*pjet_wta_f_dphi)[pthf_x]->Fill(photon_wta_dphi, pjt->w);

        /* require back-to-back jets */
        if (photon_jet_dphi < 0.875_pi) { continue; }

        (*pjet_f_jpt)[pthf_x]->Fill(jet_pt, pjt->w);
        (*pjet_f_x)[pthf_x]->Fill(jet_pt / photon_pt, pjt->w);

        double jt_deta = jet_eta - jet_wta_eta;
        double jt_dphi = revert_radian(jet_phi - jet_wta_phi);
        double jt_dr = std::sqrt(jt_deta * jt_deta + jt_dphi * jt_dphi);

        (*pjet_f_ddr)[pthf_x]->Fill(jt_dr, pjt->w);

        if (jet_pt >= 200) { continue; }

        (*pjet_f_r)[pthf_x]->Fill(mr->index_for(v{jt_dr, jet_pt}), pjt->w);
    }
}

int populate(char const* config) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto frequency = conf->get<int64_t>("frequency");
    auto tag = conf->get<std::string>("tag");

    /* options */
    auto gen_iso = conf->get<bool>("generator_isolation");

    /* selections */
    auto const photon_pt_min = conf->get<float>("photon_pt_min");
    auto const photon_eta_abs = conf->get<float>("photon_eta_abs");
    auto const hovere_max = conf->get<float>("hovere_max");

    auto rjpt = conf->get<std::vector<float>>("jpt_range");
    auto rdphi = conf->get<std::vector<float>>("dphi_range");
    auto rx = conf->get<std::vector<float>>("x_range");
    auto rdr = conf->get<std::vector<float>>("dr_range");

    auto rdrr = conf->get<std::vector<float>>("drr_range");
    auto rptr = conf->get<std::vector<float>>("ptr_range");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");

    /* convert to integral angle units (cast to double) */
    convert_in_place_pi(rdphi);

    /* exclude most peripheral events */
    auto hf_min = dhf.front();

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    TFile* f = new TFile(input.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto pjt = new pjtree(gen_iso, false, t, { 1, 1, 1, 1, 1, 0 });

    printf("iterate..\n");

    int64_t bad_count = 0;
    int64_t all_count = 0;

    int64_t nentries = static_cast<int64_t>(t->GetEntries());
    for (int64_t i = 0; i < nentries; ++i) {
        if (i % frequency == 0) { printf("entry: %li/%li\n", i, nentries); }

        t->GetEntry(i);

        if (pjt->hiHF <= hf_min) { continue; }
        if (pjt->hiHF >= 5199.95) { continue; }

        int64_t leading = -1;
        float leadingEt = 0;
        for (int64_t j = 0; j < pjt->nPho; ++j) {
            if ((*pjt->phoEt)[j] <= photon_pt_min) { continue; }
            if (std::abs((*pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
            if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }
            if ((*pjt->phoEt)[j] > leadingEt) {
                if (leadingEt != 0) { bad_count++}
                leading = j;
                leadingEt = (*pjt->phoEt)[j];
            }
        }
        std::cout << std::endl;
        
        // /* require leading photon */
        if (leading < 0) { continue; }
        all_count++;

        // if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > see_max
        //         || (*pjt->phoSigmaIEtaIEta_2012)[leading] < see_min)
        //     continue;

        // /* hem failure region exclusion */
        // if (heavyion && within_hem_failure_region(pjt, leading)) { continue; }

        // /* isolation requirement */
        // if (gen_iso) {
        //     auto gen_index = (*pjt->pho_genMatchedIndex)[leading];
        //     if (gen_index == -1) { continue; }

        //     float isolation = (*pjt->mcCalIsoDR04)[gen_index];
        //     if (isolation > iso_max) { continue; }
        // } else {
        //     float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
        //         + (*pjt->pho_hcalRechitIsoR3)[leading]
        //         + (*pjt->pho_trackIsoR3PtCut20)[leading];
        //     if (isolation > iso_max) { continue; }
        // }

        // /* leading photon axis */
        // auto photon_eta = (*pjt->phoEta)[leading];
        // auto photon_phi = convert_radian((*pjt->phoPhi)[leading]);

        // /* electron rejection */
        // if (ele_rej) {
        //     bool electron = false;
        //     for (int64_t j = 0; j < pjt->nEle; ++j) {
        //         if (std::abs((*pjt->eleSCEta)[j]) > 1.4442) { continue; }

        //         auto deta = photon_eta - (*pjt->eleEta)[j];
        //         if (deta > 0.1) { continue; }

        //         auto ele_phi = convert_radian((*pjt->elePhi)[j]);
        //         auto dphi = revert_radian(photon_phi - ele_phi);
        //         auto dr2 = deta * deta + dphi * dphi;

        //         if (dr2 < 0.01 && passes_electron_id<
        //                     det::barrel, wp::loose, pjtree
        //                 >(pjt, j, heavyion)) {
        //             electron = true; break; }
        //     }

        //     if (electron) { continue; }
        // }

        // double photon_pt = (*pjt->phoEt)[leading];
        // auto pt_x = ipt->index_for(photon_pt);

        // double hf = pjt->hiHF;
        // auto hf_x = ihf->index_for(hf);

        // auto pthf_x = mpthf->index_for(x{pt_x, hf_x});

        // fill_axes(pjt, pthf_x, mr,
        //           photon_pt, photon_eta, photon_phi,
        //           nevt, pjet_es_f_dphi, pjet_wta_f_dphi,
        //           pjet_f_x, pjet_f_ddr, pjet_f_jpt,
        //           pjet_f_r);

        // /* mixing events in minimum bias */
        // for (int64_t k = 0; k < mix; m = (m + 1) % mentries) {
        //     tm->GetEntry(m);

        //     /* hf within +/- 10% */
        //     if (std::abs(pjtm->hiHF / pjt->hiHF - 1.) > 0.1) { continue; }

        //     fill_axes(pjtm, pthf_x, mr,
        //               photon_pt, photon_eta, photon_phi,
        //               nmix, mix_pjet_es_f_dphi, mix_pjet_wta_f_dphi,
        //               mix_pjet_f_x, mix_pjet_f_ddr, mix_pjet_f_jpt,
        //               mix_pjet_f_r);

        //     ++k;
        // }
        //
    }

    // /* normalise histograms */
    // if (mix > 0)
    //     scale(1. / mix,
    //         mix_pjet_es_f_dphi,
    //         mix_pjet_wta_f_dphi,
    //         mix_pjet_f_ddr,
    //         mix_pjet_f_x,
    //         mix_pjet_f_jpt,
    //         mix_pjet_f_r);

    // /* scale by bin width */
    // scale_bin_width(
    //     pjet_f_x,
    //     pjet_f_ddr,
    //     pjet_f_jpt,
    //     mix_pjet_f_x,
    //     mix_pjet_f_ddr,
    //     mix_pjet_f_jpt);

    // scale_ia_bin_width(
    //     pjet_es_f_dphi,
    //     pjet_wta_f_dphi,
    //     mix_pjet_es_f_dphi,
    //     mix_pjet_wta_f_dphi);

    // /* normalise by number of photons (events) */
    // pjet_es_f_dphi->divide(*nevt);
    // pjet_wta_f_dphi->divide(*nevt);
    // pjet_f_x->divide(*nevt);
    // pjet_f_ddr->divide(*nevt);
    // pjet_f_jpt->divide(*nevt);
    // pjet_f_r->divide(*nevt);

    // mix_pjet_es_f_dphi->divide(*nevt);
    // mix_pjet_wta_f_dphi->divide(*nevt);
    // mix_pjet_f_x->divide(*nevt);
    // mix_pjet_f_ddr->divide(*nevt);
    // mix_pjet_f_jpt->divide(*nevt);
    // mix_pjet_f_r->divide(*nevt);

    // /* subtract histograms */
    // auto sub_pjet_es_f_dphi = new memory<TH1F>(*pjet_es_f_dphi, "sub");
    // auto sub_pjet_wta_f_dphi = new memory<TH1F>(*pjet_wta_f_dphi, "sub");
    // auto sub_pjet_f_x = new memory<TH1F>(*pjet_f_x, "sub");
    // auto sub_pjet_f_ddr = new memory<TH1F>(*pjet_f_ddr, "sub");
    // auto sub_pjet_f_jpt = new memory<TH1F>(*pjet_f_jpt, "sub");
    // auto sub_pjet_f_r = new memory<TH1F>(*pjet_f_r, "sub");

    // *sub_pjet_es_f_dphi -= *mix_pjet_es_f_dphi;
    // *sub_pjet_wta_f_dphi -= *mix_pjet_wta_f_dphi;
    // *sub_pjet_f_x -= *mix_pjet_f_x;
    // *sub_pjet_f_ddr -= *mix_pjet_f_ddr;
    // *sub_pjet_f_jpt -= *mix_pjet_f_jpt;
    // *sub_pjet_f_r -= *mix_pjet_f_r;

    // /* save histograms */
    // in(output, [&]() {
    //     nevt->save(tag);
    //     nmix->save(tag);

    //     pjet_es_f_dphi->save(tag);
    //     pjet_wta_f_dphi->save(tag);
    //     pjet_f_x->save(tag);
    //     pjet_f_ddr->save(tag);
    //     pjet_f_jpt->save(tag);
    //     pjet_f_r->save(tag);

    //     mix_pjet_es_f_dphi->save(tag);
    //     mix_pjet_wta_f_dphi->save(tag);
    //     mix_pjet_f_x->save(tag);
    //     mix_pjet_f_ddr->save(tag);
    //     mix_pjet_f_jpt->save(tag);
    //     mix_pjet_f_r->save(tag);

    //     sub_pjet_es_f_dphi->save(tag);
    //     sub_pjet_wta_f_dphi->save(tag);
    //     sub_pjet_f_x->save(tag);
    //     sub_pjet_f_ddr->save(tag);
    //     sub_pjet_f_jpt->save(tag);
    //     sub_pjet_f_r->save(tag);
    // });

    // printf("destroying objects..\n");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 2)
        return populate(argv[1]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
