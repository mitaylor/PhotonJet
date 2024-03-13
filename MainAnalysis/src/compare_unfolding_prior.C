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

    auto filenames = conf->get<std::vector<std::string>>("filenames");

    auto label = conf->get<std::string>("label");
    auto algorithm = conf->get<std::string>("algorithm");

    auto file_mc = conf->get<std::string>("file_mc");
    auto file_flat = conf->get<std::string>("file_flat");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto rdrg = sel->get<std::vector<float>>("drg_range");
    auto rptg = sel->get<std::vector<float>>("ptg_range");

    auto osg = sel->get<std::vector<int64_t>>("osg");

    auto rpt = sel->get<std::vector<float>>("photon_pt_bounds");

    /* create intervals and multivals */
    auto idrg = new interval("#deltaj"s, rdrg);
    auto iptg = new interval("p_{T}^{j}"s, rptg);

    auto mg = new multival(*idrg, *iptg);
    auto mpthf = new multival(rpt, dhf);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    std::vector<TFile*> fdata_mc(filenames.size(), nullptr);
    std::vector<TFile*> fdata_flat(filenames.size(), nullptr);

    zip([&](auto& fmc, auto& fflat, auto const& filename) {
        fmc = new TFile(("unfolded/Data/"s + set + "/" + algorithm + "/MC/kErrors/"s + filename).data(), "read");
        fflat = new TFile(("unfolded/Data/"s + set + "/" + algorithm+ "/Flat/kErrors/"s + filename).data(), "read");
    }, fdata_mc, fdata_flat, filenames);

    TFile* freg_mc = new TFile((base + file_mc).data(), "read");
    TFile* freg_flat = new TFile((base + file_flat).data(), "read");

    auto regularization_mc = new history<TH1F>(freg_mc, tag + "_mse"s);
    auto regularization_flat = new history<TH1F>(freg_flat, tag + "_mse"s);

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    /* prepare data */
    auto unfolded_mc = new history<TH1F>("unfolded_mc", "", null<TH1F>, (int64_t) filenames.size());
    auto unfolded_mc_fold0 = new history<TH1F>("unfolded_mc_fold0", "", null<TH1F>, (int64_t) filenames.size());
    auto unfolded_mc_fold1 = new history<TH1F>("unfolded_mc_fold1", "", null<TH1F>, (int64_t) filenames.size());

    auto unfolded_flat = new history<TH1F>("unfolded_flat", "", null<TH1F>, (int64_t) filenames.size());
    auto unfolded_flat_fold0 = new history<TH1F>("unfolded_flat_fold0", "", null<TH1F>, (int64_t) filenames.size());
    auto unfolded_flat_fold1 = new history<TH1F>("unfolded_flat_fold1", "", null<TH1F>, (int64_t) filenames.size());

    /* determine the regularization to use */
    std::vector<int64_t> choice_mc(filenames.size(), 1);
    std::vector<int64_t> choice_flat(filenames.size(), 1);

    for (size_t i = 0; i < filenames.size(); ++i) {
        choice_mc[i] = (*regularization_mc)[i]->GetMinimumBin();
        choice_flat[i] = (*regularization_flat)[i]->GetMinimumBin();
    }

    /* extract chosen histograms */
    for (size_t j = 0; j < filenames.size(); ++j) {
        std::string unfold_name_mc = "HUnfolded" + algorithm + std::to_string(choice_mc[j]);
        std::string matrix_name_mc = "MUnfolded" + algorithm + std::to_string(choice_mc[j]);

        std::string unfold_name_flat = "HUnfolded" + algorithm + std::to_string(choice_flat[j]);
        std::string matrix_name_flat = "MUnfolded" + algorithm + std::to_string(choice_flat[j]);
        
        auto HUnfoldedMC = (TH1F*) fdata_mc[j]->Get(unfold_name_mc.data());
        auto MUnfoldedMC = (TMatrixT<double>*) fdata_mc[j]->Get(matrix_name_mc.data());

        auto HUnfoldedFlat = (TH1F*) fdata_flat[j]->Get(unfold_name_flat.data());
        auto MUnfoldedFlat = (TMatrixT<double>*) fdata_flat[j]->Get(matrix_name_flat.data());

        (*unfolded_flat)[j] = HUnfoldedFlat;
        (*unfolded_flat_fold0)[j] = fold_mat(HUnfoldedFlat, MUnfoldedFlat, mg, 0, osg);
        (*unfolded_flat_fold1)[j] = fold_mat(HUnfoldedFlat, MUnfoldedFlat, mg, 1, osg);

        (*unfolded_mc)[j] = HUnfoldedMC;
        (*unfolded_mc_fold0)[j] = fold_mat(HUnfoldedMC, MUnfoldedMC, mg, 0, osg);
        (*unfolded_mc_fold1)[j] = fold_mat(HUnfoldedMC, MUnfoldedMC, mg, 1, osg);
    }

    /* rename histograms */
    unfolded_mc->rename("unfolded_mc");
    unfolded_mc_fold0->rename("unfolded_mc_fold0");
    unfolded_mc_fold1->rename("unfolded_mc_fold1");

    unfolded_flat->rename("unfolded_flat");
    unfolded_flat_fold0->rename("unfolded_flat_fold0");
    unfolded_flat_fold1->rename("unfolded_flat_fold1");

    /* save histograms */
    unfolded_mc->save();
    unfolded_mc_fold0->save();
    unfolded_mc_fold1->save();

    unfolded_flat->save();
    unfolded_flat_fold0->save();
    unfolded_flat_fold1->save();

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
            auto reg = (algorithm == "Bayes") ? "k_{reg}"s : "it"s;
            auto min = "Regularization: "s + reg + "^{mc} = "s + to_text(choice_mc[index-1]) + " and " + reg + "^{flat} = "s + to_text(choice_flat[index-1]);
            auto alg = "Algorithm: "s + algorithm;
            auto src = "Source: "s + label;

            TLatex* l = new TLatex();
            l->SetTextFont(43);
            l->SetTextAlign(11);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.3, 0.65, min.data());
            l->DrawLatexNDC(0.3, 0.60, alg.data());
            l->DrawLatexNDC(0.3, 0.55, src.data());
        }
    };

    /* plot histograms */
    auto hb = new pencil();

    hb->category("type", "data");
    hb->category("prior", "MC", "Flat");

    hb->set_binary("prior");

    auto p1 = new paper(set + "_unfolding_prior_" + tag + "_data_" + label + "_" + algorithm + "_dj", hb);

    p1->divide(filenames.size(), -1);
    p1->accessory(pthf_info);
    p1->accessory(minimum);
    apply_style(p1, cms, system_tag, -2, 20);

    for (size_t i = 0; i < filenames.size(); ++i) {
        // (*mse)[i]->GetXaxis()->SetTitle(title.data());

        p1->add((*unfolded_mc_fold0)[i], "data", "MC");
        p1->stack((*unfolded_flat_fold0)[i], "data", "Flat");
    }

    auto p2 = new paper(set + "_unfolding_prior_" + tag + "_data_" + label + "_" + algorithm + "_jpt", hb);

    p2->divide(filenames.size(), -1);
    p2->accessory(pthf_info);
    p2->accessory(minimum);
    apply_style(p2, cms, system_tag, -0.003, 0.03);

    for (size_t i = 0; i < filenames.size(); ++i) {
        // (*mse)[i]->GetXaxis()->SetTitle(title.data());

        p2->add((*unfolded_mc_fold1)[i], "data", "MC");
        p2->stack((*unfolded_flat_fold1)[i], "data", "Flat");
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
