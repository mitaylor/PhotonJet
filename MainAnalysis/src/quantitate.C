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

TH2F* get_covariance(TMatrixT<double>* covariance, TH1* efficiency) {
    int N = covariance->GetNrows();
    auto hcovariance = new TH2F("covariance", "", N, 0, N, N, 0, N);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            float value = (*covariance)(i, j) / efficiency->GetBinContent(i + 1) / efficiency->GetBinContent(j + 1);
            hcovariance->SetBinContent(i + 1, j + 1, value);
        }
    }

    return hcovariance;
}

int quantitate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto tag = conf->get<std::string>("tag");

    auto before = conf->get<std::string>("before");
    auto before_label = conf->get<std::string>("before_label");
    auto before_figures = conf->get<std::vector<std::string>>("before_figures");
    auto before_folds = conf->get<std::vector<std::string>>("before_folds");

    auto afters = conf->get<std::vector<std::string>>("afters");
    auto errors = conf->get<std::vector<std::string>>("errors");
    auto regularization = conf->get<std::string>("regularization");
    auto qcd = conf->get<bool>("qcd");
    auto stat = conf->get<bool>("stat");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto rdrr = sel->get<std::vector<float>>("drr_range");
    auto rdrg = sel->get<std::vector<float>>("drg_range");
    auto rptr = sel->get<std::vector<float>>("ptr_range");
    auto rptg = sel->get<std::vector<float>>("ptg_range");

    auto osr = sel->get<std::vector<int64_t>>("osr");
    auto osg = sel->get<std::vector<int64_t>>("osg");

    auto osg_part1 = sel->get<std::vector<int64_t>>("osg_part1");
    auto osg_part2 = sel->get<std::vector<int64_t>>("osg_part2");

    /* create intervals and multivals */
    // auto idrr = new interval("#deltaj"s, rdrr);
    // auto iptr = new interval("p_{T}^{j}"s, rptr);

    auto idrg = new interval("#deltaj"s, rdrg);
    auto iptg = new interval("p_{T}^{j}"s, rptg);

    // auto mr = new multival(*idrr, *iptr);
    auto mg = new multival(*idrg, *iptg);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open input files */
    TFile* fbefore = new TFile((base + before).data(), "read");

    std::vector<TFile*> fafters(afters.size(), nullptr);
    zip([&](auto& fafter, auto const& after) {
        fafter = new TFile(("unfolded/Data/"s + set + "/Bayes/Original/kErrors/"s + after).data(), "read");
    }, fafters, afters);

    std::vector<TFile*> ferrors(errors.size(), nullptr);
    zip([&](auto& ferror, auto const& error) {
        ferror = new TFile(("unfolded/Errors/"s + set + "/Bayes/Original/kErrors/"s + error).data(), "read");
    }, ferrors, errors);

    TFile* freg = new TFile((base + regularization).data(), "read");
    auto reg = new history<TH1F>(freg, tag + "_mse"s);

    /* prepare output from pre-unfolded data */
    TFile* fout = new TFile(output, "recreate");

    zip([&](auto const& figure) {
        auto stub = "_"s + figure;

        auto hin = new history<TH1F>(fbefore, tag + "_"s + before_label + stub);
        hin->save();

    }, before_figures);

    /* prepare the post-unfolded data */
    auto unfolded = new history<TH1F>("unfolded", "", null<TH1F>, (int64_t) afters.size());
    auto unfolded_fold0 = new history<TH1F>("unfolded_fold0", "", null<TH1F>, (int64_t) afters.size());
    auto unfolded_fold1 = new history<TH1F>("unfolded_fold1", "", null<TH1F>, (int64_t) afters.size());
    auto unfolded_fold0_part1 = new history<TH1F>("unfolded_fold0_part1", "", null<TH1F>, (int64_t) afters.size());
    auto unfolded_fold1_part1 = new history<TH1F>("unfolded_fold1_part1", "", null<TH1F>, (int64_t) afters.size());
    auto unfolded_fold0_part2 = new history<TH1F>("unfolded_fold0_part2", "", null<TH1F>, (int64_t) afters.size());
    auto unfolded_fold1_part2 = new history<TH1F>("unfolded_fold1_part2", "", null<TH1F>, (int64_t) afters.size());
    auto covariance = new history<TH2F>("covariance", "", null<TH2F>, (int64_t) afters.size());

    /* determine the regularization to use */
    std::vector<int64_t> choice(afters.size(), 1);

    for (size_t i = 0; i < afters.size(); ++i) {
        choice[i] = (*reg)[i]->GetMinimumBin();
        if (!qcd && choice[i] == 1) choice[i] = 2;
        std::cout << choice[i] << std::endl;
    }

    /* extract chosen histograms */
    for (size_t j = 0; j < fafters.size(); ++j) {
        std::string unfold_name = "HUnfoldedBayes" + std::to_string(choice[j]);
        std::string stat_name = "Test0HUnfoldedBayes" + std::to_string(choice[j]);
        std::string calc_name = "MUnfoldedBayes" + std::to_string(choice[j]);
        std::string toys_name = "HCovarianceDist" + std::to_string(choice[j]);
        std::string efficiency_name = "HMCTruthEfficiency";
        
        if (qcd) {
            auto HUnfoldedBayes = (TH1F*) fafters[j]->Get(unfold_name.data());
            auto MUnfolded = (TMatrixT<double>*) fafters[j]->Get(calc_name.data());
            auto Efficiency = (TH1F*) fafters[j]->Get(efficiency_name.data());

            (*covariance)[j] = get_covariance(MUnfolded, Efficiency);
            (*unfolded)[j] = HUnfoldedBayes;
        }
        else if (stat) {
            auto HUnfoldedBayes = (TH1F*) fafters[j]->Get(stat_name.data());
            auto MUnfolded = (TH2F*) ferrors[j]->Get(toys_name.data());

            (*covariance)[j] = MUnfolded;
            (*unfolded)[j] = HUnfoldedBayes;
        }
        else {
            auto HUnfoldedBayes = (TH1F*) fafters[j]->Get(unfold_name.data());
            auto MUnfolded = (TH2F*) ferrors[j]->Get(toys_name.data());

            (*covariance)[j] = MUnfolded;
            (*unfolded)[j] = HUnfoldedBayes;
        }

        (*unfolded_fold0)[j] = fold((*unfolded)[j], (*covariance)[j], mg, 0, osg);
        (*unfolded_fold1)[j] = fold((*unfolded)[j], (*covariance)[j], mg, 1, osg);
        (*unfolded_fold0_part1)[j] = fold((*unfolded)[j], (*covariance)[j], mg, 0, osg_part1);
        (*unfolded_fold1_part1)[j] = fold((*unfolded)[j], (*covariance)[j], mg, 1, osg_part1);
        (*unfolded_fold0_part2)[j] = fold((*unfolded)[j], (*covariance)[j], mg, 0, osg_part2);
        (*unfolded_fold1_part2)[j] = fold((*unfolded)[j], (*covariance)[j], mg, 1, osg_part2);
    }

    /* rename histograms */
    covariance->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_jpt_sum0_covariance"s);
    unfolded->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_jpt_sum0_unfolded"s);
    unfolded_fold0->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_jpt_sum0_unfolded_fold0"s);
    unfolded_fold1->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_jpt_sum0_unfolded_fold1"s);
    unfolded_fold0_part1->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_jpt_sum0_unfolded_fold0_part1"s);
    unfolded_fold1_part1->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_jpt_sum0_unfolded_fold1_part1"s);
    unfolded_fold0_part2->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_jpt_sum0_unfolded_fold0_part2"s);
    unfolded_fold1_part2->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_jpt_sum0_unfolded_fold1_part2"s);

    /* save histograms */
    covariance->save();
    unfolded->save();
    unfolded_fold0->save();
    unfolded_fold1->save();
    unfolded_fold0_part1->save();
    unfolded_fold1_part1->save();
    unfolded_fold0_part2->save();
    unfolded_fold1_part2->save();

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return quantitate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
