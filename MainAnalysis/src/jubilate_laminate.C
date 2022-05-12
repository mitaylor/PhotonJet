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

template <typename... T> //obj->Integral("width")
void normalise_to_unity(double integral, T&... args) {
    (void)(int [sizeof...(T)]) { (args->apply([integral](TH1* obj) {
        obj->Scale(1. / integral); }), 0)... };
}

int jubilate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");

    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");
    auto background = conf->get<bool>("background");

    auto ihf = new interval(dhf);

    /* load history objects */
    TFile* f = new TFile(input.data(), "read");

    TH1::SetDefaultSumw2();

    history<TH1F>* pjet_lead_jet_deta;
    history<TH1F>* mix_pjet_lead_jet_deta;

    if (!background) {
        pjet_lead_jet_deta = new history<TH1F>(f, "raw_pjet_lead_jet_deta");
        mix_pjet_lead_jet_deta = new history<TH1F>(f, "raw_mix_pjet_lead_jet_deta");
    } else {
        pjet_lead_jet_deta = new history<TH1F>(f, "bkg_pjet_lead_jet_deta");
        mix_pjet_lead_jet_deta = new history<TH1F>(f, "bkg_mix_pjet_lead_jet_deta");
    }

    /* normalize */
    for (int64_t i = 0; i < pjet_lead_jet_deta->size(); ++i) {
        auto maximum = std::max((*pjet_lead_jet_deta)[i]->GetMaximum(),(*mix_pjet_lead_jet_deta)[i]->GetMaximum());
        maximum *= 1.5;

        (*pjet_lead_jet_deta)[i]->SetMaximum(maximum);
        (*mix_pjet_lead_jet_deta)[i]->SetMaximum(maximum);

        (*pjet_lead_jet_deta)[i]->SetMinimum(0);
        (*mix_pjet_lead_jet_deta)[i]->SetMinimum(0);

        (*pjet_lead_jet_deta)[i]->GetXaxis()->SetRange(0,1.6);
        (*mix_pjet_lead_jet_deta)[i]->GetXaxis()->SetRange(0,1.6);

        (*pjet_lead_jet_deta)[i]->GetYaxis()->SetTitle("dN/d#Delta#eta^{jj}");
        (*mix_pjet_lead_jet_deta)[i]->GetYaxis()->SetTitle("dN/d#Delta#eta^{jj}");
    }


    /* shrink to remove overflow photon pt bin */
    auto shape = pjet_lead_jet_deta->shape();
    shape[0] = shape[0] - 1;

    auto wrap = [&](history<TH1F>*& h) {
        h = h->shrink("s", shape, { 0, 0 }); };

    wrap(pjet_lead_jet_deta);
    wrap(mix_pjet_lead_jet_deta);

    /* draw figures */
    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{cor jet} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, pjet_lead_jet_deta, pt_info, hf_info); };

    auto hb = new pencil();
    hb->category("system", "pp", "PbPb");
    hb->category("type", "raw", "mix");

    hb->set_binary("type");

    auto collisions = system + " #sqrt{s_{NN}} = 5.02 TeV"s;

    auto c1 = new paper(tag + "_mixing_jet_deta_pthf", hb);
    apply_style(c1, collisions);
    c1->accessory(pthf_info);
    c1->divide(-1 , ihf->size());

    for (int64_t i = 0; i < pjet_lead_jet_deta->size(); ++i) {
        c1->add((*pjet_lead_jet_deta)[i], system, "raw");
        c1->stack((*mix_pjet_lead_jet_deta)[i], system, "mix");
    }

    hb->sketch();

    for (auto const& c : { c1 })
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
