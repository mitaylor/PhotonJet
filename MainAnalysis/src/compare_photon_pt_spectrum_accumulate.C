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
#include "TF1.h"

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

template <typename... T>
void normalise_to_unity(T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1. / obj->Integral()); }), 0)... };
}

template <typename... T>
void scale_bin_width(T*... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1., "width"); }), 0)... };
}

int compare_photon_pt_spectrum(char const* config, char const* selections, const char* output) {
    auto conf = new configurer(config);

    auto input_data = conf->get<std::string>("input_data");
    auto input_mc = conf->get<std::string>("input_mc");

    auto tag = conf->get<std::string>("tag");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    /* selections */
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto heavyion = sel->get<bool>("heavyion");

    /* load history objects */
    TFile* f_data = new TFile((base + input_data).data(), "read");
    TFile* f_mc = new TFile((base + input_mc).data(), "read");

    auto h_data_nevt = new history<TH1F>(f_data, tag + "_nominal_s_pure_raw_nevt");
    h_data_nevt->rename("h_data_nevt");
    auto h_mc_nevt = new history<TH1F>(f_mc, tag + "_qcd_nominal_s_pure_raw_nevt");
    h_mc_nevt->rename("h_mc_nevt");

    /* create intervals and multivals */
    auto mpthf = new multival(dpt, dhf);
    auto ihf = new interval(dhf);
    auto ipt = new interval(dpt);
    auto fpt = std::bind(&interval::book<TH1F>, ipt, _1, _2, _3);

    auto photon_pt_data = new history<TH1F>("photon_pt_data"s, "", fpt, ihf->size());
    auto photon_pt_mc = new history<TH1F>("photon_pt_mc"s, "", fpt, ihf->size());

    /* set histogram contents */
    for (int64_t i = 0; i < ihf->size(); ++i) {
        for (int j = 0; j < (*photon_pt_data)[i]->GetNbinsX(); ++j) {
            auto index = mpthf->index_for(x{j, i});
            (*photon_pt_data)[i]->SetBinContent(j+1, (*h_data_nevt)[index]->GetBinContent(1));
            (*photon_pt_data)[i]->SetBinError(j+1, (*h_data_nevt)[index]->GetBinError(1));
            (*photon_pt_mc)[i]->SetBinContent(j+1, (*h_mc_nevt)[index]->GetBinContent(1));
            (*photon_pt_mc)[i]->SetBinError(j+1, (*h_mc_nevt)[index]->GetBinError(1));
        }
    }

    scale_bin_width(photon_pt_data, photon_pt_mc);
    normalise_to_unity(photon_pt_data, photon_pt_mc);

    auto photon_pt_ratio = new history<TH1F>("photon_pt_ratio"s, "", fpt, ihf->size());
    for (int64_t i = 0; i < ihf->size(); ++i) {
        (*photon_pt_ratio)[i]->Divide((*photon_pt_data)[i], (*photon_pt_mc)[i]);
    }

    photon_pt_ratio->apply([&](TH1* h) { h->Fit("expo"); });

    auto fit_info = [&](int64_t index) {
        auto photon_pt_ratio_fit = (*photon_pt_ratio)[index-1]->GetFunction("expo");

        auto photon_pt_ratio_fit_const = photon_pt_ratio_fit->GetParameter(0);
        auto photon_pt_ratio_fit_expo = photon_pt_ratio_fit->GetParameter(1);

        char buffer[128] = { '\0' };
        sprintf(buffer, "fit: %.3fe^{%.3f}", photon_pt_ratio_fit_const, photon_pt_ratio_fit_expo);

        TLatex* l = new TLatex();
        l->SetTextAlign(31);
        l->SetTextFont(43);
        l->SetTextSize(12);

        l->DrawLatexNDC(0.865, 0.6, buffer);
    };

    photon_pt_data->apply([&](TH1* h) { h->GetXaxis()->SetRangeUser(40, 200); });
    photon_pt_mc->apply([&](TH1* h) { h->GetXaxis()->SetRangeUser(40, 200); });

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
        }
    };

    auto hb = new pencil();
    hb->category("type", "data", "mc", "ratio");

    if (heavyion) hb->alias("data", "PbPb Data");
    else             hb->alias("data", "pp Data");
    if (heavyion) hb->alias("mc", "PbPb MC");
    else             hb->alias("mc", "pp MC");

    auto p1 = new paper(set + "_" + tag + "_photon_pt_comparison_accumulate", hb);
    p1->divide(ihf->size(), -1);
    p1->accessory(hf_info);
    p1->accessory(kinematics);
    apply_style(p1, cms, system_tag);
    p1->set(paper::flags::logy);
    
    photon_pt_data->apply([&](TH1* h) { p1->add(h, "data"); });
    photon_pt_mc->apply([&](TH1* h, int64_t index) { p1->stack(index + 1, h, "mc"); });

    auto p2 = new paper(set + "_" + tag + "_photon_pt_comparison_accumulate_ratio", hb);
    p2->divide(ihf->size(), -1);
    p2->accessory(hf_info);
    p2->accessory(kinematics);
    p2->accessory(fit_info);
    apply_style(p2, cms, system_tag);
    // p2->set(paper::flags::logy);
    
    photon_pt_ratio->apply([&](TH1* h) { p2->add(h, "ratio"); });

    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");

    in(output, [&]() {
        photon_pt_data->save();
        photon_pt_mc->save();
        photon_pt_ratio->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return compare_photon_pt_spectrum(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
