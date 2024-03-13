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

int quantitate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto tag = conf->get<std::string>("tag");

    auto filenames = conf->get<std::vector<std::string>>("filenames");
    auto mc_filenames = conf->get<std::vector<std::string>>("mc_filenames");

    auto label = conf->get<std::string>("label");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto rdrr = sel->get<std::vector<float>>("drr_range");
    auto rptr = sel->get<std::vector<float>>("ptr_range");

    auto osr = sel->get<std::vector<int64_t>>("osr");

    auto rpt = sel->get<std::vector<float>>("photon_pt_bounds");

    /* create intervals and multivals */
    auto idrr = new interval("#deltaj"s, rdrr);
    auto iptr = new interval("p_{T}^{j}"s, rptr);

    auto mr = new multival(*idrr, *iptr);
    auto mpthf = new multival(rpt, dhf);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    std::vector<TFile*> fdata(filenames.size(), nullptr);
    std::vector<TFile*> fmc(filenames.size(), nullptr);

    zip([&](auto& fdata, auto& fmc, auto const& filename, auto const& mc_filename) {
        fdata = new TFile(("/data/submit/mitay/unfolding/240313/Input/Theory/"s + set + "/"s + filename).data(), "read");
        fmc = new TFile(("/data/submit/mitay/unfolding/240313/Input/Theory/"s + set + "/"s + mc_filename).data(), "read");
    }, fdata, fmc, filenames, mc_filenames);

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    /* prepare data */
    auto input_mc = new history<TH1F>("input_mc", "", null<TH1F>, (int64_t) filenames.size());
    auto input_mc_fold0 = new history<TH1F>("input_mc_fold0", "", null<TH1F>, (int64_t) filenames.size());
    auto input_mc_fold1 = new history<TH1F>("input_mc_fold1", "", null<TH1F>, (int64_t) filenames.size());

    auto input_data = new history<TH1F>("input_data", "", null<TH1F>, (int64_t) filenames.size());
    auto input_data_fold0 = new history<TH1F>("input_data_fold0", "", null<TH1F>, (int64_t) filenames.size());
    auto input_data_fold1 = new history<TH1F>("input_data_fold1", "", null<TH1F>, (int64_t) filenames.size());

    auto input_theory = new history<TH1F>("input_theory", "", null<TH1F>, (int64_t) filenames.size());
    auto input_theory_fold0 = new history<TH1F>("input_theory_fold0", "", null<TH1F>, (int64_t) filenames.size());
    auto input_theory_fold1 = new history<TH1F>("input_theory_fold1", "", null<TH1F>, (int64_t) filenames.size());

    /* extract chosen histograms */
    for (size_t j = 0; j < filenames.size(); ++j) {
        auto HInputMC = (TH1F*) fmc[j]->Get("HDataErrors");
        auto HInputData = (TH1F*) fdata[j]->Get("HDataErrors");
        auto HInputTheory = (TH1F*) fdata[j]->Get("HDataReco");

        (*input_mc)[j] = HInputMC;
        (*input_mc_fold0)[j] = fold(HInputMC, nullptr, mr, 0, osr);
        (*input_mc_fold1)[j] = fold(HInputMC, nullptr, mr, 1, osr);

        (*input_data)[j] = HInputData;
        (*input_data_fold0)[j] = fold(HInputData, nullptr, mr, 0, osr);
        (*input_data_fold1)[j] = fold(HInputData, nullptr, mr, 1, osr);

        (*input_theory)[j] = HInputTheory;
        (*input_theory_fold0)[j] = fold(HInputTheory, nullptr, mr, 0, osr);
        (*input_theory_fold1)[j] = fold(HInputTheory, nullptr, mr, 1, osr);
    }

    /* rename histograms */
    input_mc->rename("input_mc");
    input_mc_fold0->rename("input_mc_fold0");
    input_mc_fold1->rename("input_mc_fold1");

    input_data->rename("input_data");
    input_data_fold0->rename("input_data_fold0");
    input_data_fold1->rename("input_data_fold1");

    input_theory->rename("input_theory");
    input_theory_fold0->rename("input_theory_fold0");
    input_theory_fold1->rename("input_theory_fold1");

    /* save histograms */
    input_mc->save();
    input_mc_fold0->save();
    input_mc_fold1->save();

    input_data->save();
    input_data_fold0->save();
    input_data_fold1->save();

    input_theory->save();
    input_theory_fold0->save();
    input_theory_fold1->save();

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

    auto minimum = [&](int64_t index) {
        if (index > -1) {
            auto src = "Theory: "s + label;

            TLatex* l = new TLatex();
            l->SetTextFont(43);
            l->SetTextAlign(11);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.3, 0.65, src.data());
        }
    };

    /* plot histograms */
    auto hb = new pencil();

    hb->category("type", "Data", "MC", "Theory");

    auto p1 = new paper(set + "_unfolding_inputs_" + tag + "_" + label + "_dj", hb);

    p1->divide(filenames.size(), -1);
    p1->accessory(pthf_info);
    p1->accessory(minimum);
    apply_style(p1, cms, system_tag, -2, 20);

    for (size_t i = 0; i < filenames.size(); ++i) {
        p1->add((*input_mc_fold0)[i], "MC");
        p1->stack((*input_data_fold0)[i], "Data");
        p1->stack((*input_theory_fold0)[i], "Theory");
    }

    auto p2 = new paper(set + "_unfolding_inputs_" + tag + "_" + label + "_jpt", hb);

    p2->divide(filenames.size(), -1);
    p2->accessory(pthf_info);
    p2->accessory(minimum);
    apply_style(p2, cms, system_tag, -0.003, 0.03);

    for (size_t i = 0; i < filenames.size(); ++i) {
        p2->add((*input_mc_fold1)[i], "MC");
        p2->stack((*input_data_fold1)[i], "Data");
        p2->stack((*input_theory_fold1)[i], "Theory");
    }

    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return quantitate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
