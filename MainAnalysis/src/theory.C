#include "../include/lambdas.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TColor.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TLine.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

static int const data = TColor::GetColor("#000000");
static int const colors[6] = {TColor::GetColor("#5790fc"), TColor::GetColor("#f89c20"),  TColor::GetColor("#e42536"), TColor::GetColor("#964a8b"), TColor::GetColor("#9c9ca1"), TColor::GetColor("#7a21dd"),};

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

int theory(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto data_input = conf->get<std::string>("data_input");
    auto figure = conf->get<std::string>("figure");

    auto theory_inputs = conf->get<std::vector<std::string>>("theory_inputs");
    auto theory_figures = conf->get<std::vector<std::string>>("theory_figures");
    auto theory_legends = conf->get<std::vector<std::string>>("theory_legends");

    auto tag = conf->get<std::string>("tag");
    auto prefix = conf->get<std::string>("prefix");

    auto xmin = conf->get<float>("xmin");
    auto xmax = conf->get<float>("xmax");
    auto ymin = conf->get<float>("ymin");
    auto ymax = conf->get<float>("ymax");

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open data file */
    TFile* file_data = new TFile(data_input.data(), "read");

    /* load histograms */
    std::string base_stub = tag + "_base_"s + tag + "_nominal_s_pure_raw_sub_"s;
    std::string syst_stub = tag + "_total_base_"s + tag + "_nominal_s_pure_raw_sub_"s;

    auto hist = new history<TH1F>(file_data, base_stub + figure);
    title(std::bind(rename_axis, _1, "1/N^{#gammaj}dN/d#deltaj"), hist);
    auto syst = new history<TH1F>(file_data, syst_stub + figure);

    file_data->Close();

    /* link histograms, uncertainties */
    std::unordered_map<TH1*, TH1*> links;
    hist->apply([&](TH1* h, int64_t index) { links[h] = (*syst)[index]; });

    /* get theory predictions */ 
    std::vector<TFile*> theory_files(10);
    std::vector<TH1*> theory_hists(10);

    for (size_t i = 0; i < theory_inputs.size(); ++i) {
        theory_files[i] = new TFile(theory_inputs[i].data(), "read");
        theory_hists[i] = (TH1*) theory_files[i]->Get(theory_figures[i].data());
    }

    /* uncertainty box */
    auto box = [&](TH1* h, int64_t) {
        if (links.count(h)) {
            TGraph* gr = new TGraph();
            gr->SetFillStyle(1001);
            gr->SetFillColorAlpha(data, 0.48);

            for (int i = 1; i <= h->GetNbinsX(); ++i) {
                if (h->GetBinError(i) == 0) continue;

                double x = h->GetBinCenter(i);
                double width = h->GetBinWidth(i);
                double val = h->GetBinContent(i);
                double err = links[h]->GetBinContent(i);

                gr->SetPoint(0, x - (width / 2), val - err);
                gr->SetPoint(1, x + (width / 2), val - err);
                gr->SetPoint(2, x + (width / 2), val + err);
                gr->SetPoint(3, x - (width / 2), val + err);

                gr->DrawClone("f");
            }
        }
    };

    /* prepare plots */
    auto hb = new pencil();
    hb->category("system", "pp", "aa", "jewel", "pyquen", "pyquen_wide", "pythia");
    hb->alias("aa", "PbPb 0-10%");
    hb->alias("pp", "pp");
    if (tag == "aa") hb->alias("jewel", "Jewel PbPb 0-10%");
    if (tag == "pp") hb->alias("jewel", "Jewel pp");
    if (tag == "aa") hb->alias("pyquen_wide", "Pyquen Wide PbPb 0-10%");
    if (tag == "pp") hb->alias("pyquen", "Pyquen pp");
    if (tag == "aa") hb->alias("pyquen", "Pyquen PbPb 0-10%");
    if (tag == "pp") hb->alias("pythia", "PYTHIA pp");
    

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.65, "40 < p_{T}^{#gamma} < 200, |#eta^{#gamma}| < 1.44");
            l->DrawLatexNDC(0.865, 0.60, "anti-k_{T} R = 0.3, 30 < p_{T}^{jet} < 120, |#eta^{jet}| < 1.6");
        }
    };

    /* prepare papers */
    auto p = new paper(prefix + "_" + tag + "_theory_comparison", hb);
    if (tag == "pp") apply_style(p, "#bf{#scale[1.4]{CMS}}     #sqrt{s} = 5.02 TeV"s, "pp 302 pb^{-1}"s, ymin, ymax);
    if (tag == "aa") apply_style(p, "#bf{#scale[1.4]{CMS}}     #sqrt{s_{NN}} = 5.02 TeV"s, "PbPb 1.6 nb^{-1}"s, ymin, ymax);
    p->accessory(std::bind(line_at, _1, 0.f, xmin, xmax));
    p->accessory(kinematics);
    p->jewellery(box);
    p->divide(-1, 1);

    /* draw histograms with uncertainties */
    if (tag == "aa") p->add((*hist)[3], "aa");
    if (tag == "pp") p->add((*hist)[0], "pp");

    for (size_t i = 0; i < theory_inputs.size(); ++i) {
        p->stack(theory_hists[i], theory_legends[i]);
        p->adjust(theory_hists[i], "3", "f");
    }

    auto data_style = [](TH1* h) {
        h->SetLineColor(1);
        h->SetMarkerColor(data);
        h->SetMarkerStyle(20);
        h->SetMarkerSize(0.60);
    };

    auto jewel_style = [](TH1* h) {
        h->SetMarkerColor(colors[0]);
        h->SetLineColor(colors[0]);
        h->SetFillColorAlpha(colors[0], 0.25);
        h->SetMarkerStyle(20);
        h->SetMarkerSize(0.60);
    };

    auto pyquen_style = [](TH1* h) {
        h->SetMarkerColor(colors[1]);
        h->SetLineColor(colors[1]);
        h->SetFillColorAlpha(colors[1], 0.25);
        h->SetMarkerStyle(47);
        h->SetMarkerSize(0.60);
    };

    auto pyquen_wide_style = [](TH1* h) {
        h->SetMarkerColor(colors[2]);
        h->SetLineColor(colors[2]);
        h->SetFillColorAlpha(colors[2], 0.25);
        h->SetMarkerStyle(47);
        h->SetMarkerSize(0.60);
    };

    auto pythia_style = [](TH1* h) {
        h->SetMarkerColor(colors[3]);
        h->SetLineColor(colors[3]);
        h->SetFillColorAlpha(colors[3], 0.25);
        h->SetMarkerStyle(47);
        h->SetMarkerSize(0.60);
    };

    hb->style("pp", data_style);
    hb->style("aa", data_style);
    hb->style("jewel", jewel_style);
    hb->style("pyquen", pyquen_style);
    hb->style("pyquen_wide", pyquen_wide_style);
    hb->style("pythia", pythia_style);
    hb->sketch();

    p->draw("pdf");

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return theory(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
