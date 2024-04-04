#include "../include/lambdas.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TMatrixT.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

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

TH1F* fold(TH1* flat, TH2* covariance, multival const* m, int64_t axis,
           std::vector<int64_t>& offsets) {
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

TH1F* fold_mat(TH1* flat, TMatrixT<double>* covariance, multival const* m, int64_t axis,
           std::vector<int64_t>& offsets) {
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

    for (int64_t i = 0; i < size; ++i) {
        auto indices = list[i];
        int64_t count = indices.size();

        auto error = 0.;
        for (int64_t j = 0; j < count; ++j) {
            auto j_x = indices[j];
            for (int64_t k = 0; k < count; ++k) {
                auto k_x = indices[k];
                error = error + (*covariance)(j_x, k_x);
            }
        }

        hfold->SetBinError(i + 1, std::sqrt(error));
    }

    delete [] list;

    hfold->Scale(1., "width");

    return hfold;
}

int quantitate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto tag = conf->get<std::string>("tag");
    auto label = conf->get<std::string>("before_label");

    auto afters = conf->get<std::vector<std::string>>("afters");

    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto heavyion = sel->get<bool>("heavyion");

    auto rdrg = sel->get<std::vector<float>>("drg_range");
    auto rptg = sel->get<std::vector<float>>("ptg_range");

    auto osg = sel->get<std::vector<int64_t>>("osg");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    auto bdr = sel->get<std::vector<float>>("dr_bounds");
    auto bjet_pt = sel->get<std::vector<float>>("jet_pt_bounds");
    auto bpho_pt = sel->get<std::vector<float>>("photon_pt_bounds");

    /* create intervals and multivals */
    auto idrg = new interval("#deltaj"s, rdrg);
    auto iptg = new interval("p_{T}^{j}"s, rptg);

    auto mg = new multival(*idrg, *iptg);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    std::vector<TFile*> fafters(afters.size(), nullptr);
    zip([&](auto& fafter, auto const& after) {
        fafter = new TFile(("unfolded/" + set + "/SVD/" + after).data(), "read");
    }, fafters, afters);

    /* prepare output file */
    TFile* fout = new TFile(output, "recreate");

    /* prepare the post-unfolded data */
    std::vector<std::string> iterations{"1.0", "2.0", "3.0", "4.0", "5.0", "6.0", "7.0", "8.0", "9.0"};

    std::vector<history<TH1F>*> unfolded;
    std::vector<history<TH1F>*> unfolded_fold0;
    std::vector<history<TH1F>*> unfolded_fold1;
    
    for (size_t i = 0; i < iterations.size(); ++i) {
        unfolded.push_back(new history<TH1F>("unfolded_iteration"s + iterations[i], "", null<TH1F>, (int64_t) afters.size()));
        unfolded_fold0.push_back(new history<TH1F>("unfolded_fold0_iteration"s + iterations[i], ""s, null<TH1F>, (int64_t) afters.size()));
        unfolded_fold1.push_back(new history<TH1F>("unfolded_fold1_iteration"s + iterations[i], ""s, null<TH1F>, (int64_t) afters.size()));
    }

    /* extract histograms */
    for (size_t i = 0; i < iterations.size(); ++i) {
        for (size_t j = 0; j < afters.size(); ++j) {
            std::string unfold_name = "HUnfoldedSVD"s + iterations[i];
            std::string matrix_name = "MUnfoldedSVD"s + iterations[i];

            auto HUnfoldedSVD = (TH1F*) fafters[j]->Get(unfold_name.data());
            auto MUnfolded = (TMatrixT<double>*) fafters[j]->Get(matrix_name.data());

            (*unfolded[i])[j] = HUnfoldedSVD;
            (*unfolded_fold0[i])[j] = fold_mat((*unfolded[i])[j], MUnfolded, mg, 0, osg);
            (*unfolded_fold1[i])[j] = fold_mat((*unfolded[i])[j], MUnfolded, mg, 1, osg);
        }

        // normalise_to_unity(unfolded_fold0[i], unfolded_fold1[i]);

        unfolded[i]->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_jpt_unfolded_iteration"s + iterations[i]);
        unfolded_fold0[i]->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_jpt_unfolded_fold0_iteration"s + iterations[i]);
        unfolded_fold1[i]->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_jpt_unfolded_fold1_iteration"s + iterations[i]);

        unfolded[i]->save();
        unfolded_fold0[i]->save();
        unfolded_fold1[i]->save();
    }

    fout->Close();

    /* iteration comparison plot */
    auto system_tag = "  #sqrt{s_{NN}} = 5.02 TeV"s;
    system_tag += (heavyion) ? ", 1.69 nb^{-1}"s : ", 302 pb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

    auto hf_info = [&](int64_t index) { info_text(index, 0.75, "Cent. %i - %i%%", dcent, true); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto line1 = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs);
            auto line2 = "#Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator) + ", anti-k_{T} R = 0.3";
            auto line3 = to_text(bjet_pt[0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.4, line1.data());
            l->DrawLatexNDC(0.865, 0.35, line2.data());
            l->DrawLatexNDC(0.865, 0.3, line3.data());
        }
    };

    auto hb = new pencil();
    hb->category("type", "k_{reg} = 1.0", "k_{reg} = 2.0", "k_{reg} = 3.0", "k_{reg} = 4.0", "k_{reg} = 5.0", "k_{reg} = 6.0", "k_{reg} = 7.0", "k_{reg} = 8.0", "k_{reg} = 9.0");

    auto p1 = new paper(set + "_" + tag + "_svd_kerrors_unnormalized_comparison_" + label + "_dj", hb);
    p1->divide(afters.size(), -1);
    p1->accessory(hf_info);
    p1->accessory(kinematics);
    apply_style(p1, cms, system_tag, -2., 27.);
    p1->accessory(std::bind(line_at, _1, 0.f, bdr[0], bdr[1]));

    unfolded_fold0[0]->apply([&](TH1* h) { p1->add(h, "k_{reg} = "s + iterations[0]); });

    for (size_t i = 1; i < iterations.size(); ++i) {
        for (size_t j = 0; j < afters.size(); ++j) {
            p1->stack(j + 1, (*unfolded_fold0[i])[j], "k_{reg} = "s + iterations[i]);
        }
    }

    auto p2 = new paper(set + "_" + tag + "_svd_kerrors_unnormalized_comparison_" + label + "_jpt", hb);
    p2->divide(afters.size(), -1);
    p2->accessory(hf_info);
    p2->accessory(kinematics);
    apply_style(p2, cms, system_tag, 0, 0.04);
    p2->accessory(std::bind(line_at, _1, 0.f, bdr[0], bdr[1]));

    unfolded_fold1[0]->apply([&](TH1* h) { p2->add(h, "k_{reg} = "s + iterations[0]); });

    for (size_t i = 1; i < iterations.size(); ++i) {
        for (size_t j = 0; j < afters.size(); ++j) {
            p2->stack(j + 1, (*unfolded_fold1[i])[j], "k_{reg} = "s + iterations[i]);
        }
    }

    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return quantitate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
