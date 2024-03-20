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

    auto regularization = conf->get<std::string>("regularization");

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

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    TFile* fbefore = new TFile((base + before_file).data(), "read");

    std::vector<TFile*> fafter(after_file.size(), nullptr);
    zip([&](auto& fafter, auto const& after) {
        fafter = new TFile(("unfolded/" + set + "/" + after).data(), "read");
    }, fafter, after_file);

    TFile* fiter = new TFile((base + regularization).data(), "read");
    auto sum = new history<TH1F>(fiter, "sum"s);

    TFile* ftheory = new TFile((base + theory_file).data(), "read");

    TFile* fmatrix = new TFile((base + matrix_file).data(), "read");
    
    /* DATA BEFORE UNFOLDING */
    TFile* fout = new TFile(output, "recreate");

    auto data_before = new history<TH1F>(fbefore, tag + "_"s + before_label);
    data_before->apply([](TH1* h) { h->Scale(1. / h->Integral()); });

    auto data_before_fold0 = new history<TH1F>(tag + "_"s + before_label + "_fold0"s, "", null<TH1F>, data_before->shape());
    auto data_before_fold1 = new history<TH1F>(tag + "_"s + before_label + "_fold1"s, "", null<TH1F>, data_before->shape());

    for (int64_t i = 0; i < data_before->size(); ++i) {
        (*data_before_fold0)[i] = fold((*data_before)[i], nullptr, mr, 0, osr);
        (*data_before_fold1)[i] = fold((*data_before)[i], nullptr, mr, 1, osr);
    }

    data_before_fold0->rename(tag + "_"s + before_label + "_fold0"s);
    data_before_fold1->rename(tag + "_"s + before_label + "_fold1"s);

    data_before->save();
    data_before_fold0->save();
    data_before_fold1->save();

    std::vector<TMatrixT<double>*> data_before_vector_fold0(data_before_fold0->size(), nullptr);

    for (int i = 0; i < data_before_fold0->size(); ++i) {
        std::vector<double> data_before_elements((*data_before_fold0)[i]->GetNbinsX());

        for (int j = 0; j < (*data_before_fold0)[i]->GetNbinsX(); ++j) {
            data_before_elements[j] = (*data_before_fold0)[i]->GetBinContent(j + 1);
        }
        
        data_before_vector_fold0[i] = new TMatrixT<double>(1, (*data_before_fold0)[i]->GetNbinsX(), &data_before_elements[0]);
    }

    std::vector<TMatrixT<double>*> data_before_vector_fold1(data_before_fold1->size(), nullptr);

    for (int i = 0; i < data_before_fold1->size(); ++i) {
        std::vector<double> data_before_elements((*data_before_fold1)[i]->GetNbinsX());

        for (int j = 0; j < (*data_before_fold1)[i]->GetNbinsX(); ++j) {
            data_before_elements[j] = (*data_before_fold1)[i]->GetBinContent(j + 1);
        }
        
        data_before_vector_fold1[i] = new TMatrixT<double>(1, (*data_before_fold1)[i]->GetNbinsX(), &data_before_elements[0]);
    }

    std::vector<int64_t> iterations {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};

    auto func = [&](int64_t, std::string const& name, std::string const&) {
        return new TH1F(name.data(), ";iteration;", iterations.back(), 0.5, iterations.back()+0.5); };

    auto chi2_dj_smear = new history<TH1F>("chi2_dj_smear"s, "", func, sum->size());
    auto chi2_pt_smear = new history<TH1F>("chi2_pt_smear"s, "", func, sum->size());

    auto chi2_simple_dj_smear = new history<TH1F>("chi2_simple_dj_smear"s, "", func, sum->size());
    auto chi2_simple_pt_smear = new history<TH1F>("chi2_simple_pt_smear"s, "", func, sum->size());

    auto chi2_dj_unf = new history<TH1F>("chi2_dj_unf"s, "", func, sum->size());
    auto chi2_pt_unf = new history<TH1F>("chi2_pt_unf"s, "", func, sum->size());

    auto chi2_simple_dj_unf = new history<TH1F>("chi2_simple_dj_unf"s, "", func, sum->size());
    auto chi2_simple_pt_unf = new history<TH1F>("chi2_simple_pt_unf"s, "", func, sum->size());

    for (int iteration = 1; iteration < 31; iteration++) {

        /* DATA AFTER UNFOLDING */
        std::vector<int64_t> choice(sum->size(), 1);

        for (int i = 0; i < sum->size(); ++i) {
            choice[i] = iteration;
        }

        auto data_after = new history<TH1F>("unfolded", "", null<TH1F>, (int64_t) after_file.size());

        auto data_after_fold0 = new history<TH1F>("unfolded_fold0", "", null<TH1F>, (int64_t) after_file.size());
        auto data_after_fold1 = new history<TH1F>("unfolded_fold1", "", null<TH1F>, (int64_t) after_file.size());

        for (size_t j = 0; j < after_file.size(); ++j) {
            std::string unfold_name = "HUnfoldedBayes" + std::to_string(choice[j]);
            std::string matrix_name = "MUnfoldedBayes" + std::to_string(choice[j]);

            auto HUnfoldedBayes = (TH1F*) fafter[j]->Get(unfold_name.data());
            auto MUnfolded = (TMatrixT<double>*) fafter[j]->Get(matrix_name.data());

            (*data_after)[j] = HUnfoldedBayes;
            (*data_after)[j]->Scale(1. / (*data_after)[j]->Integral());

            (*data_after_fold0)[j] = fold_mat((*data_after)[j], MUnfolded, mg, 0, osg);
            (*data_after_fold1)[j] = fold_mat((*data_after)[j], MUnfolded, mg, 1, osg);
        }

        data_after->rename(tag + "_"s + before_label + "_unfolded"s);
        data_after_fold0->rename(tag + "_"s + before_label + "_unfolded_fold0"s);
        data_after_fold1->rename(tag + "_"s + before_label + "_unfolded_fold1"s);
        
        data_after->save();
        data_after_fold0->save();
        data_after_fold1->save();

        std::vector<TMatrixT<double>*> data_after_vector_fold0(data_after_fold0->size(), nullptr);

        for (int i = 0; i < data_after_fold0->size(); ++i) {
            std::vector<double> data_after_elements((*data_after_fold0)[i]->GetNbinsX());

            for (int j = 0; j < (*data_after_fold0)[i]->GetNbinsX(); ++j) {
                data_after_elements[j] = (*data_after_fold0)[i]->GetBinContent(j + 1);
            }
            
            data_after_vector_fold0[i] = new TMatrixT<double>(1, (*data_after_fold0)[i]->GetNbinsX(), &data_after_elements[0]);
        }

        std::vector<TMatrixT<double>*> data_after_vector_fold1(data_after_fold1->size(), nullptr);

        for (int i = 0; i < data_after_fold1->size(); ++i) {
            std::vector<double> data_after_elements((*data_after_fold1)[i]->GetNbinsX());

            for (int j = 0; j < (*data_after_fold1)[i]->GetNbinsX(); ++j) {
                data_after_elements[j] = (*data_after_fold1)[i]->GetBinContent(j + 1);
            }
            
            data_after_vector_fold1[i] = new TMatrixT<double>(1, (*data_after_fold1)[i]->GetNbinsX(), &data_after_elements[0]);
        }

        /* RESPONSE MATRIX */
        auto matrix = new history<TH2F>(fmatrix, tag + "_c"s);
        matrix->save();
        
        /* COVARIANCE MATRIX BEFORE UNFOLDING */
        std::vector<TMatrixT<double>*> covariance_before_matrix_fold0(data_before_fold0->size(), nullptr);

        for (int i = 0; i < data_before_fold0->size(); ++i) {
            std::vector<double> covariance_before_elements((*data_before_fold0)[i]->GetNbinsX() * (*data_before_fold0)[i]->GetNbinsX(), 0);

            for (int j = 0; j < (*data_before_fold0)[i]->GetNbinsX(); ++j) {
                auto err = (*data_before_fold0)[i]->GetBinError(j + 1);
                covariance_before_elements[j * (*data_before_fold0)[i]->GetNbinsX() + j] = err * err;
            }
            
            std::string name = "covariance_before_fold0_" + to_text(i);

            covariance_before_matrix_fold0[i] = new TMatrixT<double>((*data_before_fold0)[i]->GetNbinsX(), (*data_before_fold0)[i]->GetNbinsX(), &covariance_before_elements[0]);
            covariance_before_matrix_fold0[i]->Write(name.c_str());
        }

        std::vector<TMatrixT<double>*> covariance_before_matrix_fold1(data_before_fold1->size(), nullptr);

        for (int i = 0; i < data_before_fold1->size(); ++i) {
            std::vector<double> covariance_before_elements((*data_before_fold1)[i]->GetNbinsX() * (*data_before_fold1)[i]->GetNbinsX(), 0);

            for (int j = 0; j < (*data_before_fold1)[i]->GetNbinsX(); ++j) {
                auto err = (*data_before_fold1)[i]->GetBinError(j + 1);
                covariance_before_elements[j * (*data_before_fold1)[i]->GetNbinsX() + j] = err * err;
            }
            
            std::string name = "covariance_before_fold1_" + to_text(i);

            covariance_before_matrix_fold1[i] = new TMatrixT<double>((*data_before_fold1)[i]->GetNbinsX(), (*data_before_fold1)[i]->GetNbinsX(), &covariance_before_elements[0]);
            covariance_before_matrix_fold1[i]->Write(name.c_str());
        }

        /* COVARIANCE MATRIX AFTER UNFOLDING */
        std::vector<TMatrixT<double>*> covariance_after_matrix_fold0(data_after_fold0->size(), nullptr);

        for (int i = 0; i < data_after_fold0->size(); ++i) {
            std::string covariance_name = "MUnfoldedBayes" + std::to_string(choice[i]);
            auto covariance_after_matrix_full = (TMatrixT<double>*) fafter[i]->Get(covariance_name.data());

            std::vector<double> covariance_after_elements((*data_after_fold0)[i]->GetNbinsX() * (*data_after_fold0)[i]->GetNbinsX(), 0);

            for (int j = 0; j < idrg->size(); ++j) { // rows
                for (int k = 0; k < idrg->size(); ++k) { // columns
                    double sum = 0;

                    for (int m = 0; m < iptg->size(); ++m) { // rows
                        for (int n = 0; n < iptg->size(); ++n) { // columns
                            int index_row = m * idrg->size() + j;
                            int index_col = n * idrg->size() + k;

                            sum += (*covariance_after_matrix_full)(index_row, index_col);
                        }
                    }

                    covariance_after_elements[j * (*data_after_fold0)[i]->GetNbinsX() + k] = sum;
                }
            }

            std::string name = "covariance_after_fold0_" + to_text(i);

            covariance_after_matrix_fold0[i] = new TMatrixT<double>((*data_after_fold0)[i]->GetNbinsX(), (*data_after_fold0)[i]->GetNbinsX(), &covariance_after_elements[0]);
            covariance_after_matrix_fold0[i]->Write(name.c_str());
        }

        std::vector<TMatrixT<double>*> covariance_after_matrix_fold1(data_after_fold1->size(), nullptr);

        for (int i = 0; i < data_after_fold1->size(); ++i) {
            std::string covariance_name = "MUnfoldedBayes" + std::to_string(choice[i]);
            auto covariance_after_matrix_full = (TMatrixT<double>*) fafter[i]->Get(covariance_name.data());

            std::vector<double> covariance_after_elements((*data_after_fold1)[i]->GetNbinsX() * (*data_after_fold1)[i]->GetNbinsX(), 0);

            for (int j = 0; j < iptg->size(); ++j) { // rows
                for (int k = 0; k < iptg->size(); ++k) { // columns
                    double sum = 0;

                    for (int m = 0; m < idrg->size(); ++m) { // rows
                        for (int n = 0; n < idrg->size(); ++n) { // columns

                            int index_row = j * idrg->size() + m;
                            int index_col = k * idrg->size() + n;

                            sum += (*covariance_after_matrix_full)(index_row, index_col);
                        }
                    }

                    covariance_after_elements[j * (*data_after_fold1)[i]->GetNbinsX() + k] = sum;
                }
            }

            std::string name = "covariance_after_fold1_" + to_text(i);

            covariance_after_matrix_fold1[i] = new TMatrixT<double>((*data_after_fold1)[i]->GetNbinsX(), (*data_after_fold1)[i]->GetNbinsX(), &covariance_after_elements[0]);
            covariance_after_matrix_fold1[i]->Write(name.c_str());
        }
        
        /* THEORY GEN LEVEL */
        auto theory_gen = new history<TH1F>(ftheory, tag + "_"s + theory_label);
        theory_gen->apply([](TH1* h) { h->Scale(1. / h->Integral()); });
        theory_gen->apply([](TH1* h) { 
            for (int i = 0; i < h->GetNbinsX(); ++i) {
                h->SetBinError(i + 1, 0);
            }
        });

        auto theory_gen_fold0 = new history<TH1F>(tag + "_"s + theory_label + "_fold0"s, "", null<TH1F>, theory_gen->shape());
        auto theory_gen_fold1 = new history<TH1F>(tag + "_"s + theory_label + "_fold1"s, "", null<TH1F>, theory_gen->shape());

        for (int64_t i = 0; i < theory_gen->size(); ++i) {
            (*theory_gen_fold0)[i] = fold((*theory_gen)[i], nullptr, mg, 0, osg);
            (*theory_gen_fold1)[i] = fold((*theory_gen)[i], nullptr, mg, 1, osg);
        }

        theory_gen_fold0->rename(tag + "_"s + theory_label + "_fold0"s);
        theory_gen_fold1->rename(tag + "_"s + theory_label + "_fold1"s);

        theory_gen->save();
        theory_gen_fold0->save();
        theory_gen_fold1->save();

        std::vector<TMatrixT<double>*> theory_gen_vector_fold0(theory_gen_fold0->size(), nullptr);

        for (int i = 0; i < theory_gen_fold0->size(); ++i) {
            std::vector<double> theory_gen_elements((*theory_gen_fold0)[i]->GetNbinsX());

            for (int j = 0; j < (*theory_gen_fold0)[i]->GetNbinsX(); ++j) {
                theory_gen_elements[j] = (*theory_gen_fold0)[i]->GetBinContent(j + 1);
            }
            
            theory_gen_vector_fold0[i] = new TMatrixT<double>(1, (*theory_gen_fold0)[i]->GetNbinsX(), &theory_gen_elements[0]);
        }

        std::vector<TMatrixT<double>*> theory_gen_vector_fold1(theory_gen_fold1->size(), nullptr);

        for (int i = 0; i < theory_gen_fold1->size(); ++i) {
            std::vector<double> theory_gen_elements((*theory_gen_fold1)[i]->GetNbinsX());

            for (int j = 0; j < (*theory_gen_fold1)[i]->GetNbinsX(); ++j) {
                theory_gen_elements[j] = (*theory_gen_fold1)[i]->GetBinContent(j + 1);
            }
            
            theory_gen_vector_fold1[i] = new TMatrixT<double>(1, (*theory_gen_fold1)[i]->GetNbinsX(), &theory_gen_elements[0]);
        }

        /* THEORY SMEARED */
        auto theory_smear = new history<TH1F>(tag + "_"s + theory_label + "_smear"s, "", null<TH1F>, theory_gen->shape());
        
        for (int i = 0; i < theory_gen->size(); ++i) {
            (*theory_smear)[i] = forward_fold((*theory_gen)[i], (*matrix)[i]);
        }

        theory_smear->apply([](TH1* h) { h->Scale(1. / h->Integral()); });

        auto theory_smear_fold0 = new history<TH1F>(tag + "_"s + theory_label + "_fold0"s, "", null<TH1F>, theory_smear->shape());
        auto theory_smear_fold1 = new history<TH1F>(tag + "_"s + theory_label + "_fold1"s, "", null<TH1F>, theory_smear->shape());

        for (int64_t i = 0; i < theory_smear->size(); ++i) {
            (*theory_smear_fold0)[i] = fold((*theory_smear)[i], nullptr, mr, 0, osr);
            (*theory_smear_fold1)[i] = fold((*theory_smear)[i], nullptr, mr, 1, osr);
        }

        theory_smear->rename(tag + "_"s + theory_label + "_smear"s);
        theory_smear_fold0->rename(tag + "_"s + theory_label + "_smear_fold0"s);
        theory_smear_fold1->rename(tag + "_"s + theory_label + "_smear_fold1"s);

        theory_smear->save();
        theory_smear_fold0->save();
        theory_smear_fold1->save();

        std::vector<TMatrixT<double>*> theory_smear_vector_fold0(theory_smear_fold0->size(), nullptr);

        for (int i = 0; i < theory_smear_fold0->size(); ++i) {
            std::vector<double> theory_smear_elements((*theory_smear_fold0)[i]->GetNbinsX());

            for (int j = 0; j < (*theory_smear_fold0)[i]->GetNbinsX(); ++j) {
                theory_smear_elements[j] = (*theory_smear_fold0)[i]->GetBinContent(j + 1);
            }
            
            theory_smear_vector_fold0[i] = new TMatrixT<double>(1, (*theory_smear_fold0)[i]->GetNbinsX(), &theory_smear_elements[0]);
        }

        std::vector<TMatrixT<double>*> theory_smear_vector_fold1(theory_smear_fold1->size(), nullptr);

        for (int i = 0; i < theory_smear_fold1->size(); ++i) {
            std::vector<double> theory_smear_elements((*theory_smear_fold1)[i]->GetNbinsX());

            for (int j = 0; j < (*theory_smear_fold1)[i]->GetNbinsX(); ++j) {
                theory_smear_elements[j] = (*theory_smear_fold1)[i]->GetBinContent(j + 1);
            }
            
            theory_smear_vector_fold1[i] = new TMatrixT<double>(1, (*theory_smear_fold1)[i]->GetNbinsX(), &theory_smear_elements[0]);
        }
        
        /* CHI SQUARE CALCULATIONS */
        for (size_t i = 0; i < after_file.size(); ++ i) {
            std::cout << "centrality " << i << std::endl;

            /* CHI SQUARE IN SMEARED SPACE */

            // fold0
            auto smear_diff_vector_fold0 = new TMatrixT<double>(1, (*theory_smear_fold0)[i]->GetNbinsX());
            smear_diff_vector_fold0->Minus(*data_before_vector_fold0[i], *theory_smear_vector_fold0[i]);
            
            auto smear_diff_vector_fold0_T = new TMatrixT<double>((*theory_smear_fold0)[i]->GetNbinsX(), 1);
            smear_diff_vector_fold0_T->Transpose(*smear_diff_vector_fold0);

            auto covariance_before_matrix_fold0_I = covariance_before_matrix_fold0[i]->Invert();

            auto step1_smear_fold0 = new TMatrixT<double>(1, (*theory_smear_fold0)[i]->GetNbinsX());
            step1_smear_fold0->Mult(*smear_diff_vector_fold0, covariance_before_matrix_fold0_I);

            auto step2_smear_fold0 = new TMatrixT<double>(1, 1);
            step2_smear_fold0->Mult(*step1_smear_fold0, *smear_diff_vector_fold0_T);

            // fold1
            auto smear_diff_vector_fold1 = new TMatrixT<double>(1, (*theory_smear_fold1)[i]->GetNbinsX());
            smear_diff_vector_fold1->Minus(*data_before_vector_fold1[i], *theory_smear_vector_fold1[i]);
            
            auto smear_diff_vector_fold1_T = new TMatrixT<double>((*theory_smear_fold1)[i]->GetNbinsX(), 1);
            smear_diff_vector_fold1_T->Transpose(*smear_diff_vector_fold1);

            auto covariance_before_matrix_fold1_I = covariance_before_matrix_fold1[i]->Invert();

            auto step1_smear_fold1 = new TMatrixT<double>(1, (*theory_smear_fold1)[i]->GetNbinsX());
            step1_smear_fold1->Mult(*smear_diff_vector_fold1, covariance_before_matrix_fold1_I);

            auto step2_smear_fold1 = new TMatrixT<double>(1, 1);
            step2_smear_fold1->Mult(*step1_smear_fold1, *smear_diff_vector_fold1_T);

            // std::cout << "smeared: " << (*step2_smear_fold0)(0,0) / (*data_before_fold0)[i]->GetNbinsX() << " " << (*step2_smear_fold1)(0,0) / (*data_before_fold1)[i]->GetNbinsX() << std::endl;
            std::cout << "smeared: " << (*step2_smear_fold0)(0,0) << " " << (*step2_smear_fold1)(0,0) << std::endl;

            /* CHI SQUARE IN UNFOLDED SPACE */

            // fold0
            auto unfolded_diff_vector_fold0 = new TMatrixT<double>(1, (*theory_gen_fold0)[i]->GetNbinsX());
            unfolded_diff_vector_fold0->Minus(*data_after_vector_fold0[i], *theory_gen_vector_fold0[i]);
            
            auto unfolded_diff_vector_fold0_T = new TMatrixT<double>((*theory_gen_fold0)[i]->GetNbinsX(), 1);
            unfolded_diff_vector_fold0_T->Transpose(*unfolded_diff_vector_fold0);
            
            auto covariance_after_matrix_fold0_I = covariance_after_matrix_fold0[i]->Invert();

            auto step1_unfolded_fold0 = new TMatrixT<double>(1, (*theory_gen_fold0)[i]->GetNbinsX());
            step1_unfolded_fold0->Mult(*unfolded_diff_vector_fold0, covariance_after_matrix_fold0_I);

            auto step2_unfolded_fold0 = new TMatrixT<double>(1, 1);
            step2_unfolded_fold0->Mult(*step1_unfolded_fold0, *unfolded_diff_vector_fold0_T);

            // fold1
            auto unfolded_diff_vector_fold1 = new TMatrixT<double>(1, (*theory_gen_fold1)[i]->GetNbinsX());
            unfolded_diff_vector_fold1->Minus(*data_after_vector_fold1[i], *theory_gen_vector_fold1[i]);
            
            auto unfolded_diff_vector_fold1_T = new TMatrixT<double>((*theory_gen_fold1)[i]->GetNbinsX(), 1);
            unfolded_diff_vector_fold1_T->Transpose(*unfolded_diff_vector_fold1);
            
            auto covariance_after_matrix_fold1_I = covariance_after_matrix_fold1[i]->Invert();

            auto step1_unfolded_fold1 = new TMatrixT<double>(1, (*theory_gen_fold1)[i]->GetNbinsX());
            step1_unfolded_fold1->Mult(*unfolded_diff_vector_fold1, covariance_after_matrix_fold1_I);

            auto step2_unfolded_fold1 = new TMatrixT<double>(1, 1);
            step2_unfolded_fold1->Mult(*step1_unfolded_fold1, *unfolded_diff_vector_fold1_T);

            // std::cout << "unfolded: " << (*step2_unfolded_fold0)(0,0) / (*data_after_fold0)[i]->GetNbinsX() << " " << (*step2_unfolded_fold1)(0,0) / (*data_after_fold1)[i]->GetNbinsX() << std::endl << std::endl;

            std::cout << "unfolded: " << (*step2_unfolded_fold0)(0,0) << " " << (*step2_unfolded_fold1)(0,0) << std::endl << std::endl;

            (*chi2_dj_smear)[i]->SetBinContent(iteration, (*step2_smear_fold0)(0,0));
            (*chi2_pt_smear)[i]->SetBinContent(iteration, (*step2_smear_fold1)(0,0));
            (*chi2_dj_unf)[i]->SetBinContent(iteration, (*step2_unfolded_fold0)(0,0));
            (*chi2_pt_unf)[i]->SetBinContent(iteration, (*step2_unfolded_fold1)(0,0));
        }


        for (size_t i = 0; i < after_file.size(); ++ i) {
            std::cout << "centrality " << i << std::endl;

            /* CHI SQUARE IN SMEARED SPACE */

            // fold0
            double chi2_smear_fold0 = 0;

            for (int j = 0; j < (*data_before_fold0)[i]->GetNbinsX(); ++j) {
                auto diff = (*data_before_fold0)[i]->GetBinContent(j + 1) - (*theory_smear_fold0)[i]->GetBinContent(j + 1);
                chi2_smear_fold0 += diff * diff / (*data_before_fold0)[i]->GetBinContent(j + 1);
            }

            // fold1
            double chi2_smear_fold1 = 0;

            for (int j = 0; j < (*data_before_fold1)[i]->GetNbinsX(); ++j) {
                auto diff = (*data_before_fold1)[i]->GetBinContent(j + 1) - (*theory_smear_fold1)[i]->GetBinContent(j + 1);
                chi2_smear_fold1 += diff * diff / (*data_before_fold1)[i]->GetBinContent(j + 1);
            }

            std::cout << "smeared: " << chi2_smear_fold0  << " " << chi2_smear_fold1 << std::endl;

            /* CHI SQUARE IN UNFOLDED SPACE */

            // fold0
            double chi2_unfolded_fold0 = 0;

            for (int j = 0; j < (*data_after_fold0)[i]->GetNbinsX(); ++j) {
                auto diff = (*data_after_fold0)[i]->GetBinContent(j + 1) - (*theory_gen_fold0)[i]->GetBinContent(j + 1);
                chi2_unfolded_fold0 += diff * diff / (*data_after_fold0)[i]->GetBinContent(j + 1);
            }

            // fold1
            double chi2_unfolded_fold1 = 0;

            for (int j = 0; j < (*data_after_fold1)[i]->GetNbinsX(); ++j) {
                auto diff = (*data_after_fold1)[i]->GetBinContent(j + 1) - (*theory_gen_fold1)[i]->GetBinContent(j + 1);
                chi2_unfolded_fold1 += diff * diff / (*data_after_fold1)[i]->GetBinContent(j + 1);
            }

            std::cout << "unfolded: " << chi2_unfolded_fold0 << " " << chi2_unfolded_fold1  << std::endl;

            (*chi2_simple_dj_smear)[i]->SetBinContent(iteration, chi2_smear_fold0);
            (*chi2_simple_pt_smear)[i]->SetBinContent(iteration, chi2_smear_fold1);
            (*chi2_simple_dj_unf)[i]->SetBinContent(iteration, chi2_unfolded_fold0);
            (*chi2_simple_pt_unf)[i]->SetBinContent(iteration, chi2_unfolded_fold1);
        }
    }


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
    hb->alias("unfolded", "#chi^{2}_{D'Agostini}");

    auto p1 = new paper(set + "_iteration_chi_squared_dj_" + plot_name, hb);

    p1->divide(sum->size(), -1);
    p1->accessory(pthf_info);
    apply_style(p1, cms, system_tag);

    for (int64_t i = 0; i < sum->size(); ++i) {
        p1->add((*chi2_dj_smear)[i], "smear");
        p1->stack((*chi2_dj_unf)[i], "unfolded");
    }

    std::cout << std::endl << "Iteration choice" << std::endl;
    for (int64_t i = 0; i < sum->size(); ++i) {
        for (int j = 0; j < (*chi2_dj_smear)[i]->GetNbinsX(); ++j) {
            if ((*chi2_dj_unf)[i]->GetBinContent(j + 1) < (*chi2_dj_smear)[i]->GetBinContent(j + 1)) {
                std::cout << j + 1 << " ";
                break;
            }
        }
    }

    auto p2 = new paper(set + "_iteration_chi_squared_pt_" + plot_name, hb);

    p2->divide(sum->size(), -1);
    p2->accessory(pthf_info);
    apply_style(p2, cms, system_tag);

    for (int64_t i = 0; i < sum->size(); ++i) {
        p2->add((*chi2_pt_smear)[i], "smear");
        p2->stack((*chi2_pt_unf)[i], "unfolded");
    }

    auto p3 = new paper(set + "_iteration_chi_squared_simple_dj_" + plot_name, hb);

    p3->divide(sum->size(), -1);
    p3->accessory(pthf_info);
    apply_style(p3, cms, system_tag);

    for (int64_t i = 0; i < sum->size(); ++i) {
        p3->add((*chi2_simple_dj_smear)[i], "smear");
        p3->stack((*chi2_simple_dj_unf)[i], "unfolded");
    }

    auto p4 = new paper(set + "_iteration_chi_squared_simple_pt_" + plot_name, hb);

    p4->divide(sum->size(), -1);
    p4->accessory(pthf_info);
    apply_style(p4, cms, system_tag);

    for (int64_t i = 0; i < sum->size(); ++i) {
        p4->add((*chi2_simple_pt_smear)[i], "smear");
        p4->stack((*chi2_simple_pt_unf)[i], "unfolded");
    }

    hb->sketch();
    p1->draw("pdf");
    p2->draw("pdf");
    p3->draw("pdf");
    p4->draw("pdf");

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return bottom_line_test(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
