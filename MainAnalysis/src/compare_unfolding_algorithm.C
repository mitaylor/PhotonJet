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

TH1F* fold_mat(TH1* flat, TMatrixT<double>* covariance, multival const* m, int64_t axis,
           std::vector<int64_t>& offsets) {
    auto name = std::string(flat->GetName()) + "_fold" + std::to_string(axis);std::cout << __LINE__ << std::endl;
    auto hfold = m->axis(axis).book<TH1F, 2>(0, name, "",
        { offsets[axis << 1], offsets[(axis << 1) + 1] });
std::cout << __LINE__ << std::endl;
    auto shape = m->shape();std::cout << __LINE__ << std::endl;

    auto front = std::vector<int64_t>(m->dims(), 0);std::cout << __LINE__ << std::endl;
    auto back = std::vector<int64_t>(m->dims(), 0);std::cout << __LINE__ << std::endl;
    for (int64_t i = 0; i < m->dims(); ++i) {
        front[i] = offsets[i << 1];
        back[i] = shape[i] - offsets[(i << 1) + 1];
    }
std::cout << __LINE__ << std::endl;
    auto size = back[axis] - front[axis];std::cout << __LINE__ << std::endl;
    auto list = new std::vector<int64_t>[size];std::cout << __LINE__ << std::endl;
std::cout << __LINE__ << std::endl;
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
std::cout << __LINE__ << std::endl;
    for (int64_t i = 0; i < size; ++i) {
        auto indices = list[i];
        int64_t count = indices.size();
std::cout << __LINE__ << std::endl;
        auto error = 0.;
        for (int64_t j = 0; j < count; ++j) {
            auto j_x = indices[j];std::cout << __LINE__ << std::endl;
            for (int64_t k = 0; k < count; ++k) {
                auto k_x = indices[k];std::cout << __LINE__ << std::endl;
                std::cout << error << std::endl; std::cout << (covariance == nullptr) << std::endl;
                std::cout << (*covariance)(1, 1) << std::endl;
                error = error + (*covariance)(j_x, k_x);std::cout << __LINE__ << std::endl;
            }
        }
std::cout << __LINE__ << std::endl;
        hfold->SetBinError(i + 1, std::sqrt(error));
    }
std::cout << __LINE__ << std::endl;
    delete [] list;
std::cout << __LINE__ << std::endl;
    hfold->Scale(1., "width");
std::cout << __LINE__ << std::endl;
    return hfold;
}

