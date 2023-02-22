#include "../include/pjtree.h"
#include "../include/specifics.h"
#include "../include/lambdas.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

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
    auto dphi = revert_radian(convert_radian(phi1) - convert_radian(phi2));

    return deta * deta + dphi * dphi;
}

float res(float c, float s, float n, float pt) {
    return std::sqrt(c*c + s*s / pt + n*n / (pt * pt));
}

template <typename... T>
void normalise_to_unity(T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1. / obj->Integral("width")); }), 0)... };
}

template <typename T>
T* null(int64_t, std::string const&, std::string const&) {
    return nullptr;
}

TH2F* variance(TH1* flat, multival const* m) {
    auto cov = new TH2F("cov", "", m->size(), 0, m->size(),
        m->size(), 0, m->size());

    for (int64_t i = 0; i < m->size(); ++i) {
        auto err = flat->GetBinError(i + 1);
        cov->SetBinContent(i + 1, i + 1, err * err);
    }

    return cov;
}

template <std::size_t N>
TH1F* fold(TH1* flat, TH2* covariance, multival const* m, int64_t axis,
           std::array<int64_t, N> const& offsets) {
    auto name = std::string(flat->GetName()) + "_fold" + std::to_string(axis);
    auto hfold = m->axis(axis).book<TH1F, 2>(0, name, "",
        { offsets[axis << 1], offsets[(axis << 1) + 1] });

    auto shape = m->shape();

    auto front = std::vector<int64_t>(m->dims(), 0);
    auto back = std::vector<int64_t>(m->dims(), 0);
    for (int64_t i = 0; i < m->dims(); ++i) {
        front[i] = offsets[i << 1];
        back[i] = shape[i] - offsets[(i << 1) + 1];
    }

    auto size = back[axis] - front[axis];
    auto list = new std::vector<int64_t>[size];

    for (int64_t i = 0; i < m->size(); ++i) {
        auto indices = m->indices_for(i);

        bool flag = false;
        zip([&](int64_t index, int64_t f, int64_t b) {
            flag = flag || index < f || index >= b;
        }, indices, front, back);
        if (flag) { continue; }

        auto index = indices[axis] - front[axis];
        hfold->SetBinContent(index + 1, hfold->GetBinContent(index + 1)
            + flat->GetBinContent(i + 1));

        list[index].push_back(i);
    }

    auto cov = covariance ? (TH2F*)covariance->Clone() : variance(flat, m);

    for (int64_t i = 0; i < size; ++i) {
        auto indices = list[i];
        int64_t count = indices.size();

        auto error = 0.;
        for (int64_t j = 0; j < count; ++j) {
            auto j_x = indices[j] + 1;
            for (int64_t k = 0; k < count; ++k) {
                auto k_x = indices[k] + 1;
                error = error + cov->GetBinContent(j_x, k_x);
            }
        }

        hfold->SetBinError(i + 1, std::sqrt(error));
    }

    delete [] list;
    delete cov;

    hfold->Scale(1., "width");

    return hfold;
}

