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
#include "TH1.h"
#include "TH2.h"

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

int compare_before_unfolding(char const* config, char const* selections, const char* output) {
    auto conf = new configurer(config);

    auto input_analysis = conf->get<std::string>("input_analysis");
    auto input_truth = conf->get<std::string>("input_truth");

    auto analysis_label = conf->get<std::string>("analysis_label");
    auto truth_label = conf->get<std::string>("truth_label");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_pt_min = sel->get<float>("photon_pt_min");
    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    /* create intervals and multivals */
    auto ihf = new interval(dhf);

    /* load history objects */
    TFile* f_analysis = new TFile((base + input_analysis).data(), "read");
    TFile* f_truth = new TFile((base + input_truth).data(), "read");

    auto h_analysis_dr = new history<TH1F>(f_analysis, analysis_label + "_raw_sub_pjet_f_dr_sum0"s);
    auto h_truth_dr = new history<TH1F>(f_truth, truth_label + "_raw_sub_pjet_f_dr_sum0"s);
    auto h_analysis_jtpt = new history<TH1F>(f_analysis, analysis_label + "_raw_sub_pjet_f_jpt_sum0"s);
    auto h_truth_jtpt = new history<TH1F>(f_truth, truth_label + "_raw_sub_pjet_f_jpt_sum0"s);

    // normalise_to_unity(h_analysis_dr, h_truth_dr, h_analysis_jtpt, h_truth_jtpt);

    /* set up figures */
    auto system_tag = "  #sqrt{s_{NN}} = 5.02 TeV, 1.69 nb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

    auto hf_info = [&](int64_t index) {
        info_text(index, 0.70, "Cent. %i - %i%%", dcent, true); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto photon_selections = to_text(photon_pt_min) + " < p_{T}^{#gamma} < 200 GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs)  + 
                ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
            auto jet_selections = "anti-k_{T} R = 0.3, |#eta^{jet}| < "s + to_text(jet_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.70, photon_selections.data());
        }
    };

    auto hb = new pencil();
    hb->category("type", "analysis", "truth");

    hb->alias("analysis", "Analysis");
    hb->alias("truth", "MEBS Nonclosure Syst");

    auto p1 = new paper(set + "_mebs_nonclosure_aa_dr", hb);
    p1->divide(ihf->size(), -1);
    p1->accessory(hf_info);
    p1->accessory(kinematics);
    apply_style(p1, cms, system_tag);
    
    h_analysis_dr->apply([&](TH1* h) { p1->add(h, "analysis"); });
    h_truth_dr->apply([&](TH1* h, int64_t index) { p1->stack(index + 1, h, "truth"); });

    auto p2 = new paper(set + "_mebs_nonclosure_aa_jtpt", hb);
    p2->divide(ihf->size(), -1);
    p2->accessory(hf_info);
    p2->accessory(kinematics);
    apply_style(p2, cms, system_tag);
    
    h_analysis_jtpt->apply([&](TH1* h) { p2->add(h, "analysis"); });
    h_truth_jtpt->apply([&](TH1* h, int64_t index) { p2->stack(index + 1, h, "truth"); });

    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");

    in(output, [&]() {
        h_analysis_dr->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return compare_before_unfolding(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}