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

    hfold->Scale(1., "width");

    return hfold;
}

TH1F* extract_errors(TH1* h) {
    TH1F* err = (TH1F*) h->Clone("errors");
    err->Reset();

    for (int i = 0; i < err->GetNbinsX(); ++i) {
        err->SetBinContent(i + 1, h->GetBinError(i + 1));
        err->SetBinError(i + 1, 0);
    }

    return err;
}

TH2F* convert_covariance(TMatrixT<double>* matrix, TH1* efficiency) {
    int N = matrix->GetNrows();

    TH2F* cov = new TH2F("covariance", "", N, 0, N, N, 0, N);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cov->SetBinContent(i + 1, j + 1, (*matrix)(i, j) / efficiency->GetBinContent(i + 1) / efficiency->GetBinContent(j + 1));
            cov->SetBinError(i + 1, 0);
        }
    }

    return cov;
}

TH2F* collapse_covariance(TH2F* cov, interval const* idrg, interval const* iptg, int64_t axis) {
    int NX = idrg->size();
    int NY = iptg->size();

    TH2F *proj;

    if (axis == 0) {
        proj = new TH2F("covariance", "", NX, idrg->edges(), NX, idrg->edges());

        for (int j = 0; j < NX; ++j) {             // rows
            for (int k = 0; k < NX; ++k) {         // columns
                double sum = 0;

                for (int m = 0; m < NY; ++m) {     // rows
                    for (int n = 0; n < NY; ++n) { // columns
                        int index_row = m * NX + j + 1;
                        int index_col = n * NX + k + 1;

                        sum += cov->GetBinContent(index_row, index_col);
                    }
                }

                sum /= idrg->width(j);
                sum /= idrg->width(k);

                proj->SetBinContent(j + 1, k + 1, sum);
            }
        }
    }
    else {
        proj = new TH2F("covariance", "", NY, iptg->edges(), NY, iptg->edges());

        for (int j = 0; j < NY; ++j) {             // rows
            for (int k = 0; k < NY; ++k) {         // columns
                double sum = 0;

                for (int m = 0; m < NX; ++m) {     // rows
                    for (int n = 0; n < NX; ++n) { // columns

                        int index_row = j * NX + m + 1;
                        int index_col = k * NX + n + 1;

                        sum += cov->GetBinContent(index_row, index_col);
                    }
                }

                sum /= iptg->width(j);
                sum /= iptg->width(k);

                proj->SetBinContent(j + 1, k + 1, sum);
            }
        }
    }

    return proj;
}

