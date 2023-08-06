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

static auto const data = TColor::GetColor("#5c5c5c");
static int const colors[5] = {
    TColor::GetColor("#5790fc"), 
    TColor::GetColor("#f89c20"), 
    TColor::GetColor("#964a8b"), 
    TColor::GetColor("#9c9ca1"), 
    TColor::GetColor("#7a21dd"),
    };

static int const markers[4] = { 22, 48, 47, 45 };

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

int theory(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto data_input = conf->get<std::string>("data_input");
    auto figure = conf->get<std::string>("figure");
    auto type = conf->get<std::string>("type");

    auto theory_inputs = conf->get<std::vector<std::string>>("theory_inputs");
    auto theory_figures = conf->get<std::vector<std::string>>("theory_figures");
    auto theory_tags = conf->get<std::vector<std::string>>("theory_tags");
    auto theory_legends = conf->get<std::vector<std::string>>("theory_legends");

    auto theory_legend_styles = conf->get<std::vector<std::string>>("theory_legend_styles");
    auto theory_plot_styles = conf->get<std::vector<std::string>>("theory_plot_styles");

    auto tag = conf->get<std::string>("tag");
    auto prefix = conf->get<std::string>("prefix");

    auto xmin = conf->get<float>("xmin");
    auto xmax = conf->get<float>("xmax");
    auto ymin = conf->get<float>("ymin");
    auto ymax = conf->get<float>("ymax");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto heavyion = sel->get<bool>("heavyion");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    auto bpho_pt = sel->get<std::vector<float>>("photon_pt_bounds");
    auto bdr = sel->get<std::vector<float>>("dr_bounds");
    auto bjet_pt = sel->get<std::vector<float>>("jet_pt_bounds");

    std::vector<int32_t> dcent = {10, 0};

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();
std::cout << __LINE__ << std::endl;
    /* open data file */
    TFile* file_data = new TFile((base + data_input).data(), "read");
std::cout << __LINE__ << std::endl;
    /* load histograms */
    std::string base_stub = tag + "_base_"s + tag + "_nominal_s_pure_raw_sub_"s;
    std::string syst_stub = tag + "_total_base_"s + tag + "_nominal_s_pure_raw_sub_"s;
std::cout << __LINE__ << std::endl;
    auto hist = new history<TH1F>(file_data, base_stub + figure);
    auto syst = new history<TH1F>(file_data, syst_stub + figure);
std::cout << __LINE__ << std::endl;
    title(std::bind(rename_axis, _1, "1/N^{#gammaj}dN/d#deltaj"), hist);
std::cout << __LINE__ << std::endl;
    file_data->Close();
std::cout << __LINE__ << std::endl;
    /* link histograms, uncertainties */
    std::unordered_map<TH1*, TH1*> links;
    hist->apply([&](TH1* h, int64_t index) { links[h] = (*syst)[index]; });
std::cout << __LINE__ << std::endl;
    /* get theory predictions */ 
    std::vector<TFile*> theory_files(theory_inputs.size());
    std::vector<history<TH1F>*> theory_hists(theory_inputs.size());
std::cout << __LINE__ << std::endl;
    for (size_t i = 0; i < theory_inputs.size(); ++i) {
        theory_files[i] = new TFile((base + theory_inputs[i]).data(), "read");
        theory_hists[i] = new history<TH1F>(theory_files[i], theory_figures[i]);
    }
std::cout << __LINE__ << std::endl;
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
std::cout << __LINE__ << std::endl;
    /* prepare plots */
    auto hb = new pencil();
    hb->category("type", "total", theory_tags);
std::cout << __LINE__ << std::endl;
    zip([&](auto const& tag, auto const& legend) {
        hb->alias(tag, legend); }, theory_tags, theory_legends);
std::cout << __LINE__ << std::endl;
    hb->alias("aa", "PbPb");
    hb->alias("pp", "pp");
    std::cout << __LINE__ << std::endl;
    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };
std::cout << __LINE__ << std::endl;
    auto aa_hf_info = [&](int64_t index, history<TH1F>* h) {
        stack_text(index, 0.84, 0.04, h, hf_info); };
std::cout << __LINE__ << std::endl;
    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto photon_selections = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs)  + 
                ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
            auto jet_selections = "anti-k_{T} R = 0.3, " + to_text(bjet_pt[0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.70, photon_selections.data());
            l->DrawLatexNDC(0.865, 0.64, jet_selections.data());
        }
    };
std::cout << __LINE__ << std::endl;
    auto luminosity = [&](int64_t index) {
        if (index > 0) {
            auto values = heavyion ? "PbPb 1.69 nb^{-1}"s : "pp 302 pb^{-1}"s;

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.55, values.data());
        }
    };
std::cout << __LINE__ << std::endl;
    /* prepare papers */
    auto p = new paper(set + "_theory_comparison_" + type + "_" + tag, hb);
    apply_style(p, "#bf{#scale[1.4]{CMS}}"s, "#sqrt{s} = 5.02 TeV"s, ymin, ymax);
    p->accessory(std::bind(line_at, _1, 0.f, xmin, xmax));
    p->accessory(kinematics);
    p->accessory(luminosity);
    p->jewellery(box);
    if (heavyion) p->accessory(std::bind(aa_hf_info, _1, hist)); 
    p->divide(-1, 1);
std::cout << __LINE__ << std::endl;
    /* draw histograms with uncertainties */
    if (tag == "aa") {
        p->add((*hist)[3], "aa");
        p->adjust((*hist)[3], "pe", "plf");
    }
    if (tag == "pp") { 
        p->add((*hist)[0], "pp");
        p->adjust((*hist)[0], "pe", "plf");
    }
std::cout << __LINE__ << std::endl;
    for (size_t i = 0; i < theory_inputs.size(); ++i) {
        p->stack((*theory_hists[i])[0], theory_tags[i]);
    }
std::cout << __LINE__ << std::endl;
    auto data_style = [&](TH1* h) {
        h->SetLineColor(1);
        h->SetFillColorAlpha(data, 0.5);
        h->SetMarkerStyle(20);
        h->SetMarkerSize(0.60);

        p->adjust(h, "le", "plf");
    };
std::cout << __LINE__ << std::endl;
    auto theory_style = [&](TH1* h, int i) {
        h->SetMarkerColor(colors[i]);
        h->SetLineColor(colors[i]);
        h->SetFillColorAlpha(colors[i], 0.5);
        h->SetMarkerStyle(markers[i]);
        h->SetMarkerSize(0.60);

        p->adjust(h, theory_plot_styles[i], theory_legend_styles[i]);
    };
std::cout << __LINE__ << std::endl;
    hb->style("pp", data_style);
    hb->style("aa", data_style);
std::cout << __LINE__ << std::endl;
    for (size_t i = 0; i < theory_inputs.size(); ++i) {
        hb->style(theory_tags[i], std::bind(theory_style, _1, i));
    }
std::cout << __LINE__ << std::endl;
    hb->sketch();
std::cout << __LINE__ << std::endl;
    p->draw("pdf");

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return theory(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
