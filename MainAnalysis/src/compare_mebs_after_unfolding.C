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

    auto input_nominal = conf->get<std::string>("input_nominal");
    auto input_mebs = conf->get<std::string>("input_mebs");

    auto label_nominal = conf->get<std::string>("label_nominal");
    auto label_mebs = conf->get<std::string>("label_mebs");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto type = conf->get<std::string>("type");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    auto bpho_pt = sel->get<std::vector<float>>("photon_pt_bounds");
    auto bdr = sel->get<std::vector<float>>("dr_bounds");
    auto bjet_pt = sel->get<std::vector<float>>("jet_pt_bounds");

    /* create intervals and multivals */
    auto ihf = new interval(dhf);

    /* load history objects */
    TFile* f_nominal = new TFile((base + input_nominal).data(), "read");
    TFile* f_mebs = new TFile((base + input_mebs).data(), "read");

    auto h_nominal_dr = new history<TH1F>(f_nominal, label_nominal + "_fold0");
    auto h_mebs_dr = new history<TH1F>(f_mebs, label_mebs + "_fold0");
    auto h_nominal_jtpt = new history<TH1F>(f_nominal, label_nominal + "_fold1");
    auto h_mebs_jtpt = new history<TH1F>(f_mebs, label_mebs + "_fold1");

    normalise_to_unity(h_nominal_dr, h_mebs_dr, h_nominal_jtpt, h_mebs_jtpt);

    /* set up figures */
    auto system_tag = "  #sqrt{s_{NN}} = 5.02 TeV, 1.69 nb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

    auto hf_info = [&](int64_t index) {
        info_text(index, 0.70, "Cent. %i - %i%%", dcent, true); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto photon_selections = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs)  + 
                ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
            auto jet_selections = "anti-k_{T} R = 0.3, " + to_text(bjet_pt[0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.71, photon_selections.data());
            l->DrawLatexNDC(0.865, 0.65, jet_selections.data());
        }
    };

    auto hb = new pencil();
    hb->category("type", "nominal", "mebs");

    hb->alias("nominal", "10% HF Energy Matching");
    hb->alias("mebs", "Subtracted pfEnergy Matching");

    auto p1 = new paper(set + "_" + type + "_quantitate_aa_mebs_dr_comparison", hb);
    p1->divide(ihf->size(), -1);
    p1->accessory(hf_info);
    p1->accessory(kinematics);
    apply_style(p1, cms, system_tag);
    
    h_nominal_dr->apply([&](TH1* h) { p1->add(h, "nominal"); });
    h_mebs_dr->apply([&](TH1* h, int64_t index) { p1->stack(index + 1, h, "mebs"); });

    auto p2 = new paper(set + "_" + type + "_quantitate_aa_mebs_jtpt_comparison", hb);
    p2->divide(ihf->size(), -1);
    p2->accessory(hf_info);
    p2->accessory(kinematics);
    apply_style(p2, cms, system_tag);
    
    h_nominal_jtpt->apply([&](TH1* h) { p2->add(h, "nominal"); });
    h_mebs_jtpt->apply([&](TH1* h, int64_t index) { p2->stack(index + 1, h, "mebs"); });

    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");

    in(output, [&]() {
        h_nominal_dr->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return compare_before_unfolding(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}