int compare_unfolding_errors(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto tag = conf->get<std::string>("tag");

    auto file_data = conf->get<std::vector<std::string>>("file_data");
    auto file_regularization = conf->get<std::string>("file_regularization");

    auto prior = conf->get<std::string>("prior");
    auto algorithm = conf->get<std::string>("algorithm");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto rdrg = sel->get<std::vector<float>>("drg_range");
    auto rptg = sel->get<std::vector<float>>("ptg_range");

    auto rpt = sel->get<std::vector<float>>("photon_pt_bounds");

    /* create intervals and multivals */
    auto idrg = new interval("#deltaj"s, rdrg);
    auto iptg = new interval("p_{T}^{j}"s, rptg);

    auto mg = new multival(*idrg, *iptg);
    auto mpthf = new multival(rpt, dhf);

    std::vector<int64_t> osg {0, 0, 0, 0};

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    std::vector<TFile*> fdata(file_data.size(), nullptr);

    zip([&](auto& fdata, auto const& filename) {
        fdata = new TFile(("unfolded/Errors/"s + set + "/" + algorithm + "/" + prior + "/kErrors/"s + filename).data(), "read");
    }, fdata, file_data);

    TFile* freg = new TFile((base + file_regularization).data(), "read");

    auto regularization = new history<TH1F>(freg, tag + "_mse"s);

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    /* determine the regularization to use */
    std::vector<std::vector<int64_t>> choices(4, std::vector<int64_t>(file_data.size(), 1));

    for (size_t i = 0; i < file_data.size(); ++i) {
        choices[0][i] = 1;
        choices[1][i] = (*regularization)[i]->GetMinimumBin();
        choices[2][i] = 50;
        choices[3][i] = 100;
    }

    /* plotting setup */
    auto system_tag = "  #sqrt{s_{NN}} = 5.02 TeV"s;
    system_tag += (tag == "aa") ? ", 1.69 nb^{-1}"s : ", 302 pb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", rpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.85, 0.04, mpthf, pt_info, hf_info); };

    auto pthf_info_scaled = [&](int64_t index) {
        int64_t scaled_index = (index + 1) / 2;
        std::string cent_low = to_text(dcent[scaled_index]);
        std::string cent_high = to_text(dcent[scaled_index-1]);
        auto pri = "Cent. " + cent_low + " - " + cent_high + "%";
        auto alg = to_text(rpt[0]) + " < p_{T}^{#gamma} < " + to_text(rpt[1]);

        TLatex* l = new TLatex();
        l->SetTextFont(43);
        l->SetTextAlign(11);
        l->SetTextSize(13);
        l->DrawLatexNDC(0.125, 0.85, pri.data());
        l->DrawLatexNDC(0.125, 0.80, alg.data());
    };
    
    auto minimum = [&](int64_t index, int64_t choice) {
        if (index > -1) {
            auto reg = (algorithm == "SVD") ? "k_{reg}"s : "it"s;
            auto min = "Regularization: "s + reg + " = "s + to_text(choices[choice][index - 1]);
            auto alg = "Algorithm: "s + algorithm;
            auto pri = "Prior: "s + prior;

            TLatex* l = new TLatex();
            l->SetTextFont(43);
            l->SetTextAlign(11);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.3, 0.65, min.data());
            l->DrawLatexNDC(0.3, 0.60, alg.data());
            l->DrawLatexNDC(0.3, 0.55, pri.data());
        }
    };

    for (size_t i = 0; i < choices.size(); ++i) {
        /* prepare data */
        auto unfolded = new history<TH1F>("unfolded", "", null<TH1F>, (int64_t) file_data.size());
        auto unfolded_fold0 = new history<TH1F>("unfolded_fold0", "", null<TH1F>, (int64_t) file_data.size());
        auto unfolded_fold1 = new history<TH1F>("unfolded_fold1", "", null<TH1F>, (int64_t) file_data.size());

        auto calc_errors = new history<TH1F>("calc_errors", "", null<TH1F>, (int64_t) file_data.size());
        auto calc_errors_fold0 = new history<TH1F>("calc_errors_fold0", "", null<TH1F>, (int64_t) file_data.size());
        auto calc_errors_fold1 = new history<TH1F>("calc_errors_fold1", "", null<TH1F>, (int64_t) file_data.size());

        auto toy_errors = new history<TH1F>("toy_errors", "", null<TH1F>, (int64_t) file_data.size());
        auto toy_errors_fold0 = new history<TH1F>("toy_errors_fold0", "", null<TH1F>, (int64_t) file_data.size());
        auto toy_errors_fold1 = new history<TH1F>("toy_errors_fold1", "", null<TH1F>, (int64_t) file_data.size());

        auto calc_covariance = new history<TH2F>("calc_covariance", "", null<TH2F>, (int64_t) file_data.size());
        auto calc_covariance_fold0 = new history<TH2F>("calc_covariance_fold0", "", null<TH2F>, (int64_t) file_data.size());
        auto calc_covariance_fold1 = new history<TH2F>("calc_covariance_fold1", "", null<TH2F>, (int64_t) file_data.size());

        auto toy_covariance = new history<TH2F>("toy_covariance", "", null<TH2F>, (int64_t) file_data.size());
        auto toy_covariance_fold0 = new history<TH2F>("toy_covariance_fold0", "", null<TH2F>, (int64_t) file_data.size());
        auto toy_covariance_fold1 = new history<TH2F>("toy_covariance_fold1", "", null<TH2F>, (int64_t) file_data.size());

        /* extract chosen histograms */
        for (size_t j = 0; j < file_data.size(); ++j) {
            std::string unfolded_name = "Test0HUnfolded" + algorithm + std::to_string(choices[i][j]);
            std::string calc_covariance_name = "MUnfolded" + algorithm + std::to_string(choices[i][j]);
            std::string toy_covariance_name = "HCovarianceDist" + std::to_string(choices[i][j]);
            std::string toy_covariance_fold0_name = "HCovarianceDist" + std::to_string(choices[i][j]) + "Fold0";
            std::string toy_covariance_fold1_name = "HCovarianceDist" + std::to_string(choices[i][j]) + "Fold1";
            std::string toy_errors_name = "HErrorDist" + std::to_string(choices[i][j]);
            std::string toy_errors_fold0_name = "HErrorDist" + std::to_string(choices[i][j]) + "Fold0";
            std::string toy_errors_fold1_name = "HErrorDist" + std::to_string(choices[i][j]) + "Fold1";
            std::string efficiency_name = "HMCTruthEfficiency";
            
            /* extract ingredients */
            auto unfolded_temp = (TH1F*) fdata[j]->Get(unfolded_name.data());
            auto calc_covariance_temp = (TMatrixT<double>*) fdata[j]->Get(calc_covariance_name.data());
            auto toy_errors_temp = (TH1F*) fdata[j]->Get(toy_errors_name.data());
            auto toy_errors_fold0_temp = (TH1F*) fdata[j]->Get(toy_errors_fold0_name.data());
            auto toy_errors_fold1_temp = (TH1F*) fdata[j]->Get(toy_errors_fold1_name.data());
            auto toy_covariance_temp = (TH2F*) fdata[j]->Get(toy_covariance_name.data());
            auto toy_covariance_fold0_temp = (TH2F*) fdata[j]->Get(toy_covariance_fold0_name.data());
            auto toy_covariance_fold1_temp = (TH2F*) fdata[j]->Get(toy_covariance_fold1_name.data());
            auto efficiency_temp = (TH1F*) fdata[j]->Get(efficiency_name.data());

            /* synthesise ingredients */
            (*calc_covariance)[j] = convert_covariance(calc_covariance_temp, efficiency_temp);
            (*calc_covariance_fold0)[j] = collapse_covariance((*calc_covariance)[j], idrg, iptg, 0);
            (*calc_covariance_fold1)[j] = collapse_covariance((*calc_covariance)[j], idrg, iptg, 1);

            (*unfolded)[j] = unfolded_temp;
            (*unfolded_fold0)[j] = fold((*unfolded)[j], (*calc_covariance)[j], mg, 0, osg);
            (*unfolded_fold1)[j] = fold((*unfolded)[j], (*calc_covariance)[j], mg, 1, osg);

            (*calc_errors)[j] = extract_errors((*unfolded)[j]);
            (*calc_errors_fold0)[j] = extract_errors((*unfolded_fold0)[j]);
            (*calc_errors_fold1)[j] = extract_errors((*unfolded_fold1)[j]);

            (*toy_errors)[j] = toy_errors_temp;
            (*toy_errors_fold0)[j] = toy_errors_fold0_temp;
            (*toy_errors_fold1)[j] = toy_errors_fold1_temp;

            (*toy_covariance)[j] = toy_covariance_temp;
            (*toy_covariance_fold0)[j] = toy_covariance_fold0_temp;
            (*toy_covariance_fold1)[j] = toy_covariance_fold1_temp;
        }

        /* rename histograms */
        unfolded->rename("unfolded_index" + std::to_string(i + 1));
        unfolded_fold0->rename("unfolded_fold0_index" + std::to_string(i + 1));
        unfolded_fold1->rename("unfolded_fold1_index" + std::to_string(i + 1));

        calc_errors->rename("calc_errors_index" + std::to_string(i + 1));
        calc_errors_fold0->rename("calc_errors_fold0_index" + std::to_string(i + 1));
        calc_errors_fold1->rename("calc_errors_fold1_index" + std::to_string(i + 1));

        toy_errors->rename("toy_errors_index" + std::to_string(i + 1));
        toy_errors_fold0->rename("toy_errors_fold0_index" + std::to_string(i + 1));
        toy_errors_fold1->rename("toy_errors_fold1_index" + std::to_string(i + 1));

        calc_covariance->rename("calc_covariance_index" + std::to_string(i + 1));
        calc_covariance_fold0->rename("calc_covariance_fold0_index" + std::to_string(i + 1));
        calc_covariance_fold1->rename("calc_covariance_fold1_index" + std::to_string(i + 1));

        toy_covariance->rename("toy_covariance_index" + std::to_string(i + 1));
        toy_covariance_fold0->rename("toy_covariance_fold0_index" + std::to_string(i + 1));
        toy_covariance_fold1->rename("toy_covariance_fold1_index" + std::to_string(i + 1));

        /* save histograms */
        unfolded->save();
        unfolded_fold0->save();
        unfolded_fold1->save();

        calc_errors->save();
        calc_errors_fold0->save();
        calc_errors_fold1->save();

        toy_errors->save();
        toy_errors_fold0->save();
        toy_errors_fold1->save();

        calc_covariance->save();
        calc_covariance_fold0->save();
        calc_covariance_fold1->save();

        toy_covariance->save();
        toy_covariance_fold0->save();
        toy_covariance_fold1->save();

        /* plot histograms */
        auto hb = new pencil();

        hb->category("type", "Calculated", "Toy");

        auto p1 = new paper(set + "_errors_" + tag + "_" + prior + "_" + algorithm + "_errors_index" + std::to_string(i), hb);
        p1->divide(file_data.size(), -1);
        p1->accessory(pthf_info);
        p1->accessory(std::bind(minimum, _1, i));
        apply_style(p1, cms, system_tag);

        for (size_t i = 0; i < file_data.size(); ++i) {
            auto max = ((*toy_errors)[i]->GetMaximum() > (*calc_errors)[i]->GetMaximum()) ? (*toy_errors)[i]->GetMaximum() : (*calc_errors)[i]->GetMaximum();
            auto min = ((*toy_errors)[i]->GetMinimum() < (*calc_errors)[i]->GetMinimum()) ? (*toy_errors)[i]->GetMinimum() : (*calc_errors)[i]->GetMinimum();

            max *= 1.2;
            min = (min < 0) ? min * 1.2 : 0;
            
            p1->add((*toy_errors)[i], "Toy");
            p1->stack((*calc_errors)[i], "Calculated");
            p1->adjust((*toy_errors)[i], "hist", "");
            p1->adjust((*calc_errors)[i], "hist", "");
            (*toy_errors)[i]->SetMaximum(max);
            (*calc_errors)[i]->SetMaximum(max);
            (*toy_errors)[i]->SetMinimum(min);
            (*calc_errors)[i]->SetMinimum(min);
        }

        auto p2 = new paper(set + "_errors_" + tag + "_" + prior + "_" + algorithm + "_errors_fold0_index" + std::to_string(i), hb);
        p2->divide(file_data.size(), -1);
        p2->accessory(pthf_info);
        p2->accessory(std::bind(minimum, _1, i));
        apply_style(p2, cms, system_tag);

        for (size_t i = 0; i < file_data.size(); ++i) {
            auto max = ((*toy_errors_fold0)[i]->GetMaximum() > (*calc_errors_fold0)[i]->GetMaximum()) ? (*toy_errors_fold0)[i]->GetMaximum() : (*calc_errors_fold0)[i]->GetMaximum();
            auto min = ((*toy_errors_fold0)[i]->GetMinimum() < (*calc_errors_fold0)[i]->GetMinimum()) ? (*toy_errors_fold0)[i]->GetMinimum() : (*calc_errors_fold0)[i]->GetMinimum();

            max *= 1.2;
            min = (min < 0) ? min * 1.2 : 0;

            p2->add((*toy_errors_fold0)[i], "Toy");
            p2->stack((*calc_errors_fold0)[i], "Calculated");
            p2->adjust((*toy_errors_fold0)[i], "hist", "");
            p2->adjust((*calc_errors_fold0)[i], "hist", "");
            (*toy_errors_fold0)[i]->SetMaximum(max);
            (*calc_errors_fold0)[i]->SetMaximum(max);
            (*toy_errors_fold0)[i]->SetMinimum(min);
            (*calc_errors_fold0)[i]->SetMinimum(min);
        }

        auto p3 = new paper(set + "_errors_" + tag + "_" + prior + "_" + algorithm + "_errors_fold1_index" + std::to_string(i), hb);
        p3->divide(file_data.size(), -1);
        p3->accessory(pthf_info);
        p3->accessory(std::bind(minimum, _1, i));
        apply_style(p3, cms, system_tag);

        for (size_t i = 0; i < file_data.size(); ++i) {
            auto max = ((*toy_errors_fold1)[i]->GetMaximum() > (*calc_errors_fold1)[i]->GetMaximum()) ? (*toy_errors_fold1)[i]->GetMaximum() : (*calc_errors_fold1)[i]->GetMaximum();
            auto min = ((*toy_errors_fold1)[i]->GetMinimum() < (*calc_errors_fold1)[i]->GetMinimum()) ? (*toy_errors_fold1)[i]->GetMinimum() : (*calc_errors_fold1)[i]->GetMinimum();

            max *= 1.2;
            min = (min < 0) ? min * 1.2 : 0;

            p3->add((*toy_errors_fold1)[i], "Toy");
            p3->stack((*calc_errors_fold1)[i], "Calculated");
            p3->adjust((*toy_errors_fold1)[i], "hist", "");
            p3->adjust((*calc_errors_fold1)[i], "hist", "");
            (*toy_errors_fold1)[i]->SetMaximum(max);
            (*calc_errors_fold1)[i]->SetMaximum(max);
            (*toy_errors_fold1)[i]->SetMinimum(min);
            (*calc_errors_fold1)[i]->SetMinimum(min);
        }

        auto p4 = new paper(set + "_errors_" + tag + "_" + prior + "_" + algorithm + "_covariance_index" + std::to_string(i), hb);
        p4->divide(2, file_data.size());
        p4->accessory(pthf_info_scaled);
        apply_style(p4, cms, system_tag);

        for (size_t i = 0; i < file_data.size(); ++i) {
            auto max = ((*toy_covariance)[i]->GetMaximum() > (*calc_covariance)[i]->GetMaximum()) ? (*toy_covariance)[i]->GetMaximum() : (*calc_covariance)[i]->GetMaximum();
            auto min = ((*toy_covariance)[i]->GetMinimum() < (*calc_covariance)[i]->GetMinimum()) ? (*toy_covariance)[i]->GetMinimum() : (*calc_covariance)[i]->GetMinimum();

            max *= 1.2;
            min = (min < 0) ? min * 1.2 : 0;

            p4->add((*toy_covariance)[i], "Toy");
            p4->add((*calc_covariance)[i], "Calculated");
            p4->adjust((*toy_covariance)[i], "colz", "");
            p4->adjust((*calc_covariance)[i], "colz", "");
            (*toy_covariance)[i]->GetZaxis()->SetLabelSize(0.02);
            (*calc_covariance)[i]->GetZaxis()->SetLabelSize(0.02);
            (*toy_covariance)[i]->SetMaximum(max);
            (*calc_covariance)[i]->SetMaximum(max);
            (*toy_covariance)[i]->SetMinimum(min);
            (*calc_covariance)[i]->SetMinimum(min);
        }

        auto p5 = new paper(set + "_errors_" + tag + "_" + prior + "_" + algorithm + "_covariance_fold0_index" + std::to_string(i), hb);
        p5->divide(2, file_data.size());
        p5->accessory(pthf_info_scaled);
        apply_style(p5, cms, system_tag);

        for (size_t i = 0; i < file_data.size(); ++i) {
            auto max = ((*toy_covariance_fold0)[i]->GetMaximum() > (*calc_covariance_fold0)[i]->GetMaximum()) ? (*toy_covariance_fold0)[i]->GetMaximum() : (*calc_covariance_fold0)[i]->GetMaximum();
            auto min = ((*toy_covariance_fold0)[i]->GetMinimum() < (*calc_covariance_fold0)[i]->GetMinimum()) ? (*toy_covariance_fold0)[i]->GetMinimum() : (*calc_covariance_fold0)[i]->GetMinimum();

            max *= 1.2;
            min = (min < 0) ? min * 1.2 : 0;

            p5->add((*toy_covariance_fold0)[i], "Toy");
            p5->add((*calc_covariance_fold0)[i], "Calculated");
            p5->adjust((*toy_covariance_fold0)[i], "colz", "");
            p5->adjust((*calc_covariance_fold0)[i], "colz", "");
            (*toy_covariance_fold0)[i]->GetZaxis()->SetLabelSize(0.02);
            (*calc_covariance_fold0)[i]->GetZaxis()->SetLabelSize(0.02);
            (*toy_covariance_fold0)[i]->SetMaximum(max);
            (*calc_covariance_fold0)[i]->SetMaximum(max);
            (*toy_covariance_fold0)[i]->SetMinimum(min);
            (*calc_covariance_fold0)[i]->SetMinimum(min);
        }

        auto p6 = new paper(set + "_errors_" + tag + "_" + prior + "_" + algorithm + "_covariance_fold1_index" + std::to_string(i), hb);
        p6->divide(2, file_data.size());
        p6->accessory(pthf_info_scaled);
        apply_style(p6, cms, system_tag);

        for (size_t i = 0; i < file_data.size(); ++i) {
            auto max = ((*toy_covariance_fold1)[i]->GetMaximum() > (*calc_covariance_fold1)[i]->GetMaximum()) ? (*toy_covariance_fold1)[i]->GetMaximum() : (*calc_covariance_fold1)[i]->GetMaximum();
            auto min = ((*toy_covariance_fold1)[i]->GetMinimum() < (*calc_covariance_fold1)[i]->GetMinimum()) ? (*toy_covariance_fold1)[i]->GetMinimum() : (*calc_covariance_fold1)[i]->GetMinimum();

            max *= 1.2;
            min = (min < 0) ? min * 1.2 : 0;

            p6->add((*toy_covariance_fold1)[i], "Toy");
            p6->add((*calc_covariance_fold1)[i], "Calculated");
            p6->adjust((*toy_covariance_fold1)[i], "colz", "");
            p6->adjust((*calc_covariance_fold1)[i], "colz", "");
            (*toy_covariance_fold1)[i]->GetZaxis()->SetLabelSize(0.02);
            (*calc_covariance_fold1)[i]->GetZaxis()->SetLabelSize(0.02);
            (*toy_covariance_fold1)[i]->SetMaximum(max);
            (*calc_covariance_fold1)[i]->SetMaximum(max);
            (*toy_covariance_fold1)[i]->SetMinimum(min);
            (*calc_covariance_fold1)[i]->SetMinimum(min);
        }

        hb->sketch();

        p1->draw("pdf");
        p2->draw("pdf");
        p3->draw("pdf");
        p4->draw("pdf");
        p5->draw("pdf");
        p6->draw("pdf");
    }

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return compare_unfolding_errors(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
