#include "../include/lambdas.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

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

template <typename... T>
void normalise_to_unity(T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1. / obj->Integral("width")); }), 0)... };
}

template <std::size_t N>
TH1F* fold(TH1* flat, TH2* covariance, multival const* m, int64_t axis,
           std::array<int64_t, N> const& offsets) {
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

int data_mc_comparison(char const* config, const char* output) {
    auto conf = new configurer(config);

    auto input_qcd = conf->get<std::string>("input_qcd");
    auto qcd_after_label = conf->get<std::string>("qcd_after_label");
    auto r_qcd_after_label = conf->get<std::string>("r_qcd_after_label");
    auto j_qcd_after_label = conf->get<std::string>("j_qcd_after_label");

    auto input_truth = conf->get<std::string>("input_truth");
    auto truth_reco_iso_label = conf->get<std::string>("truth_reco_iso_label");
    auto r_truth_reco_iso_label = conf->get<std::string>("r_truth_reco_iso_label");
    auto j_truth_reco_iso_label = conf->get<std::string>("j_truth_reco_iso_label");

    auto tag = conf->get<std::string>("tag");

    auto rdr = conf->get<std::vector<float>>("dr_range");
    auto rpt = conf->get<std::vector<float>>("pt_range");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto rdrg = conf->get<std::vector<float>>("drg_range");
    auto rptg = conf->get<std::vector<float>>("ptg_range");

    /* create intervals and multivals */
    std::array<int64_t, 4> osg = { 0, 0, 1, 1 };

    auto idr = new interval(rdrg);
    auto ihf = new interval(dhf);
    auto mg = new multival(rdrg, rptg);
    auto mpthf = new multival(dpt, dhf);

    /* load history objects */
    TFile* fqcd = new TFile(input_qcd.data(), "read");
    TFile* ftruth = new TFile(input_truth.data(), "read");

    auto h_qcd_after = new history<TH1F>(fqcd, tag + "_"s + qcd_after_label);
    auto h_r_qcd_after = new history<TH1F>(fqcd, tag + "_"s + r_qcd_after_label);
    auto h_j_qcd_after = new history<TH1F>(fqcd, tag + "_"s + j_qcd_after_label);

    // auto h_truth_reco_iso = new history<TH1F>(ftruth, tag + "_"s + truth_reco_iso_label);
    auto h_r_truth_reco_iso = new history<TH1F>(ftruth, tag + "_"s + r_truth_reco_iso_label);
    auto h_j_truth_reco_iso = new history<TH1F>(ftruth, tag + "_"s + j_truth_reco_iso_label);

    /* do the reweighting */
    for (int64_t i = 0; i < h_j_qcd_after->size(); ++i) {
        std::vector<float> weights;

        for (int64_t j = 0; j < (*h_j_qcd_after)[i]->GetNbinsX(); ++j) {
            weights.push_back((*h_j_truth_reco_iso)[i]->GetBinContent(j + 1) / (*h_j_qcd_after)[i]->GetBinContent(j + 1));
        }

        for (int64_t j = 0; j < (*h_qcd_after)[i]->GetNbinsX(); ++j) {
            int64_t index = j / idr->size() - osg[2];
            std::cout << index << std::endl;

            if (index >= 0 && index < (int64_t) weights.size()) {
                auto old = (*h_qcd_after)[i]->GetBinContent(j + 1);
                (*h_qcd_after)[i]->SetBinContent(j + 1, old * weights[index]);
            }
        }
    }

    auto h_r_weighted_qcd_after = new history<TH1F>("r_weighted_qcd_fold0", "", null<TH1F>, ihf->size());
    auto h_j_weighted_qcd_after = new history<TH1F>("j_weighted_qcd_fold1", "", null<TH1F>, ihf->size());
    
    for (int64_t i = 0; i < ihf->size(); ++i) {
        (*h_r_weighted_qcd_after)[i] = fold((*h_qcd_after)[i], nullptr, mg, 0, osg);
        (*h_j_weighted_qcd_after)[i] = fold((*h_qcd_after)[i], nullptr, mg, 1, osg);
    }

    h_r_weighted_qcd_after->rename("r_weighted_qcd_fold0");
    h_j_weighted_qcd_after->rename("j_weighted_qcd_fold1");

    normalise_to_unity(h_r_weighted_qcd_after, h_j_weighted_qcd_after);

    /* set up figures */
    auto collisions = "#sqrt{s_{NN}} = 5.02 TeV"s;

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, mpthf, pt_info, hf_info); };

    auto hb = new pencil();
    hb->category("type", "weighted_qcd", "truth_reco_iso", "qcd_after");

    hb->alias("qcd_after", "Analyzed MC After Unfolding");
    hb->alias("truth_reco_iso", "MC Truth Reco Iso");
    hb->alias("weighted_qcd", "Weighted Analyzed MC");

    /* (1) unfolded MC vs gen truth dr */
    auto p1 = new paper(tag + "_dj_unfolded_mc_vs_truth_reco_iso_vs_weighted", hb);
    p1->divide(ihf->size(), -1);
    p1->accessory(pthf_info);
    apply_style(p1, collisions, -2., 27.);
    p1->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));
    
    h_r_qcd_after->apply([&](TH1* h) { p1->add(h, "qcd_after"); });
    h_r_truth_reco_iso->apply([&](TH1* h, int64_t index) { p1->stack(index + 1, h, "truth_reco_iso"); });
    h_r_weighted_qcd_after->apply([&](TH1* h, int64_t index) { p1->stack(index + 1, h, "weighted_qcd"); });
    
    /* (2) unfolded MC vs gen truth jtpt */
    auto p2 = new paper(tag + "_jtpt_unfolded_mc_vs_truth_reco_iso_vs_weighted", hb);
    p2->divide(ihf->size(), -1);
    p2->accessory(pthf_info);
    apply_style(p2, collisions, -0.001, 0.04);
    p2->accessory(std::bind(line_at, _1, 0.f, rpt[0], rpt[1]));
    
    h_j_qcd_after->apply([&](TH1* h) { p2->add(h, "qcd_after"); });
    h_j_truth_reco_iso->apply([&](TH1* h, int64_t index) { p2->stack(index + 1, h, "truth_reco_iso"); });
    h_j_weighted_qcd_after->apply([&](TH1* h, int64_t index) { p2->stack(index + 1, h, "weighted_qcd"); });

    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");

    in(output, [&]() {
        h_j_qcd_after->save();
        h_j_truth_reco_iso->save();
        h_j_weighted_qcd_after->save();
        h_r_qcd_after->save();
        h_r_truth_reco_iso->save();
        h_r_weighted_qcd_after->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return data_mc_comparison(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
