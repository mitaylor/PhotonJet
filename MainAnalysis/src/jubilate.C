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

int jubilate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto dhf = conf->get<std::vector<float>>("hf_diff");

    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto background = conf->get<bool>("background");

    auto rdr = conf->get<std::vector<float>>("dr_range");
    auto rjpt = conf->get<std::vector<float>>("jpt_range");

    /* selections */
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
    TFile* f = new TFile((base + input).data(), "read");

    TH1::SetDefaultSumw2();

    history<TH1F>* nevt;
    history<TH1F>* pjet_f_dr;
    history<TH1F>* pjet_f_jpt;
    history<TH1F>* mix_pjet_f_dr;
    history<TH1F>* mix_pjet_f_jpt;

    if (!background) {
        nevt = new history<TH1F>(f, "raw_nevt");

        pjet_f_dr = new history<TH1F>(f, "raw_pjet_f_dr");
        pjet_f_jpt = new history<TH1F>(f, "raw_pjet_f_jpt");

        mix_pjet_f_dr = new history<TH1F>(f, "raw_mix_pjet_f_dr");
        mix_pjet_f_jpt = new history<TH1F>(f, "raw_mix_pjet_f_jpt");
    } else {
        nevt = new history<TH1F>(f, "bkg_nevt");

        pjet_f_dr = new history<TH1F>(f, "bkg_pjet_f_dr");
        pjet_f_jpt = new history<TH1F>(f, "bkg_pjet_f_jpt");

        mix_pjet_f_dr = new history<TH1F>(f, "bkg_mix_pjet_f_dr");
        mix_pjet_f_jpt = new history<TH1F>(f, "bkg_mix_pjet_f_jpt");
    }

    /* shrink to remove overflow photon pt bin */
    auto shape = nevt->shape();
    shape[0] = shape[0] - 1;

    auto wrap = [&](history<TH1F>*& h) {
        h = h->shrink("s", shape, { 0, 0 }); };

    wrap(nevt);
    wrap(pjet_f_dr);
    wrap(pjet_f_jpt);
    wrap(mix_pjet_f_dr);
    wrap(mix_pjet_f_jpt);

    /* draw figures */
    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, nevt, pt_info, hf_info); };

    auto hb = new pencil();
    hb->category("system", "pp", "PbPb");
    hb->category("type", "raw", "mix");

    hb->set_binary("type");

    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV, 1.69 nb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;
    cms += "                  anti-k_{T} R = 0.3, p_{T}^{jet} > "s + to_text(jet_pt_min) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs) + ", ";
    cms += "|#eta^{#gamma}| < "s + to_text(photon_eta_abs) + ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);

    auto c1 = new paper(set + "_" + tag + "_mixing_dr_d_pthf", hb);
    apply_style(c1, cms, system_tag, -2.7, 27.);
    c1->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));
    c1->accessory(pthf_info);
    c1->divide(-1 , ihf->size());

    auto c2 = new paper(set + "_" + tag + "_mixing_jpt_d_pthf", hb);
    apply_style(c2, cms, system_tag, -0.008, 0.08);
    c2->accessory(std::bind(line_at, _1, 0.f, rjpt[0], rjpt[1]));
    c2->accessory(pthf_info);
    c2->divide(-1 , ihf->size());

    for (int64_t i = 0; i < nevt->size(); ++i) {
        c1->add((*pjet_f_dr)[i], system, "raw");
        c1->stack((*mix_pjet_f_dr)[i], system, "mix");

        c2->add((*pjet_f_jpt)[i], system, "raw");
        c2->stack((*mix_pjet_f_jpt)[i], system, "mix");
    }

    hb->sketch();

    for (auto const& c : { c1, c2 })
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
