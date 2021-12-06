#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

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

template <typename... T>
void scale_ia_bin_width(T*... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        for (int64_t i = 1; i <= obj->GetNbinsX(); ++i) {
            auto width = revert_radian(obj->GetBinWidth(i));
            obj->SetBinContent(i, obj->GetBinContent(i) / width);
        }
    }), 0)... };
}

static bool in_hem_failure_region(float eta, float phi) {
    return (eta < -1.242 && -1.72 < phi && phi < -0.72);
}

void fill_axes(pjtree* pjt, int64_t index_x, float weight, float iso, float jetpt_min,
               float photon_eta, int64_t photon_phi, bool heavyion,
               multival* mdphi, multival* mdr,
               memory<TH1F>* nevt,
               memory<TH1F>* pjet_es_f_dphi,
               memory<TH1F>* pjet_wta_f_dphi,
               memory<TH1F>* pjet_f_dr,
               memory<TH1F>* pjet_f_jpt,
               memory<TH1F>* pjet_es_u_dphi,
               memory<TH1F>* pjet_wta_u_dphi,
               memory<TH1F>* pjet_u_dr,
               memory<TH2F>* pjet_dphi_deta) {
    (*nevt)[index_x]->Fill(1., weight);

    for (int64_t j = 0; j < pjt->nref; ++j) {
        auto jet_pt = (*pjt->jtpt)[j];
        if (jet_pt <= jetpt_min) { continue; }

        auto jet_eta = (*pjt->jteta)[j];
        if (std::abs(jet_eta) >= 1.6) { continue; }

        auto jet_phi = convert_radian((*pjt->jtphi)[j]);

        auto pj_deta = photon_eta - jet_eta;
        auto pj_dphi = revert_radian(std::abs(photon_phi - jet_phi));
        auto pj_dr = std::sqrt(pj_deta * pj_deta + pj_dphi * pj_dphi);

        if (pj_dr < iso) { continue; }

        /* hem failure region exclusion */
        if (heavyion && in_hem_failure_region(jet_eta, revert_radian(jet_phi))) { continue; }

        (*pjet_dphi_deta)[index_x]->Fill(pj_dphi, pj_deta, weight);

        auto jet_wta_eta = (*pjt->WTAeta)[j];
        auto jet_wta_phi = convert_radian((*pjt->WTAphi)[j]);

        auto photon_jet_dphi = std::abs(photon_phi - jet_phi);
        auto photon_wta_dphi = std::abs(photon_phi - jet_wta_phi);

        (*pjet_es_f_dphi)[index_x]->Fill(photon_jet_dphi, weight);
        (*pjet_wta_f_dphi)[index_x]->Fill(photon_wta_dphi, weight);

        if (jet_pt < 200) (*pjet_es_u_dphi)[index_x]->Fill(
            mdphi->index_for(v{revert_pi(photon_jet_dphi), jet_pt}), weight); 
        if (jet_pt < 200) (*pjet_wta_u_dphi)[index_x]->Fill(
            mdphi->index_for(v{revert_pi(photon_wta_dphi), jet_pt}), weight);

        /* require back-to-back jets */
        if (photon_jet_dphi < 0.875_pi) { continue; }

        (*pjet_f_jpt)[index_x]->Fill(jet_pt, weight);

        double jt_deta = jet_eta - jet_wta_eta;
        double jt_dphi = revert_radian(jet_phi - jet_wta_phi);
        double jt_dr = std::sqrt(jt_deta * jt_deta + jt_dphi * jt_dphi);

        (*pjet_f_dr)[index_x]->Fill(jt_dr, weight);

        if (jet_pt < 200) (*pjet_u_dr)[index_x]->Fill(mdr->index_for(v{jt_dr, jet_pt}), weight);
    }
}