int quantitate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto tag = conf->get<std::string>("tag");

    auto filenames = conf->get<std::vector<std::string>>("filenames");

    // auto label = conf->get<std::string>("label");
    auto prior = conf->get<std::string>("prior");

    auto file_svd = conf->get<std::string>("file_svd");
    auto file_bayes = conf->get<std::string>("file_bayes");

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

    std::vector<TFile*> fdata_svd(filenames.size(), nullptr);
    std::vector<TFile*> fdata_bayes(filenames.size(), nullptr);

    zip([&](auto& fsvd, auto& fbayes, auto const& filename) {
        fsvd = new TFile(("unfolded/Data/"s + set + "/SVD/"s + prior + "/kErrors/"s + filename).data(), "read");
        fbayes = new TFile(("unfolded/Data/"s + set + "/Bayes/"s + prior + "/kErrors/"s + filename).data(), "read");
        std::cout << "unfolded/Data/"s + set + "/SVD/"s + prior + "/kErrors/"s + filename << std::endl;
        std::cout << "unfolded/Data/"s + set + "/Bayes/"s + prior + "/kErrors/"s + filename << std::endl;
    }, fdata_svd, fdata_bayes, filenames);

    TFile* freg_svd = new TFile((base + file_svd).data(), "read");
    TFile* freg_bayes = new TFile((base + file_bayes).data(), "read");

    auto regularization_svd = new history<TH1F>(freg_svd, tag + "_mse"s);
    auto regularization_bayes = new history<TH1F>(freg_bayes, tag + "_mse"s);

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    /* prepare data */
    auto unfolded_svd = new history<TH1F>("unfolded_svd", "", null<TH1F>, (int64_t) filenames.size());
    auto unfolded_svd_fold0 = new history<TH1F>("unfolded_svd_fold0", "", null<TH1F>, (int64_t) filenames.size());
    auto unfolded_svd_fold1 = new history<TH1F>("unfolded_svd_fold1", "", null<TH1F>, (int64_t) filenames.size());

    auto unfolded_bayes = new history<TH1F>("unfolded_bayes", "", null<TH1F>, (int64_t) filenames.size());
    auto unfolded_bayes_fold0 = new history<TH1F>("unfolded_bayes_fold0", "", null<TH1F>, (int64_t) filenames.size());
    auto unfolded_bayes_fold1 = new history<TH1F>("unfolded_bayes_fold1", "", null<TH1F>, (int64_t) filenames.size());

    /* determine the regularization to use */
    std::vector<int64_t> choice_svd(filenames.size(), 1);
    std::vector<int64_t> choice_bayes(filenames.size(), 1);

    for (size_t i = 0; i < filenames.size(); ++i) {
        choice_svd[i] = (*regularization_svd)[i]->GetMinimumBin();
        choice_bayes[i] = (*regularization_bayes)[i]->GetMinimumBin();
    }

    /* extract chosen histograms */
    for (size_t j = 0; j < filenames.size(); ++j) {
        std::string unfold_name_svd = "HUnfoldedSVD" + std::to_string(choice_svd[j]);
        std::string matrix_name_svd = "MUnfoldedSVD" + std::to_string(choice_svd[j]);

        std::string unfold_name_bayes = "HUnfoldedBayes" + std::to_string(choice_bayes[j]);
        std::string matrix_name_bayes = "MUnfoldedBayes" + std::to_string(choice_bayes[j]);
std::cout << unfold_name_svd << " " << matrix_name_svd << " " << unfold_name_bayes << " " << matrix_name_bayes << std::endl;
        auto HUnfoldedSVD = (TH1F*) fdata_svd[j]->Get(unfold_name_svd.data());
        auto MUnfoldedSVD = (TMatrixT<double>*) fdata_svd[j]->Get(unfold_name_svd.data());

        auto HUnfoldedBayes = (TH1F*) fdata_bayes[j]->Get(unfold_name_bayes.data());std::cout << __LINE__ << std::endl;
        auto MUnfoldedBayes = (TMatrixT<double>*) fdata_bayes[j]->Get(unfold_name_bayes.data());std::cout << __LINE__ << std::endl;
        std::cout << MUnfoldedBayes->GetNcols() << " " << MUnfoldedBayes->GetNrows() << std::endl;
        std::cout << MUnfoldedSVD->GetNcols() << " " << MUnfoldedSVD->GetNrows() << std::endl;
        (*unfolded_bayes)[j] = HUnfoldedBayes;std::cout << __LINE__ << std::endl;
        (*unfolded_bayes_fold0)[j] = fold_mat(HUnfoldedBayes, MUnfoldedBayes, mg, 0, osg);std::cout << __LINE__ << std::endl;
        (*unfolded_bayes_fold1)[j] = fold_mat(HUnfoldedBayes, MUnfoldedBayes, mg, 1, osg);std::cout << __LINE__ << std::endl;

        (*unfolded_svd)[j] = HUnfoldedSVD;std::cout << __LINE__ << std::endl;
        (*unfolded_svd_fold0)[j] = fold_mat(HUnfoldedSVD, MUnfoldedSVD, mg, 0, osg);std::cout << __LINE__ << std::endl;
        (*unfolded_svd_fold1)[j] = fold_mat(HUnfoldedSVD, MUnfoldedSVD, mg, 1, osg);std::cout << __LINE__ << std::endl;
    }
std::cout << __LINE__ << std::endl;
    /* rename histograms */
    unfolded_svd->rename("unfolded_svd");
    unfolded_svd_fold0->rename("unfolded_svd_fold0");
    unfolded_svd_fold1->rename("unfolded_svd_fold1");
std::cout << __LINE__ << std::endl;
    unfolded_bayes->rename("unfolded_bayes");
    unfolded_bayes_fold0->rename("unfolded_bayes_fold0");
    unfolded_bayes_fold1->rename("unfolded_bayes_fold1");
std::cout << __LINE__ << std::endl;
    /* save histograms */
    unfolded_svd->save();
    unfolded_svd_fold0->save();
    unfolded_svd_fold1->save();
std::cout << __LINE__ << std::endl;
    unfolded_bayes->save();
    unfolded_bayes_fold0->save();
    unfolded_bayes_fold1->save();
std::cout << __LINE__ << std::endl;
    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return quantitate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
