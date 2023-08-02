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

    auto before = conf->get<std::string>("before");
    auto before_label = conf->get<std::string>("before_label");
    auto before_figures = conf->get<std::vector<std::string>>("before_figures");
    auto before_folds = conf->get<std::vector<std::string>>("before_folds");

    auto afters = conf->get<std::vector<std::string>>("afters");
    auto merge = conf->get<std::string>("merge");

    auto regularization = conf->get<std::string>("regularization");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto rdrr = sel->get<std::vector<float>>("drr_range");
    auto rdrg = sel->get<std::vector<float>>("drg_range");
    auto rptr = sel->get<std::vector<float>>("ptr_range");
    auto rptg = sel->get<std::vector<float>>("ptg_range");

    auto osr = sel->get<std::vector<int64_t>>("osr");
    auto osg = sel->get<std::vector<int64_t>>("osg");

    /* create intervals and multivals */
    auto idrr = new interval("#deltaj"s, rdrr);
    auto iptr = new interval("p_{T}^{j}"s, rptr);

    auto idrg = new interval("#deltaj"s, rdrg);
    auto iptg = new interval("p_{T}^{j}"s, rptg);

    auto mr = new multival(*idrr, *iptr);
    auto mg = new multival(*idrg, *iptg);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    TFile* fbefore = new TFile((base + before).data(), "read");

    std::vector<TFile*> fafters(afters.size(), nullptr);
    zip([&](auto& fafter, auto const& after) {
        fafter = new TFile(("unfolded/" + set + "/" + after).data(), "read");
    }, fafters, afters);

    auto fmerge = new TFile(("unfolded/" + set + "/" + merge).data(), "read");

    TFile* fiter = new TFile((base + regularization).data(), "read");
    auto sum = new history<TH1F>(fiter, "sum"s);
    auto sum_merge = new history<TH1F>(fiter, "sum_merge"s);

    /* prepare output from pre-unfolded data */
    TFile* fout = new TFile(output, "recreate");

    zip([&](auto const& figure) {
        auto stub = "_"s + figure;

        auto hin = new history<TH1F>(fbefore, tag + "_"s + before_label + stub);
        hin->save();

    }, before_figures);

    /* prepare folds from pre-unfolded data */
    zip([&](auto const& figure) {
        auto stub = "_"s + figure;

        auto hin = new history<TH1F>(fbefore, tag + "_"s + before_label + stub);
        auto shape = hin->shape();

        auto side0 = new history<TH1F>(tag + "_"s + before_label + stub + "_side0"s, "", null<TH1F>, shape);
        auto side1 = new history<TH1F>(tag + "_"s + before_label + stub + "_side1"s, "", null<TH1F>, shape);

        for (int64_t i = 0; i < hin->size(); ++i) {
            (*side0)[i] = fold((*hin)[i], nullptr, mr, 0, osr);
            (*side1)[i] = fold((*hin)[i], nullptr, mr, 1, osr);
        }

        normalise_to_unity(side0, side1);

        side0->rename(tag + "_"s + before_label + stub + "_side0"s);
        side1->rename(tag + "_"s + before_label + stub + "_side1"s);

        side0->save();
        side1->save();

    }, before_folds);

    /* prepare the post-unfolded data */
    auto unfolded = new history<TH1F>("unfolded", "", null<TH1F>, (int64_t) afters.size());
    auto unfolded_fold0 = new history<TH1F>("unfolded_fold0", "", null<TH1F>, (int64_t) afters.size());
    auto unfolded_fold1 = new history<TH1F>("unfolded_fold1", "", null<TH1F>, (int64_t) afters.size());

    auto refolded = new history<TH1F>("refolded", "", null<TH1F>, (int64_t) afters.size());
    auto refolded_fold0 = new history<TH1F>("refolded_fold0", "", null<TH1F>, (int64_t) afters.size());
    auto refolded_fold1 = new history<TH1F>("refolded_fold1", "", null<TH1F>, (int64_t) afters.size());

    auto measured = new history<TH1F>("measured", "", null<TH1F>, (int64_t) afters.size());
    auto measured_fold0 = new history<TH1F>("measured_fold0", "", null<TH1F>, (int64_t) afters.size());
    auto measured_fold1 = new history<TH1F>("measured_fold1", "", null<TH1F>, (int64_t) afters.size());

    auto unfolded_merge = new history<TH1F>("unfolded_merge", "", null<TH1F>, 1);
    auto unfolded_merge_fold0 = new history<TH1F>("unfolded_merge_fold0", "", null<TH1F>, 1);
    auto unfolded_merge_fold1 = new history<TH1F>("unfolded_merge_fold1", "", null<TH1F>, 1);

    auto refolded_merge = new history<TH1F>("refolded_merge", "", null<TH1F>, 1);
    auto refolded_merge_fold0 = new history<TH1F>("refolded_merge_fold0", "", null<TH1F>, 1);
    auto refolded_merge_fold1 = new history<TH1F>("refolded_merge_fold1", "", null<TH1F>, 1);

    auto measured_merge = new history<TH1F>("measured_merge", "", null<TH1F>, 1);
    auto measured_merge_fold0 = new history<TH1F>("measured_merge_fold0", "", null<TH1F>, 1);
    auto measured_merge_fold1 = new history<TH1F>("measured_merge_fold1", "", null<TH1F>, 1);

    /* determine the number of iterations to use */
    std::vector<int64_t> choice(sum->size(), 1);
    int64_t choice_merge = 1;

    for (int i = 0; i < sum->size(); ++i) {
        double min = 99999999999;

        for (int j = 1; j <= (*sum)[i]->GetNbinsX(); ++j) {
            auto top = (*sum)[i]->GetBinContent(j);

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

        std::cout << std::endl << choice[i] << std::endl;
    }

    double min = 99999999999;

    for (int j = 1; j <= (*sum_merge)[0]->GetNbinsX(); ++j) {
        auto top = (*sum_merge)[0]->GetBinContent(j);

        if (top == 0) { continue; }

        std::cout << top << " ";

        if (top < min) {
            min = top;
            choice_merge = j;
        }
        else {
            break;
        }
    }

    std::cout << std::endl << choice_merge << std::endl;

    /* extract chosen histograms */
    for (size_t j = 0; j < fafters.size(); ++j) {
        std::string unfold_name = "HUnfoldedBayes" + std::to_string(choice[j]);
        std::string matrix_name = "MUnfoldedBayes" + std::to_string(choice[j]);
        std::string refold_name = "HRefoldedBayes" + std::to_string(choice[j]);

        auto HUnfoldedBayes = (TH1F*) fafters[j]->Get(unfold_name.data());
        auto MUnfolded = (TMatrixT<double>*) fafters[j]->Get(matrix_name.data());
        auto HRefolded = (TH1F*) fafters[j]->Get(refold_name.data());
        auto HMeasured = (TH1F*) fafters[j]->Get("HMCMeasured");

        (*unfolded)[j] = HUnfoldedBayes;
        (*unfolded_fold0)[j] = fold_mat(HUnfoldedBayes, MUnfolded, mg, 0, osg);
        (*unfolded_fold1)[j] = fold_mat(HUnfoldedBayes, MUnfolded, mg, 1, osg);

        (*refolded)[j] = HRefolded;
        (*refolded_fold0)[j] = fold(HRefolded, nullptr, mr, 0, osr);
        (*refolded_fold1)[j] = fold(HRefolded, nullptr, mr, 1, osr);

        (*measured)[j] = HMeasured;
        (*measured_fold0)[j] = fold(HMeasured, nullptr, mr, 0, osr);
        (*measured_fold1)[j] = fold(HMeasured, nullptr, mr, 1, osr);
    }

    normalise_to_unity(unfolded_fold0, unfolded_fold1, refolded_fold0, refolded_fold1, measured_fold0, measured_fold1);
    
    if (fafters.size() == 4) {
        int entries0, entries1, entries2, entries3;
        
        entries0 = (*unfolded)[0]->GetEntries(); entries1 = (*unfolded)[1]->GetEntries(); entries2 = (*unfolded)[2]->GetEntries(); entries3 = (*unfolded)[3]->GetEntries();

        (*unfolded_merge)[0] = (TH1F*) (*unfolded)[0]->Clone("unfolded_merge");
        (*unfolded_merge)[0]->Add((*unfolded)[0], (*unfolded)[1], entries0, entries1);
        (*unfolded_merge)[0]->Add((*unfolded)[2], entries2);
        (*unfolded_merge)[0]->Add((*unfolded)[3], entries3);
        (*unfolded_merge)[0]->Scale(1/(entries0 + entries1 + entries2 + entries3));

        entries0 = (*unfolded_fold0)[0]->GetEntries(); entries1 = (*unfolded_fold0)[1]->GetEntries(); entries2 = (*unfolded_fold0)[2]->GetEntries(); entries3 = (*unfolded_fold0)[3]->GetEntries();

        (*unfolded_merge_fold0)[0] = (TH1F*) (*unfolded_fold0)[0]->Clone("unfolded_merge");
        (*unfolded_merge_fold0)[0]->Add((*unfolded_fold0)[0], (*unfolded_fold0)[1], entries0, entries1);
        (*unfolded_merge_fold0)[0]->Add((*unfolded_fold0)[2], entries2);
        (*unfolded_merge_fold0)[0]->Add((*unfolded_fold0)[3], entries3);
        (*unfolded_merge_fold0)[0]->Scale(1/(entries0 + entries1 + entries2 + entries3));

        entries0 = (*unfolded_fold1)[0]->GetEntries(); entries1 = (*unfolded_fold1)[1]->GetEntries(); entries2 = (*unfolded_fold1)[2]->GetEntries(); entries3 = (*unfolded_fold1)[3]->GetEntries();

        (*unfolded_merge_fold1)[0] = (TH1F*) (*unfolded_fold1)[0]->Clone("unfolded_merge");
        (*unfolded_merge_fold1)[0]->Add((*unfolded_fold1)[0], (*unfolded_fold1)[1], entries0, entries1);
        (*unfolded_merge_fold1)[0]->Add((*unfolded_fold1)[2], entries2);
        (*unfolded_merge_fold1)[0]->Add((*unfolded_fold1)[3], entries3);
        (*unfolded_merge_fold1)[0]->Scale(1/(entries0 + entries1 + entries2 + entries3));

        entries0 = (*refolded)[0]->GetEntries(); entries1 = (*refolded)[1]->GetEntries(); entries2 = (*refolded)[2]->GetEntries(); entries3 = (*refolded)[3]->GetEntries();

        (*refolded_merge)[0] = (TH1F*) (*refolded)[0]->Clone("unfolded_merge");
        (*refolded_merge)[0]->Add((*refolded)[0], (*refolded)[1], entries0, entries1);
        (*refolded_merge)[0]->Add((*refolded)[2], entries2);
        (*refolded_merge)[0]->Add((*refolded)[3], entries3);
        (*refolded_merge)[0]->Scale(1/(entries0 + entries1 + entries2 + entries3));

        entries0 = (*refolded_fold0)[0]->GetEntries(); entries1 = (*refolded_fold0)[1]->GetEntries(); entries2 = (*refolded_fold0)[2]->GetEntries(); entries3 = (*refolded_fold0)[3]->GetEntries();

        (*refolded_merge_fold0)[0] = (TH1F*) (*refolded_fold0)[0]->Clone("unfolded_merge");
        (*refolded_merge_fold0)[0]->Add((*refolded_fold0)[0], (*refolded_fold0)[1], entries0, entries1);
        (*refolded_merge_fold0)[0]->Add((*refolded_fold0)[2], entries2);
        (*refolded_merge_fold0)[0]->Add((*refolded_fold0)[3], entries3);
        (*refolded_merge_fold0)[0]->Scale(1/(entries0 + entries1 + entries2 + entries3));

        entries0 = (*refolded_fold1)[0]->GetEntries(); entries1 = (*refolded_fold1)[1]->GetEntries(); entries2 = (*refolded_fold1)[2]->GetEntries(); entries3 = (*refolded_fold1)[3]->GetEntries();

        (*refolded_merge_fold1)[0] = (TH1F*) (*refolded_fold1)[0]->Clone("unfolded_merge");
        (*refolded_merge_fold1)[0]->Add((*refolded_fold1)[0], (*refolded_fold1)[1], entries0, entries1);
        (*refolded_merge_fold1)[0]->Add((*refolded_fold1)[2], entries2);
        (*refolded_merge_fold1)[0]->Add((*refolded_fold1)[3], entries3);
        (*refolded_merge_fold1)[0]->Scale(1/(entries0 + entries1 + entries2 + entries3));

        entries0 = (*measured)[0]->GetEntries(); entries1 = (*measured)[1]->GetEntries(); entries2 = (*measured)[2]->GetEntries(); entries3 = (*measured)[3]->GetEntries();

        (*measured_merge)[0] = (TH1F*) (*measured)[0]->Clone("unfolded_merge");
        (*measured_merge)[0]->Add((*measured)[0], (*measured)[1], entries0, entries1);
        (*measured_merge)[0]->Add((*measured)[2], entries2);
        (*measured_merge)[0]->Add((*measured)[3], entries3);
        (*measured_merge)[0]->Scale(1/(entries0 + entries1 + entries2 + entries3));

        entries0 = (*measured_fold0)[0]->GetEntries(); entries1 = (*measured_fold0)[1]->GetEntries(); entries2 = (*measured_fold0)[2]->GetEntries(); entries3 = (*measured_fold0)[3]->GetEntries();

        (*measured_merge_fold0)[0] = (TH1F*) (*measured_fold0)[0]->Clone("unfolded_merge");
        (*measured_merge_fold0)[0]->Add((*measured_fold0)[0], (*measured_fold0)[1], entries0, entries1);
        (*measured_merge_fold0)[0]->Add((*measured_fold0)[2], entries2);
        (*measured_merge_fold0)[0]->Add((*measured_fold0)[3], entries3);
        (*measured_merge_fold0)[0]->Scale(1/(entries0 + entries1 + entries2 + entries3));

        entries0 = (*measured_fold1)[0]->GetEntries(); entries1 = (*measured_fold1)[1]->GetEntries(); entries2 = (*measured_fold1)[2]->GetEntries(); entries3 = (*measured_fold1)[3]->GetEntries();

        (*measured_merge_fold1)[0] = (TH1F*) (*measured_fold1)[0]->Clone("unfolded_merge");
        (*measured_merge_fold1)[0]->Add((*measured_fold1)[0], (*measured_fold1)[1], entries0, entries1);
        (*measured_merge_fold1)[0]->Add((*measured_fold1)[2], entries2);
        (*measured_merge_fold1)[0]->Add((*measured_fold1)[3], entries3);
        (*measured_merge_fold1)[0]->Scale(1/(entries0 + entries1 + entries2 + entries3));
    } else {
        std::string unfold_name = "HUnfoldedBayes" + std::to_string(choice_merge);
        std::string matrix_name = "MUnfoldedBayes" + std::to_string(choice_merge);
        std::string refold_name = "HRefoldedBayes" + std::to_string(choice_merge);

        auto HUnfoldedBayes = (TH1F*) fmerge->Get(unfold_name.data());
        auto MUnfolded = (TMatrixT<double>*) fmerge->Get(matrix_name.data());
        auto HRefolded = (TH1F*) fmerge->Get(refold_name.data());
        auto HMeasured = (TH1F*) fmerge->Get("HMCMeasured");

        (*unfolded_merge)[0] = HUnfoldedBayes;
        (*unfolded_merge_fold0)[0] = fold_mat(HUnfoldedBayes, MUnfolded, mg, 0, osg);
        (*unfolded_merge_fold1)[0] = fold_mat(HUnfoldedBayes, MUnfolded, mg, 1, osg);

        (*refolded_merge)[0] = HRefolded;
        (*refolded_merge_fold0)[0] = fold(HRefolded, nullptr, mr, 0, osr);
        (*refolded_merge_fold1)[0] = fold(HRefolded, nullptr, mr, 1, osr);

        (*measured_merge)[0] = HMeasured;
        (*measured_merge_fold0)[0] = fold(HMeasured, nullptr, mr, 0, osr);
        (*measured_merge_fold1)[0] = fold(HMeasured, nullptr, mr, 1, osr);
    }

    normalise_to_unity(unfolded_merge_fold0, unfolded_merge_fold1, refolded_merge_fold0, refolded_merge_fold1, measured_merge_fold0, measured_merge_fold1);

    /* rename histograms */
    unfolded->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_sum0_unfolded"s);
    unfolded_fold0->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_sum0_unfolded_fold0"s);
    unfolded_fold1->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_sum0_unfolded_fold1"s);

    refolded->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_sum0_refolded"s);
    refolded_fold0->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_sum0_refolded_fold0"s);
    refolded_fold1->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_sum0_refolded_fold1"s);

    measured->rename(tag + "_r");
    measured_fold0->rename(tag + "_r_fold0");
    measured_fold1->rename(tag + "_r_fold1");

    unfolded_merge->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_merge_unfolded"s);
    unfolded_merge_fold0->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_merge_unfolded_fold0"s);
    unfolded_merge_fold1->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_merge_unfolded_fold1"s);

    refolded_merge->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_merge_refolded"s);
    refolded_merge_fold0->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_merge_refolded_fold0"s);
    refolded_merge_fold1->rename(tag + "_"s + before_label + "_raw_sub_pjet_u_dr_merge_refolded_fold1"s);

    measured_merge->rename(tag + "_r_merge");
    measured_merge_fold0->rename(tag + "_r_merge_fold0");
    measured_merge_fold1->rename(tag + "_r_merge_fold1");

    /* save histograms */
    unfolded->save();
    unfolded_fold0->save();
    unfolded_fold1->save();

    refolded->save();
    refolded_fold0->save();
    refolded_fold1->save();

    measured->save();
    measured_fold0->save();
    measured_fold1->save();

    unfolded_merge->save();
    unfolded_merge_fold0->save();
    unfolded_merge_fold1->save();

    refolded_merge->save();
    refolded_merge_fold0->save();
    refolded_merge_fold1->save();

    measured_merge->save();
    measured_merge_fold0->save();
    measured_merge_fold1->save();

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return quantitate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
