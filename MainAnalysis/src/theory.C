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

static auto const red = TColor::GetColor("#f2777a");
static auto const blue = TColor::GetColor("#6699cc");

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

int congratulate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto data_input = conf->get<std::string>("data_input");
    auto figure = conf->get<std::string>("figure");

    auto theory_input = conf->get<std::string>("theory_input");
    auto jewel_figure = conf->get<std::string>("jewel_figure");
    auto pyquen_figure = conf->get<std::string>("pyquen_figure");

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
    TFile* file_theory = new TFile(theory_input.data(), "read");
    auto jewel = (TH1D*) file_theory->Get(jewel_figure.data());
    auto pyquen = (TH1D*) file_theory->Get(pyquen_figure.data());

    file_theory->Close();
    float xbins[11] = { 0, 0.015, 0.03, 0.045, 0.06, 0.08, 0.1, 0.12, 0.15, 0.2, 0.3 };
    auto jewel_test = new TH1F("jewel_test", "", 10, xbins);
    auto pyquen_test = new TH1F("pyquen_test", "", 10, xbins);

    /* uncertainty box */
    auto box = [&](TH1* h, int64_t) {
        TGraph* gr = new TGraph();
        gr->SetFillStyle(1001);
        gr->SetFillColorAlpha(tag == "aa" ? red : blue, 0.48);

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
    };

    /* prepare plots */
    auto hb = new pencil();
    hb->category("system", "pp", "aa", "jewel", "pyquen");
    hb->alias("aa", "PbPb 0-10%");
    hb->alias("pp", "pp");
    if (tag == "aa") hb->alias("jewel", "Jewel PbPb 0-10%");
    if (tag == "pp") hb->alias("jewel", "Jewel pp");
    if (tag == "aa") hb->alias("pyquen", "Pyquen PbPb 0-10%");
    if (tag == "pp") hb->alias("pyquen", "Pyquen pp");
    

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.70, "40 < p_{T}^{#gamma} < 200, |#eta^{#gamma}| < 1.44");
            l->DrawLatexNDC(0.865, 0.65, "anti-k_{T} R = 0.3, 30 < p_{T}^{jet} < 120, |#eta^{jet}| < 1.6");
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
    p->stack(jewel_test, "jewel");
    p->stack(pyquen_test, "pyquen");std::cout << __LINE__ << std::endl;

    for (int i = 1; i <= jewel->GetNbinsX(); ++i) {
        std::cout << "jewel: " << jewel->GetBinContent(i) << std::endl;
        std::cout << "pyquen: " << pyquen->GetBinContent(i) << std::endl;
        std::cout << "data: " << (*hist)[0]->GetBinContent(i) << std::endl;
    }

    auto pp_style = [](TH1* h) {
        h->SetLineColor(1);
        h->SetMarkerStyle(25);
        h->SetMarkerSize(0.60);
    };

    auto aa_style = [](TH1* h) {
        h->SetLineColor(1);
        h->SetMarkerStyle(20);
        h->SetMarkerSize(0.60);
    };

    // auto jewel_style = [](TH1* h) {
    //     h->SetMarkerColor(51);
    //     h->SetMarkerStyle(39);
    //     h->SetMarkerSize(0.60);
    // };

    // auto pyquen_style = [](TH1* h) {
    //     h->SetMarkerColor(74);
    //     h->SetMarkerStyle(47);
    //     h->SetMarkerSize(0.60);
    // };

    hb->style("pp", pp_style);std::cout << __LINE__ << std::endl;
    hb->style("aa", aa_style);
    // hb->style("jewel", jewel_style);std::cout << __LINE__ << std::endl;
    // hb->style("pyquen", pyquen_style);std::cout << __LINE__ << std::endl;
    hb->sketch();std::cout << __LINE__ << std::endl;

    jewel->Draw("same");
    pyquen->Draw("same");

    p->draw("pdf");std::cout << __LINE__ << std::endl;

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return congratulate(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
