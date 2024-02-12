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

    auto truth = conf->get<std::string>("truth");
    auto analysis = conf->get<std::string>("analysis");

    auto tag = conf->get<std::string>("tag");
    auto system = conf->get<std::string>("system");

    auto dhf = conf->get<std::vector<float>>("hf_diff");

    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

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
    TFile* a = new TFile((base + analysis).data(), "read");
    TFile* t = new TFile((base + truth).data(), "read");

    TH1::SetDefaultSumw2();

    auto nevt = new history<TH1F>(a, "raw_nevt");

    auto analysis_pjet_f_dr = new history<TH1F>(a, "raw_sub_pjet_f_dr");
    auto analysis_pjet_f_jpt = new history<TH1F>(a, "raw_sub_pjet_f_jpt");
    auto analysis_pjet_u_dr_jpt = new history<TH1F>(a, "raw_sub_pjet_u_dr_jpt");

    auto truth_pjet_f_dr = new history<TH1F>(t, "raw_reco_pjet_f_dr");
    auto truth_pjet_f_jpt = new history<TH1F>(t, "raw_reco_pjet_f_jpt");
    auto truth_pjet_u_dr_jpt = new history<TH1F>(t, "raw_reco_pjet_u_dr_jpt");

    /* shrink to remove overflow photon pt bin */
    auto shape = nevt->shape();
    shape[0] = shape[0] - 1;

    auto wrap = [&](history<TH1F>*& h) {
        h = h->shrink("s", shape, { 0, 0 }); };

    wrap(nevt);

    wrap(analysis_pjet_f_dr);
    wrap(analysis_pjet_f_jpt);
    wrap(analysis_pjet_u_dr_jpt);
    
    wrap(truth_pjet_f_dr);
    wrap(truth_pjet_f_jpt);
    wrap(truth_pjet_u_dr_jpt);

    /* draw figures */
    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, nevt, pt_info, hf_info); };

    auto hb = new pencil();
    hb->category("type", "reco", "analysis");

    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV, 1.69 nb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;
    cms += "                  anti-k_{T} R = 0.3, p_{T}^{jet} > "s + to_text(jet_pt_min) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs) + ", ";
    cms += "|#eta^{#gamma}| < "s + to_text(photon_eta_abs) + ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);

    auto c1 = new paper(set + "_" + tag + "_mebs_dr_d_pthf", hb);
    apply_style(c1, cms, system_tag, -1., 24.);
    c1->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));
    c1->accessory(pthf_info);
    c1->divide(-1 , ihf->size());

    auto c2 = new paper(set + "_" + tag + "_mebs_jpt_d_pthf", hb);
    apply_style(c2, cms, system_tag, -0.007, 0.07);
    c2->accessory(std::bind(line_at, _1, 0.f, rjpt[0], rjpt[1]));
    c2->accessory(pthf_info);
    c2->divide(-1 , ihf->size());

    auto c3 = new paper(set + "_" + tag + "_mebs_dr_jpt_d_pthf", hb);
    apply_style(c3, cms, system_tag, 0, 0.07);
    c3->accessory(pthf_info);
    c3->divide(-1 , ihf->size());

    for (int64_t i = 0; i < nevt->size(); ++i) {
        c1->add((*truth_pjet_f_dr)[i], "reco");
        c1->stack((*analysis_pjet_f_dr)[i], "analysis");

        c2->add((*truth_pjet_f_jpt)[i], "reco");
        c2->stack((*analysis_pjet_f_jpt)[i], "analysis");

        c3->add((*truth_pjet_u_dr_jpt)[i], "reco");
        c3->stack((*analysis_pjet_u_dr_jpt)[i], "analysis");
    }

    hb->sketch();

    for (auto const& c : { c1, c2, c3})
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
