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
static int const colors[8] = {
    TColor::GetColor("#003f5c"), 
    TColor::GetColor("#58508d"), 
    TColor::GetColor("#bc5090"), 
    TColor::GetColor("#ff6361"),
    TColor::GetColor("#ffc425"),
    TColor::GetColor("#f37735"),
    TColor::GetColor("#00aedb"),
    TColor::GetColor("#00b159"),
    };

static int const markers[8] = { 49, 48, 47, 45, 6, 6, 6, 6 };

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

    auto offset = conf->get<int32_t>("offset");

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

    /* open data file */
    TFile* file_data = new TFile((base + data_input).data(), "read");

    /* load histograms */
    std::string base_stub = tag + "_base_"s + tag + "_nominal_s_pure_raw_sub_"s;
    std::string syst_stub = tag + "_total_base_"s + tag + "_nominal_s_pure_raw_sub_"s;

    auto hist = new history<TH1F>(file_data, base_stub + figure);
    auto syst = new history<TH1F>(file_data, syst_stub + figure);

    title(std::bind(rename_axis, _1, "1/N^{#gamma}dN^{#gammaj}/d#deltaj"), hist);

    file_data->Close();

    /* link histograms, uncertainties */
    std::unordered_map<TH1*, TH1*> links;
    hist->apply([&](TH1* h, int64_t index) { links[h] = (*syst)[index]; });

    /* get theory predictions */ 
    std::vector<TFile*> theory_files(theory_inputs.size());
    std::vector<history<TH1F>*> theory_hists(theory_inputs.size());

    for (size_t i = 0; i < theory_inputs.size(); ++i) {
        theory_files[i] = new TFile((base + theory_inputs[i]).data(), "read");
        theory_hists[i] = new history<TH1F>(theory_files[i], theory_figures[i]);
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
    hb->category("type", "aa", "pp", theory_tags);

    zip([&](auto const& tag, auto const& legend) {
        hb->alias(tag, legend); }, theory_tags, theory_legends);

    hb->alias("aa", "PbPb");
    hb->alias("pp", "pp");
    
    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto aa_hf_info = [&](int64_t index, history<TH1F>* h) {
        stack_text(index, 0.73, 0.04, h, hf_info); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto photon_selections = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs)  + 
                ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
            auto jet_selections = "anti-k_{T} R = 0.3, " + to_text(bjet_pt[0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.86, 0.63, photon_selections.data());
            l->DrawLatexNDC(0.86, 0.58, jet_selections.data());
        }
    };

    auto blurb = [&](int64_t index) {
        if (index > 0) {
            auto system_tag = heavyion ? "PbPb 1.69 nb^{-1}"s : "pp 302 pb^{-1}"s;
            auto cms = "#bf{#scale[1.4]{CMS}} #sqrt{s_{NN}} = 5.02 TeV"s;

            TLatex* l = new TLatex();
            l->SetTextAlign(11);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.14, 0.83, cms.data());
            l->DrawLatexNDC(0.14, 0.77, system_tag.data());
        }
    };

    /* prepare papers */
    auto p = new paper(set + "_theory_comparison_" + type + "_" + tag, hb);
    apply_style(p, ""s, ""s, ymin, ymax);
    p->accessory(std::bind(line_at, _1, 0.f, xmin, xmax));
    p->accessory(kinematics);
    p->accessory(blurb);
    p->jewellery(box);
    if (heavyion) p->accessory(std::bind(aa_hf_info, _1, hist)); 
    p->divide(-1, 1);

    /* draw histograms with uncertainties */
    if (tag == "aa") {
        p->add((*hist)[3], "aa");
        p->adjust((*hist)[3], "pe", "plf");
    }
    if (tag == "pp") { 
        p->add((*hist)[0], "pp");
        p->adjust((*hist)[0], "pe", "plf");
    }

    for (size_t i = 0; i < theory_inputs.size(); ++i) {
        p->stack((*theory_hists[i])[0], theory_tags[i]);
    }

    auto data_style = [&](TH1* h) {
        h->SetLineColor(1);
        h->SetFillColorAlpha(data, 0.5);
        h->SetMarkerStyle(20);
        h->SetMarkerSize(0.60);

        p->adjust(h, "le", "plf");
    };

    auto theory_style = [&](TH1* h, int i) {
        h->SetMarkerColor(colors[i + offset]);
        h->SetLineColor(colors[i + offset]);
        h->SetFillColorAlpha(colors[i + offset], 0.5);
        h->SetMarkerStyle(markers[i + offset]);
        h->SetMarkerSize(0.60);

        p->adjust(h, theory_plot_styles[i], theory_legend_styles[i]);
    };

    hb->style("pp", data_style);
    hb->style("aa", data_style);

    for (size_t i = 0; i < theory_inputs.size(); ++i) {
        hb->style(theory_tags[i], std::bind(theory_style, _1, i));
    }

    hb->sketch();

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
