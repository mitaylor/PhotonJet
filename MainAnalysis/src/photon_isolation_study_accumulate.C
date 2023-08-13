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
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

int accumulate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input_data = conf->get<std::string>("input_data");
    auto input_qcd = conf->get<std::string>("input_qcd");

    auto label = conf->get<std::string>("label");
    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");

    auto xmin = conf->get<float>("xmin");
    auto xmax = conf->get<float>("xmax");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    auto ihf = new interval(dhf);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open input files */
    TFile* f_data = new TFile((base + input_data).data(), "read");
    TFile* f_qcd = new TFile((base + input_qcd).data(), "read");

    /* load histograms */
    auto pjet_f_dr_data = new history<TH1F>(
        f_data, label + "_raw_pjet_f_dr"s);
    auto pjet_f_dr_qcd = new history<TH1F>(
        f_qcd, label + "_raw_pjet_f_dr"s);

    pjet_f_dr_data->rename("data");
    pjet_f_dr_qcd->rename("qcd");

    /* discard overflow photon pt bin */
    auto discard = [](history<TH1F>*& h, int64_t axis) {
        auto shape = h->shape();
        shape[axis] = shape[axis] - 1;
        h = h->shrink("s", shape, std::vector<int64_t>(h->dims(), 0));
    };

    discard(pjet_f_dr_data, 0);
    discard(pjet_f_dr_qcd, 0);

    /* integrate histograms */
    auto pjet_f_dr_d_pt_data = pjet_f_dr_data->sum(1);
    auto pjet_f_dr_d_hf_data = pjet_f_dr_data->sum(0);

    auto pjet_f_dr_d_pt_qcd = pjet_f_dr_qcd->sum(1);
    auto pjet_f_dr_d_hf_qcd = pjet_f_dr_qcd->sum(0);

    title(std::bind(rename_axis, _1, "Normalized counts"),
        pjet_f_dr_data,
        pjet_f_dr_d_pt_data,
        pjet_f_dr_d_hf_data,
        pjet_f_dr_qcd,
        pjet_f_dr_d_pt_qcd,
        pjet_f_dr_d_hf_qcd
    );

    /* save histograms */
    in(output, [&]() {
        pjet_f_dr_data->save(tag);
        pjet_f_dr_d_pt_data->save(tag);
        pjet_f_dr_d_hf_data->save(tag);
        pjet_f_dr_qcd->save(tag);
        pjet_f_dr_d_pt_qcd->save(tag);
        pjet_f_dr_d_hf_qcd->save(tag);
    });

    /* draw plots */
    printf("painting..\n");

    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV"s;
    system_tag += (system == "PbPb") ? ", 1.69 nb^{-1}"s : ", 302 pb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}}"s;
    cms += " #it{#scale[1.2]{Preliminary}}"s;
    cms += "                                                                   ";
    cms += "                                                                   ";

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.75, 0.04, pjet_f_dr_data, pt_info, hf_info); };

    auto hb = new pencil();
    hb->category("system", "PbPb", "pp");
    hb->category("type", "data", "MC");

    auto suffixes = { "d_pthf"s, "d_pt"s, "d_hf"s };
    auto texts = std::vector<std::function<void(int64_t)>> {
        pthf_info, std::bind(pt_info, _1, 0.75), std::bind(hf_info, _1, 0.75) };

    std::vector<paper*> c1(3, nullptr);

    zip([&](paper*& c, int64_t rows, std::string const& suffix,
            std::function<void(int64_t)> text) {
        c = new paper(set + "_sumiso_comparison_" + tag + "_" + suffix, hb);
        c->divide(-1, rows);
        c->accessory(text);

        apply_style(c, cms, system_tag);
    }, c1, x{ ihf->size(), 1L, 1L}, suffixes, texts);

    pjet_f_dr_data->apply([&](TH1* h) { 
        h->GetXaxis()->SetRangeUser(xmin, xmax);
        c1[0]->add(h, system, "data"); 
    });
    pjet_f_dr_d_pt_data->apply([&](TH1* h) { 
        h->GetXaxis()->SetRangeUser(xmin, xmax);
        c1[1]->add(h, system, "data"); 
    });
    pjet_f_dr_d_hf_data->apply([&](TH1* h) { 
        h->GetXaxis()->SetRangeUser(xmin, xmax);
        c1[2]->add(h, system, "data"); 
    });

    pjet_f_dr_qcd->apply([&](TH1* h, int64_t index) { c1[0]->stack(index + 1, h, "MC"); });
    pjet_f_dr_d_pt_qcd->apply([&](TH1* h, int64_t index) { c1[1]->stack(index + 1, h, "MC"); });
    pjet_f_dr_d_hf_qcd->apply([&](TH1* h, int64_t index) { c1[2]->stack(index + 1, h, "MC"); });

    hb->set_binary("type");
    hb->sketch();

    for (auto const& c : { c1 })
        for (auto p : c) { p->draw("pdf"); }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return accumulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
