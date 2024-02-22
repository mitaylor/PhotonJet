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
#include "TH2D.h"
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
    auto cov = new TH2F("cov", "", m->size(), 0, m->size(), m->size(), 0, m->size());

    for (int64_t i = 0; i < m->size(); ++i) {
        auto err = flat->GetBinError(i + 1);
        cov->SetBinContent(i + 1, i + 1, err * err);
    }

    return cov;
}

TH2D* convert_matrix(TMatrixT<double>* matrix) {
    auto histogram = new TH2D(*matrix); 
    return histogram;
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

    auto type = conf->get<std::string>("type");
    auto error = conf->get<std::string>("error");

    auto regularization = conf->get<std::string>("regularization");

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
        fafter = new TFile(("unfolded/"s + set + "/"s + type + "/"s + error + "/"s + after).data(), "read");
    }, fafters, afters);

    /* prepare output file */
    TFile* fout = new TFile(output, "recreate");

    /* prepare the post-unfolded data */
    std::vector<history<TH1F>*> unfolded;
    std::vector<history<TH1F>*> unfolded_fold0;
    std::vector<history<TH1F>*> unfolded_fold1;
    std::vector<history<TH2D>*> covariance;
    
    unfolded.push_back(new history<TH1F>("unfolded_regularization"s + regularization, "", null<TH1F>, (int64_t) afters.size()));
    unfolded_fold0.push_back(new history<TH1F>("unfolded_fold0_regularization"s + regularization, ""s, null<TH1F>, (int64_t) afters.size()));
    unfolded_fold1.push_back(new history<TH1F>("unfolded_fold1_regularization"s + regularization, ""s, null<TH1F>, (int64_t) afters.size()));
    covariance.push_back(new history<TH2D>("covariance_regularization"s + regularization, ""s, null<TH2D>, (int64_t) afters.size()));

    /* extract histograms */
    for (size_t j = 0; j < afters.size(); ++j) {
        std::string unfold_name = (type == "SVD"s) ? "HUnfoldedSVD" + regularization : "HUnfoldedBayes" + regularization;
        std::string matrix_name = (type == "SVD"s) ? "MUnfoldedSVD" + regularization : "MUnfoldedBayes" + regularization;

        std::cout << unfold_name << " " << matrix_name << std::endl;

        auto HUnfolded = (TH1F*) fafters[j]->Get(unfold_name.data());
        auto MUnfolded = (TMatrixT<double>*) fafters[j]->Get(matrix_name.data());

        (*unfolded[0])[j] = HUnfolded;
        (*unfolded_fold0[0])[j] = fold_mat((*unfolded[0])[j], MUnfolded, mg, 0, osg);
        (*unfolded_fold1[0])[j] = fold_mat((*unfolded[0])[j], MUnfolded, mg, 1, osg);
        (*covariance[0])[j] = convert_matrix(MUnfolded);
    }

    normalise_to_unity(unfolded_fold0[0], unfolded_fold1[0]);

    unfolded[0]->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_jpt_unfolded_regularization"s + regularization);
    unfolded_fold0[0]->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_jpt_unfolded_fold0_regularization"s + regularization);
    unfolded_fold1[0]->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_jpt_unfolded_fold1_regularization"s + regularization);
    covariance[0]->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_jpt_covariance_regularization"s + regularization);

    unfolded[0]->save();
    unfolded_fold0[0]->save();
    unfolded_fold1[0]->save();
    covariance[0]->save();

    fout->Close();

    /* regularization comparison plot */
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
    hb->category("type", "data");

    auto p1 = new paper(set + "_" + tag + "_" + type + "_" + error + "_" + label + "_dj", hb);
    p1->divide(afters.size(), -1);
    p1->accessory(hf_info);
    p1->accessory(kinematics);
    apply_style(p1, cms, system_tag, -2., 27.);
    p1->accessory(std::bind(line_at, _1, 0.f, bdr[0], bdr[1]));

    unfolded_fold0[0]->apply([&](TH1* h) { p1->add(h, "data"s); });

    auto p2 = new paper(set + "_" + tag + "_" + type + "_" + error + "_" + label + "_jpt", hb);
    p2->divide(afters.size(), -1);
    p2->accessory(hf_info);
    p2->accessory(kinematics);
    apply_style(p2, cms, system_tag, 0, 0.04);
    p2->accessory(std::bind(line_at, _1, 0.f, bjet_pt[0], bjet_pt[1]));

    unfolded_fold1[0]->apply([&](TH1* h) { p2->add(h, "data"s); });

    auto p3 = new paper(set + "_" + tag + "_" + type + "_" + error + "_" + label + "_dj_jpt", hb);
    p3->divide(afters.size(), -1);
    p3->accessory(hf_info);
    p3->accessory(kinematics);
    apply_style(p3, cms, system_tag);

    unfolded[0]->apply([&](TH1* h) { p3->add(h, "data"s); });

    auto p4 = new paper(set + "_" + tag + "_" + type + "_" + error + "_" + label + "_cov", hb);
    p4->divide(afters.size(), -1);
    p4->accessory(hf_info);
    p4->accessory(kinematics);
    apply_style(p4, cms, system_tag);

    covariance[0]->apply([&](TH1* h) { p4->add(h, "data"s); });
    covariance[0]->apply([&](TH1* h) { 
        h->GetZaxis()->SetLabelSize(0.02);
        p4->adjust(h, "colz", ""); 
        });

    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");
    p3->draw("pdf");
    p4->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return quantitate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
