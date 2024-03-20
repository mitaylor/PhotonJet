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

    return hfold;
}

TH1F *forward_fold(TH1 *HGen, TH2F *HResponse)
{
    if(HGen == nullptr || HResponse == nullptr) {
        return nullptr;
    }

    static int Count = 0;
    Count = Count + 1;

    int NGen = HResponse->GetNbinsY();
    int NReco = HResponse->GetNbinsX();

    TH1F *HResult = new TH1F(Form("HFold%d", Count), "", NReco, 0, NReco);

    HResult->Sumw2();

    for(int iG = 1; iG <= NGen; iG++)
    {
        float N = 0;
        for (int iR = 1; iR <= NReco; iR++)
            N = N + HResponse->GetBinContent(iR, iG);

        if (N == 0) { continue; }

        for (int iR = 1; iR <= NReco; iR++) {
            float T = HResponse->GetBinContent(iR, iG) / N;
            float G = HGen->GetBinContent(iG);

            float V = T * G;

            float Current = HResult->GetBinContent(iR);
            HResult->SetBinContent(iR, Current + V);
            HResult->SetBinError(iR, 0);
        }
    }

    return HResult;
}


int bottom_line_test(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto tag = conf->get<std::string>("tag");
    auto plot_name = conf->get<std::string>("plot_name");

    auto before_file = conf->get<std::string>("before_file");
    auto before_label = conf->get<std::string>("before_label");

    auto after_file = conf->get<std::vector<std::string>>("after_file");

    auto theory_file = conf->get<std::string>("theory_file");
    auto theory_label = conf->get<std::string>("theory_label");

    auto matrix_file = conf->get<std::string>("matrix_file");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto rdrr = sel->get<std::vector<float>>("drr_range");
    auto rdrg = sel->get<std::vector<float>>("drg_range");
    auto rptr = sel->get<std::vector<float>>("ptr_range");
    auto rptg = sel->get<std::vector<float>>("ptg_range");

    auto rpt = sel->get<std::vector<float>>("photon_pt_bounds");

    auto mpthf = new multival(rpt, dhf);

    // auto osr = sel->get<std::vector<int64_t>>("osr");
    // auto osg = sel->get<std::vector<int64_t>>("osg");  

    std::vector<int64_t> osr {0, 0, 0, 0};
    std::vector<int64_t> osg {0, 0, 0, 0};

    /* create intervals and multivals */
    auto idrr = new interval("#deltaj"s, rdrr);
    auto iptr = new interval("p_{T}^{j}"s, rptr);

    auto idrg = new interval("#deltaj"s, rdrg);
    auto iptg = new interval("p_{T}^{j}"s, rptg);

    auto mr = new multival(*idrr, *iptr);
    auto mg = new multival(*idrg, *iptg);

    int size = after_file.size();

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();
    
    TFile* fbefore = new TFile((base + before_file).data(), "read");
    
    std::vector<TFile*> fafter(size, nullptr);
    
    zip([&](auto& fafter, auto const& after) {
        fafter = new TFile(("unfolded/Data/"s + set + "/Bayes/MC/kErrors/"s + after).data(), "read");
    }, fafter, after_file);
    
    TFile* ftheory = new TFile((base + theory_file).data(), "read");
    TFile* fmatrix = new TFile((base + matrix_file).data(), "read");
    
    /* data before unfolding */
    TFile* fout = new TFile(output, "recreate");
    
    auto data_before = new history<TH1F>(fbefore, tag + "_"s + before_label);
    auto data_before_fold0 = new history<TH1F>("data_before_fold0"s, "", null<TH1F>, data_before->shape());
    auto data_before_fold1 = new history<TH1F>("data_before_fold1"s, "", null<TH1F>, data_before->shape());
    
    for (int64_t i = 0; i < size; ++i) {
        (*data_before_fold0)[i] = fold((*data_before)[i], nullptr, mr, 0, osr);
        (*data_before_fold1)[i] = fold((*data_before)[i], nullptr, mr, 1, osr);
    }
    
    data_before->rename("data_before"s);
    data_before_fold0->rename("data_before_fold0"s);
    data_before_fold1->rename("data_before_fold1"s);
    
    data_before->save();
    data_before_fold0->save();
    data_before_fold1->save();
    
    std::vector<TMatrixT<double>*> data_before_vector(size, nullptr);
    std::vector<TMatrixT<double>*> data_before_vector_fold0(size, nullptr);
    std::vector<TMatrixT<double>*> data_before_vector_fold1(size, nullptr);

    for (int i = 0; i < size; ++i) {
        std::vector<double> data_before_elements((*data_before)[i]->GetNbinsX());
        std::vector<double> data_before_fold0_elements((*data_before_fold0)[i]->GetNbinsX());
        std::vector<double> data_before_fold1_elements((*data_before_fold1)[i]->GetNbinsX());

        for (int j = 0; j < (*data_before)[i]->GetNbinsX(); ++j) {
            data_before_elements[j] = (*data_before)[i]->GetBinContent(j+1);
        }

        for (int j = 0; j < (*data_before_fold0)[i]->GetNbinsX(); ++j) {
            data_before_fold0_elements[j] = (*data_before_fold0)[i]->GetBinContent(j+1);
        }

        for (int j = 0; j < (*data_before_fold1)[i]->GetNbinsX(); ++j) {
            data_before_fold1_elements[j] = (*data_before_fold1)[i]->GetBinContent(j+1);
        }
        
        data_before_vector[i] = new TMatrixT<double>(1, (*data_before)[i]->GetNbinsX(), &data_before_elements[0]);
        data_before_vector_fold0[i] = new TMatrixT<double>(1, (*data_before_fold0)[i]->GetNbinsX(), &data_before_fold0_elements[0]);
        data_before_vector_fold1[i] = new TMatrixT<double>(1, (*data_before_fold1)[i]->GetNbinsX(), &data_before_fold1_elements[0]);
    }

    /* covariance matrix before unfolding */
    std::vector<TMatrixT<double>*> covariance_matrix_before(size, nullptr);
    std::vector<TMatrixT<double>*> covariance_matrix_before_fold0(size, nullptr);
    std::vector<TMatrixT<double>*> covariance_matrix_before_fold1(size, nullptr);

    for (int i = 0; i < size; ++i) {
        std::vector<double> covariance_before_elements((*data_before)[i]->GetNbinsX() * (*data_before)[i]->GetNbinsX(), 0);
        std::vector<double> covariance_before_fold0_elements((*data_before_fold0)[i]->GetNbinsX() * (*data_before_fold0)[i]->GetNbinsX(), 0);
        std::vector<double> covariance_before_fold1_elements((*data_before_fold1)[i]->GetNbinsX() * (*data_before_fold1)[i]->GetNbinsX(), 0);

        for (int j = 0; j < (*data_before)[i]->GetNbinsX(); ++j) {
            auto err = (*data_before)[i]->GetBinError(j+1);
            covariance_before_elements[j * (*data_before)[i]->GetNbinsX() + j] = err * err;
        }

        for (int j = 0; j < (*data_before_fold0)[i]->GetNbinsX(); ++j) {
            auto err = (*data_before_fold0)[i]->GetBinError(j+1);
            covariance_before_fold0_elements[j * (*data_before_fold0)[i]->GetNbinsX() + j] = err * err;
        }

        for (int j = 0; j < (*data_before_fold1)[i]->GetNbinsX(); ++j) {
            auto err = (*data_before_fold1)[i]->GetBinError(j+1);
            covariance_before_fold1_elements[j * (*data_before_fold1)[i]->GetNbinsX() + j] = err * err;
        }

        covariance_matrix_before[i] = new TMatrixT<double>((*data_before)[i]->GetNbinsX(), (*data_before)[i]->GetNbinsX(), &covariance_before_elements[0]);
        covariance_matrix_before_fold0[i] = new TMatrixT<double>((*data_before_fold0)[i]->GetNbinsX(), (*data_before_fold0)[i]->GetNbinsX(), &covariance_before_fold0_elements[0]);
        covariance_matrix_before_fold1[i] = new TMatrixT<double>((*data_before_fold1)[i]->GetNbinsX(), (*data_before_fold1)[i]->GetNbinsX(), &covariance_before_fold1_elements[0]);
        
        covariance_matrix_before[i]->Write(("covariance_before_"s + to_text(i)).data());
        covariance_matrix_before_fold0[i]->Write(("covariance_before_fold0_"s + to_text(i)).data());
        covariance_matrix_before_fold1[i]->Write(("covariance_before_fold1_"s + to_text(i)).data());
    }

    /* response matrix */
    auto matrix = new history<TH2F>(fmatrix, tag + "_c"s);
    matrix->rename("response"s);
    matrix->save();

    /* theory after unfolding */
    auto theory_after = new history<TH1F>("theory_after"s, "", null<TH1F>, data_before->shape());
    auto theory_after_fold0 = new history<TH1F>("theory_after_fold0"s, "", null<TH1F>, data_before->shape());
    auto theory_after_fold1 = new history<TH1F>("theory_after_fold1"s, "", null<TH1F>, data_before->shape());

    for (int64_t i = 0; i < size; ++i) {
        auto theory_after_base = new history<TH1F>(ftheory, theory_label + "_dr_jpt"s);

        (*theory_after)[i] = (TH1F*) (*theory_after_base)[0]->Clone(to_text(i).data());
        (*theory_after_fold0)[i] = fold((*theory_after)[i], nullptr, mg, 0, osg);
        (*theory_after_fold1)[i] = fold((*theory_after)[i], nullptr, mg, 1, osg);
    }
    
    theory_after->rename("theory_after"s);
    theory_after_fold0->rename("theory_after_fold0"s);
    theory_after_fold1->rename("theory_after_fold1"s);

    theory_after->save();
    theory_after_fold0->save();
    theory_after_fold1->save();

    std::vector<TMatrixT<double>*> theory_after_vector(size, nullptr);
    std::vector<TMatrixT<double>*> theory_after_vector_fold0(size, nullptr);
    std::vector<TMatrixT<double>*> theory_after_vector_fold1(size, nullptr);

    for (int i = 0; i < size; ++i) {
        std::vector<double> theory_after_elements((*theory_after)[i]->GetNbinsX());
        std::vector<double> theory_after_fold0_elements((*theory_after_fold0)[i]->GetNbinsX());
        std::vector<double> theory_after_fold1_elements((*theory_after_fold1)[i]->GetNbinsX());

        for (int j = 0; j < (*theory_after)[i]->GetNbinsX(); ++j) {
            theory_after_elements[j] = (*theory_after)[i]->GetBinContent(j+1);
        }

        for (int j = 0; j < (*theory_after_fold0)[i]->GetNbinsX(); ++j) {
            theory_after_fold0_elements[j] = (*theory_after_fold0)[i]->GetBinContent(j+1);
        }

        for (int j = 0; j < (*theory_after_fold1)[i]->GetNbinsX(); ++j) {
            theory_after_fold1_elements[j] = (*theory_after_fold1)[i]->GetBinContent(j+1);
        }
        
        theory_after_vector[i] = new TMatrixT<double>(1, (*theory_after)[i]->GetNbinsX(), &theory_after_elements[0]);
        theory_after_vector_fold0[i] = new TMatrixT<double>(1, (*theory_after_fold0)[i]->GetNbinsX(), &theory_after_fold0_elements[0]);
        theory_after_vector_fold1[i] = new TMatrixT<double>(1, (*theory_after_fold1)[i]->GetNbinsX(), &theory_after_fold1_elements[0]);
    }
    
    /* theory before unfolding */
    auto theory_before = new history<TH1F>("theory_before"s, "", null<TH1F>, theory_after->shape());
    auto theory_before_fold0 = new history<TH1F>("theory_before_fold0"s, "", null<TH1F>, theory_after->shape());
    auto theory_before_fold1 = new history<TH1F>("theory_before_fold1"s, "", null<TH1F>, theory_after->shape());

    for (int i = 0; i < size; ++i) {
        (*theory_before)[i] = forward_fold((*theory_after)[i], (*matrix)[i]);
        (*theory_before_fold0)[i] = fold((*theory_before)[i], nullptr, mr, 0, osr);
        (*theory_before_fold1)[i] = fold((*theory_before)[i], nullptr, mr, 1, osr);
    }

    theory_before->rename("theory_before"s);
    theory_before_fold0->rename("theory_before_fold0"s);
    theory_before_fold1->rename("theory_before_fold1"s);

    theory_before->save();
    theory_before_fold0->save();
    theory_before_fold1->save();

    std::vector<TMatrixT<double>*> theory_before_vector(size, nullptr);
    std::vector<TMatrixT<double>*> theory_before_vector_fold0(size, nullptr);
    std::vector<TMatrixT<double>*> theory_before_vector_fold1(size, nullptr);

    for (int i = 0; i < size; ++i) {
        std::vector<double> theory_before_elements((*theory_before)[i]->GetNbinsX());
        std::vector<double> theory_before_fold0_elements((*theory_before_fold0)[i]->GetNbinsX());
        std::vector<double> theory_before_fold1_elements((*theory_before_fold1)[i]->GetNbinsX());

        for (int j = 0; j < (*theory_before)[i]->GetNbinsX(); ++j) {
            theory_before_elements[j] = (*theory_before)[i]->GetBinContent(j+1);
        }

        for (int j = 0; j < (*theory_before_fold0)[i]->GetNbinsX(); ++j) {
            theory_before_fold0_elements[j] = (*theory_before_fold0)[i]->GetBinContent(j+1);
        }

        for (int j = 0; j < (*theory_before_fold1)[i]->GetNbinsX(); ++j) {
            theory_before_fold1_elements[j] = (*theory_before_fold1)[i]->GetBinContent(j+1);
        }
        
        theory_before_vector[i] = new TMatrixT<double>(1, (*theory_before)[i]->GetNbinsX(), &theory_before_elements[0]);
        theory_before_vector_fold0[i] = new TMatrixT<double>(1, (*theory_before_fold0)[i]->GetNbinsX(), &theory_before_fold0_elements[0]);
        theory_before_vector_fold1[i] = new TMatrixT<double>(1, (*theory_before_fold1)[i]->GetNbinsX(), &theory_before_fold1_elements[0]);
    }
    
    /* define unfolding parameters */
    std::vector<int64_t> iterations {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150};

    auto func = [&](int64_t, std::string const& name, std::string const&) {
        return new TH1F(name.data(), ";iteration;", iterations.back(), 0.5, iterations.back()+0.5); };

    auto chi2_before = new history<TH1F>("chi2_before"s, "", func, size);
    auto chi2_before_dj = new history<TH1F>("chi2_before_dj"s, "", func, size);
    auto chi2_before_jpt = new history<TH1F>("chi2_before_jpt"s, "", func, size);

    // auto chi2_before_simple = new history<TH1F>("chi2_before_simple"s, "", func, size);
    // auto chi2_before_simple_dj = new history<TH1F>("chi2_before_simple_dj"s, "", func, size);
    // auto chi2_before_simple_jpt = new history<TH1F>("chi2_before_simple_jpt"s, "", func, size);

    auto chi2_after = new history<TH1F>("chi2_after"s, "", func, size);
    auto chi2_after_dj = new history<TH1F>("chi2_after_dj"s, "", func, size);
    auto chi2_after_jpt = new history<TH1F>("chi2_after_jpt"s, "", func, size);

    // auto chi2_after_simple = new history<TH1F>("chi2_after_simple"s, "", func, size);
    // auto chi2_after_simple_dj = new history<TH1F>("chi2_after_simple_dj"s, "", func, size);
    // auto chi2_after_simple_jpt = new history<TH1F>("chi2_after_simple_jpt"s, "", func, size);
    
    for (size_t k = 0; k < iterations.size(); ++k) {
        /* data and covariances after unfolding */
        auto data_after = new history<TH1F>("unfolded", "", null<TH1F>, size);
        auto data_after_fold0 = new history<TH1F>("unfolded_fold0", "", null<TH1F>, size);
        auto data_after_fold1 = new history<TH1F>("unfolded_fold1", "", null<TH1F>, size);
        
        std::vector<TMatrixT<double>*> covariance_matrix_after(size, nullptr);
        std::vector<TMatrixT<double>*> covariance_matrix_after_fold0(size, nullptr);
        std::vector<TMatrixT<double>*> covariance_matrix_after_fold1(size, nullptr);
        
        for (int i = 0; i < size; ++i) {
            std::string unfold_name = "HUnfoldedBayes" + std::to_string(iterations[k]);
            std::string matrix_name = "MUnfoldedBayes" + std::to_string(iterations[k]);

            auto HUnfolded = (TH1F*) fafter[i]->Get(unfold_name.data());
            auto MUnfolded = (TMatrixT<double>*) fafter[i]->Get(matrix_name.data());

            (*data_after)[i] = HUnfolded;
            (*data_after_fold0)[i] = fold_mat(HUnfolded, MUnfolded, mg, 0, osg);
            (*data_after_fold1)[i] = fold_mat(HUnfolded, MUnfolded, mg, 1, osg);

            covariance_matrix_after[i] = MUnfolded;

            std::vector<double> covariance_after_fold0_elements((*data_after_fold0)[i]->GetNbinsX() * (*data_after_fold0)[i]->GetNbinsX(), 0);
            std::vector<double> covariance_after_fold1_elements((*data_after_fold1)[i]->GetNbinsX() * (*data_after_fold1)[i]->GetNbinsX(), 0);

            for (int j = 0; j < idrg->size(); ++j) {             // rows
                for (int k = 0; k < idrg->size(); ++k) {         // columns
                    double sum = 0;

                    for (int m = 0; m < iptg->size(); ++m) {     // rows
                        for (int n = 0; n < iptg->size(); ++n) { // columns
                            int index_row = m * idrg->size() + j;
                            int index_col = n * idrg->size() + k;

                            sum += (*covariance_matrix_after[i])(index_row, index_col);
                        }
                    }

                    covariance_after_fold0_elements[j * (*data_after_fold0)[i]->GetNbinsX() + k] = sum;
                }
            }

            for (int j = 0; j < iptg->size(); ++j) {             // rows
                for (int k = 0; k < iptg->size(); ++k) {         // columns
                    double sum = 0;

                    for (int m = 0; m < idrg->size(); ++m) {     // rows
                        for (int n = 0; n < idrg->size(); ++n) { // columns

                            int index_row = j * idrg->size() + m;
                            int index_col = k * idrg->size() + n;

                            sum += (*covariance_matrix_after[i])(index_row, index_col);
                        }
                    }

                    covariance_after_fold1_elements[j * (*data_after_fold1)[i]->GetNbinsX() + k] = sum;
                }
            }
            
            covariance_matrix_after_fold0[i] = new TMatrixT<double>((*data_after_fold0)[i]->GetNbinsX(), (*data_after_fold0)[i]->GetNbinsX(), &covariance_after_fold0_elements[0]);
            covariance_matrix_after_fold1[i] = new TMatrixT<double>((*data_after_fold1)[i]->GetNbinsX(), (*data_after_fold1)[i]->GetNbinsX(), &covariance_after_fold1_elements[0]);

            covariance_matrix_after[i]->Write(("covariance_after_"s + to_text(i)).data());
            covariance_matrix_after_fold0[i]->Write(("covariance_after_fold0_"s + to_text(i)).data());
            covariance_matrix_after_fold1[i]->Write(("covariance_after_fold1_"s + to_text(i)).data());
        }
        
        data_after->rename("data_after_iteration_"s + std::to_string(iterations[k]));
        data_after_fold0->rename("data_after_iteration_"s + std::to_string(iterations[k]) + "_fold0"s);
        data_after_fold1->rename("data_after_iteration_"s + std::to_string(iterations[k]) + "_fold1"s);

        data_after->save();
        data_after_fold0->save();
        data_after_fold1->save();

        std::vector<TMatrixT<double>*> data_after_vector(size, nullptr);
        std::vector<TMatrixT<double>*> data_after_vector_fold0(size, nullptr);
        std::vector<TMatrixT<double>*> data_after_vector_fold1(size, nullptr);

        for (int i = 0; i < size; ++i) {
            std::vector<double> data_after_elements((*data_after)[i]->GetNbinsX());
            std::vector<double> data_after_fold0_elements((*data_after_fold0)[i]->GetNbinsX());
            std::vector<double> data_after_fold1_elements((*data_after_fold1)[i]->GetNbinsX());

            for (int j = 0; j < (*data_after)[i]->GetNbinsX(); ++j) {
                data_after_elements[j] = (*data_after)[i]->GetBinContent(j+1);
            }

            for (int j = 0; j < (*data_after_fold0)[i]->GetNbinsX(); ++j) {
                data_after_fold0_elements[j] = (*data_after_fold0)[i]->GetBinContent(j+1);
            }

            for (int j = 0; j < (*data_after_fold1)[i]->GetNbinsX(); ++j) {
                data_after_fold1_elements[j] = (*data_after_fold1)[i]->GetBinContent(j+1);
            }
            
            data_after_vector[i] = new TMatrixT<double>(1, (*data_after)[i]->GetNbinsX(), &data_after_elements[0]);
            data_after_vector_fold0[i] = new TMatrixT<double>(1, (*data_after_fold0)[i]->GetNbinsX(), &data_after_fold0_elements[0]);
            data_after_vector_fold1[i] = new TMatrixT<double>(1, (*data_after_fold1)[i]->GetNbinsX(), &data_after_fold1_elements[0]);
        }
        
        /* chi square calculations */
        for (int i = 0; i < size; ++i) {
            /* chi square in reco space */
            // nominal
            auto smear_diff_vector = new TMatrixT<double>(1, (*theory_before)[i]->GetNbinsX());
            auto smear_diff_vector_T = new TMatrixT<double>((*theory_before)[i]->GetNbinsX(), 1);
            auto covariance_matrix_before_I = covariance_matrix_before[i]->Invert();
            auto step1_smear = new TMatrixT<double>(1, (*theory_before)[i]->GetNbinsX());
            auto step2_smear = new TMatrixT<double>(1, 1);

            smear_diff_vector->Minus(*data_before_vector[i], *theory_before_vector[i]);
            smear_diff_vector_T->Transpose(*smear_diff_vector);
            step1_smear->Mult(*smear_diff_vector, covariance_matrix_before_I);
            step2_smear->Mult(*step1_smear, *smear_diff_vector_T);
            
            // fold0
            auto smear_diff_vector_fold0 = new TMatrixT<double>(1, (*theory_before_fold0)[i]->GetNbinsX());
            auto smear_diff_vector_fold0_T = new TMatrixT<double>((*theory_before_fold0)[i]->GetNbinsX(), 1);
            auto covariance_matrix_before_fold0_I = covariance_matrix_before_fold0[i]->Invert();
            auto step1_smear_fold0 = new TMatrixT<double>(1, (*theory_before_fold0)[i]->GetNbinsX());
            auto step2_smear_fold0 = new TMatrixT<double>(1, 1);

            smear_diff_vector_fold0->Minus(*data_before_vector_fold0[i], *theory_before_vector_fold0[i]);
            smear_diff_vector_fold0_T->Transpose(*smear_diff_vector_fold0);
            step1_smear_fold0->Mult(*smear_diff_vector_fold0, covariance_matrix_before_fold0_I);
            step2_smear_fold0->Mult(*step1_smear_fold0, *smear_diff_vector_fold0_T);

            // fold1
            auto smear_diff_vector_fold1 = new TMatrixT<double>(1, (*theory_before_fold1)[i]->GetNbinsX());
            auto smear_diff_vector_fold1_T = new TMatrixT<double>((*theory_before_fold1)[i]->GetNbinsX(), 1);
            auto covariance_matrix_before_fold1_I = covariance_matrix_before_fold1[i]->Invert();
            auto step1_smear_fold1 = new TMatrixT<double>(1, (*theory_before_fold1)[i]->GetNbinsX());
            auto step2_smear_fold1 = new TMatrixT<double>(1, 1);

            smear_diff_vector_fold1->Minus(*data_before_vector_fold1[i], *theory_before_vector_fold1[i]);
            smear_diff_vector_fold1_T->Transpose(*smear_diff_vector_fold1);
            step1_smear_fold1->Mult(*smear_diff_vector_fold1, covariance_matrix_before_fold1_I);
            step2_smear_fold1->Mult(*step1_smear_fold1, *smear_diff_vector_fold1_T);
            
            /* chi square in gen space */
            // nominal
            auto unfolded_diff_vector = new TMatrixT<double>(1, (*theory_after)[i]->GetNbinsX());
            auto unfolded_diff_vector_T = new TMatrixT<double>((*theory_after)[i]->GetNbinsX(), 1);
            auto covariance_matrix_after_I = covariance_matrix_after[i]->Invert();
            auto step1_unfolded = new TMatrixT<double>(1, (*theory_after)[i]->GetNbinsX());
            auto step2_unfolded = new TMatrixT<double>(1, 1);

            unfolded_diff_vector->Minus(*data_after_vector[i], *theory_after_vector[i]);
            unfolded_diff_vector_T->Transpose(*unfolded_diff_vector);
            step1_unfolded->Mult(*unfolded_diff_vector, covariance_matrix_after_I);
            step2_unfolded->Mult(*step1_unfolded, *unfolded_diff_vector_T);

            // fold0
            auto unfolded_diff_vector_fold0 = new TMatrixT<double>(1, (*theory_after_fold0)[i]->GetNbinsX());
            auto unfolded_diff_vector_fold0_T = new TMatrixT<double>((*theory_after_fold0)[i]->GetNbinsX(), 1);
            auto covariance_matrix_after_fold0_I = covariance_matrix_after_fold0[i]->Invert();
            auto step1_unfolded_fold0 = new TMatrixT<double>(1, (*theory_after_fold0)[i]->GetNbinsX());
            auto step2_unfolded_fold0 = new TMatrixT<double>(1, 1);

            unfolded_diff_vector_fold0->Minus(*data_after_vector_fold0[i], *theory_after_vector_fold0[i]);
            unfolded_diff_vector_fold0_T->Transpose(*unfolded_diff_vector_fold0);
            step1_unfolded_fold0->Mult(*unfolded_diff_vector_fold0, covariance_matrix_after_fold0_I);
            step2_unfolded_fold0->Mult(*step1_unfolded_fold0, *unfolded_diff_vector_fold0_T);

            // fold0
            auto unfolded_diff_vector_fold1 = new TMatrixT<double>(1, (*theory_after_fold1)[i]->GetNbinsX());
            auto unfolded_diff_vector_fold1_T = new TMatrixT<double>((*theory_after_fold1)[i]->GetNbinsX(), 1);
            auto covariance_matrix_after_fold1_I = covariance_matrix_after_fold1[i]->Invert();
            auto step1_unfolded_fold1 = new TMatrixT<double>(1, (*theory_after_fold1)[i]->GetNbinsX());
            auto step2_unfolded_fold1 = new TMatrixT<double>(1, 1);

            unfolded_diff_vector_fold1->Minus(*data_after_vector_fold1[i], *theory_after_vector_fold1[i]);
            unfolded_diff_vector_fold1_T->Transpose(*unfolded_diff_vector_fold1);
            step1_unfolded_fold1->Mult(*unfolded_diff_vector_fold1, covariance_matrix_after_fold1_I);
            step2_unfolded_fold1->Mult(*step1_unfolded_fold1, *unfolded_diff_vector_fold1_T);
            
            /* fill chi2 histograms */
            (*chi2_before)[i]->SetBinContent(iterations[k], (*step2_smear)(0,0));
            (*chi2_before_dj)[i]->SetBinContent(iterations[k], (*step2_smear_fold0)(0,0));
            (*chi2_before_jpt)[i]->SetBinContent(iterations[k], (*step2_smear_fold1)(0,0));

            (*chi2_after)[i]->SetBinContent(iterations[k], (*step2_unfolded)(0,0));
            (*chi2_after_dj)[i]->SetBinContent(iterations[k], (*step2_unfolded_fold0)(0,0));
            (*chi2_after_jpt)[i]->SetBinContent(iterations[k], (*step2_unfolded_fold1)(0,0));
            
        }
    }
    
    fout->Close();

    /* set up figures */
    std::string system_tag = "  #sqrt{s_{NN}} = 5.02 TeV"s;
    system_tag += (tag == "aa") ? ", 1.69 nb^{-1}"s : ", 302 pb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}}"s;
    cms += " #it{#scale[1.2]{Preliminary}}"s;

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", rpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.85, 0.04, mpthf, pt_info, hf_info); };

    auto hb = new pencil();

    hb->category("space", "smear", "unfolded");

    hb->alias("smear", "#chi^{2}_{smeared}");
    hb->alias("unfolded", "#chi^{2}_{unfolded}");
    
    auto p0 = new paper(set + "_iteration_chi_squared_" + plot_name, hb);

    p0->divide(size, -1);
    p0->accessory(pthf_info);
    apply_style(p0, cms, system_tag);

    for (int64_t i = 0; i < size; ++i) {
        (*chi2_before)[i]->SetMaximum((*chi2_before)[i]->GetMaximum()*5);
        p0->add((*chi2_before)[i], "smear");
        p0->stack((*chi2_after)[i], "unfolded");
    }
    
    auto p1 = new paper(set + "_iteration_chi_squared_dj_" + plot_name, hb);

    p1->divide(size, -1);
    p1->accessory(pthf_info);
    apply_style(p1, cms, system_tag);

    for (int i = 0; i < size; ++i) {
        (*chi2_before_dj)[i]->SetMaximum((*chi2_before_dj)[i]->GetMaximum()*5);
        p1->add((*chi2_before_dj)[i], "smear");
        p1->stack((*chi2_after_dj)[i], "unfolded");
    }
    
    auto p2 = new paper(set + "_iteration_chi_squared_jpt_" + plot_name, hb);

    p2->divide(size, -1);
    p2->accessory(pthf_info);
    apply_style(p2, cms, system_tag);

    for (int64_t i = 0; i < size; ++i) {
        (*chi2_before_jpt)[i]->SetMaximum((*chi2_before_jpt)[i]->GetMaximum()*5);
        p2->add((*chi2_before_jpt)[i], "smear");
        p2->stack((*chi2_after_jpt)[i], "unfolded");
    }
    
    hb->sketch();
    p0->draw("pdf");
    p1->draw("pdf");
    p2->draw("pdf");
    
    
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return bottom_line_test(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output] [centrality bin] [iteration]\n", argv[0]);
    return 1;
}
