#include "../include/lambdas.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TH1.h"

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

int data_mc_comparison(char const* config) {
    auto conf = new configurer(config);

    auto input_data = conf->get<std::string>("input_data");
    auto data_before_label = conf->get<std::string>("data_before_label");
    auto data_after_label = conf->get<std::string>("data_after_label");

    auto input_qcd = conf->get<std::string>("input_qcd");
    auto qcd_before_label = conf->get<std::string>("qcd_before_label");
    auto qcd_after_label = conf->get<std::string>("qcd_after_label");

    auto input_truth = conf->get<std::string>("input_truth");
    auto truth_gen_label = conf->get<std::string>("truth_gen_label");
    auto truth_reco_label = conf->get<std::string>("truth_reco_label");

    auto tag = conf->get<std::string>("tag");

    auto rdr = conf->get<std::vector<float>>("dr_range");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto rdrr = conf->get<std::vector<float>>("drr_range");
    auto rptr = conf->get<std::vector<float>>("ptr_range");

    auto rdrg = conf->get<std::vector<float>>("drg_range");
    auto rptg = conf->get<std::vector<float>>("ptg_range");

    /* create intervals and multivals */
    auto idrr = new interval("#deltaj"s, rdrr);
    auto iptr = new interval("p_{T}^{j}"s, rptr);

    auto idrg = new interval("#deltaj"s, rdrg);
    auto iptg = new interval("p_{T}^{j}"s, rptg);

    auto mr = new multival(*idrr, *iptr);
    auto mg = new multival(*idrg, *iptg);
    
    auto ihf = new interval(dhf);
    auto mpthf = new multival(dpt, dhf);

    /* load history objects */
    TFile* fdata = new TFile(input_data.data(), "read");
    TFile* fqcd = new TFile(input_qcd.data(), "read");
    TFile* ftruth = new TFile(input_truth.data(), "read");

    auto h_data_before = new history<TH1F>(fdata, tag + "_"s + data_before_label);
    auto h_data_after = new history<TH1F>(fdata, tag + "_"s + data_before_label);

    auto h_qcd_before = new history<TH1F>(fqcd, tag + "_"s + qcd_before_label);
    auto h_qcd_after = new history<TH1F>(fqcd, tag + "_"s + qcd_after_label);

    auto h_truth_gen_full = new history<TH1F>(ftruth, tag + "_"s + truth_gen_label);
    auto h_truth_reco_full = new history<TH1F>(ftruth, tag + "_"s + truth_reco_label);

    auto size = h_data_before->size();
    std::array<int64_t, 4> osr = { 0, 0, 1, 3 };
    std::array<int64_t, 4> osg = { 0, 0, 2, 1 };

    auto h_truth_gen = new history<TH1F>("truth", "", null<TH1F>, size);
    auto h_truth_reco = new history<TH1F>("reco", "", null<TH1F>, size);

    for (int64_t i = 0; i < size; ++i) {
        (*fold0)[j] = fold((*unfolded)[j], nullptr, mg, 0, osg);
        (*fold1)[j] = fold((*unfolded)[j], nullptr, mr, 1, osr);
    }

    normalise_to_unity(h_truth_gen, h_truth_reco);

    /* set up figures */
    auto collisions = "#sqrt{s_{NN}} = 5.02 TeV"s;

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, mpthf, pt_info, hf_info); };

    auto hb = new pencil();
    hb->category("before_unfolding", "data_before", "qcd_before", "truth_reco", "data_after", "qcd_after", "truth_gen");

    hb->alias("data_before", "Data Before Unfolding");
    hb->alias("data_after", "Data After Unfolding");
    hb->alias("qcd_before", "MC Before Unfolding");
    hb->alias("qcd_after", "MC After Unfolding");
    hb->alias("truth_gen", "MC Truth Gen");
    hb->alias("truth_reco", "MC Truth Reco");

    /* (1) unfolded MC vs gen truth */
    auto p1 = new paper(tag + "_unfolded_mc_gen_truth", hb);
    p1->divide(ihf->size(), -1);
    p1->accessory(pthf_info);
    apply_style(p1, collisions, -2., 27.);
    p1->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));
    
    h_qcd_after->apply([&](TH1* h) { p1->add(h, "qcd_after"); });
    h_truth_gen->apply([&](TH1* h, int64_t index) { p1->stack(index + 1, h, "truth_gen"); });
    
    /* (2) unfolded data vs unfolded MC vs gen truth */
    auto p2 = new paper(tag + "_unfolded_data_unfolded_mc_gen_truth", hb);
    p2->divide(ihf->size(), -1);
    p2->accessory(pthf_info);
    apply_style(p2, collisions, -2., 27.);
    p2->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    h_data_after->apply([&](TH1* h) { p2->add(h, "data_after"); });
    h_qcd_after->apply([&](TH1* h, int64_t index) { p2->stack(index + 1, h, "qcd_after"); });
    h_truth_gen->apply([&](TH1* h, int64_t index) { p2->stack(index + 1, h, "truth_gen"); });

    /* (3) data vs MC before unfolding vs reco truth*/
    auto p3 = new paper(tag + "_data_mc_before_unfolding", hb);
    p3->divide(ihf->size(), -1);
    p3->accessory(pthf_info);
    apply_style(p3, collisions, -2., 27.);
    p3->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    h_data_before->apply([&](TH1* h) { p3->add(h, "data_before"); });
    h_qcd_before->apply([&](TH1* h, int64_t index) { p3->stack(index + 1, h, "qcd_before"); });
    h_truth_reco->apply([&](TH1* h, int64_t index) { p3->stack(index + 1, h, "truth_reco"); });


    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");
    p3->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 2)
        return data_mc_comparison(argv[1]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
