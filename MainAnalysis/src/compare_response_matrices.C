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

int data_mc_comparison(char const* config, const char* output) {
    auto conf = new configurer(config);

    auto input_nominal = conf->get<std::string>("input_nominal");
    auto input_old = conf->get<std::string>("input_old");
    auto input_nominal_unfolding = conf->get<std::string>("input_nominal_unfolding");
    auto input_old_unfolding = conf->get<std::string>("input_old_unfolding");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    /* create intervals and multivals */
    auto ihf = new interval(dhf);

    /* load history objects */
    TFile* f_nominal = new TFile(input_nominal.data(), "read");
    TFile* f_old = new TFile(input_old.data(), "read");
    TFile* f_nominalu = new TFile(input_nominal_unfolding.data(), "read");
    TFile* f_oldu = new TFile(input_old_unfolding.data(), "read");

    /* n */
    // auto h_n_nominal = new history<TH1F>(f_nominal, "aa_n");
    // auto h_n_old = new history<TH1F>(f_old, "aa_n");
    // auto h_n_nominalu = new history<TH1F>(f_nominalu, "aa_n");
    // auto h_n_oldu = new history<TH1F>(f_oldu, "aa_n");

    /* r */
    auto h_r_nominal = new history<TH1F>(f_nominal, "aa_r");
    auto h_r_old = new history<TH1F>(f_old, "aa_r");
    auto h_r_nominalu = new history<TH1F>(f_nominalu, "aa_r");
    auto h_r_oldu = new history<TH1F>(f_oldu, "aa_r");
    
    /* g */
    // auto h_g_nominal = new history<TH1F>(f_nominal, "aa_g");
    // auto h_g_old = new history<TH1F>(f_old, "aa_g");
    // auto h_g_nominalu = new history<TH1F>(f_nominalu, "aa_g");
    // auto h_g_oldu = new history<TH1F>(f_oldu, "aa_g");

    /* normalize distributions */
    // h_r_nominal->divide(*h_n_nominal);
    // h_g_nominal->divide(*h_n_nominal);
    // h_r_old->divide(*h_n_old);
    // h_g_old->divide(*h_n_old);
    // h_r_nominalu->divide(*h_n_nominalu);
    // h_g_nominalu->divide(*h_n_nominalu);
    // h_r_oldu->divide(*h_n_oldu);
    // h_g_oldu->divide(*h_n_oldu);

    /* set up figures */
    auto system_tag = "  #sqrt{s_{NN}} = 5.02 TeV, 1.69 nb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

    auto hf_info = [&](int64_t index) {
        info_text(index, 0.75, "Cent. %i - %i%%", dcent, true); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.71, "40 < p_{T}^{#gamma} < 200, |#eta^{#gamma}| < 1.44");
            l->DrawLatexNDC(0.865, 0.67, "anti-k_{T} R = 0.3, 20 < p_{T}^{jet} < 200, |#eta^{jet}| < 1.6");
        }
    };

    auto hb = new pencil();
    hb->category("type", "nominal", "old", "nominal_previous", "old_previous");

    hb->alias("nominal", "Nominal w/ new weights");
    hb->alias("old", "Old forest w/ new skim");
    hb->alias("nominal_previous", "Nominal w/ old weights");
    hb->alias("old_previous", "Old response matrix");

    /* (1) reco comparisions */
    auto p1 = new paper("vacillate_aa_r_comparison_latest", hb);
    p1->divide(ihf->size(), -1);
    p1->accessory(hf_info);
    p1->accessory(kinematics);
    apply_style(p1, cms, system_tag);
    
    h_r_nominal->apply([&](TH1* h) { p1->add(h, "nominal"); });
    h_r_old->apply([&](TH1* h, int64_t index) { p1->stack(index + 1, h, "old"); });

    auto p2 = new paper("vacillate_aa_r_comparison_nominal", hb);
    p2->divide(ihf->size(), -1);
    p2->accessory(hf_info);
    p2->accessory(kinematics);
    apply_style(p2, cms, system_tag);
    
    h_r_nominal->apply([&](TH1* h) { p2->add(h, "nominal"); });
    h_r_nominalu->apply([&](TH1* h, int64_t index) { p2->stack(index + 1, h, "nominal_previous"); });

    auto p3 = new paper("vacillate_aa_r_comparison_old", hb);
    p3->divide(ihf->size(), -1);
    p3->accessory(hf_info);
    p3->accessory(kinematics);
    apply_style(p3, cms, system_tag);
    
    h_r_old->apply([&](TH1* h) { p3->add(h, "old"); });
    h_r_oldu->apply([&](TH1* h, int64_t index) { p3->stack(index + 1, h, "old_previous"); });
    
    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");
    p3->draw("pdf");

    in(output, [&]() {
        h_r_nominal->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return data_mc_comparison(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
