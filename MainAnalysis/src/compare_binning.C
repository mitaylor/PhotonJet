#include "../include/lambdas.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

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

int compare_binning(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto tag = conf->get<std::string>("tag");

    auto before_label = conf->get<std::string>("before_label");
    auto after_label = conf->get<std::string>("after_label");
    auto covariance_label = conf->get<std::string>("covariance_label");

    auto rebin1_config = conf->get<std::string>("rebin1_config");
    auto rebin2_config = conf->get<std::string>("rebin2_config");
    auto rebin3_config = conf->get<std::string>("rebin3_config");
    auto rebin4_config = conf->get<std::string>("rebin4_config");

    /* rebin1 */
    auto rebin1 = new configurer(rebin1_config);

    auto rebin1_set = rebin1->get<std::string>("set");
    auto rebin1_base = rebin1->get<std::string>("base");

    auto rebin1_rdrr = rebin1->get<std::vector<float>>("drr_range");
    auto rebin1_rdrg = rebin1->get<std::vector<float>>("drg_range");
    auto rebin1_rptr = rebin1->get<std::vector<float>>("ptr_range");
    auto rebin1_rptg = rebin1->get<std::vector<float>>("ptg_range");

    std::vector<int64_t> rebin1_osr = {0, 1, 0, 2};
    std::vector<int64_t> rebin1_osg = {0, 1, 0, 2};

    auto rebin1_idrr = new interval("#deltaj"s, rebin1_rdrr);
    auto rebin1_iptr = new interval("p_{T}^{j}"s, rebin1_rptr);

    auto rebin1_idrg = new interval("#deltaj"s, rebin1_rdrg);
    auto rebin1_iptg = new interval("p_{T}^{j}"s, rebin1_rptg);

    auto rebin1_mr = new multival(*rebin1_idrr, *rebin1_iptr);
    auto rebin1_mg = new multival(*rebin1_idrg, *rebin1_iptg);

    /* rebin2 */
    auto rebin2 = new configurer(rebin2_config);

    auto rebin2_set = rebin2->get<std::string>("set");
    auto rebin2_base = rebin2->get<std::string>("base");

    auto rebin2_rdrr = rebin2->get<std::vector<float>>("drr_range");
    auto rebin2_rdrg = rebin2->get<std::vector<float>>("drg_range");
    auto rebin2_rptr = rebin2->get<std::vector<float>>("ptr_range");
    auto rebin2_rptg = rebin2->get<std::vector<float>>("ptg_range");

    std::vector<int64_t> rebin2_osr = {0, 1, 0, 2};
    std::vector<int64_t> rebin2_osg = {0, 1, 0, 2};

    auto rebin2_idrr = new interval("#deltaj"s, rebin2_rdrr);
    auto rebin2_iptr = new interval("p_{T}^{j}"s, rebin2_rptr);

    auto rebin2_idrg = new interval("#deltaj"s, rebin2_rdrg);
    auto rebin2_iptg = new interval("p_{T}^{j}"s, rebin2_rptg);

    auto rebin2_mr = new multival(*rebin2_idrr, *rebin2_iptr);
    auto rebin2_mg = new multival(*rebin2_idrg, *rebin2_iptg);

    /* rebin3 */
    auto rebin3 = new configurer(rebin3_config);

    auto rebin3_set = rebin3->get<std::string>("set");
    auto rebin3_base = rebin3->get<std::string>("base");

    auto rebin3_rdrr = rebin3->get<std::vector<float>>("drr_range");
    auto rebin3_rdrg = rebin3->get<std::vector<float>>("drg_range");
    auto rebin3_rptr = rebin3->get<std::vector<float>>("ptr_range");
    auto rebin3_rptg = rebin3->get<std::vector<float>>("ptg_range");

    std::vector<int64_t> rebin3_osr = {0, 1, 0, 1};
    std::vector<int64_t> rebin3_osg = {0, 1, 0, 1};

    auto rebin3_idrr = new interval("#deltaj"s, rebin3_rdrr);
    auto rebin3_iptr = new interval("p_{T}^{j}"s, rebin3_rptr);

    auto rebin3_idrg = new interval("#deltaj"s, rebin3_rdrg);
    auto rebin3_iptg = new interval("p_{T}^{j}"s, rebin3_rptg);

    auto rebin3_mr = new multival(*rebin3_idrr, *rebin3_iptr);
    auto rebin3_mg = new multival(*rebin3_idrg, *rebin3_iptg);

    /* rebin4 */
    auto rebin4 = new configurer(rebin4_config);

    auto rebin4_set = rebin4->get<std::string>("set");
    auto rebin4_base = rebin4->get<std::string>("base");

    auto rebin4_rdrr = rebin4->get<std::vector<float>>("drr_range");
    auto rebin4_rdrg = rebin4->get<std::vector<float>>("drg_range");
    auto rebin4_rptr = rebin4->get<std::vector<float>>("ptr_range");
    auto rebin4_rptg = rebin4->get<std::vector<float>>("ptg_range");

    std::vector<int64_t> rebin4_osr = {0, 0, 0, 1};
    std::vector<int64_t> rebin4_osg = {0, 0, 0, 1};

    auto rebin4_idrr = new interval("#deltaj"s, rebin4_rdrr);
    auto rebin4_iptr = new interval("p_{T}^{j}"s, rebin4_rptr);

    auto rebin4_idrg = new interval("#deltaj"s, rebin4_rdrg);
    auto rebin4_iptg = new interval("p_{T}^{j}"s, rebin4_rptg);

    auto rebin4_mr = new multival(*rebin4_idrr, *rebin4_iptr);
    auto rebin4_mg = new multival(*rebin4_idrg, *rebin4_iptg);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();
    int size = size;

    /* open input files */
    TFile* finput1 = new TFile((rebin1_base + input).data(), "read");
    TFile* finput2 = new TFile((rebin2_base + input).data(), "read");
    TFile* finput3 = new TFile((rebin3_base + input).data(), "read");
    TFile* finput4 = new TFile((rebin4_base + input).data(), "read");

    /* create ouput file */
    TFile* fout = new TFile(output, "recreate");

    /* prepare output from pre-unfolded data */
    auto rebin1_before = new history<TH1F>(finput1, tag + "_"s + before_label);
    auto rebin2_before = new history<TH1F>(finput2, tag + "_"s + before_label);
    auto rebin3_before = new history<TH1F>(finput3, tag + "_"s + before_label);
    auto rebin4_before = new history<TH1F>(finput4, tag + "_"s + before_label);

    auto rebin1_before_fold0 = new history<TH1F>("rebin1_before_fold0", "", null<TH1F>, size);
    auto rebin1_before_fold1 = new history<TH1F>("rebin1_before_fold1", "", null<TH1F>, size);
    auto rebin2_before_fold0 = new history<TH1F>("rebin2_before_fold0", "", null<TH1F>, size);
    auto rebin2_before_fold1 = new history<TH1F>("rebin2_before_fold1", "", null<TH1F>, size);
    auto rebin3_before_fold0 = new history<TH1F>("rebin3_before_fold0", "", null<TH1F>, size);
    auto rebin3_before_fold1 = new history<TH1F>("rebin3_before_fold1", "", null<TH1F>, size);
    auto rebin4_before_fold0 = new history<TH1F>("rebin4_before_fold0", "", null<TH1F>, size);
    auto rebin4_before_fold1 = new history<TH1F>("rebin4_before_fold1", "", null<TH1F>, size);

    for (int j = 0; j < size; ++j) {
        (*rebin1_before_fold0)[j] = fold((*rebin1_before)[j], nullptr, rebin1_mr, 0, rebin1_osr);
        (*rebin1_before_fold1)[j] = fold((*rebin2_before)[j], nullptr, rebin1_mr, 1, rebin1_osr);
        (*rebin2_before_fold0)[j] = fold((*rebin2_before)[j], nullptr, rebin2_mr, 0, rebin2_osr);
        (*rebin2_before_fold1)[j] = fold((*rebin2_before)[j], nullptr, rebin2_mr, 1, rebin2_osr);
        (*rebin3_before_fold0)[j] = fold((*rebin3_before)[j], nullptr, rebin3_mr, 0, rebin3_osr);
        (*rebin3_before_fold1)[j] = fold((*rebin2_before)[j], nullptr, rebin3_mr, 1, rebin3_osr);
        (*rebin4_before_fold0)[j] = fold((*rebin4_before)[j], nullptr, rebin4_mr, 0, rebin4_osr);
        (*rebin4_before_fold1)[j] = fold((*rebin2_before)[j], nullptr, rebin4_mr, 1, rebin4_osr);
    }

    /* prepare the post-unfolded data */
    auto rebin1_after = new history<TH1F>(finput1, tag + "_"s + after_label);
    auto rebin2_after = new history<TH1F>(finput2, tag + "_"s + after_label);
    auto rebin3_after = new history<TH1F>(finput3, tag + "_"s + after_label);
    auto rebin4_after = new history<TH1F>(finput4, tag + "_"s + after_label);

    auto rebin1_covariance = new history<TH2F>(finput1, tag + "_"s + covariance_label);
    auto rebin2_covariance = new history<TH2F>(finput2, tag + "_"s + covariance_label);
    auto rebin3_covariance = new history<TH2F>(finput3, tag + "_"s + covariance_label);
    auto rebin4_covariance = new history<TH2F>(finput4, tag + "_"s + covariance_label);

    auto rebin1_after_fold0 = new history<TH1F>("rebin1_after_fold0", "", null<TH1F>, size);
    auto rebin1_after_fold1 = new history<TH1F>("rebin1_after_fold1", "", null<TH1F>, size);
    auto rebin2_after_fold0 = new history<TH1F>("rebin2_after_fold0", "", null<TH1F>, size);
    auto rebin2_after_fold1 = new history<TH1F>("rebin2_after_fold1", "", null<TH1F>, size);
    auto rebin3_after_fold0 = new history<TH1F>("rebin3_after_fold0", "", null<TH1F>, size);
    auto rebin3_after_fold1 = new history<TH1F>("rebin3_after_fold1", "", null<TH1F>, size);
    auto rebin4_after_fold0 = new history<TH1F>("rebin4_after_fold0", "", null<TH1F>, size);
    auto rebin4_after_fold1 = new history<TH1F>("rebin4_after_fold1", "", null<TH1F>, size);

    for (int j = 0; j < size; ++j) {
        (*rebin1_after_fold0)[j] = fold((*rebin1_after)[j], (*rebin1_covariance)[j], rebin1_mg, 0, rebin1_osg);
        (*rebin1_after_fold1)[j] = fold((*rebin2_after)[j], (*rebin1_covariance)[j], rebin1_mg, 1, rebin1_osg);
        (*rebin2_after_fold0)[j] = fold((*rebin2_after)[j], (*rebin2_covariance)[j], rebin2_mg, 0, rebin2_osg);
        (*rebin2_after_fold1)[j] = fold((*rebin2_after)[j], (*rebin2_covariance)[j], rebin2_mg, 1, rebin2_osg);
        (*rebin3_after_fold0)[j] = fold((*rebin3_after)[j], (*rebin3_covariance)[j], rebin3_mg, 0, rebin3_osg);
        (*rebin3_after_fold1)[j] = fold((*rebin2_after)[j], (*rebin3_covariance)[j], rebin3_mg, 1, rebin3_osg);
        (*rebin4_after_fold0)[j] = fold((*rebin4_after)[j], (*rebin4_covariance)[j], rebin4_mg, 0, rebin4_osg);
        (*rebin4_after_fold1)[j] = fold((*rebin2_after)[j], (*rebin4_covariance)[j], rebin4_mg, 1, rebin4_osg);
    }

    /* save histograms */
    rebin1_before_fold0->save();
    rebin1_before_fold1->save();
    rebin2_before_fold0->save();
    rebin2_before_fold1->save();
    rebin3_before_fold0->save();
    rebin3_before_fold1->save();
    rebin4_before_fold0->save();
    rebin4_before_fold1->save();

    rebin1_after_fold0->save();
    rebin1_after_fold1->save();
    rebin2_after_fold0->save();
    rebin2_after_fold1->save();
    rebin3_after_fold0->save();
    rebin3_after_fold1->save();
    rebin4_after_fold0->save();
    rebin4_after_fold1->save();

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return compare_binning(argv[1], argv[2]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
