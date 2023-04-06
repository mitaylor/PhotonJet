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
        obj->Scale(1. / obj->Integral()); }), 0)... };
}

template <typename... T>
void scale_bin_width(T*... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1., "width"); }), 0)... };
}

int compare_photon_pt_spectrum(char const* config, const char* output) {
    auto conf = new configurer(config);

    auto input_data = conf->get<std::string>("input_data");
    auto input_mc = conf->get<std::string>("input_mc");

    auto tag = conf->get<std::string>("tag");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    /* load history objects */
    TFile* f_data = new TFile(input_data.data(), "read");
    TFile* f_mc = new TFile(input_mc.data(), "read"); 

    auto h_data = new history<TH1F>(f_data, "raw_pt_spectrum");
    h_data->rename("h_data");
    auto h_mc = new history<TH1F>(f_mc, "raw_pt_spectrum");
    h_mc->rename("h_mc");

    /* create intervals and multivals */
    auto ihf = new interval(dhf);
    
    scale_bin_width(h_data, h_mc);
    normalise_to_unity(h_data, h_mc);

    h_data->GetXaxis()->SetRangeUser(40, 200);
    h_mc->GetXaxis()->SetRangeUser(40, 200);

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
    hb->category("type", "data", "mc");

    if (tag == "aa") hb->alias("data", "PbPb Data");
    else             hb->alias("data", "pp Data");
    if (tag == "aa") hb->alias("mc", "PbPb MC");
    else             hb->alias("mc", "pp MC");

    auto p1 = new paper("photon_pt_comparison_" + tag, hb);
    p1->divide(ihf->size(), -1);
    p1->accessory(hf_info);
    p1->accessory(kinematics);
    apply_style(p1, cms, system_tag);
    
    h_data->apply([&](TH1* h) { p1->add(h, "data"); });
    h_mc->apply([&](TH1* h, int64_t index) { p1->stack(index + 1, h, "mc"); });

    hb->sketch();

    p1->draw("pdf");

    in(output, [&]() {
        h_data->save();
        h_mc->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return compare_photon_pt_spectrum(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
