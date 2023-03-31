#include "../include/lambdas.h"
#include "../include/specifics.h"
#include "../include/text.h"

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
#include "TLatex.h"
#include "TLine.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename... T>
void normalise_to_unity(T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1. / obj->Integral("width")); }), 0)... };
}

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

int accumulate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto label = conf->get<std::string>("label");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto rjpt = conf->get<std::vector<float>>("jpt_range");
    auto rdr = conf->get<std::vector<float>>("dr_range");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto rrdr = sel->get<std::vector<float>>("rdr_range");
    auto rrpt = sel->get<std::vector<float>>("rpt_range");

    auto const jet_pt_min = sel->get<float>("jet_pt_min");
    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_pt_min = sel->get<float>("photon_pt_min");
    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    auto ihf = new interval(dhf);

    auto mdr = new multival(rrdr, rrpt);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open input files */
    TFile* f = new TFile((base + input).data(), "read");

    /* load histograms */
    auto nevt = new history<TH1F>(f, label + "_raw_nevt"s);

    auto pjet_f_dr = new history<TH1F>(
        f, label + "_raw_sub_pjet_f_dr"s);
    auto pjet_f_jpt = new history<TH1F>(
        f, label + "_raw_sub_pjet_f_jpt"s);
    auto pjet_u_dr = new history<TH1F>(
        f, label + "_raw_sub_pjet_u_dr"s);

    /* rescale by number of signal photons (events) */
    pjet_f_dr->multiply(*nevt);
    pjet_f_jpt->multiply(*nevt);
    pjet_u_dr->multiply(*nevt);

    /* discard overflow photon pt bin */
    auto discard = [](history<TH1F>*& h, int64_t axis) {
        auto shape = h->shape();
        shape[axis] = shape[axis] - 1;
        h = h->shrink("s", shape, std::vector<int64_t>(h->dims(), 0));
    };

    discard(nevt, 0);
    discard(pjet_f_dr, 0);
    discard(pjet_f_jpt, 0);
    discard(pjet_u_dr, 0);

    /* integrate histograms */
    auto nevt_d_pt = nevt->sum(1);
    auto nevt_d_hf = nevt->sum(0);

    auto pjet_f_dr_d_pt = pjet_f_dr->sum(1);
    auto pjet_f_dr_d_hf = pjet_f_dr->sum(0);
    auto pjet_f_jpt_d_pt = pjet_f_jpt->sum(1);
    auto pjet_f_jpt_d_hf = pjet_f_jpt->sum(0);
    auto pjet_u_dr_d_pt = pjet_u_dr->sum(1);
    auto pjet_u_dr_d_hf = pjet_u_dr->sum(0);

    /* normalise by number of signal photons (events) */
    pjet_f_dr->divide(*nevt);
    pjet_f_jpt->divide(*nevt);
    pjet_u_dr->divide(*nevt);

    pjet_f_dr_d_pt->divide(*nevt_d_pt);
    pjet_f_dr_d_hf->divide(*nevt_d_hf);
    pjet_f_jpt_d_pt->divide(*nevt_d_pt);
    pjet_f_jpt_d_hf->divide(*nevt_d_hf);
    pjet_u_dr_d_pt->divide(*nevt_d_pt);
    pjet_u_dr_d_hf->divide(*nevt_d_hf);

    /* normalise to unity */
    normalise_to_unity(
        pjet_f_dr,
        pjet_f_dr_d_pt,
        pjet_f_dr_d_hf);

    title(std::bind(rename_axis, _1, "1/N^{#gammaj}dN/d#deltaj"),
        pjet_f_dr,
        pjet_f_dr_d_pt,
        pjet_f_dr_d_hf);

    /* save histograms */
    in(output, [&]() {
        nevt->save(tag);

        pjet_f_dr->save(tag);
        pjet_f_jpt->save(tag);
        pjet_u_dr->save(tag);

        pjet_f_dr_d_pt->save(tag);
        pjet_f_dr_d_hf->save(tag);
        pjet_f_jpt_d_pt->save(tag);
        pjet_f_jpt_d_hf->save(tag);
        pjet_u_dr_d_pt->save(tag);
        pjet_u_dr_d_hf->save(tag);
    });

    /* draw plots */
    printf("painting..\n");

    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV"s;
    system_tag += (system == "PbPb") ? ", 1.69 nb^{-1}"s : ", 302 pb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}}"s;
    cms += " #it{#scale[1.2]{Preliminary}}"s;
    cms += "                                                                   ";
    cms += "                                                                   ";
    cms += "anti-k_{T} R = 0.3, p_{T}^{jet} > "s + to_text(jet_pt_min) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs) + ", ";
    cms += ", p_{T}^{#gamma} > "s + to_text(photon_pt_min) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs) + ", #Delta#phi_{j#gamma} > "s + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, nevt, pt_info, hf_info); };

    auto hb = new pencil();
    hb->category("system", "PbPb", "pp");
    hb->category("axis", "na", "wta", "es");

    hb->alias("na", "");
    hb->alias("es", "E-Scheme");
    hb->alias("wta", "WTA");

    hb->ditto("es", "na");

    auto suffixes = { "d_pthf"s, "d_pt"s, "d_hf"s };
    auto texts = std::vector<std::function<void(int64_t)>> {
        pthf_info, std::bind(pt_info, _1, 0.75), std::bind(hf_info, _1, 0.75) };

    std::vector<paper*> c1(3, nullptr);
    std::vector<paper*> c2(3, nullptr);
    std::vector<paper*> c3(3, nullptr);

    zip([&](paper*& c, int64_t rows, std::string const& suffix,
            std::function<void(int64_t)> text) {
        c = new paper(set + "_" + tag + "_dr_" + suffix, hb);
        c->divide(-1, rows);
        c->accessory(text);

        apply_style(c, cms, system_tag, -2., 27.);
        c->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));
    }, c1, x{ ihf->size(), 1L, 1L }, suffixes, texts);

    pjet_f_dr->apply([&](TH1* h) { c1[0]->add(h, system); });
    pjet_f_dr_d_pt->apply([&](TH1* h) { c1[1]->add(h, system); });
    pjet_f_dr_d_hf->apply([&](TH1* h) { c1[2]->add(h, system); });

    zip([&](paper*& c, int64_t rows, std::string const& suffix,
            std::function<void(int64_t)> text) {
        c = new paper(set + "_" + tag + "_jpt_" + suffix, hb);
        c->divide(-1, rows);
        c->accessory(text);

        apply_style(c, cms, system_tag, -0.001, 0.02);
        c->accessory(std::bind(line_at, _1, 0.f, rjpt[0], rjpt[1]));
    }, c2, x{ ihf->size(), 1L, 1L }, suffixes, texts);

    pjet_f_jpt->apply([&](TH1* h) { c2[0]->add(h, system); });
    pjet_f_jpt_d_pt->apply([&](TH1* h) { c2[1]->add(h, system); });
    pjet_f_jpt_d_hf->apply([&](TH1* h) { c2[2]->add(h, system); });

    zip([&](paper*& c, int64_t rows, std::string const& suffix,
            std::function<void(int64_t)> text) {
        c = new paper(set + "_" + tag + "_u_dr_" + suffix, hb);
        c->divide(-1, rows);
        c->accessory(text);

        apply_style(c, cms, system_tag, -0.01, 0.07);
        c->accessory(std::bind(line_at, _1, 0.f, 0, mdr->size()));
    }, c3, x{ ihf->size(), 1L, 1L }, suffixes, texts);

    pjet_u_dr->apply([&](TH1* h) { c3[0]->add(h, system); });
    pjet_u_dr_d_pt->apply([&](TH1* h) { c3[1]->add(h, system); });
    pjet_u_dr_d_hf->apply([&](TH1* h) { c3[2]->add(h, system); });

    hb->set_binary("system");
    hb->sketch();

    for (auto const& c : { c1, c2, c3 })
        for (auto p : c) { p->draw("pdf"); }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return accumulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