int populate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto mb = conf->get<std::string>("mb");
    auto entries = conf->get<int64_t>("entries");
    auto mix = conf->get<int64_t>("mix");
    auto frequency = conf->get<int64_t>("frequency");
    auto tag = conf->get<std::string>("tag");

    /* options */
    auto heavyion = conf->get<bool>("heavyion");
    auto gen_iso = conf->get<bool>("generator_isolation");
    auto ele_rej = conf->get<bool>("electron_rejection");
    auto use_hibin = conf->get<bool>("use_hibin");

    /* selections */
    auto const photon_pt_min = conf->get<float>("photon_pt_min");
    auto const photon_eta_abs = conf->get<float>("photon_eta_abs");
    auto const hovere_max = conf->get<float>("hovere_max");
    auto const see_min = conf->get<float>("see_min");
    auto const see_max = conf->get<float>("see_max");
    auto const iso_max = conf->get<float>("iso_max");

    auto rjpt = conf->get<std::vector<float>>("jpt_range");
    auto rdphi = conf->get<std::vector<float>>("dphi_range");
    auto rdr = conf->get<std::vector<float>>("dr_range");

    auto rrdr = conf->get<std::vector<float>>("rdr_range");
    auto rrdphi = conf->get<std::vector<float>>("rdphi_range");
    auto rrpt = conf->get<std::vector<float>>("rpt_range");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto diso = conf->get<std::vector<float>>("iso_diff");
    auto djtmin = conf->get<std::vector<float>>("jtmin_diff");

    /* convert to integral angle units (cast to double) */
    convert_in_place_pi(rdphi);

    /* push back vectors for the photon isolation cone and min jet pT */
    diso.push_back(999999);
    djtmin.push_back(999999);

    /* exclude most peripheral events */
    auto hf_min = dhf.front();

    auto ipt = new interval(dpt);
    auto ihf = new interval(dhf);
    auto iiso = new interval(diso);
    auto ijtmin = new interval(djtmin);

    auto mindex = new multival(dpt, dhf, diso, djtmin);

    auto incl = new interval(""s, 1, 0.f, 9999.f);
    auto idphi = new interval("#Delta#phi^{#gammaj}"s, rdphi);
    auto idr = new interval("#deltaj"s, rdr);
    auto ijpt = new interval("p_{T}^{j}"s, rjpt);

    auto mdr = new multival(rrdr, rrpt);
    auto mdphi = new multival(rrdphi, rrpt);

    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto fdphi = std::bind(&interval::book<TH1F>, idphi, _1, _2, _3);
    auto fdr = std::bind(&interval::book<TH1F>, idr, _1, _2, _3);
    auto fjpt = std::bind(&interval::book<TH1F>, ijpt, _1, _2, _3);

    auto fudr = [&](int64_t, std::string const& name, std::string const&) {
        return new TH1F(name.data(), ";index;", mdr->size(), 0, mdr->size()); };
    auto fudphi = [&](int64_t, std::string const& name, std::string const&) {
        return new TH1F(name.data(), ";index;", mdphi->size(), 0, mdphi->size()); };
    auto fdpde = [&](int64_t, std::string const& name, std::string const&) {
        return new TH2F(name.data(), ";dphi;deta", 60, 0, 3.14159, 60, -4, 4); };

    auto nevt = new memory<TH1F>("nevt"s, "", fincl, mindex);
    auto nmix = new memory<TH1F>("nmix"s, "", fincl, mindex);

    auto pjet_es_f_dphi = new memory<TH1F>("pjet_es_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi^{#gammaj}", fdphi, mindex);
    auto pjet_wta_f_dphi = new memory<TH1F>("pjet_wta_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi^{#gammaj}", fdphi, mindex);
    auto pjet_f_dr = new memory<TH1F>("pjet_f_dr"s,
        "1/N^{#gamma} dN/d#deltaj", fdr, mindex);
    auto pjet_f_jpt = new memory<TH1F>("pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mindex);

    auto mix_pjet_es_f_dphi = new memory<TH1F>("mix_pjet_es_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi^{#gammaj}", fdphi, mindex);
    auto mix_pjet_wta_f_dphi = new memory<TH1F>("mix_pjet_wta_f_dphi"s,
        "1/N^{#gamma} dN/d#Delta#phi^{#gammaj}", fdphi, mindex);
    auto mix_pjet_f_dr = new memory<TH1F>("mix_pjet_f_dr",
        "1/N^{#gamma} dN/d#deltaj", fdr, mindex);
    auto mix_pjet_f_jpt = new memory<TH1F>("mix_pjet_f_jpt"s,
        "1/N^{#gamma} dN/dp_{T}^{j}", fjpt, mindex);

    auto pjet_u_dr = new memory<TH1F>("pjet_u_dr"s, "", fudr, mindex);
    auto pjet_es_u_dphi = new memory<TH1F>("pjet_es_u_dphi"s, "", fudphi, mindex);
    auto pjet_wta_u_dphi = new memory<TH1F>("pjet_wta_u_dphi"s, "", fudphi, mindex);
    auto pjet_dphi_deta = new memory<TH2F>("pjet_dphi_deta"s, "", fdpde, mindex);

    auto mix_pjet_u_dr = new memory<TH1F>("mix_pjet_u_dr"s, "", fudr, mindex);
    auto mix_pjet_es_u_dphi = new memory<TH1F>("mix_pjet_es_u_dphi"s, "", fudphi, mindex);
    auto mix_pjet_wta_u_dphi = new memory<TH1F>("mix_pjet_wta_u_dphi"s, "", fudphi, mindex);
    auto mix_pjet_dphi_deta = new memory<TH2F>("mix_pjet_dphi_deta"s, "", fdpde, mindex);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    TFile* f = new TFile(input.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto pjt = new pjtree(gen_iso, false, t, { 1, 1, 1, 1, 1, 0 });

    TFile* fm = new TFile(mb.data(), "read");
    TTree* tm = (TTree*)fm->Get("pj");
    auto pjtm = new pjtree(gen_iso, false, tm, { 1, 1, 1, 1, 1, 0});

    printf("iterate..\n");

    int64_t nentries = static_cast<int64_t>(t->GetEntries());
    int64_t mod = 1;
    if (entries) { mod = nentries / entries; }
    if (mod !=1) { std::cout << "mod: " << mod << std::endl; }

    int64_t mentries = static_cast<int64_t>(tm->GetEntries());
    int64_t tentries = 0;
    clock_t time = clock();
    clock_t duration = 0;
    clock_t mebs_time = 0;
    clock_t mebs_duration = 0;
    int64_t tries = 0;

    for (int64_t i = 0, m = 0; i < nentries; ++i) {
        if (i % frequency == 0) { printf("entry: %li/%li\n", i, nentries); }
        if (i % frequency == 0) { 
            if (tentries != 0) {
                duration = clock() - time;
                std::cout << "Time per " << frequency/mod << " entries: " << (double)(duration)/CLOCKS_PER_SEC << " seconds" << std::endl;
                std::cout << "Time for loading each MEBS try: " << (double)(mebs_duration)/CLOCKS_PER_SEC/tries << " seconds" << std::endl;
                std::cout << "Entries: " << tentries << std::endl;
                std::cout << "Average tries: " << (double) tries / tentries << std::endl;
                mebs_duration = 0;
                tentries = 0;
                tries = 0;
                time = clock();
            }
        }

        if (i % mod != 0) { continue; }

        t->GetEntry(i);

        if (pjt->hiHF <= hf_min) { continue; }
        if (pjt->hiHF >= 5199.95) { continue; }

        int64_t leading = -1;
        float leading_pt = 0;
        for (int64_t j = 0; j < pjt->nPho; ++j) {
            if ((*pjt->phoEt)[j] <= photon_pt_min) { continue; }
            if (std::abs((*pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
            if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }
            if ((*pjt->phoEt)[j] > leading_pt) {
                leading = j;
                leading_pt = (*pjt->phoEt)[j];
            }
        }

        /* require leading photon */
        if (leading < 0) { continue; }

        if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > see_max
                || (*pjt->phoSigmaIEtaIEta_2012)[leading] < see_min)
            continue;

        /* hem failure region exclusion */
        if (heavyion && within_hem_failure_region(pjt, leading)) { continue; }

        /* isolation requirement */
        if (gen_iso) {
            auto gen_index = (*pjt->pho_genMatchedIndex)[leading];
            if (gen_index == -1) { continue; }

            float isolation = (*pjt->mcCalIsoDR04)[gen_index];
            if (isolation > iso_max) { continue; }
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

        double photon_pt = (*pjt->phoEt)[leading];
        auto pt_x = ipt->index_for(photon_pt);

        double hf = pjt->hiHF;
        auto hf_x = ihf->index_for(hf);

        auto weight = pjt->w;

        for (auto r : diso) {
            if (r == diso.back()) { continue; }
            auto iso_x = iiso->index_for(r);

            for (auto min : djtmin) {
                if (min == djtmin.back()) { continue; }

                auto jtmin_x = ijtmin->index_for(min);
                auto index_x = mindex->index_for(x{pt_x, hf_x, iso_x, jtmin_x});

                fill_axes(pjt, index_x, weight, r, min,
                        photon_eta, photon_phi, heavyion, mdphi, mdr, nevt,
                        pjet_es_f_dphi, pjet_wta_f_dphi,
                        pjet_f_dr, pjet_f_jpt,
                        pjet_es_u_dphi, pjet_wta_u_dphi, 
                        pjet_u_dr, pjet_dphi_deta);
            }
        }

        /* mixing events in minimum bias */
        mebs_time = clock();
        auto big_times = 0;
        for (int64_t k = 0; k < mix; m = (m + 1) % mentries) {
            tm->GetEntry(m);
            tries++;
            big_times++;

            if(m == 0) { std::cout << "looping " << mentries << std::endl;}

            if (use_hibin) {
                /* hibin within +/- 2 */
                if (std::abs(pjtm->hiBin - pjt->hiBin) > 2) { continue; }
            }
            else {
                /* hf within +/- 10% */
                if (std::abs(pjtm->hiHF / pjt->hiHF - 1.) > 0.1) { continue; }
            }

            if((double)(clock()-mebs_time)/CLOCKS_PER_SEC > 1) {
                std::cout << "here: " << big_times << " " << pjt->hiHF << std::endl;
            }

            big_times = 0;

            mebs_duration += clock() - mebs_time;

            for (auto r : diso) {
                if (r == diso.back()) { continue; }
                auto iso_x = iiso->index_for(r);

                for (auto min : djtmin) {
                    if (min == djtmin.back()) { continue; }
                    auto jtmin_x = ijtmin->index_for(min);
                    auto index_x = mindex->index_for(x{pt_x, hf_x, iso_x, jtmin_x});

                    fill_axes(pjtm, index_x, weight, r, min,
                            photon_eta, photon_phi, heavyion, mdphi, mdr, nmix,
                            mix_pjet_es_f_dphi, mix_pjet_wta_f_dphi,
                            mix_pjet_f_dr, mix_pjet_f_jpt,
                            mix_pjet_es_u_dphi, mix_pjet_wta_u_dphi,
                            mix_pjet_u_dr, mix_pjet_dphi_deta);
                }
            }

            ++k;
            mebs_time = clock();
        }

        tentries++;
    }

    /* normalise histograms */
    if (mix > 0)
        scale(1. / mix,
            mix_pjet_es_f_dphi,
            mix_pjet_wta_f_dphi,
            mix_pjet_f_dr,
            mix_pjet_f_jpt,
            mix_pjet_es_u_dphi,
            mix_pjet_wta_u_dphi,
            mix_pjet_u_dr, 
            mix_pjet_dphi_deta);

    /* scale by bin width */
    scale_bin_width(
        pjet_f_dr,
        pjet_f_jpt,
        mix_pjet_f_dr,
        mix_pjet_f_jpt);

    scale_ia_bin_width(
        pjet_es_f_dphi,
        pjet_wta_f_dphi,
        mix_pjet_es_f_dphi,
        mix_pjet_wta_f_dphi);

    /* normalise by number of photons (events) */
    pjet_es_f_dphi->divide(*nevt);
    pjet_wta_f_dphi->divide(*nevt);
    pjet_f_dr->divide(*nevt);
    pjet_f_jpt->divide(*nevt);
    pjet_es_u_dphi->divide(*nevt);
    pjet_wta_u_dphi->divide(*nevt);
    pjet_u_dr->divide(*nevt);

    mix_pjet_es_f_dphi->divide(*nevt);
    mix_pjet_wta_f_dphi->divide(*nevt);
    mix_pjet_f_dr->divide(*nevt);
    mix_pjet_f_jpt->divide(*nevt);
    mix_pjet_es_u_dphi->divide(*nevt);
    mix_pjet_wta_u_dphi->divide(*nevt);
    mix_pjet_u_dr->divide(*nevt);

    /* subtract histograms */
    auto sub_pjet_es_f_dphi = new memory<TH1F>(*pjet_es_f_dphi, "sub");
    auto sub_pjet_wta_f_dphi = new memory<TH1F>(*pjet_wta_f_dphi, "sub");
    auto sub_pjet_f_dr = new memory<TH1F>(*pjet_f_dr, "sub");
    auto sub_pjet_f_jpt = new memory<TH1F>(*pjet_f_jpt, "sub");
    auto sub_pjet_es_u_dphi = new memory<TH1F>(*pjet_es_u_dphi, "sub");
    auto sub_pjet_wta_u_dphi = new memory<TH1F>(*pjet_wta_u_dphi, "sub");
    auto sub_pjet_u_dr = new memory<TH1F>(*pjet_u_dr, "sub");
    auto sub_pjet_dphi_deta = new memory<TH2F>(*pjet_dphi_deta, "sub");

    *sub_pjet_es_f_dphi -= *mix_pjet_es_f_dphi;
    *sub_pjet_wta_f_dphi -= *mix_pjet_wta_f_dphi;
    *sub_pjet_f_dr -= *mix_pjet_f_dr;
    *sub_pjet_f_jpt -= *mix_pjet_f_jpt;
    *sub_pjet_es_u_dphi -= *mix_pjet_es_u_dphi;
    *sub_pjet_wta_u_dphi -= *mix_pjet_wta_u_dphi;
    *sub_pjet_u_dr -= *mix_pjet_u_dr;
    *sub_pjet_dphi_deta -= *mix_pjet_dphi_deta;

    /* save histograms */
    in(output, [&]() {
        nevt->save(tag);
        nmix->save(tag);

        pjet_es_f_dphi->save(tag);
        pjet_wta_f_dphi->save(tag);
        pjet_f_dr->save(tag);
        pjet_f_jpt->save(tag);
        pjet_es_u_dphi->save(tag);
        pjet_wta_u_dphi->save(tag);
        pjet_u_dr->save(tag);
        pjet_dphi_deta->save(tag);

        mix_pjet_es_f_dphi->save(tag);
        mix_pjet_wta_f_dphi->save(tag);
        mix_pjet_f_dr->save(tag);
        mix_pjet_f_jpt->save(tag);
        mix_pjet_es_u_dphi->save(tag);
        mix_pjet_wta_u_dphi->save(tag);
        mix_pjet_u_dr->save(tag);
        mix_pjet_dphi_deta->save(tag);

        sub_pjet_es_f_dphi->save(tag);
        sub_pjet_wta_f_dphi->save(tag);
        sub_pjet_f_dr->save(tag);
        sub_pjet_f_jpt->save(tag);
        sub_pjet_es_u_dphi->save(tag);
        sub_pjet_wta_u_dphi->save(tag);
        sub_pjet_u_dr->save(tag);
        sub_pjet_dphi_deta->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return populate(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
