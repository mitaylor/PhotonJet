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
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

template <typename... T>
void normalise_to_unity(T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1. / obj->Integral("width")); }), 0)... };
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

    /* create intervals */
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

    auto h_truth_gen = new history<TH1F>(ftruth, tag + "_"s + truth_gen_label);
    auto h_truth_reco = new history<TH1F>(ftruth, tag + "_"s + truth_reco_label);

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
    hb->category("before_unfolding", "data_before", "qcd_before", "truth_reco");
    hb->category("after_unfolding", "data_after", "qcd_after", "truth_gen");

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
    h_truth_gen->apply([&](TH1* h, int64_t index) { p1->stack(index, h, "truth_gen"); });
    
    /* (2) unfolded data vs unfolded MC vs gen truth */
    auto p2 = new paper(tag + "_unfolded_data_unfolded_mc_gen_truth", hb);
    p2->divide(ihf->size(), -1);
    p2->accessory(pthf_info);
    apply_style(p2, collisions, -2., 27.);
    p2->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    h_data_after->apply([&](TH1* h) { p2->add(h, "data_after"); });
    h_qcd_after->apply([&](TH1* h, int64_t index) { p2->stack(index, h, "qcd_after"); });
    h_truth_gen->apply([&](TH1* h, int64_t index) { p2->stack(index, h, "truth_gen"); });

    /* (3) data vs MC before unfolding vs reco truth*/
    auto p3 = new paper(tag + "_data_mc_before_unfolding", hb);
    p3->divide(ihf->size(), -1);
    p3->accessory(pthf_info);
    apply_style(p3, collisions, -2., 27.);
    p3->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    h_data_before->apply([&](TH1* h) { p3->add(h, "data_before"); });
    h_qcd_before->apply([&](TH1* h, int64_t index) { p3->stack(index, h, "qcd_before"); });
    h_truth_reco->apply([&](TH1* h, int64_t index) { p3->stack(index, h, "truth_reco"); });


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
