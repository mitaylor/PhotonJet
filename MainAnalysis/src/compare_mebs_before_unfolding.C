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

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

template <typename... T>
void normalise_to_unity(T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1. / obj->Integral("width")); }), 0)... };
}

int compare_before_unfolding(char const* config, const char* output) {
    auto conf = new configurer(config);

    auto input_eff = conf->get<std::string>("input_eff");
    auto input_noeff = conf->get<std::string>("input_noeff");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    /* create intervals and multivals */
    auto ihf = new interval(dhf);

    /* load history objects */
    TFile* f_eff = new TFile(input_eff.data(), "read");
    TFile* f_noeff = new TFile(input_noeff.data(), "read");

    auto h_eff_dr = new history<TH1F>(f_eff, "aa_nominal_s_pure_raw_sub_pjet_f_dr_sum0");
    auto h_noeff_dr = new history<TH1F>(f_noeff, "aa_nominal_mebs_s_pure_raw_sub_pjet_f_dr_sum0");
    auto h_eff_jtpt = new history<TH1F>(f_eff, "aa_nominal_s_pure_raw_sub_pjet_f_jpt_sum0");
    auto h_noeff_jtpt = new history<TH1F>(f_noeff, "aa_nominal_mebs_s_pure_raw_sub_pjet_f_jpt_sum0");

    normalise_to_unity(h_eff_dr, h_noeff_dr, h_eff_jtpt, h_noeff_jtpt);

    
    /* set up figures */
    auto system_tag = "  #sqrt{s_{NN}} = 5.02 TeV, 1.69 nb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

    auto hf_info = [&](int64_t index) {
        info_text(index, 0.70, "Cent. %i - %i%%", dcent, true); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(12);
            l->DrawLatexNDC(0.865, 0.70, "40 < p_{T}^{#gamma} < 200, |#eta^{#gamma}| < 1.44");
            l->DrawLatexNDC(0.865, 0.65, "anti-k_{T} R = 0.3, 30 < p_{T}^{jet} < 120, |#eta^{jet}| < 1.6");
        }
    };

    auto hb = new pencil();
    hb->category("type", "eff", "noeff");

    hb->alias("eff", "10% HF Energy Matching");
    hb->alias("noeff", "Subtracted pfEnergy Matching");

    auto p1 = new paper("accumulate_aa_mebs_dr_comparison", hb);
    p1->divide(ihf->size(), -1);
    p1->accessory(hf_info);
    p1->accessory(kinematics);
    apply_style(p1, cms, system_tag);
    
    h_eff_dr->apply([&](TH1* h) { p1->add(h, "eff"); });
    h_noeff_dr->apply([&](TH1* h, int64_t index) { p1->stack(index + 1, h, "noeff"); });

    auto p2 = new paper("accumulate_aa_mebs_jtpt_comparison", hb);
    p2->divide(ihf->size(), -1);
    p2->accessory(hf_info);
    p2->accessory(kinematics);
    apply_style(p2, cms, system_tag);
    
    h_eff_jtpt->apply([&](TH1* h) { p2->add(h, "eff"); });
    h_noeff_jtpt->apply([&](TH1* h, int64_t index) { p2->stack(index + 1, h, "noeff"); });

    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");

    in(output, [&]() {
        h_eff_dr->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return compare_before_unfolding(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
