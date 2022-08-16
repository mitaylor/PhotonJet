#include "../include/lambdas.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
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
 
template <typename... T>
void normalise_to_unity(T&... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1. / obj->Integral("width")); }), 0)... };
}

int jubilate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto rdphi = conf->get<std::vector<float>>("dphi_range");
    auto rdr = conf->get<std::vector<float>>("dr_range");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");

    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto background = conf->get<bool>("background");

    /* convert to integral angle units (cast to double) */
    convert_in_place_pi(rdphi);

    auto ihf = new interval(dhf);

    /* load history objects */
    TFile* f = new TFile(input.data(), "read");

    TH1::SetDefaultSumw2();

    history<TH1F>* nevt;
    history<TH1F>* pjet_es_f_dphi;
    history<TH1F>* pjet_wta_f_dphi;
    history<TH1F>* pjet_f_dr;
    history<TH1F>* mix_pjet_es_f_dphi;
    history<TH1F>* mix_pjet_wta_f_dphi;
    history<TH1F>* mix_pjet_f_dr;

    if (!background) {
        nevt = new history<TH1F>(f, "raw_nevt");

        pjet_es_f_dphi = new history<TH1F>(f, "raw_pjet_es_f_dphi");
        pjet_wta_f_dphi = new history<TH1F>(f, "raw_pjet_wta_f_dphi");
        pjet_f_dr = new history<TH1F>(f, "raw_pjet_f_dr");

        mix_pjet_es_f_dphi = new history<TH1F>(f, "raw_mix_pjet_es_f_dphi");
        mix_pjet_wta_f_dphi = new history<TH1F>(f, "raw_mix_pjet_wta_f_dphi");
        mix_pjet_f_dr = new history<TH1F>(f, "raw_mix_pjet_f_dr");
    } else {
        nevt = new history<TH1F>(f, "bkg_nevt");

        pjet_es_f_dphi = new history<TH1F>(f, "bkg_pjet_es_f_dphi");
        pjet_wta_f_dphi = new history<TH1F>(f, "bkg_pjet_wta_f_dphi");
        pjet_f_dr = new history<TH1F>(f, "bkg_pjet_f_dr");

        mix_pjet_es_f_dphi = new history<TH1F>(f, "bkg_mix_pjet_es_f_dphi");
        mix_pjet_wta_f_dphi = new history<TH1F>(f, "bkg_mix_pjet_wta_f_dphi");
        mix_pjet_f_dr = new history<TH1F>(f, "bkg_mix_pjet_f_dr");
    }

    /* shrink to remove overflow photon pt bin */
    auto shape = nevt->shape();
    shape[0] = shape[0] - 1;

    auto wrap = [&](history<TH1F>*& h) {
        h = h->shrink("s", shape, { 0, 0 }); };

    wrap(nevt);
    wrap(pjet_es_f_dphi);
    wrap(pjet_wta_f_dphi);
    wrap(pjet_f_dr);
    wrap(mix_pjet_es_f_dphi);
    wrap(mix_pjet_wta_f_dphi);
    wrap(mix_pjet_f_dr);

// /* scale by bin width */
// scale_bin_width(
//     pjet_f_x,
//     pjet_f_dr,
//     mix_pjet_f_x,
//     mix_pjet_f_dr);

    /* draw figures */
    auto redraw_dphi_axis = [&](TH1* h, int64_t) {
        transform_axis(h, [](int64_t val) -> float {
            return std::abs(revert_radian(val)); }); };

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, nevt, pt_info, hf_info); };

    auto hb = new pencil();
    hb->category("system", "pp", "PbPb");
    hb->category("type", "raw", "mix");

    hb->set_binary("type");

    auto collisions = system + " #sqrt{s_{NN}} = 5.02 TeV"s;

    auto c1 = new paper(tag + "_mixing_dphi_es_d_pthf", hb);
    apply_style(c1, collisions, -0.3, 3, false);
    c1->accessory(std::bind(line_at, _1, 0.f, rdphi[0], rdphi[1]));
    c1->accessory(pthf_info);
    c1->jewellery(redraw_dphi_axis);
    c1->divide(-1 , ihf->size());

    auto c2 = new paper(tag + "_mixing_dphi_wta_d_pthf", hb);
    apply_style(c2, collisions, -0.3, 3, false);
    c2->accessory(std::bind(line_at, _1, 0.f, rdphi[0], rdphi[1]));
    c2->accessory(pthf_info);
    c2->jewellery(redraw_dphi_axis);
    c2->divide(-1 , ihf->size());

    auto c3 = new paper(tag + "_mixing_dr_d_pthf", hb);
    apply_style(c3, collisions, -1., 24., false);
    c3->accessory(std::bind(line_at, _1, 0.f, rdr[0], rdr[1]));
    c3->accessory(pthf_info);
    c3->divide(-1 , ihf->size());

    for (int64_t i = 0; i < nevt->size(); ++i) {
        c1->add((*pjet_es_f_dphi)[i], system, "raw");
        c1->stack((*mix_pjet_es_f_dphi)[i], system, "mix");

        c2->add((*pjet_wta_f_dphi)[i], system, "raw");
        c2->stack((*mix_pjet_wta_f_dphi)[i], system, "mix");

        c3->add((*pjet_f_dr)[i], system, "raw");
        c3->stack((*mix_pjet_f_dr)[i], system, "mix");
    }

    hb->sketch();

    for (auto const& c : { c1, c2, c3 })
        c->draw("pdf");

    /* save output */
    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return jubilate(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
