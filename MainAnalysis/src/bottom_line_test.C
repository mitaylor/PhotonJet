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

TH1F *forward_fold(TH1 *HGen, TH2D *HResponse)
{
   if(HGen == nullptr || HResponse == nullptr)
      return nullptr;

   static int Count = 0;
   Count = Count + 1;

   int NGen = HResponse->GetNbinsY();
   int NReco = HResponse->GetNbinsX();

   TH1F *HResult = new TH1F(Form("HFold%d", Count), "", NReco, 0, NReco);

   HResult->Sumw2();

   for(int iG = 1; iG <= NGen; iG++)
   {
      double N = 0;
      for(int iR = 1; iR <= NReco; iR++)
         N = N + HResponse->GetBinContent(iR, iG);

      if(N == 0)
         continue;

      for(int iR = 1; iR <= NReco; iR++)
      {
         double T = HResponse->GetBinContent(iR, iG) / N;
         double G = HGen->GetBinContent(iG);
         double ET = HResponse->GetBinError(iR, iG) / HResponse->GetBinContent(iR, iG);
         double EG = HGen->GetBinError(iG) / HGen->GetBinContent(iG);

         if(HResponse->GetBinContent(iR, iG) == 0)
            ET = 0;

         double V = T * G;
         double E = sqrt(ET * ET + EG * EG) * V;

         double Current = HResult->GetBinContent(iR);
         HResult->SetBinContent(iR, Current + V);

         double Error = HResult->GetBinError(iR);
         Error = sqrt(Error * Error + E * E);
         HResult->SetBinError(iR, Error);
      }
   }

   return HResult;
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

int bottom_line_test(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto tag = conf->get<std::string>("tag");

    auto before_file = conf->get<std::string>("before_file");
    auto before_label = conf->get<std::string>("before_label");

    auto after_file = conf->get<std::vector<std::string>>("after_file");

    auto theory_file = conf->get<std::string>("theory_file");
    auto theory_label = conf->get<std::string>("theory_label");

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

    TFile* fbefore = new TFile((base + before_file).data(), "read");

    std::vector<TFile*> fafter(after_file.size(), nullptr);
    zip([&](auto& fafter, auto const& after) {
        fafter = new TFile(("unfolded/" + set + "/" + after).data(), "read");
    }, fafter, after_file);

    TFile* fiter = new TFile((base + regularization).data(), "read");
    auto sum = new history<TH1F>(fiter, "sum"s);

    TFile* ftheory = new TFile((base + theory_file).data(), "read");
    
    /* DATA BEFORE UNFOLDING */
    TFile* fout = new TFile(output, "recreate");

    auto data_before = new history<TH1F>(fbefore, tag + "_"s + before_label);
    auto shape = data_before->shape();

    auto data_before_side0 = new history<TH1F>(tag + "_"s + before_label + "_side0"s, "", null<TH1F>, shape);
    auto data_before_side1 = new history<TH1F>(tag + "_"s + before_label + "_side1"s, "", null<TH1F>, shape);

    for (int64_t i = 0; i < data_before->size(); ++i) {
        (*data_before_side0)[i] = fold((*data_before)[i], nullptr, mr, 0, osr);
        (*data_before_side1)[i] = fold((*data_before)[i], nullptr, mr, 1, osr);
    }

    normalise_to_unity(data_before_side0, data_before_side1);

    data_before_side0->rename(tag + "_"s + before_label + "_side0"s);
    data_before_side1->rename(tag + "_"s + before_label + "_side1"s);

    data_before->save();
    data_before_side0->save();
    data_before_side1->save();

    std::vector<double> data_before_elements((*data_before)[0]->GetNbinsX());
    (*data_before)[0]->Scale(1/(*data_before)[0]->Integral());

    for (int i = 0; i < (*data_before)[0]->GetNbinsX(); ++i) {
        data_before_elements[i] = (*data_before)[0]->GetBinContent(i+1);
        // std::cout << data_before_elements[i] << " ";
    }

    // std::cout << std::endl << std::endl;
    
    auto data_before_vector = new TMatrixT<double>(1, (*data_before)[0]->GetNbinsX(), &data_before_elements[0]);
    
    /* DATA AFTER UNFOLDING */
    auto data_after = new history<TH1F>("unfolded", "", null<TH1F>, (int64_t) after_file.size());
    auto data_after_side0 = new history<TH1F>("unfolded_side0", "", null<TH1F>, (int64_t) after_file.size());
    auto data_after_side1 = new history<TH1F>("unfolded_side1", "", null<TH1F>, (int64_t) after_file.size());

    std::vector<int64_t> choice(sum->size(), 1);

    for (int i = 0; i < sum->size(); ++i) {
        double min = 99999999999;

        for (int j = 1; j <= (*sum)[i]->GetNbinsX(); ++j) {
            auto top = (*sum)[i]->GetBinContent(j);

            if (top == 0) { continue; }

            if (top < min) {
                min = top;
                choice[i] = j;
            }
            else {
                break;
            }
        }
    }

    for (size_t j = 0; j < after_file.size(); ++j) {
        std::string unfold_name = "HUnfoldedBayes" + std::to_string(choice[j]);
        std::string matrix_name = "MUnfoldedBayes" + std::to_string(choice[j]);

        auto HUnfoldedBayes = (TH1F*) fafter[j]->Get(unfold_name.data());
        auto MUnfolded = (TMatrixT<double>*) fafter[j]->Get(matrix_name.data());

        (*data_after)[j] = HUnfoldedBayes;
        (*data_after_side0)[j] = fold_mat(HUnfoldedBayes, MUnfolded, mg, 0, osg);
        (*data_after_side1)[j] = fold_mat(HUnfoldedBayes, MUnfolded, mg, 1, osg);
    }

    normalise_to_unity(data_after_side0, data_after_side1);

    data_after->rename(tag + "_"s + before_label + "_unfolded"s);
    data_after_side0->rename(tag + "_"s + before_label + "_unfolded_side0"s);
    data_after_side1->rename(tag + "_"s + before_label + "_unfolded_side1"s);

    data_after->save();
    data_after_side0->save();
    data_after_side1->save();

    std::vector<double> data_after_elements((*data_after)[0]->GetNbinsX());
    (*data_after)[0]->Scale(1/(*data_after)[0]->Integral());

    for (int i = 0; i < (*data_after)[0]->GetNbinsX(); ++i) {
        data_after_elements[i] = (*data_after)[0]->GetBinContent(i+1);
        // std::cout << data_after_elements[i] << " ";
    }

    // std::cout << std::endl << std::endl;

    auto data_after_vector = new TMatrixT<double>(1, (*data_after)[0]->GetNbinsX(), &data_after_elements[0]);
    
    /* RESPONSE MATRIX */
    auto HResponse = (TH2D*) fafter[0]->Get("HMCResponse");
    
    /* COVARIANCE MATRIX BEFORE UNFOLDING */
    std::vector<double> covariance_before_elements((*data_before)[0]->GetNbinsX() * (*data_before)[0]->GetNbinsX(), 0);
    std::vector<double> covariance_before_elements_I((*data_before)[0]->GetNbinsX() * (*data_before)[0]->GetNbinsX(), 0);
    
    for (int i = 0; i < (*data_before)[0]->GetNbinsX(); ++i) {
        auto err = (*data_before)[0]->GetBinError(i + 1);
        covariance_before_elements[i*i] = err * err;

        if (err != 0) {
            covariance_before_elements_I[i*i] = 1/(err * err);
        }
    }

    // auto covariance_before_matrix = new TMatrixT<double>((*data_before)[0]->GetNbinsX(), (*data_before)[0]->GetNbinsX(), &covariance_before_elements[0]);
    auto covariance_before_matrix_I = new TMatrixT<double>((*data_before)[0]->GetNbinsX(), (*data_before)[0]->GetNbinsX(), &covariance_before_elements_I[0]);
    
    /* COVARIANCE MATRIX AFTER UNFOLDING */
    std::string covariance_name = "MUnfoldedBayes" + std::to_string(choice[0]);
    auto covariance_after_matrix = (TMatrixT<double>*) fafter[0]->Get(covariance_name.data());
    
    /* THEORY GEN LEVEL */
    auto theory_gen = new history<TH1F>(ftheory, tag + "_"s + theory_label);

    std::vector<double> theory_gen_elements((*theory_gen)[0]->GetNbinsX());
    (*theory_gen)[0]->Scale(1/(*theory_gen)[0]->Integral());

    for (int i = 0; i < (*theory_gen)[0]->GetNbinsX(); ++i) {
        theory_gen_elements[i] = (*theory_gen)[0]->GetBinContent(i+1);
        // std::cout << theory_gen_elements[i] << " ";
    }

    // std::cout << std::endl << std::endl;

    auto theory_gen_vector = new TMatrixT<double>(1, (*theory_gen)[0]->GetNbinsX(), &theory_gen_elements[0]);
    
    /* THEORY SMEARED */
    auto theory_smear = forward_fold((*theory_gen)[0], HResponse);

    std::vector<double> theory_smear_elements(theory_smear->GetNbinsX());
    theory_smear->Scale(1/theory_smear->Integral());

    for (int i = 0; i < theory_smear->GetNbinsX(); ++i) {
        theory_smear_elements[i] = theory_smear->GetBinContent(i+1);
        // std::cout << theory_smear_elements[i] << " ";
    }

    // std::cout << std::endl << std::endl;

    auto theory_smear_vector = new TMatrixT<double>(1, theory_smear->GetNbinsX(), &theory_smear_elements[0]);
    
    /* CHI SQUARE IN SMEARED SPACE */
    //data_after_vector data_before_vector covariance_before_matrix covariance_after_matrix theory_gen_vector theory_smear_vector

    auto smear_diff_vector = new TMatrixT<double>(1, theory_smear->GetNbinsX());
    smear_diff_vector->Minus(*data_before_vector, *theory_smear_vector);

    for (int i = 0; i < theory_smear->GetNbinsX(); ++i) {
        std::cout << (*smear_diff_vector)(0, i);
    }

    std::cout << std::endl << std::endl;
    
    auto smear_diff_vector_T = new TMatrixT<double>(theory_smear->GetNbinsX(), 1);
    smear_diff_vector_T->Transpose(*smear_diff_vector);

    auto step1_smear = new TMatrixT<double>(1, theory_smear->GetNbinsX());
    step1_smear->Mult(*smear_diff_vector, *covariance_before_matrix_I);

    auto step2_smear = new TMatrixT<double>(1, 1);
    step2_smear->Mult(*step1_smear, *smear_diff_vector_T);

    std::cout << (*step2_smear)(0,0) << std::endl;

    /* CHI SQUARE IN UNFOLDED SPACE */
    auto unfolded_diff_vector = new TMatrixT<double>(1, (*theory_gen)[0]->GetNbinsX());
    unfolded_diff_vector->Minus(*data_after_vector, *theory_gen_vector);

    for (int i = 0; i < (*theory_gen)[0]->GetNbinsX(); ++i) {
        std::cout << (*unfolded_diff_vector)(0, i);
    }

    std::cout << std::endl << std::endl;
    
    auto unfolded_diff_vector_T = new TMatrixT<double>((*theory_gen)[0]->GetNbinsX(), 1);
    unfolded_diff_vector_T->Transpose(*unfolded_diff_vector);
    
    auto covariance_after_matrix_I = covariance_after_matrix->Invert();

    auto step1_unfolded = new TMatrixT<double>(1, (*theory_gen)[0]->GetNbinsX());
    step1_unfolded->Mult(*unfolded_diff_vector, covariance_after_matrix_I);

    auto step2_unfolded = new TMatrixT<double>(1, 1);
    step2_unfolded->Mult(*step1_unfolded, *unfolded_diff_vector_T);

    std::cout << (*step2_unfolded)(0,0) << std::endl;

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return bottom_line_test(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
