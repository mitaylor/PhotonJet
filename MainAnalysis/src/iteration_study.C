#include "../include/lambdas.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

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
    auto label = conf->get<std::string>("before_label");
    auto fix = conf->get<std::vector<float>>("fix");

    auto afters = conf->get<std::vector<std::string>>("afters");
    auto regularization = conf->get<std::string>("regularization");

    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

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
        fafter = new TFile(("unfolded/" + set + "/" + after).data(), "read");
    }, fafters, afters);

    TFile* fiter = new TFile((base + regularization).data(), "read");
    auto chi_square = new history<TH1F>(fiter, "test_chi_square"s);

    /* prepare output file */
    TFile* fout = new TFile(output, "recreate");

    /* prepare the post-unfolded data */
    auto unfolded_nominal = new history<TH1F>("unfolded_nominal", "", null<TH1F>, (int64_t) afters.size());
    auto unfolded_nominal_fold0 = new history<TH1F>("unfolded_nominal_fold0", "", null<TH1F>, (int64_t) afters.size());
    auto unfolded_nominal_fold1 = new history<TH1F>("unfolded_nominal_fold1", "", null<TH1F>, (int64_t) afters.size());

    auto unfolded_plus = new history<TH1F>("unfolded_plus", "", null<TH1F>, (int64_t) afters.size());
    auto unfolded_plus_fold0 = new history<TH1F>("unfolded_plus_fold0", "", null<TH1F>, (int64_t) afters.size());
    auto unfolded_plus_fold1 = new history<TH1F>("unfolded_plus_fold1", "", null<TH1F>, (int64_t) afters.size());

    auto unfolded_minus = new history<TH1F>("unfolded_minus", "", null<TH1F>, (int64_t) afters.size());
    auto unfolded_minus_fold0 = new history<TH1F>("unfolded_minus_fold0", "", null<TH1F>, (int64_t) afters.size());
    auto unfolded_minus_fold1 = new history<TH1F>("unfolded_minus_fold1", "", null<TH1F>, (int64_t) afters.size());

    /* determine the number of iterations to use */
    std::vector<float> min_nominal(chi_square->size(), 1);
    std::vector<float> min_plus(chi_square->size(), 1);
    std::vector<float> min_minus(chi_square->size(), 1);

    std::vector<int64_t> choice_nominal(chi_square->size(), 1);
    std::vector<int64_t> choice_plus(chi_square->size(), 1);
    std::vector<int64_t> choice_minus(chi_square->size(), 1);

    /* find nominal choice */
    for (int64_t i = 0; i < chi_square->size(); ++i) {
        min_nominal[i] = 99999999999;

        for (int64_t j = 0; j < (*chi_square)[i]->GetNbinsX(); ++j) {
            auto top = (*chi_square)[i]->GetBinContent(j + 1) + (*chi_square)[i]->GetBinError(j + 1);

            if (top == 0) { continue; }

            std::cout << top << " ";

            if (top < min_nominal[i]) {
                min_nominal[i] = top;
                choice_nominal[i] = j;
            }
            else {
                break;
            }
        }

        if (fix.size() == choice_nominal.size()) { choice_nominal[i] = fix[i]; }

        std::cout << std::endl << choice_nominal[i] << std::endl;
    }

    /* find deviation in choice within 5% */
    for (int64_t i = 0; i < chi_square->size(); ++i) {
        choice_plus[i] = choice_nominal[i];

        for (int64_t j = choice_nominal[i]; j < (*chi_square)[i]->GetNbinsX(); ++j) {
            auto top = (*chi_square)[i]->GetBinContent(j + 1) + (*chi_square)[i]->GetBinError(j + 1);

            if (top == 0) { continue; }

            if ((top - min_nominal[i]) / min_nominal[i] < 0.05) {
                min_plus[i] = top;
                choice_plus[i] = j;
            }
            else {
                break;
            }
        }

        std::cout << std::endl << choice_plus[i] << std::endl;
    }

    /* find deviation in choice within 5% */
    for (int64_t i = 0; i < chi_square->size(); ++i) {
        choice_minus[i] = 1;

        for (int64_t j = choice_nominal[i] - 1; j > 0; --j) {
            auto top = (*chi_square)[i]->GetBinContent(j + 1) + (*chi_square)[i]->GetBinError(j + 1);

            if (top == 0) { continue; }

            if ((top - min_nominal[i]) / min_nominal[i] < 0.05 || choice_minus[i] < j) {
                min_minus[i] = top;
                choice_minus[i] = j;
            }
            else {
                break;
            }
        }

        std::cout << std::endl << choice_minus[i] << std::endl;
    }

    for (size_t j = 0; j < fafters.size(); ++j) {
        std::string unfold_name = "HUnfoldedBayes" + std::to_string(choice_nominal[j]);
        std::string matrix_name = "MUnfoldedBayes" + std::to_string(choice_nominal[j]);

        auto HUnfoldedBayes = (TH1F*) fafters[j]->Get(unfold_name.data());
        auto MUnfolded = (TMatrixT<double>*) fafters[j]->Get(matrix_name.data());

        (*unfolded_nominal)[j] = HUnfoldedBayes;
        (*unfolded_nominal_fold0)[j] = fold_mat((*unfolded_nominal)[j], MUnfolded, mg, 0, osg);
        (*unfolded_nominal_fold1)[j] = fold_mat((*unfolded_nominal)[j], MUnfolded, mg, 1, osg);
    }

    for (size_t j = 0; j < fafters.size(); ++j) {
        std::string unfold_name = "HUnfoldedBayes" + std::to_string(choice_plus[j]);
        std::string matrix_name = "MUnfoldedBayes" + std::to_string(choice_plus[j]);

        auto HUnfoldedBayes = (TH1F*) fafters[j]->Get(unfold_name.data());
        auto MUnfolded = (TMatrixT<double>*) fafters[j]->Get(matrix_name.data());

        (*unfolded_plus)[j] = HUnfoldedBayes;
        (*unfolded_plus_fold0)[j] = fold_mat((*unfolded_plus)[j], MUnfolded, mg, 0, osg);
        (*unfolded_plus_fold1)[j] = fold_mat((*unfolded_plus)[j], MUnfolded, mg, 1, osg);
    }

    for (size_t j = 0; j < fafters.size(); ++j) {
        std::string unfold_name = "HUnfoldedBayes" + std::to_string(choice_minus[j]);
        std::string matrix_name = "MUnfoldedBayes" + std::to_string(choice_minus[j]);

        auto HUnfoldedBayes = (TH1F*) fafters[j]->Get(unfold_name.data());
        auto MUnfolded = (TMatrixT<double>*) fafters[j]->Get(matrix_name.data());

        (*unfolded_minus)[j] = HUnfoldedBayes;
        (*unfolded_minus_fold0)[j] = fold_mat((*unfolded_minus)[j], MUnfolded, mg, 0, osg);
        (*unfolded_minus_fold1)[j] = fold_mat((*unfolded_minus)[j], MUnfolded, mg, 1, osg);
    }

    normalise_to_unity(unfolded_nominal_fold0, unfolded_nominal_fold1, unfolded_plus_fold0, unfolded_plus_fold1, unfolded_minus_fold0, unfolded_minus_fold1);

    unfolded_nominal->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_sum0_unfolded_nominal"s);
    unfolded_nominal_fold0->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_sum0_unfolded_nominal_fold0"s);
    unfolded_nominal_fold1->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_sum0_unfolded_nominal_fold1"s);

    unfolded_plus->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_sum0_unfolded_plus"s);
    unfolded_plus_fold0->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_sum0_unfolded_plus_fold0"s);
    unfolded_plus_fold1->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_sum0_unfolded_plus_fold1"s);

    unfolded_minus->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_sum0_unfolded_minus"s);
    unfolded_minus_fold0->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_sum0_unfolded_minus_fold0"s);
    unfolded_minus_fold1->rename(tag + "_"s + label + "_raw_sub_pjet_u_dr_sum0_unfolded_minus_fold1"s);

    unfolded_nominal->save();
    unfolded_nominal_fold0->save();
    unfolded_nominal_fold1->save();

    fout->Close();

    /* iteration comparison plot */
    auto system_tag = "  #sqrt{s_{NN}} = 5.02 TeV"s;
    system_tag += (heavyion) ? ", 1.69 nb^{-1}"s : ", 302 pb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

    auto hf_info = [&](int64_t index) {
        info_text(index, 0.75, "Cent. %i - %i%%", dcent, true); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto photon_selections = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs)  + 
                ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
            auto jet_selections = "anti-k_{T} R = 0.3, " + to_text(bjet_pt[0]) + "p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.71, photon_selections.data());
            l->DrawLatexNDC(0.865, 0.65, jet_selections.data());
        }
    };

    auto hb = new pencil();
    hb->category("type", "nominal", "plus", "minus");

    hb->alias("plus", "increased iterations");
    hb->alias("minus", "decreased iterations");

    auto p1 = new paper(set + "_" + label + "_iteration_comparison", hb);
    p1->divide(ihf->size(), -1);
    p1->accessory(hf_info);
    p1->accessory(kinematics);
    apply_style(p1, cms, system_tag, -2., 27.);
    p1->accessory(std::bind(line_at, _1, 0.f, bdr[0], bdr[1]));

    unfolded_nominal_fold0->apply([&](TH1* h) { p1->add(h, "nominal"); });

    for (int64_t i = 0; i < chi_square->size(); ++i) {
        if (choice_plus[i] != choice_nominal[i]) p1->stack(i + 1, (*unfolded_plus_fold0)[i], "plus");
        if (choice_minus[i] != choice_nominal[i]) p1->stack(i + 1, (*unfolded_minus_fold0)[i], "minus");
    }

    hb->sketch();

    p1->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return quantitate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