int create_truth_gen_reco(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto inputs = conf->get<std::vector<std::string>>("inputs");
    auto tag = conf->get<std::string>("tag");

    auto rho = conf->get<std::string>("rho");
    auto rho_label = conf->get<std::string>("rho_label");

    auto acc = conf->get<std::string>("acc");
    auto acc_label_ref = conf->get<std::string>("acc_label_ref");
    auto acc_label_acc = conf->get<std::string>("acc_label_acc");

    auto smear = conf->get<bool>("smear");
    auto smear_input_aa = conf->get<std::string>("smear_input_aa");
    auto smear_input_pp = conf->get<std::string>("smear_input_pp");
    auto smear_tag = conf->get<std::string>("smear_tag");
    auto cent = conf->get<int64_t>("cent");

    auto dr_diff = conf->get<std::vector<float>>("dr_diff");

    auto mod = conf->get<bool>("mod");
    auto parity = conf->get<bool>("parity");

    auto heavyion = conf->get<bool>("heavyion");
    auto apply_er = conf->get<bool>("apply_er");
    auto no_jes = conf->get<bool>("no_jes");
    auto ele_rej = conf->get<bool>("ele_rej");

    auto jet_eta_max = conf->get<float>("jet_eta_max");
    auto photon_pt_min = conf->get<float>("photon_pt_min");
    auto photon_eta_max = conf->get<float>("photon_eta_max");
    auto hovere_max = conf->get<float>("hovere_max");
    auto see_min = conf->get<float>("see_min");
    auto see_max = conf->get<float>("see_max");
    auto iso_max = conf->get<float>("iso_max");

    auto rdrr = conf->get<std::vector<float>>("drr_range");
    auto rdrg = conf->get<std::vector<float>>("drg_range");
    auto rptr = conf->get<std::vector<float>>("ptr_range");
    auto rptg = conf->get<std::vector<float>>("ptg_range");
    auto rdphi = conf->get<std::vector<float>>("dphi_range");

    auto dhf = conf->get<std::vector<float>>("hf_diff");

    /* prepare histograms */
    auto ihf = new interval(dhf);
    auto isdr = new interval("#deltaj"s, dr_diff);
    auto idphi = new interval("#Delta#phi^{#gammaj}"s, rdphi);

    std::array<int64_t, 4> osr = { 0, 0, 1, 3 };
    std::array<int64_t, 4> osg = { 0, 0, 1, 1 };

    auto mr = new multival(rdrr, rptr);
    auto mg = new multival(rdrg, rptg);

    auto fr = [&](int64_t, std::string const& name, std::string const& label) {
        return new TH1F(name.data(), (";Reconstructed Bin;"s + label).data(),
            mr->size(), 0, mr->size()); };

    auto fg = [&](int64_t, std::string const& name, std::string const& label) {
        return new TH1F(name.data(), (";Generator Bin;"s + label).data(),
            mg->size(), 0, mg->size()); };

    auto g_gen_iso = new history<TH1F>("g_gen_iso"s, "counts", fg, ihf->size());
    auto g_reco_iso = new history<TH1F>("g_reco_iso"s, "counts", fg, ihf->size());
    auto r_gen_iso = new history<TH1F>("r_gen_iso"s, "counts", fr, ihf->size());
    auto r_reco_iso_matched = new history<TH1F>("r_reco_iso_matched"s, "counts", fr, ihf->size());
    auto r_reco_iso_unmatched = new history<TH1F>("r_reco_iso_unmatched"s, "counts", fr, ihf->size());

    auto h_r_truth_gen_iso = new history<TH1F>("r_truth_gen_iso", "", null<TH1F>, ihf->size());
    auto h_r_truth_reco_iso = new history<TH1F>("r_truth_reco_iso", "", null<TH1F>, ihf->size());
    auto h_r_reco_gen_iso = new history<TH1F>("r_reco_gen_iso", "", null<TH1F>, ihf->size());
    auto h_r_reco_reco_iso_matched = new history<TH1F>("r_reco_reco_iso_matched", "", null<TH1F>, ihf->size());
    auto h_r_reco_reco_iso_unmatched = new history<TH1F>("r_reco_reco_iso_unmatched", "", null<TH1F>, ihf->size());

    auto h_j_truth_gen_iso = new history<TH1F>("j_truth_gen_iso", "", null<TH1F>, ihf->size());
    auto h_j_truth_reco_iso = new history<TH1F>("j_truth_reco_iso", "", null<TH1F>, ihf->size());
    auto h_j_reco_gen_iso = new history<TH1F>("j_reco_gen_iso", "", null<TH1F>, ihf->size());
    auto h_j_reco_reco_iso_matched = new history<TH1F>("j_reco_reco_iso_matched", "", null<TH1F>, ihf->size());
    auto h_j_reco_reco_iso_unmatched = new history<TH1F>("j_reco_reco_iso_unmatched", "", null<TH1F>, ihf->size());

    std::vector<double> pass_fail_gen_reco(4,0);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load centrality weighting for MC */
    TFile* frho;
    history<TH1F>* rho_weighting = nullptr;

    if (!rho.empty()) {
        frho = new TFile(rho.data(), "read");
        rho_weighting = new history<TH1F>(frho, rho_label);
    }

    /* load acceptance weighting for HI */
    TFile* fa;
    history<TH2F>* acceptance = nullptr;
    history<TH2F>* total = nullptr;

    if (!acc.empty()) {
        fa = new TFile(acc.data(), "read");
        acceptance = new history<TH2F>(fa, acc_label_acc);
        total = new history<TH2F>(fa, acc_label_ref);
    }

    /* load the smearing information */
    TFile* fsmear_aa;
    TFile* fsmear_pp;
    history<TH1F>* smear_fits_aa = nullptr;
    history<TH1F>* smear_fits_pp = nullptr;

    if (smear) {
        fsmear_aa = new TFile(smear_input_aa.data(), "read");
        fsmear_pp = new TFile(smear_input_pp.data(), "read");
        smear_fits_aa = new history<TH1F>(fsmear_aa, "aa_" + smear_tag);
        if (cent == 0) { smear_fits_pp = new history<TH1F>(fsmear_pp, "pp_smear_50_90_" + smear_tag); }
        if (cent == 1) { smear_fits_pp = new history<TH1F>(fsmear_pp, "pp_smear_30_50_" + smear_tag); }
        if (cent == 2) { smear_fits_pp = new history<TH1F>(fsmear_pp, "pp_smear_10_30_" + smear_tag); }
        if (cent == 3) { smear_fits_pp = new history<TH1F>(fsmear_pp, "pp_smear_0_10_" + smear_tag); }
    }

    auto rng = new TRandom3(144);

    /* load input */
    for (auto const& input : inputs) {
        std::cout << input << std::endl;
        
        TFile* f = new TFile(input.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto p = new pjtree(true, false, heavyion, t, { 1, 1, 1, 0, 1, 0, heavyion, 0, 0 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        /* fill histograms */
        for (int64_t i = 0; i < nentries; ++i) {
            if (i % 100000 == 0) { printf("%li/%li\n", i, nentries); }

            if (mod) {
                if ((i + parity + 1) % 2 == 0) { continue; }
            }

            t->GetEntry(i);

            int64_t leading = -1;
            float leading_pt = 0;
            for (int64_t j = 0; j < p->nPho; ++j) {
                if ((*p->phoEt)[j] <= 30) { continue; }
                if (std::abs((*p->phoSCEta)[j]) >= photon_eta_max) { continue; }
                if ((*p->phoHoverE)[j] > hovere_max) { continue; }

                auto pho_et = (*p->phoEt)[j];
                if (heavyion && apply_er) pho_et = (*p->phoEtErNew)[j];
                if (!heavyion && apply_er) pho_et = (*p->phoEtEr)[j];

                if (pho_et < photon_pt_min) { continue; }

                if (pho_et > leading_pt) {
                    leading = j;
                    leading_pt = pho_et;
                }
            }

            /* require leading photon */
            if (leading < 0) { continue; }
            if (leading_pt > 200) { continue; }

            if ((*p->phoSigmaIEtaIEta_2012)[leading] > see_max
                    || (*p->phoSigmaIEtaIEta_2012)[leading] < see_min)
                continue;

            /* hem failure region exclusion */
            if (heavyion && in_pho_failure_region(p, leading)) { continue; }

            /* require match to gen */
            bool is_gen = true;
            auto gen_index = (*p->pho_genMatchedIndex)[leading];
            if (gen_index == -1) { is_gen = false; }

            auto pid = (*p->mcPID)[gen_index];
            auto mpid = (*p->mcMomPID)[gen_index];
            if (pid != 22 || (std::abs(mpid) > 22 && mpid != -999)) { is_gen = false; }

            float isolation = (*p->pho_ecalClusterIsoR3)[leading]
                + (*p->pho_hcalRechitIsoR3)[leading]
                + (*p->pho_trackIsoR3PtCut20)[leading];

            /* isolation requirement */
            if (!is_gen && isolation > iso_max) { continue; } // see if it should be && or ||

            /* photon axis */
            auto photon_eta = (*p->phoEta)[leading];
            auto photon_phi = convert_radian((*p->phoPhi)[leading]);

            /* electron rejection */
            if (ele_rej) {
                bool electron = false;
                for (int64_t j = 0; j < p->nEle; ++j) {
                    if (std::abs((*p->eleSCEta)[j]) > 1.4442) { continue; }

                    auto deta = photon_eta - (*p->eleEta)[j];
                    if (deta > 0.1) { continue; }

                    auto ele_phi = convert_radian((*p->elePhi)[j]);
                    auto dphi = revert_radian(photon_phi - ele_phi);
                    auto dr2 = deta * deta + dphi * dphi;

                    if (dr2 < 0.01 && passes_electron_id<
                                det::barrel, wp::loose, pjtree
                            >(p, j, heavyion)) {
                        electron = true; break; }
                }

                if (electron) { continue; }
            }

            /* fill event weight */
            auto weight = p->w;

            std::vector<float> weights(ihf->size(), weight);
            
            if (heavyion) {
                auto avg_rho = get_avg_rho(p, -photon_eta_max, photon_eta_max);

                for (int64_t j = 0; j < ihf->size(); ++j) {
                    auto bin = (*rho_weighting)[j]->FindBin(avg_rho);
                    auto cor = (*rho_weighting)[j]->GetBinContent(bin);
                    weights[j] *= cor;
                }
            }

            /* map reco jet to gen jet */
            std::unordered_map<float, int64_t> genid;
            for (int64_t j = 0; j < p->ngen; ++j)
                genid[(*p->genpt)[j]] = j;

            for (int64_t j = 0; j < p->nref; ++j) {
                auto gen_pt = (*p->refpt)[j];
                auto gen_eta = (*p->refeta)[j];
                auto gen_phi = (*p->refphi)[j];

                auto reco_pt = (!no_jes && heavyion) ? (*p->jtptCor)[j] : (*p->jtpt)[j];
                auto reco_eta = (*p->jteta)[j];
                auto reco_phi = (*p->jtphi)[j];

                if (std::abs(reco_eta) >= jet_eta_max) { continue; }

                auto pj_deta = photon_eta - reco_eta;
                auto pj_dphi = revert_radian(std::abs(photon_phi - convert_radian(reco_phi)));
                auto pj_dr = std::sqrt(pj_deta * pj_deta + pj_dphi * pj_dphi);

                if (pj_dr < 0.4) { continue; }

                if (heavyion && in_jet_failure_region(p, j))
                    continue;

                /* require back-to-back jets */
                if (std::abs(photon_phi - convert_radian(reco_phi)) < 0.875_pi)
                    continue;

                /* do acceptance weighting */
                double cor = 1;
                if (heavyion) {
                    auto dphi_x = idphi->index_for(revert_pi(std::abs(photon_phi - convert_radian(reco_phi))));
                    auto bin = (*total)[dphi_x]->FindBin(reco_eta, photon_eta);
                    cor = (*total)[dphi_x]->GetBinContent(bin) / (*acceptance)[dphi_x]->GetBinContent(bin);
                    if (cor < 1) { std::cout << "error" << std::endl; }
                }

                auto id = (gen_pt > 0) ? genid[gen_pt]: -1;
                auto gdr = (gen_pt > 0) ? std::sqrt(dr2(gen_eta, (*p->WTAgeneta)[id], gen_phi, (*p->WTAgenphi)[id]))
                                        : 0;
                auto g_x = mg->index_for(v{gdr, gen_pt});

                /* isolation requirement */
                if (is_gen) {
                    if ((*p->mcCalIsoDR04)[gen_index] < 5 && gen_pt > rptg.front()) { 
                        for (int64_t k = 0; k < ihf->size(); ++k) {
                            (*g_gen_iso)[k]->Fill(g_x, weights[k]*cor); }
                    }
                }

                if (isolation < iso_max && gen_pt > rptg.front()) { 
                    for (int64_t k = 0; k < ihf->size(); ++k) {
                        (*g_reco_iso)[k]->Fill(g_x, weights[k]*cor); }
                }

                if (reco_pt > rptr.front() && reco_pt < rptr.back()) {
                    auto rdr = std::sqrt(dr2(reco_eta, (*p->WTAeta)[j],
                                            reco_phi, (*p->WTAphi)[j]));
                    auto r_x = mr->index_for(v{rdr, reco_pt});

                    if (smear) {
                        if (rdr > 0.3) { continue; }
                        auto dr_x = isdr->index_for(rdr);

                        auto aa_c = (*smear_fits_aa)[x{dr_x, cent}]->GetBinContent(1);
                        auto aa_s = (*smear_fits_aa)[x{dr_x, cent}]->GetBinContent(2);
                        auto aa_n = (*smear_fits_aa)[x{dr_x, cent}]->GetBinContent(3);

                        auto pp_c = (*smear_fits_pp)[x{dr_x, 0}]->GetBinContent(1);
                        auto pp_s = (*smear_fits_pp)[x{dr_x, 0}]->GetBinContent(2);
                        auto pp_n = (*smear_fits_pp)[x{dr_x, 0}]->GetBinContent(3);

                        auto res_diff = res(aa_c, aa_s, aa_n, reco_pt) - res(pp_c, pp_s, pp_n, reco_pt);

                        if (res_diff > 0) {
                            auto change = rng->Exp(res(aa_c, aa_s, aa_n, reco_pt)) - rng->Exp(res(pp_c, pp_s, pp_n, reco_pt));
                            auto sign = (rng->Integer(2) == 0) ? -1 : 1;                
                            auto adj = rdr + change * sign / 2;

                            if (0 < adj && adj < 0.2 && rdr < 0.2) rdr = adj;
                        }
                    }
                    
                    if (is_gen) {
                        if ((*p->mcCalIsoDR04)[gen_index] < 5 && gen_pt > rptg.front()) {
                            for (int64_t k = 0; k < ihf->size(); ++k) {
                                (*r_gen_iso)[k]->Fill(r_x, weights[k]*cor);
                            }
                        }
                    }
                    if (isolation < iso_max && gen_pt > 20) {
                        for (int64_t k = 0; k < ihf->size(); ++k) {
                            (*r_reco_iso_matched)[k]->Fill(r_x, weights[k]*cor);
                        }
                    }
                    if (isolation < iso_max && gen_pt < 20) {
                        for (int64_t k = 0; k < ihf->size(); ++k) {
                            (*r_reco_iso_unmatched)[k]->Fill(r_x, weights[k]*cor);
                        }
                    }
                }
            }
        }
    }

    for (int64_t i = 0; i < ihf->size(); ++i) {
        (*h_r_truth_gen_iso)[i] = fold((*g_gen_iso)[i], nullptr, mg, 0, osg);
        (*h_r_truth_reco_iso)[i] = fold((*g_reco_iso)[i], nullptr, mg, 0, osg);
        (*h_r_reco_gen_iso)[i] = fold((*r_gen_iso)[i], nullptr, mr, 0, osr);
        (*h_r_reco_reco_iso_matched)[i] = fold((*r_reco_iso_matched)[i], nullptr, mr, 0, osr);
        (*h_r_reco_reco_iso_unmatched)[i] = fold((*r_reco_iso_unmatched)[i], nullptr, mr, 0, osr);

        (*h_j_truth_gen_iso)[i] = fold((*g_gen_iso)[i], nullptr, mg, 1, osg);
        (*h_j_truth_reco_iso)[i] = fold((*g_reco_iso)[i], nullptr, mg, 1, osg);
        (*h_j_reco_gen_iso)[i] = fold((*r_gen_iso)[i], nullptr, mr, 1, osr);
        (*h_j_reco_reco_iso_matched)[i] = fold((*r_reco_iso_matched)[i], nullptr, mr, 1, osr);
        (*h_j_reco_reco_iso_unmatched)[i] = fold((*r_reco_iso_unmatched)[i], nullptr, mr, 1, osr);
    }

    h_r_truth_gen_iso->rename("r_truth_gen_iso_fold0");
    h_r_truth_reco_iso->rename("r_truth_reco_iso_fold0");
    h_r_reco_gen_iso->rename("r_reco_gen_iso_fold0");
    h_r_reco_reco_iso_matched->rename("r_reco_reco_iso_matched_fold0");
    h_r_reco_reco_iso_unmatched->rename("r_reco_reco_iso_unmatched_fold0");

    h_j_truth_gen_iso->rename("j_truth_gen_iso_fold1");
    h_j_truth_reco_iso->rename("j_truth_reco_iso_fold1");
    h_j_reco_gen_iso->rename("j_reco_gen_iso_fold1");
    h_j_reco_reco_iso_matched->rename("j_reco_reco_iso_matched_fold1");
    h_j_reco_reco_iso_unmatched->rename("j_reco_reco_iso_unmatched_fold1");

    normalise_to_unity(h_r_truth_gen_iso, h_r_truth_reco_iso, h_r_reco_gen_iso, h_r_reco_reco_iso_matched, h_r_reco_reco_iso_unmatched);
    normalise_to_unity(h_j_truth_gen_iso, h_j_truth_reco_iso, h_j_reco_gen_iso, h_j_reco_reco_iso_matched, h_j_reco_reco_iso_unmatched);


    /* save output */
    in(output, [&]() {
        g_gen_iso->save(tag);
        g_reco_iso->save(tag);
        r_gen_iso->save(tag);
        r_reco_iso_matched->save(tag);
        r_reco_iso_unmatched->save(tag);
        
        h_r_truth_gen_iso->save(tag);
        h_r_truth_reco_iso->save(tag);
        h_r_reco_gen_iso->save(tag);
        h_r_reco_reco_iso_matched->save(tag);
        h_r_reco_reco_iso_unmatched->save(tag);

        h_j_truth_gen_iso->save(tag);
        h_j_truth_reco_iso->save(tag);
        h_j_reco_gen_iso->save(tag);
        h_j_reco_reco_iso_matched->save(tag);
        h_j_reco_reco_iso_unmatched->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return create_truth_gen_reco(argv[1], argv[2]);

    return 0;
}