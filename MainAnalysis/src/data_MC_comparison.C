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

int data_mc_comparison(char const* config, const char* output) {
    auto conf = new configurer(config);

    auto input_data = conf->get<std::string>("input_data");
    auto r_data_before_label = conf->get<std::string>("r_data_before_label");
    auto r_data_after_label = conf->get<std::string>("r_data_after_label");
    auto r_data_circle_label = conf->get<std::string>("r_data_circle_label");

    auto input_qcd = conf->get<std::string>("input_qcd");
    auto r_qcd_before_label = conf->get<std::string>("r_qcd_before_label");
    auto j_qcd_before_label = conf->get<std::string>("j_qcd_before_label");
    auto r_qcd_after_label = conf->get<std::string>("r_qcd_after_label");
    auto j_qcd_after_label = conf->get<std::string>("j_qcd_after_label");
    auto r_qcd_circle_label = conf->get<std::string>("r_qcd_circle_label");
    auto j_qcd_circle_label = conf->get<std::string>("j_qcd_circle_label");

    auto r_r_label = conf->get<std::string>("r_r_label");
    auto j_r_label = conf->get<std::string>("j_r_label");

    auto input_truth = conf->get<std::string>("input_truth");
    auto r_truth_gen_iso_label = conf->get<std::string>("r_truth_gen_iso_label");
    auto j_truth_gen_iso_label = conf->get<std::string>("j_truth_gen_iso_label");
    auto r_truth_reco_iso_label = conf->get<std::string>("r_truth_reco_iso_label");
    auto j_truth_reco_iso_label = conf->get<std::string>("j_truth_reco_iso_label");
    auto r_reco_reco_iso_label_matched = conf->get<std::string>("r_reco_reco_iso_label_matched");
    auto r_reco_reco_iso_label_unmatched = conf->get<std::string>("r_reco_reco_iso_label_unmatched");
    auto j_reco_reco_iso_label_matched = conf->get<std::string>("j_reco_reco_iso_label_matched");
    auto j_reco_reco_iso_label_unmatched = conf->get<std::string>("j_reco_reco_iso_label_unmatched");

    auto tag = conf->get<std::string>("tag");

    auto rdr = conf->get<std::vector<float>>("dr_range");
    auto rpt = conf->get<std::vector<float>>("pt_range");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    /* create intervals and multivals */
    auto ihf = new interval(dhf);
    auto mpthf = new multival(dpt, dhf);

    /* load history objects */
    TFile* fdata = new TFile(input_data.data(), "read");
    TFile* fqcd = new TFile(input_qcd.data(), "read");
    TFile* ftruth = new TFile(input_truth.data(), "read");

    auto h_r_data_before = new history<TH1F>(fdata, tag + "_"s + r_data_before_label);
    auto h_r_data_after = new history<TH1F>(fdata, tag + "_"s + r_data_after_label);
    auto h_r_data_circle = new history<TH1F>(fdata, tag + "_"s + r_data_circle_label);

    auto h_r_qcd_before = new history<TH1F>(fqcd, tag + "_"s + r_qcd_before_label);
    auto h_j_qcd_before = new history<TH1F>(fqcd, tag + "_"s + j_qcd_before_label);
    auto h_r_qcd_after = new history<TH1F>(fqcd, tag + "_"s + r_qcd_after_label);
    auto h_j_qcd_after = new history<TH1F>(fqcd, tag + "_"s + j_qcd_after_label);
    auto h_r_qcd_circle = new history<TH1F>(fqcd, tag + "_"s + r_qcd_circle_label);
    auto h_j_qcd_circle = new history<TH1F>(fqcd, tag + "_"s + j_qcd_circle_label);

    auto h_r_r = new history<TH1F>(fqcd, tag + "_"s + r_r_label);
    auto h_j_r = new history<TH1F>(fqcd, tag + "_"s + j_r_label);

    auto h_r_truth_gen_iso = new history<TH1F>(ftruth, tag + "_"s + r_truth_gen_iso_label);
    auto h_j_truth_gen_iso = new history<TH1F>(ftruth, tag + "_"s + j_truth_gen_iso_label);
    auto h_r_truth_reco_iso = new history<TH1F>(ftruth, tag + "_"s + r_truth_reco_iso_label);
    auto h_j_truth_reco_iso = new history<TH1F>(ftruth, tag + "_"s + j_truth_reco_iso_label);
    auto h_r_reco_reco_iso_matched = new history<TH1F>(ftruth, tag + "_"s + r_reco_reco_iso_label_matched);
    auto h_r_reco_reco_iso_unmatched = new history<TH1F>(ftruth, tag + "_"s + r_reco_reco_iso_label_unmatched);
    auto h_j_reco_reco_iso_matched = new history<TH1F>(ftruth, tag + "_"s + j_reco_reco_iso_label_matched);
    auto h_j_reco_reco_iso_unmatched = new history<TH1F>(ftruth, tag + "_"s + j_reco_reco_iso_label_unmatched);

    in(output, [&]() {
        h_r_qcd_after->save(tag);
        h_j_qcd_after->save(tag);
        h_r_truth_reco_iso->save(tag);
        h_j_truth_reco_iso->save(tag);
    };

    h_r_data_circle->apply([&](TH1* h) { for (int64_t i = 1; i <= h->GetNbinsX(); ++i) {
        h->SetBinError(i, 0); } });

    /* set up figures */
    auto collisions = "#sqrt{s_{NN}} = 5.02 TeV"s;

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, mpthf, pt_info, hf_info); };

    auto hb = new pencil();
    hb->category("type", "data_before", "qcd_before", "truth_reco_iso", 
                 "data_after", "qcd_after", "truth_gen_iso", "data_circle",
                 "reco_matched", "reco_unmatched", "matrix");

    hb->alias("data_before", "Data Before Unfolding");
    hb->alias("qcd_circle", "Analyzed MC After Refolding");
    hb->alias("qcd_before", "Analyzed MC Before Unfolding");
    hb->alias("qcd_after", "Analyzed MC After Unfolding");
    hb->alias("truth_gen_iso", "MC Truth Gen Iso");
    hb->alias("truth_reco_iso", "MC Truth Reco Iso");
    hb->alias("data_circle", "Refolded Data");
    hb->alias("reco_matched", "MC Reco + Gen Matched");
    hb->alias("reco_unmatched", "MC Reco + Not Gen Matched");
    hb->alias("matrix", "Response Matrix Reco");

    /* (1) unfolded MC vs reco truth dr */
    auto p1 = new paper(tag + "_dj_unfolded_mc_vs_truth_reco_iso", hb);
    p1->divide(ihf->size(), -1);
    p1->accessory(pthf_info);
    apply_style(p1, collisions, -2., 27.);
    p1->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));
    
    h_r_qcd_after->apply([&](TH1* h) { p1->add(h, "qcd_after"); });
    h_r_truth_reco_iso->apply([&](TH1* h, int64_t index) { p1->stack(index + 1, h, "truth_reco_iso"); });
    
    /* (2) unfolded MC vs reco truth jtpt */
    auto p2 = new paper(tag + "_jtpt_unfolded_mc_vs_truth_reco_iso", hb);
    p2->divide(ihf->size(), -1);
    p2->accessory(pthf_info);
    apply_style(p2, collisions, -0.001, 0.04);
    p2->accessory(std::bind(line_at, _1, 0.f, rpt[0], rpt[1]));
    
    h_j_qcd_after->apply([&](TH1* h) { p2->add(h, "qcd_after"); });
    h_j_truth_reco_iso->apply([&](TH1* h, int64_t index) { p2->stack(index + 1, h, "truth_reco_iso"); });

    /* (3) data vs refolded data */
    auto p3 = new paper(tag + "_refolding_test", hb);
    p3->divide(ihf->size(), -1);
    p3->accessory(pthf_info);
    apply_style(p3, collisions, -2., 27.);
    p3->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    h_r_data_before->apply([&](TH1* h) { p3->add(h, "data_before"); });
    h_r_data_circle->apply([&](TH1* h, int64_t index) { p3->stack(index + 1, h, "data_circle"); });

    /* (5) matched vs unmatched dr */
    auto p5 = new paper(tag + "_dj_matched_unmatched", hb);
    p5->divide(ihf->size(), -1);
    p5->accessory(pthf_info);
    apply_style(p5, collisions, -2., 27.);
    p5->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));
    
    h_r_reco_reco_iso_matched->apply([&](TH1* h) { p5->add(h, "reco_matched"); });
    h_r_reco_reco_iso_unmatched->apply([&](TH1* h, int64_t index) { p5->stack(index + 1, h, "reco_unmatched"); });

    /* (6) matched vs unmatched jtpt */
    auto p6 = new paper(tag + "_jtpt_matched_unmatched", hb);
    p6->divide(ihf->size(), -1);
    p6->accessory(pthf_info);
    apply_style(p6, collisions, -0.001, 0.07);
    p6->accessory(std::bind(line_at, _1, 0.f, rpt[0], rpt[1]));
    
    h_j_reco_reco_iso_matched->apply([&](TH1* h) { p6->add(h, "reco_matched"); });
    h_j_reco_reco_iso_unmatched->apply([&](TH1* h, int64_t index) { p6->stack(index + 1, h, "reco_unmatched"); });

    /* (7) MC vs reco truth dr */
    auto p7 = new paper(tag + "_dj_mc_vs_reco", hb);
    p7->divide(ihf->size(), -1);
    p7->accessory(pthf_info);
    apply_style(p7, collisions, -2., 27.);
    p7->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));
    
    h_r_reco_reco_iso_matched->apply([&](TH1* h) { p7->add(h, "reco_matched"); });
    h_r_qcd_before->apply([&](TH1* h, int64_t index) { p7->stack(index + 1, h, "qcd_before"); });
    h_r_r->apply([&](TH1* h, int64_t index) { p7->stack(index + 1, h, "matrix"); });

    /* (8) MC vs reco truth jtpt */
    auto p8 = new paper(tag + "_jtpt_mc_vs_reco", hb);
    p8->divide(ihf->size(), -1);
    p8->accessory(pthf_info);
    apply_style(p8, collisions, -0.001, 0.04);
    p8->accessory(std::bind(line_at, _1, 0.f, rpt[0], rpt[1]));
    
    h_j_reco_reco_iso_matched->apply([&](TH1* h) { p8->add(h, "reco_matched"); });
    h_j_qcd_before->apply([&](TH1* h, int64_t index) { p8->stack(index + 1, h, "qcd_before"); });
    h_j_r->apply([&](TH1* h, int64_t index) { p8->stack(index + 1, h, "matrix"); });

    /* (9) qcd vs refolded qcd */
    auto p9 = new paper(tag + "_dj_qcd_refolding_test", hb);
    p9->divide(ihf->size(), -1);
    p9->accessory(pthf_info);
    apply_style(p9, collisions, -2., 27.);
    p9->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    h_r_qcd_before->apply([&](TH1* h) { p9->add(h, "qcd_before"); });
    h_r_qcd_circle->apply([&](TH1* h, int64_t index) { p9->stack(index + 1, h, "qcd_circle"); });

    /* (10) qcd vs refolded qcd */
    auto p10 = new paper(tag + "_jtpt_qcd_refolding_test", hb);
    p10->divide(ihf->size(), -1);
    p10->accessory(pthf_info);
    apply_style(p10, collisions, -0.001, 0.04);
    p10->accessory(std::bind(line_at, _1, 0.f, rpt[0], rpt[1]));

    h_j_qcd_before->apply([&](TH1* h) { p10->add(h, "qcd_before"); });
    h_j_qcd_circle->apply([&](TH1* h, int64_t index) { p10->stack(index + 1, h, "qcd_circle"); });

    /* (11) truth reco iso vs truth gen iso */
    auto p11 = new paper(tag + "_dj_reco_iso_vs_gen_iso", hb);
    p11->divide(ihf->size(), -1);
    p11->accessory(pthf_info);
    apply_style(p11, collisions, -2., 27.);
    p11->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));
    
    h_r_truth_gen_iso->apply([&](TH1* h) { h->GetXaxis()->SetTitle("#deltaj"); });
    h_r_truth_reco_iso->apply([&](TH1* h) { h->GetXaxis()->SetTitle("#deltaj"); });
    h_r_truth_gen_iso->apply([&](TH1* h) { p11->add(h, "truth_gen_iso"); });
    h_r_truth_reco_iso->apply([&](TH1* h, int64_t index) { p11->stack(index + 1, h, "truth_reco_iso"); });

    /* (12) truth reco iso vs truth gen iso */
    auto p12 = new paper(tag + "_jtpt_reco_iso_vs_gen_iso", hb);
    p12->divide(ihf->size(), -1);
    p12->accessory(pthf_info);
    apply_style(p12, collisions, -0.001, 0.04);
    p12->accessory(std::bind(line_at, _1, 0.f, rpt[0], rpt[1]));
    
    h_j_truth_gen_iso->apply([&](TH1* h) { h->GetXaxis()->SetTitle("dp_{T}^{j}"); });
    h_j_truth_reco_iso->apply([&](TH1* h) { h->GetXaxis()->SetTitle("dp_{T}^{j}"); });
    h_j_truth_gen_iso->apply([&](TH1* h) { p12->add(h, "truth_gen_iso"); });
    h_j_truth_reco_iso->apply([&](TH1* h, int64_t index) { p12->stack(index + 1, h, "truth_reco_iso"); });

    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");
    p3->draw("pdf");
    p5->draw("pdf");
    p6->draw("pdf");
    p7->draw("pdf");
    p8->draw("pdf");
    p9->draw("pdf");
    p10->draw("pdf");
    p11->draw("pdf");
    p12->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return data_mc_comparison(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
