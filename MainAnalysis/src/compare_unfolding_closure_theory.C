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

    auto filenames = conf->get<std::vector<std::string>>("filenames");

    auto label = conf->get<std::string>("label");
    auto prior = conf->get<std::string>("prior");
    auto object = conf->get<std::string>("object");

    auto file_svd = conf->get<std::string>("file_svd");
    auto file_bayes = conf->get<std::string>("file_bayes");

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

    std::vector<TFile*> fdata_svd(filenames.size(), nullptr);
    std::vector<TFile*> fdata_bayes(filenames.size(), nullptr);

    zip([&](auto& fsvd, auto& fbayes, auto const& filename) {
        std::cout << "unfolded/" + object + "/"s + set + "/SVD/"s + prior + "/kErrors/"s + filename << std::endl;
        std::cout << "unfolded/" + object + "/"s + set + "/Bayes/"s + prior + "/kErrors/"s + filename << std::endl;
        fsvd = new TFile(("unfolded/" + object + "/"s + set + "/SVD/"s + prior + "/kErrors/"s + filename).data(), "read");
        fbayes = new TFile(("unfolded/" + object + "/"s + set + "/Bayes/"s + prior + "/kErrors/"s + filename).data(), "read");
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

    auto gen_svd = new history<TH1F>("gen_svd", "", null<TH1F>, (int64_t) filenames.size());
    auto gen_svd_fold0 = new history<TH1F>("gen_svd_fold0", "", null<TH1F>, (int64_t) filenames.size());
    auto gen_svd_fold1 = new history<TH1F>("gen_svd_fold1", "", null<TH1F>, (int64_t) filenames.size());

    auto unfolded_bayes = new history<TH1F>("unfolded_bayes", "", null<TH1F>, (int64_t) filenames.size());
    auto unfolded_bayes_fold0 = new history<TH1F>("unfolded_bayes_fold0", "", null<TH1F>, (int64_t) filenames.size());
    auto unfolded_bayes_fold1 = new history<TH1F>("unfolded_bayes_fold1", "", null<TH1F>, (int64_t) filenames.size());

    auto gen_bayes = new history<TH1F>("gen_bayes", "", null<TH1F>, (int64_t) filenames.size());
    auto gen_bayes_fold0 = new history<TH1F>("gen_bayes_fold0", "", null<TH1F>, (int64_t) filenames.size());
    auto gen_bayes_fold1 = new history<TH1F>("gen_bayes_fold1", "", null<TH1F>, (int64_t) filenames.size());

    /* determine the regularization to use */
    std::vector<int64_t> choice_svd(filenames.size(), 1);
    std::vector<int64_t> choice_bayes(filenames.size(), 1);

    for (size_t i = 0; i < filenames.size(); ++i) {
        choice_svd[i] = (*regularization_svd)[i]->GetMinimumBin();
        choice_bayes[i] = (*regularization_bayes)[i]->GetMinimumBin();
        std::cout << choice_svd[i] << " " << choice_bayes[i] << std::endl;
    }

    /* extract chosen histograms */
    for (size_t j = 0; j < filenames.size(); ++j) {
        // std::string efficiency_name = "HMCTruthEfficiency";

        std::string unfold_name_svd = "Test0HUnfoldedSVD" + std::to_string(choice_svd[j]);
        // std::string matrix_name_svd = "Test0MUnfoldedSVD" + std::to_string(choice_svd[j]);

        std::string unfold_name_bayes = "Test0HUnfoldedBayes" + std::to_string(choice_bayes[j]);
        // std::string matrix_name_bayes = "Test0MUnfoldedBayes" + std::to_string(choice_bayes[j]);
        
        auto HUnfoldedSVD = (TH1F*) fdata_svd[j]->Get(unfold_name_svd.data());
        // auto HEfficiencySVD = (TH1F*) fdata_svd[j]->Get(efficiency_name.data());
        // auto MUnfoldedSVD = get_covariance((TMatrixT<double>*) fdata_svd[j]->Get(matrix_name_svd.data()), HEfficiencySVD);
        auto HGenSVD = (TH1F*) fdata_svd[j]->Get("HInputGen");

        auto HUnfoldedBayes = (TH1F*) fdata_bayes[j]->Get(unfold_name_bayes.data());
        // auto HEfficiencyBayes = (TH1F*) fdata_bayes[j]->Get(efficiency_name.data());
        // auto MUnfoldedBayes = get_covariance((TMatrixT<double>*) fdata_bayes[j]->Get(matrix_name_bayes.data()), HEfficiencyBayes);
        auto HGenBayes = (TH1F*) fdata_bayes[j]->Get("HInputGen");

        (*unfolded_bayes)[j] = HUnfoldedBayes;
        // (*unfolded_bayes_fold0)[j] = fold(HUnfoldedBayes, MUnfoldedBayes, mg, 0, osg);
        // (*unfolded_bayes_fold1)[j] = fold(HUnfoldedBayes, MUnfoldedBayes, mg, 1, osg);
        (*unfolded_bayes_fold0)[j] = fold(HUnfoldedBayes, nullptr, mg, 0, osg);
        (*unfolded_bayes_fold1)[j] = fold(HUnfoldedBayes, nullptr, mg, 1, osg);

        (*gen_bayes)[j] = HGenBayes;
        (*gen_bayes_fold0)[j] = fold(HGenBayes, nullptr, mg, 0, osg);
        (*gen_bayes_fold1)[j] = fold(HGenBayes, nullptr, mg, 1, osg);

        (*unfolded_svd)[j] = HUnfoldedSVD;
        // (*unfolded_svd_fold0)[j] = fold(HUnfoldedSVD, MUnfoldedSVD, mg, 0, osg);
        // (*unfolded_svd_fold1)[j] = fold(HUnfoldedSVD, MUnfoldedSVD, mg, 1, osg);
        (*unfolded_svd_fold0)[j] = fold(HUnfoldedSVD, nullptr, mg, 0, osg);
        (*unfolded_svd_fold1)[j] = fold(HUnfoldedSVD, nullptr, mg, 1, osg);

        (*gen_svd)[j] = HGenSVD;
        (*gen_svd_fold0)[j] = fold(HGenSVD, nullptr, mg, 0, osg);
        (*gen_svd_fold1)[j] = fold(HGenSVD, nullptr, mg, 1, osg);
    }

    /* rename histograms */
    unfolded_svd->rename("unfolded_svd");
    unfolded_svd_fold0->rename("unfolded_svd_fold0");
    unfolded_svd_fold1->rename("unfolded_svd_fold1");

    gen_svd->rename("gen_svd");
    gen_svd_fold0->rename("gen_svd_fold0");
    gen_svd_fold1->rename("gen_svd_fold1");

    unfolded_bayes->rename("unfolded_bayes");
    unfolded_bayes_fold0->rename("unfolded_bayes_fold0");
    unfolded_bayes_fold1->rename("unfolded_bayes_fold1");

    gen_bayes->rename("gen_bayes");
    gen_bayes_fold0->rename("gen_bayes_fold0");
    gen_bayes_fold1->rename("gen_bayes_fold1");

    /* save histograms */
    unfolded_svd->save();
    unfolded_svd_fold0->save();
    unfolded_svd_fold1->save();

    gen_svd->save();
    gen_svd_fold0->save();
    gen_svd_fold1->save();

    unfolded_bayes->save();
    unfolded_bayes_fold0->save();
    unfolded_bayes_fold1->save();

    gen_bayes->save();
    gen_bayes_fold0->save();
    gen_bayes_fold1->save();

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

    auto minimum = [&](int64_t index, std::string algorithm) {
        auto min = (algorithm == "Bayes") ? "Regularization: iteration = "s + to_text(choice_bayes[index-1]) : "Regularization: k_{reg} = "s + to_text(choice_svd[index-1]);
        auto pri = "Prior: "s + prior;
        auto src = "Source: "s + label;
        auto alg = "Algorithm: "s + algorithm;

        TLatex* l = new TLatex();
        l->SetTextFont(43);
        l->SetTextAlign(11);
        l->SetTextSize(13);
        l->DrawLatexNDC(0.3, 0.65, min.data());
        l->DrawLatexNDC(0.3, 0.60, pri.data());
        l->DrawLatexNDC(0.3, 0.55, src.data());
        l->DrawLatexNDC(0.3, 0.50, alg.data());
    };

    /* plot histograms */
    auto hb = new pencil();

    hb->category("algorithm", "SVD", "Bayes");
    hb->category("type", "Gen", "Unfolded");

    hb->set_binary("type");

    auto p1 = new paper(set + "_unfolding_closure_" + tag + "_" + label + "_SVD_" + prior + "_dj", hb);

    p1->divide(filenames.size()/2, -1);
    p1->accessory(pthf_info);
    p1->accessory(std::bind(minimum, _1, "SVD"));
    apply_style(p1, cms, system_tag, -2, 20);

    for (size_t i = 0; i < filenames.size(); ++i) {
        float max = ((*unfolded_svd_fold0)[i]->GetMaximum() > (*gen_svd_fold0)[i]->GetMaximum()) ? (*unfolded_svd_fold0)[i]->GetMaximum() : (*gen_svd_fold0)[i]->GetMaximum();
        (*unfolded_svd_fold0)[i]->SetMaximum(max*1.4);
        p1->add((*unfolded_svd_fold0)[i], "Unfolded", "SVD");
        p1->stack((*gen_svd_fold0)[i], "Gen", "SVD");
    }

    auto p2 = new paper(set + "_unfolding_closure_" + tag + "_" + label + "_SVD_" + prior + "_jpt", hb);

    p2->divide(filenames.size()/2, -1);
    p2->accessory(pthf_info);
    p2->accessory(std::bind(minimum, _1, "SVD"));
    apply_style(p2, cms, system_tag, -0.003, 0.03);

    for (size_t i = 0; i < filenames.size(); ++i) {
        float max = ((*unfolded_svd_fold1)[i]->GetMaximum() > (*gen_svd_fold1)[i]->GetMaximum()) ? (*unfolded_svd_fold1)[i]->GetMaximum() : (*gen_svd_fold1)[i]->GetMaximum();
        (*unfolded_svd_fold1)[i]->SetMaximum(max*1.4);
        p2->add((*unfolded_svd_fold1)[i], "Unfolded", "SVD");
        p2->stack((*gen_svd_fold1)[i], "Gen", "SVD");
    }

    auto p3 = new paper(set + "_unfolding_closure_" + tag + "_" + label + "_Bayes_" + prior + "_dj", hb);

    p3->divide(filenames.size()/2, -1);
    p3->accessory(pthf_info);
    p3->accessory(std::bind(minimum, _1, "Bayes"));
    apply_style(p3, cms, system_tag, -2, 20);

    for (size_t i = 0; i < filenames.size(); ++i) {
        float max = ((*unfolded_bayes_fold0)[i]->GetMaximum() > (*gen_bayes_fold0)[i]->GetMaximum()) ? (*unfolded_bayes_fold0)[i]->GetMaximum() : (*gen_bayes_fold0)[i]->GetMaximum();
        (*unfolded_bayes_fold0)[i]->SetMaximum(max*1.4);
        p3->add((*unfolded_bayes_fold0)[i], "Unfolded", "Bayes");
        p3->stack((*gen_bayes_fold0)[i], "Gen", "Bayes");
    }

    auto p4 = new paper(set + "_unfolding_closure_" + tag + "_" + label + "_Bayes_" + prior + "_jpt", hb);

    p4->divide(filenames.size()/2, -1);
    p4->accessory(pthf_info);
    p4->accessory(std::bind(minimum, _1, "Bayes"));
    apply_style(p4, cms, system_tag, -0.003, 0.03);

    for (size_t i = 0; i < filenames.size(); ++i) {
        float max = ((*unfolded_bayes_fold1)[i]->GetMaximum() > (*gen_bayes_fold1)[i]->GetMaximum()) ? (*unfolded_bayes_fold1)[i]->GetMaximum() : (*gen_bayes_fold1)[i]->GetMaximum();
        (*unfolded_bayes_fold1)[i]->SetMaximum(max*1.4);
        p4->add((*unfolded_bayes_fold1)[i], "Unfolded", "Bayes");
        p4->stack((*gen_bayes_fold1)[i], "Gen", "Bayes");
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
        return quantitate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
