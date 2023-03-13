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

int compare_after_unfolding(char const* config, const char* output) {
    auto conf = new configurer(config);

    auto input_new = conf->get<std::string>("input_new");
    auto input_old = conf->get<std::string>("input_old");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    /* create intervals and multivals */
    auto ihf = new interval(dhf);

    /* load history objects */
    TFile* f_new = new TFile(input_new.data(), "read");
    TFile* f_old = new TFile(input_old.data(), "read");

    auto h_new_dr = new history<TH1F>(f_new, "aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_unfolded_fold0");
    h_new_dr->rename("aa_new_dr");
    auto h_old_dr = new history<TH1F>(f_old, "aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_unfolded_fold0");
    h_old_dr->rename("aa_old_dr");
    auto h_new_jtpt = new history<TH1F>(f_new, "aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_unfolded_fold1");
    h_new_jtpt->rename("aa_new_jtpt");
    auto h_old_jtpt = new history<TH1F>(f_old, "aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_unfolded_fold1");
    h_old_jtpt->rename("aa_old_jtpt");

    normalise_to_unity(h_new_dr, h_old_dr, h_new_jtpt, h_old_jtpt);

    
    /* set up figures */
    auto system_tag = "  #sqrt{s_{NN}} = 5.02 TeV"s;
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
            l->DrawLatexNDC(0.865, 0.67, "anti-k_{T} R = 0.3, |#eta^{jet}| < 1.6");
        }
    };

    auto hb = new pencil();
    hb->category("type", "new", "old");

    hb->alias("new", "30 < jet pT < 120 GeV");
    hb->alias("old", "30 < jet pT < 80 GeV");

    auto p1 = new paper("jet_cut_aa_dr_comparison", hb);
    p1->divide(ihf->size(), -1);
    p1->accessory(hf_info);
    p1->accessory(kinematics);
    apply_style(p1, cms, system_tag);
    
    h_new_dr->apply([&](TH1* h) { p1->add(h, "new"); });
    h_old_dr->apply([&](TH1* h, int64_t index) { p1->stack(index + 1, h, "old"); });

    auto p2 = new paper("jet_cut_aa_jtpt_comparison", hb);
    p2->divide(ihf->size(), -1);
    p2->accessory(hf_info);
    p2->accessory(kinematics);
    apply_style(p2, cms, system_tag);
    
    h_new_jtpt->apply([&](TH1* h) { p2->add(h, "new"); });
    h_old_jtpt->apply([&](TH1* h, int64_t index) { p2->stack(index + 1, h, "old"); });

    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");

    in(output, [&]() {
        h_new_dr->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return compare_after_unfolding(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
