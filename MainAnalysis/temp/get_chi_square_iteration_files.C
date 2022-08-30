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
           std::vector<int64_t>& offsets, bool fine) {
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

    if (fine) hfold->Rebin();
    hfold->Scale(1., "width");

    return hfold;
}

int quantitate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto tag = conf->get<std::string>("tag");

    auto before = conf->get<std::string>("before");
    auto before_label = conf->get<std::string>("before_label");
    auto before_folds = conf->get<std::vector<std::string>>("before_folds");

    auto fine = conf->get<bool>("fine");

    auto afters = conf->get<std::vector<std::string>>("afters");

    auto rdrr = conf->get<std::vector<float>>("drr_range");
    auto rptr = conf->get<std::vector<float>>("ptr_range");

    /* create intervals and multivals */
    auto idrr = new interval("#deltaj"s, rdrr);
    auto iptr = new interval("p_{T}^{j}"s, rptr);

    auto mr = new multival(*idrr, *iptr);

    /* set offsets for folding pre and post unfolding so that jets between 30-120 are used */
    std::vector<int64_t> osr{ 0, 0, 1, 3 };

    if (fine) {
        for (auto os : osr) os *= 2;
    }

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    TFile* fbefore = new TFile(before.data(), "read");

    std::vector<TFile*> fafters(afters.size(), nullptr);
    zip([&](auto& fafter, auto const& after) {
        fafter = new TFile(after.data(), "read");
    }, fafters, afters);

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    /* prepare fold from pre-unfolded data */
    auto stub = "_"s + before_folds[0];

    auto hin = new history<TH1F>(fbefore, tag + "_"s + before_label + stub);
    auto shape = hin->shape();

    auto side0 = new history<TH1F>(tag + "_"s + before_label + stub + "_side0"s, "", null<TH1F>, shape);
    auto side1 = new history<TH1F>(tag + "_"s + before_label + stub + "_side1"s, "", null<TH1F>, shape);

    for (int64_t i = 0; i < hin->size(); ++i) {
        (*side0)[i] = fold((*hin)[i], nullptr, mr, 0, osr, fine);
        (*side1)[i] = fold((*hin)[i], nullptr, mr, 1, osr, fine);
    }

    normalise_to_unity(side0, side0);

    side0->rename(tag + "_"s + before_label + stub + "_side0"s);
    side1->rename(tag + "_"s + before_label + stub + "_side1"s);

    side0->save();
    side1->save();

    /* prepare the refolded data */
    std::vector<int64_t> iteration {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 35, 40, 45, 50, 55, 60, 65, 70, 80, 90, 100, 125, 150, 200, 250};

    for (size_t i = 0; i < iteration.size(); ++i) {
        auto refold0 = new history<TH1F>("refold0", "", null<TH1F>, (int64_t) afters.size());
        auto refold1 = new history<TH1F>("refold1", "", null<TH1F>, (int64_t) afters.size());

        for (size_t j = 0; j < fafters.size(); ++j) {
                    std::string name = "HRefoldedBayes" + std::to_string(iteration[i]);
                    auto HRefolded = (TH1F*) fafters[j]->Get(name.data());

                    (*refold0)[j] = fold(HRefolded, nullptr, mr, 0, osr, fine);
                    (*refold1)[j] = fold(HRefolded, nullptr, mr, 1, osr, fine);
        }

        normalise_to_unity(refold0, refold1);

        refold0->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_sum0_refold0_iteration"s + std::to_string(iteration[i]));
        refold1->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_sum0_refold1_iteration"s + std::to_string(iteration[i]));

        refold0->save();
        refold1->save();
    }

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return quantitate(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
