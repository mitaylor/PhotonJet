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

TH1F* fold_mat(TH1* flat, TMatrixT<double>* covariance, multival const* m, int64_t axis,
           std::vector<int64_t>& offsets, std::string name) {
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
    auto before_label = conf->get<std::string>("before_label");
    auto afters = conf->get<std::vector<std::string>>("afters");
    auto regularization = conf->get<std::string>("regularization");

    auto fix = conf->get<std::vector<float>>("fix");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto rdrg = sel->get<std::vector<float>>("drg_range");
    auto rptg = sel->get<std::vector<float>>("ptg_range");

    auto osg = sel->get<std::vector<int64_t>>("osg");

    /* create intervals and multivals */
    auto idrg = new interval("#deltaj"s, rdrg);
    auto iptg = new interval("p_{T}^{j}"s, rptg);

    auto mg = new multival(*idrg, *iptg);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    std::vector<int64_t> cut = {1, 2, 3, 4, 5};

    std::vector<TFile*> fafters(afters.size(), nullptr);
    zip([&](auto& fafter, auto const& after) {
        fafter = new TFile(("unfolded/" + set + "/" + after).data(), "read");
    }, fafters, afters);

    TFile* fiter = new TFile((base + regularization).data(), "read");
    auto chi_square = new history<TH1F>(fiter, "test_chi_square"s);

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    /* prepare the post-unfolded data */
    auto unfolded = new history<TH1F>("unfolded", "", null<TH1F>, (int64_t) afters.size());

    std::vector<history<TH1F>*> unfolded_fold0(cut.size(), nullptr);
    std::vector<history<TH1F>*> unfolded_fold1(cut.size(), nullptr);

    for (int64_t i = 0; i < (int64_t) cut.size(); ++i) { 
        auto name0 = "unfolded_"s + std::to_string(i) + "fold0"s;
        auto name1 = "unfolded_"s + std::to_string(i) + "fold1"s;

        unfolded_fold0[i] = new history<TH1F>(name0.data(), "", null<TH1F>, (int64_t) afters.size());
        unfolded_fold1[i] = new history<TH1F>(name1.data(), "", null<TH1F>, (int64_t) afters.size());
    }

    /* determine the number of iterations to use */
    std::vector<int64_t> choice(chi_square->size(), 1);

    for (int64_t i = 0; i < chi_square->size(); ++i) {
        double min = 99999999999;

        for (int64_t j = 0; j < (*chi_square)[i]->GetNbinsX(); ++j) {
            auto top = (*chi_square)[i]->GetBinContent(j + 1) + (*chi_square)[i]->GetBinError(j + 1);

            if (top == 0) { continue; }

            std::cout << top << " ";

            if (top < min) {
                min = top;
                choice[i] = j;
            }
            else {
                break;
            }
        }

        if (fix.size() == choice.size()) { choice[i] = fix[i]; }

        std::cout << std::endl << choice[i] << std::endl;
    }

    for (int64_t j = 0; j < (int64_t) fafters.size(); ++j) {
        std::string unfold_name = "HUnfoldedBayes" + std::to_string(choice[j]);
        std::string matrix_name = "MUnfoldedBayes" + std::to_string(choice[j]);

        auto HUnfoldedBayes = (TH1F*) fafters[j]->Get(unfold_name.data());
        auto MUnfolded = (TMatrixT<double>*) fafters[j]->Get(matrix_name.data());

        (*unfolded)[j] = HUnfoldedBayes;

        for (int64_t i = 0; i < (int64_t) cut.size(); ++i) { 
            osg[3] = cut[i];

            (*unfolded_fold0[i])[j] = fold_mat((*unfolded)[j], MUnfolded, mg, 0, osg, std::to_string(i) + "_" + std::to_string(j) + "_fold0");
            (*unfolded_fold1[i])[j] = fold_mat((*unfolded)[j], MUnfolded, mg, 1, osg, std::to_string(i) + "_" + std::to_string(j) + "_fold1");
        }
    }

    for (int64_t i = 0; i < (int64_t) cut.size(); ++i) { 
        normalise_to_unity(unfolded_fold0[i], unfolded_fold1[i]);

        unfolded_fold0[i]->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_sum0_unfolded_"s + std::to_string(i) + "_fold0"s);
        unfolded_fold1[i]->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_sum0_unfolded_"s + std::to_string(i) + "_fold1"s);

        unfolded_fold0[i]->save();
        unfolded_fold1[i]->save();
    }

    unfolded->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_sum0_unfolded"s);
    unfolded->save();

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return quantitate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
