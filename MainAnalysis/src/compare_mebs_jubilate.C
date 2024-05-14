#include "../include/lambdas.h"
#include "../include/specifics.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TH1.h"

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename... T>
void scale_bin_width(T&... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1., "width"); }), 0)... };
}

int jubilate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input_nominal = conf->get<std::string>("input_nominal");
    auto input_altered = conf->get<std::string>("input_altered");

    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto rdr = conf->get<std::vector<float>>("dr_range");
    auto rjpt = conf->get<std::vector<float>>("jpt_range");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_pt_min = sel->get<float>("jet_pt_min");
    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    auto ihf = new interval(dhf);

    /* load history objects */
    TFile* fn = new TFile((base + input_nominal).data(), "read");
    TFile* fa = new TFile((base + input_altered).data(), "read");

    TH1::SetDefaultSumw2();

    history<TH1F>* n_nevt;
    history<TH1F>* n_pjet_f_dr;
    history<TH1F>* n_pjet_f_jpt;
    history<TH1F>* n_mix_pjet_f_dr;
    history<TH1F>* n_mix_pjet_f_jpt;

    history<TH1F>* a_nevt;
    history<TH1F>* a_pjet_f_dr;
    history<TH1F>* a_pjet_f_jpt;
    history<TH1F>* a_mix_pjet_f_dr;
    history<TH1F>* a_mix_pjet_f_jpt;

    n_nevt = new history<TH1F>(fn, "raw_nevt");
    n_nevt->rename("n_raw_nevt");
    n_pjet_f_dr = new history<TH1F>(fn, "raw_pjet_f_dr");
    n_pjet_f_dr->rename("n_raw_pjet_f_dr");
    n_pjet_f_jpt = new history<TH1F>(fn, "raw_pjet_f_jpt");
    n_pjet_f_jpt->rename("n_raw_pjet_f_jpt");
    n_mix_pjet_f_dr = new history<TH1F>(fn, "raw_mix_pjet_f_dr");
    n_mix_pjet_f_dr->rename("n_raw_mix_pjet_f_dr");
    n_mix_pjet_f_jpt = new history<TH1F>(fn, "raw_mix_pjet_f_jpt");
    n_mix_pjet_f_jpt->rename("n_raw_mix_pjet_f_jpt");

    a_nevt = new history<TH1F>(fa, "raw_nevt");
    a_nevt->rename("a_raw_nevt");
    a_pjet_f_dr = new history<TH1F>(fa, "raw_pjet_f_dr");
    a_pjet_f_dr->rename("a_raw_pjet_f_dr");
    a_pjet_f_jpt = new history<TH1F>(fa, "raw_pjet_f_jpt");
    a_pjet_f_jpt->rename("a_raw_pjet_f_jpt");
    a_mix_pjet_f_dr = new history<TH1F>(fa, "raw_mix_pjet_f_dr");
    a_mix_pjet_f_dr->rename("a_raw_mix_pjet_f_dr");
    a_mix_pjet_f_jpt = new history<TH1F>(fa, "raw_mix_pjet_f_jpt");
    a_mix_pjet_f_jpt->rename("a_raw_mix_pjet_f_jpt");

    /* shrink to remove overflow photon pt bin */
    auto shape = n_nevt->shape();
    shape[0] = shape[0] - 1;

    auto wrap = [&](history<TH1F>*& h) {
        h = h->shrink("s", shape, { 0, 0 }); };

    wrap(n_nevt);
    wrap(n_pjet_f_dr);
    wrap(n_pjet_f_jpt);
    wrap(n_mix_pjet_f_dr);
    wrap(n_mix_pjet_f_jpt);

    wrap(a_nevt);
    wrap(a_pjet_f_dr);
    wrap(a_pjet_f_jpt);
    wrap(a_mix_pjet_f_dr);
    wrap(a_mix_pjet_f_jpt);

    /* draw figures */
    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, n_nevt, pt_info, hf_info); };

    auto hb = new pencil();
    hb->category("system", "mebs_nominal", "mebs_altered");
    hb->category("type", "raw", "mix");
    hb->alias("mebs_nominal", "10% HF Energy Matching");
    hb->alias("mebs_altered", "Subtracted pfEnergy Matching");

    hb->set_binary("type");

    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV, 1.69 nb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;
    cms += "                  anti-k_{T} R = 0.3, p_{T}^{jet} > "s + to_text(jet_pt_min) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs) + ", ";
    cms += "|#eta^{#gamma}| < "s + to_text(photon_eta_abs) + ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);

    auto c3 = new paper(set + "_" + tag + "_mebs_mixing_dr_d_pthf", hb);
    apply_style(c3, cms, system_tag, -2.7, 27.);
    c3->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));
    c3->accessory(pthf_info);
    c3->divide(-1 , ihf->size());

    auto c4 = new paper(set + "_" + tag + "_mebs_mixing_jpt_d_pthf", hb);
    apply_style(c4, cms, system_tag, -0.01, 0.01);
    c4->accessory(std::bind(line_at, _1, 0.f, rjpt[0], rjpt[1]));
    c4->accessory(pthf_info);
    c4->divide(-1 , ihf->size());

    for (int64_t i = 0; i < n_nevt->size(); ++i) {
        c3->add((*n_pjet_f_dr)[i], "mebs_nominal", "raw");
        c3->stack((*n_mix_pjet_f_dr)[i], "mebs_nominal", "mix");
        c3->stack((*a_pjet_f_dr)[i], "mebs_altered", "raw");
        c3->stack((*a_mix_pjet_f_dr)[i], "mebs_altered", "mix");

        c4->add((*n_pjet_f_jpt)[i], "mebs_nominal", "raw");
        c4->stack((*n_mix_pjet_f_jpt)[i], "mebs_nominal", "mix");
        c4->stack((*a_pjet_f_jpt)[i], "mebs_altered", "raw");
        c4->stack((*a_mix_pjet_f_jpt)[i], "mebs_altered", "mix");
    }

    hb->sketch();

    for (auto const& c : { c3, c4 })
        c->draw("pdf");

    /* save output */
    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return jubilate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}