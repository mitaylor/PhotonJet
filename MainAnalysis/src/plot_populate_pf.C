#include "../include/text.h"
#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"
#include "../git/history/include/history.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLatex.h"
#include "TLine.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

template <typename... T>
void scale_bin_width(T*... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1., "width"); }), 0)... };
}

int accumulate(char const* config, char const* selections) {
    auto conf = new configurer(config);

    auto rdr = conf->get<std::vector<float>>("dr_range");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");

    auto rdrr = sel->get<std::vector<float>>("drr_range");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");
    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    std::vector<float> dpt = {60.0, 99999.0};

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open input files */
    TFile* f = new TFile("data/arc/pho_60_rebin3/populate_pf_aa_raw.root", "read");

    /* load histograms */
    auto nevt = new history<TH1F>(f, "raw_nevt");
    auto pjet_f_dr_base_part1 = new history<TH1F>(f, "raw_pjet_f_dr_base_part1");
    auto pjet_f_dr_base_part2 = new history<TH1F>(f, "raw_pjet_f_dr_base_part2");
    auto pjet_f_dr_chg_up_part1 = new history<TH1F>(f, "raw_pjet_f_dr_chg_up_part1");
    auto pjet_f_dr_chg_up_part2 = new history<TH1F>(f, "raw_pjet_f_dr_chg_up_part2");
    auto pjet_f_dr_chg_down_part1 = new history<TH1F>(f, "raw_pjet_f_dr_chg_down_part1");
    auto pjet_f_dr_chg_down_part2 = new history<TH1F>(f, "raw_pjet_f_dr_chg_down_part2");
    auto pjet_f_dr_neu_up_part1 = new history<TH1F>(f, "raw_pjet_f_dr_neu_up_part1");
    auto pjet_f_dr_neu_up_part2 = new history<TH1F>(f, "raw_pjet_f_dr_neu_up_part2");
    auto pjet_f_dr_neu_down_part1 = new history<TH1F>(f, "raw_pjet_f_dr_neu_down_part1");
    auto pjet_f_dr_neu_down_part2 = new history<TH1F>(f, "raw_pjet_f_dr_neu_down_part2");
    auto pjet_f_dr_pho_up_part1 = new history<TH1F>(f, "raw_pjet_f_dr_pho_up_part1");
    auto pjet_f_dr_pho_up_part2 = new history<TH1F>(f, "raw_pjet_f_dr_pho_up_part2");
    auto pjet_f_dr_pho_down_part1 = new history<TH1F>(f, "raw_pjet_f_dr_pho_down_part1");
    auto pjet_f_dr_pho_down_part2 = new history<TH1F>(f, "raw_pjet_f_dr_pho_down_part2");

    scale_bin_width(
        pjet_f_dr_base_part1,
        pjet_f_dr_base_part2,
        pjet_f_dr_chg_up_part1, 
        pjet_f_dr_chg_up_part2,
        pjet_f_dr_chg_down_part1,
        pjet_f_dr_chg_down_part2,
        pjet_f_dr_neu_up_part1, 
        pjet_f_dr_neu_up_part2,
        pjet_f_dr_neu_down_part1,
        pjet_f_dr_neu_down_part2,
        pjet_f_dr_pho_up_part1, 
        pjet_f_dr_pho_up_part2,
        pjet_f_dr_pho_down_part1,
        pjet_f_dr_pho_down_part2
    );

    title(std::bind(rename_axis, _1, "1/N^{#gamma}dN/d#deltaj"),
        pjet_f_dr_base_part1,
        pjet_f_dr_base_part2,
        pjet_f_dr_chg_up_part1, 
        pjet_f_dr_chg_up_part2,
        pjet_f_dr_chg_down_part1,
        pjet_f_dr_chg_down_part2,
        pjet_f_dr_neu_up_part1, 
        pjet_f_dr_neu_up_part2,
        pjet_f_dr_neu_down_part1,
        pjet_f_dr_neu_down_part2,
        pjet_f_dr_pho_up_part1, 
        pjet_f_dr_pho_up_part2,
        pjet_f_dr_pho_down_part1,
        pjet_f_dr_pho_down_part2
    );

    auto system_tag = "PbPb #sqrt{s_{NN}} = 5.02 TeV, 1.69 nb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;
    cms += "                                                                   ";
    cms += "                                                                   ";
    cms += "R = 0.3, 30 < p_{T}^{jet} < 60 GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);
    cms += ", p_{T}^{#gamma} > 60 GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs) + ", #Delta#phi_{j#gamma} > "s + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, nevt, pt_info, hf_info); };

    auto hb = new pencil();
    hb->category("type", "base", "chg_up", "chg_down", "neu_up", "neu_down", "pho_up", "pho_down");

    hb->alias("base", "Nominal");
    hb->alias("chg_up", "Chg. Had. PF +1%");
    hb->alias("chg_down", "Chg. Had. PF -1%");
    hb->alias("neu_up", "Neut. Had. PF +3%");
    hb->alias("neu_down", "Neut. Had. PF -3%");
    hb->alias("pho_up", "Pho PF +1%");
    hb->alias("pho_down", "Pho PF -1%");

    auto c1 = new paper(set + "_aa_dr_chg_up_part1", hb);
    c1->divide(4, -1);
    c1->accessory(std::bind(hf_info, _1, 0.75));
    apply_style(c1, cms, system_tag, -1., 15.);
    c1->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    pjet_f_dr_base_part1->apply([&](TH1* h) { c1->add(h, "base"); });
    pjet_f_dr_chg_up_part1->apply([&](TH1* h, int64_t index) { c1->stack(index, h, "chg_up"); });

    auto c2 = new paper(set + "_aa_dr_chg_down_part1", hb);
    c2->divide(4, -1);
    c2->accessory(std::bind(hf_info, _1, 0.75));
    apply_style(c2, cms, system_tag, -1., 15.);
    c2->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    pjet_f_dr_base_part1->apply([&](TH1* h) { c2->add(h, "base"); });
    pjet_f_dr_chg_down_part1->apply([&](TH1* h, int64_t index) { c2->stack(index, h, "chg_down"); });

    auto c3 = new paper(set + "_aa_dr_neu_up_part1", hb);
    c3->divide(4, -1);
    c3->accessory(std::bind(hf_info, _1, 0.75));
    apply_style(c3, cms, system_tag, -1., 15.);
    c3->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    pjet_f_dr_base_part1->apply([&](TH1* h) { c3->add(h, "base"); });
    pjet_f_dr_neu_up_part1->apply([&](TH1* h, int64_t index) { c3->stack(index, h, "neu_up"); });

    auto c4 = new paper(set + "_aa_dr_neu_down_part1", hb);
    c4->divide(4, -1);
    c4->accessory(std::bind(hf_info, _1, 0.75));
    apply_style(c4, cms, system_tag, -1., 15.);
    c4->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    pjet_f_dr_base_part1->apply([&](TH1* h) { c4->add(h, "base"); });
    pjet_f_dr_neu_down_part1->apply([&](TH1* h, int64_t index) { c4->stack(index, h, "neu_down"); });

    auto c5 = new paper(set + "_aa_dr_pho_up_part1", hb);
    c5->divide(4, -1);
    c5->accessory(std::bind(hf_info, _1, 0.75));
    apply_style(c5, cms, system_tag, -1., 15.);
    c5->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    pjet_f_dr_base_part1->apply([&](TH1* h) { c5->add(h, "base"); });
    pjet_f_dr_pho_up_part1->apply([&](TH1* h, int64_t index) { c5->stack(index, h, "pho_up"); });

    auto c6 = new paper(set + "_aa_dr_pho_down_part1", hb);
    c6->divide(4, -1);
    c6->accessory(std::bind(hf_info, _1, 0.75));
    apply_style(c6, cms, system_tag, -1., 15.);
    c6->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    pjet_f_dr_base_part1->apply([&](TH1* h) { c6->add(h, "base"); });
    pjet_f_dr_pho_down_part1->apply([&](TH1* h, int64_t index) { c6->stack(index, h, "pho_down"); });

    cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;
    cms += "                                                                   ";
    cms += "                                                                   ";
    cms += "R = 0.3, 60 < p_{T}^{jet} < 200 GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);
    cms += ", p_{T}^{#gamma} > 60 GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs) + ", #Delta#phi_{j#gamma} > "s + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);


    auto c7 = new paper(set + "_aa_dr_chg_up_part2", hb);
    c7->divide(4, -1);
    c7->accessory(std::bind(hf_info, _1, 0.75));
    apply_style(c7, cms, system_tag, -1., 15.);
    c7->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    pjet_f_dr_base_part2->apply([&](TH1* h) { c7->add(h, "base"); });
    pjet_f_dr_chg_up_part2->apply([&](TH1* h, int64_t index) { c7->stack(index, h, "chg_up"); });

    auto c8 = new paper(set + "_aa_dr_chg_down_part2", hb);
    c8->divide(4, -1);
    c8->accessory(std::bind(hf_info, _1, 0.75));
    apply_style(c8, cms, system_tag, -1., 15.);
    c8->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    pjet_f_dr_base_part2->apply([&](TH1* h) { c8->add(h, "base"); });
    pjet_f_dr_chg_down_part2->apply([&](TH1* h, int64_t index) { c8->stack(index, h, "chg_down"); });

    auto c9 = new paper(set + "_aa_dr_neu_up_part2", hb);
    c9->divide(4, -1);
    c9->accessory(std::bind(hf_info, _1, 0.75));
    apply_style(c9, cms, system_tag, -1., 15.);
    c9->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    pjet_f_dr_base_part2->apply([&](TH1* h) { c9->add(h, "base"); });
    pjet_f_dr_neu_up_part2->apply([&](TH1* h, int64_t index) { c9->stack(index, h, "neu_up"); });

    auto c10 = new paper(set + "_aa_dr_neu_down_part2", hb);
    c10->divide(4, -1);
    c10->accessory(std::bind(hf_info, _1, 0.75));
    apply_style(c10, cms, system_tag, -1., 15.);
    c10->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    pjet_f_dr_base_part2->apply([&](TH1* h) { c10->add(h, "base"); });
    pjet_f_dr_neu_down_part2->apply([&](TH1* h, int64_t index) { c10->stack(index, h, "neu_down"); });

    auto c11 = new paper(set + "_aa_dr_pho_up_part2", hb);
    c11->divide(4, -1);
    c11->accessory(std::bind(hf_info, _1, 0.75));
    apply_style(c11, cms, system_tag, -1., 15.);
    c11->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    pjet_f_dr_base_part2->apply([&](TH1* h) { c11->add(h, "base"); });
    pjet_f_dr_pho_up_part2->apply([&](TH1* h, int64_t index) { c11->stack(index, h, "pho_up"); });

    auto c12 = new paper(set + "_aa_dr_pho_down_part2", hb);
    c12->divide(4, -1);
    c12->accessory(std::bind(hf_info, _1, 0.75));
    apply_style(c12, cms, system_tag, -1., 15.);
    c12->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));

    pjet_f_dr_base_part2->apply([&](TH1* h) { c12->add(h, "base"); });
    pjet_f_dr_pho_down_part2->apply([&](TH1* h, int64_t index) { c12->stack(index, h, "pho_down"); });

    hb->sketch();

    c1->draw("pdf");
    c2->draw("pdf");
    c3->draw("pdf");
    c4->draw("pdf");
    c5->draw("pdf");
    c6->draw("pdf");
    c7->draw("pdf");
    c8->draw("pdf");
    c9->draw("pdf");
    c10->draw("pdf");
    c11->draw("pdf");
    c12->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return accumulate(argv[1], argv[2]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
