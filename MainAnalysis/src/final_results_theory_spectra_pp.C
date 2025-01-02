#include "../include/lambdas.h"
#include "../include/specifics.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TColor.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLatex.h"
#include "TPaveText.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TPad.h"
#include "TGaxis.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TBox.h"
#include "TGraphAsymmErrors.h"

#include <string>
#include <vector>
#include <cmath>

using namespace std::literals::string_literals;
using namespace std::placeholders;

void set_systematics(history<TH1F>* h, history<TH1F>* s)
{
    for (int i = 0; i < h->size(); ++i) {
        for (int j = 1; j <= (*h)[i]->GetNbinsX(); ++j) {
            double val = (*h)[i]->GetBinContent(j);
            double err = (*s)[i]->GetBinContent(j);

            (*s)[i]->SetBinContent(j, val);
            (*s)[i]->SetBinError(j, err);
        }
    }
}

std::vector<TGraphAsymmErrors> get_graph(std::vector<history<TH1F>*> h, int format, int system, double ncols)
{
    std::vector<TGraphAsymmErrors> result(h.size());

    static int style[3] = {20, 20, 20};
    static int color[3] = {TColor::GetColor("#5790FC"), TColor::GetColor("#E42536"), TColor::GetColor("#9C9C9C")};

    for (size_t i = 0; i < h.size(); ++i) {
        for (int j = 1; j <= (*h[i])[0]->GetNbinsX(); ++j) {
            double x = (*h[i])[0]->GetBinCenter(j);
            double dx = (*h[i])[0]->GetBinWidth(j)/2;

            result[i].SetPoint(j - 1, x, (*h[i])[0]->GetBinContent(j));
            result[i].SetPointError(j - 1, dx, dx, (*h[i])[0]->GetBinError(j), (*h[i])[0]->GetBinError(j));
        }

        result[i].SetMarkerStyle(style[format]);
        result[i].SetMarkerColor(1);
        result[i].SetLineColor(1);
        result[i].SetFillColorAlpha(color[format], 0.60);
        result[i].SetMarkerSize(1.5);
        result[i].SetLineWidth(10.0 / ncols);
    }

    return result;
}

std::vector<TGraphAsymmErrors> get_graph(std::vector<history<TH1F>*> h, int type, double ncols)
{
    std::vector<TGraphAsymmErrors> result(h.size());

    static int style[9] = {20, 20, 20, 20, 20, 20, 20, 20, 20};
    static int color[9] = {TColor::GetColor("#4492D8"), 
                           TColor::GetColor("#95DADD"), 
                           TColor::GetColor("#E5631D"),
                           TColor::GetColor("#BB2112"),
                           TColor::GetColor("#22872C"),
                           TColor::GetColor("#FDA82D"),
                           TColor::GetColor("#B9AB74"),
                           TColor::GetColor("#8235B4"),
                           TColor::GetColor("#A86B5B")
                           };

    for (size_t i = 0; i < h.size(); ++i) {
        for (int j = 1; j <= (*h[i])[0]->GetNbinsX(); ++j) {
            double x = (*h[i])[0]->GetBinCenter(j);
            double dx = (*h[i])[0]->GetBinWidth(j)/2;

            result[i].SetPoint(j - 1, x, (*h[i])[0]->GetBinContent(j));
            result[i].SetPointError(j - 1, dx, dx, (*h[i])[0]->GetBinError(j), (*h[i])[0]->GetBinError(j));
        }

        result[i].SetMarkerStyle(style[type]);
        result[i].SetMarkerColor(color[type]);
        result[i].SetLineColor(color[type]);
        result[i].SetFillColorAlpha(color[type], 0.35);
        result[i].SetMarkerSize(0);
        result[i].SetLineWidth(10.0 / ncols);
    }

    return result;
}

void set_pad(TPad &pad)
{
    pad.SetLeftMargin(0);
    pad.SetTopMargin(0);
    pad.SetRightMargin(0);
    pad.SetBottomMargin(0);
    pad.SetTickx();
    pad.SetTicky();
    pad.SetLogx();

    pad.Draw();
}

void set_axis(TGaxis &axis, double sf)
{
    axis.SetLabelFont(42);
    axis.SetLabelSize(0.06/sf);
    axis.SetMaxDigits(6);
    axis.SetNoExponent();
    axis.SetTickLength(0.0);
    axis.Draw();
}

int congratulate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input_pp = conf->get<std::string>("input_pp");

    auto input_pp_jewel = conf->get<std::string>("input_pp_jewel");
    auto input_pp_pyquen = conf->get<std::string>("input_pp_pyquen");
    auto input_pp_pythia = conf->get<std::string>("input_pp_pythia");
    auto input_pp_hybrid = conf->get<std::string>("input_pp_hybrid");

    auto tag_pp_jewel = conf->get<std::string>("tag_pp_jewel");
    auto tag_pp_pyquen = conf->get<std::string>("tag_pp_pyquen");
    auto tag_pp_pythia = conf->get<std::string>("tag_pp_pythia");
    auto tag_pp_hybrid = conf->get<std::string>("tag_pp_hybrid");

    auto figures = conf->get<std::vector<std::string>>("figures");
    auto theory_figures = conf->get<std::vector<std::string>>("theory_figures");

    auto name = conf->get<std::string>("name");
    auto types = conf->get<std::vector<int64_t>>("types");

    auto supplementary = conf->get<bool>("supplementary");

    auto ymins = conf->get<std::vector<float>>("ymins");
    auto ymaxs = conf->get<std::vector<float>>("ymaxs");

    auto subsets = conf->get<bool>("subsets");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    auto bpho_pt = sel->get<std::vector<float>>("photon_pt_bounds");
    auto bdr = sel->get<std::vector<float>>("dr_bounds");
    auto ptg_range = sel->get<std::vector<float>>("ptg_range");

    auto osg = sel->get<std::vector<int64_t>>("osg");
    auto osg_part1 = sel->get<std::vector<int64_t>>("osg_part1");
    auto osg_part2 = sel->get<std::vector<int64_t>>("osg_part2");

    const int ncols = figures.size();

    std::vector<std::vector<float>> bjet_pt(ncols, std::vector<float>(2, 0));

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open input files */
    auto file_pp = new TFile((base + input_pp).data(), "read");

    auto file_pp_jewel = new TFile((base + input_pp_jewel).data(), "read");
    auto file_pp_pyquen = new TFile((base + input_pp_pyquen).data(), "read");
    auto file_pp_pythia = new TFile((base + input_pp_pythia).data(), "read");
    auto file_pp_hybrid = new TFile((base + input_pp_hybrid).data(), "read");

    /* define kinematics and luminosity */
    auto text_system = "(5.02 TeV)"s;
    auto text_cms = (supplementary) ? "CMS #scale[0.8]{#font[52]{Supplementary}}"s : "CMS"s;
    auto text_photon_pt = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV"s;
    auto text_photon_eta = "|#eta^{#gamma}| < "s + to_text(photon_eta_abs);
    auto text_dphi = "#Delta#phi_{j#gamma} > #frac{"s + to_text(dphi_min_numerator) + "#pi}{"s + to_text(dphi_min_denominator) + "}"s;
    auto text_jet_alg = "anti-k_{T} R = 0.3"s;
    auto text_jet_eta = "|#eta^{jet}| < "s + to_text(jet_eta_abs);

    if (subsets) text_system = "pp 302 pb^{-1} " + text_system;

    std::vector<history<TH1F>*> hists_pp(ncols);
    std::vector<history<TH1F>*> systs_pp(ncols);

    std::vector<history<TH1F>*> hists_pp_jewel(ncols);
    std::vector<history<TH1F>*> hists_pp_pyquen(ncols);
    std::vector<history<TH1F>*> hists_pp_pythia(ncols);
    std::vector<history<TH1F>*> hists_pp_hybrid(ncols);

    for (int i = 0; i < ncols; ++i) {
        /* define jet pT bounds */
        switch (types[i]) {
        case 1:
            bjet_pt[i][0] = ptg_range[osg_part1[2]];
            bjet_pt[i][1] = ptg_range[ptg_range.size() - 1 - osg_part1[3]];
            break;
        case 2:
            bjet_pt[i][0] = ptg_range[osg_part2[2]];
            bjet_pt[i][1] = ptg_range[ptg_range.size() - 1 - osg_part2[3]];
            break;
        default:
            bjet_pt[i][0] = ptg_range[osg[2]];
            bjet_pt[i][1] = ptg_range[ptg_range.size() - 1 - osg[3]];
        }

        if (theory_figures[i] == "none") theory_figures[i] = "";

        /* get histograms */
        hists_pp[i] = new history<TH1F>(file_pp, "pp_base_pp_nominal_s_pure_raw_sub_" + figures[i]);
        systs_pp[i] = new history<TH1F>(file_pp, "pp_total_base_pp_nominal_s_pure_raw_sub_" + figures[i]);

        hists_pp_jewel[i] = new history<TH1F>(file_pp_jewel, tag_pp_jewel + "_dr" + theory_figures[i]);
        hists_pp_pyquen[i] = new history<TH1F>(file_pp_pyquen, tag_pp_pyquen + "_dr" + theory_figures[i]);
        hists_pp_pythia[i] = new history<TH1F>(file_pp_pythia, tag_pp_pythia + theory_figures[i]);
        hists_pp_hybrid[i] = new history<TH1F>(file_pp_hybrid, tag_pp_hybrid + "_dr" + theory_figures[i]);

        set_systematics(hists_pp[i], systs_pp[i]);
    }

    auto graphs_hists_pp = get_graph(hists_pp, 2, 1, ncols);
    auto graphs_systs_pp = get_graph(systs_pp, 2, 1, ncols);

    auto graphs_hists_pp_jewel = get_graph(hists_pp_jewel, 0, ncols);
    auto graphs_hists_pp_pyquen = get_graph(hists_pp_pyquen, 2, ncols);
    auto graphs_hists_pp_pythia = get_graph(hists_pp_pythia, 4, ncols);
    auto graphs_hists_pp_hybrid = get_graph(hists_pp_hybrid, 5, ncols);

    /* size canvas */
    double panel_size = 500;
    double padding_width_left = 170;
    double padding_width_right = 50;
    double padding_height = 70;

    double canvas_width = panel_size * ncols + padding_width_left + padding_width_right;
    double canvas_height = panel_size * 1 + padding_height * 2;

    double sf = 1;

    double pad_x0 = padding_width_left / canvas_width;
    double pad_y0 = padding_height / canvas_height;
    double pad_dx = panel_size / canvas_width;
    double pad_dy = panel_size / canvas_height;

    double xmin = bdr[0] + 0.003;
    double xmax = bdr[1];

    gStyle->SetLineScalePS(1);

    /* declare canvas, pads, axes, and titles */
    TCanvas canvas("canvas", "", canvas_width, canvas_height);

    TLatex latex;
    latex.SetNDC();

    std::vector<TH2F*> worlds(ncols);
    std::vector<TArrow*> arrows(ncols);
    std::vector<TPad*> pads(ncols);
    std::vector<TGaxis*> axis_x(ncols);
    std::vector<TGaxis*> axis_y(1);
    std::vector<TPaveText*> boxes(ncols);

    for (int i = 0; i < ncols; ++i) {
        worlds[i] = new TH2F("world", ";;", 100, xmin, xmax, 100, ymins[i], ymaxs[i]);
        worlds[i]->SetStats(0);

        pads[i] = new TPad("P1", "", pad_x0 + pad_dx * i, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * (i + 1), pad_y0 + pad_dy * 1, 0);
        
        set_pad(*pads[i]);

        axis_x[i] = new TGaxis(pad_x0 + pad_dx * i, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * (i + 1), pad_y0 + pad_dy * 0, xmin, xmax, 510, "SG");

        set_axis(*axis_x[i], sf);

        double arrow_y = graphs_systs_pp[i].GetPointY(0);

        arrows[i] = new TArrow(0.003, arrow_y, 0.0040, arrow_y, 0.02 / ncols, "<|");
        arrows[i]->SetAngle(40);
        arrows[i]->SetLineWidth(1);

        canvas.cd();

        latex.SetTextFont(42);
        latex.SetTextSize(0.07/sf);
        latex.SetTextAlign(22);
        latex.SetTextAngle(0);
        latex.DrawLatex(pad_x0 + pad_dx * (i + 0.5), pad_y0 * 0.5, "#Deltaj");
    }

    axis_y[0] = new TGaxis(pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 1, ymins[0] * 0.999, ymaxs[0] * 0.999, 510, "S");

    set_axis(*axis_y[0], sf);

    latex.SetTextFont(42);
    latex.SetTextSize(0.07/sf);
    latex.SetTextAlign(22);
    latex.SetTextAngle(90);
    latex.DrawLatex(pad_x0 * 0.4, pad_y0 + pad_dy * 0.5, "#frac{1}{N_{#gamma}} #frac{dN_{j#gamma}}{d#Deltaj}");

    latex.SetTextFont(62);
    latex.SetTextSize(0.07/sf);
    latex.SetTextAlign(11);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0, pad_y0 * 1.15 + pad_dy, text_cms.c_str());

    latex.SetTextFont(42);
    latex.SetTextSize(0.055/sf);
    latex.SetTextAlign(31);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0 + pad_dx * ncols, pad_y0 * 1.15 + pad_dy, text_system.c_str());

    TGraph line;
    line.SetPoint(0, xmin, 0);
    line.SetPoint(1, xmax, 0);
    line.SetLineStyle(kDashed);

    /* declare legend */
    double legend_y_min = 0;
    double legend_y_max = 0;
    double legend_x_min = 0;
    double legend_x_max = 0;

    double legend_y_min = (subsets) ? 0.45 : 0.05;
    double legend_y_max = (subsets) ? 0.80 : 0.26;
    double legend_x_min = (subsets) ? 0.65 : 0.05;
    double legend_x_max = (subsets) ? 0.95 : 0.35;

    TLegend legend_part1(legend_x_min, legend_y_min, legend_x_max, legend_y_max);
    legend_part1.SetTextFont(42);
    legend_part1.SetTextSize(0.055);
    legend_part1.SetFillStyle(0);
    legend_part1.SetBorderSize(0);
    legend_part1.AddEntry(&graphs_systs_pp[0], "CMS pp", "plf");
    legend_part1.AddEntry(&graphs_hists_pp_jewel[0], "JEWEL", "lf");
    legend_part1.AddEntry(&graphs_hists_pp_pyquen[0], "PYQUEN", "lf");
    if (subsets)    legend_part1.AddEntry(&graphs_hists_pp_pythia[0], "PYTHIA", "lf");
    if (subsets)    legend_part1.AddEntry(&graphs_hists_pp_hybrid[0], "HYBRID", "lf");

    if (!subsets)    legend_y_min = legend_y_min;
    if (!subsets)    legend_y_max = legend_y_min + 0.14;
    if (!subsets)    legend_x_min = legend_x_max;
    if (!subsets)    legend_x_max = legend_x_min + 0.30;

    TLegend legend_part2(legend_x_min, legend_y_min, legend_x_max, legend_y_max);
    legend_part2.SetTextFont(42);
    legend_part2.SetTextSize(0.055);
    legend_part2.SetFillStyle(0);
    legend_part2.SetBorderSize(0);
    if (!subsets)   legend_part2.AddEntry(&graphs_hists_pp_pythia[0], "PYTHIA", "lf");
    if (!subsets)   legend_part2.AddEntry(&graphs_hists_pp_hybrid[0], "HYBRID", "lf");

    for (int i = 0; i < ncols; i++) {
        pads[i]->cd();

        worlds[i]->Draw("axis");

        graphs_systs_pp[i].Draw("same 2");
        graphs_hists_pp_jewel[i].Draw("same 3");
        graphs_hists_pp_jewel[i].Draw("same lX");
        graphs_hists_pp_pythia[i].Draw("same 3");
        graphs_hists_pp_pythia[i].Draw("same lX");
        graphs_hists_pp_pyquen[i].Draw("same 3");
        graphs_hists_pp_pyquen[i].Draw("same lX");
        graphs_hists_pp_hybrid[i].Draw("same 3");
        graphs_hists_pp_hybrid[i].Draw("same lX");
        graphs_hists_pp[i].Draw("same PZ");

        line.Draw("l");
        arrows[i]->Draw();

        auto text_jet_pt = to_text(bjet_pt[i][0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[i][1]) + " GeV"s;
        boxes[i] = new TPaveText(0.22, 1 - 0.16, 0.78, 1 - 0.05, "NDC");
        boxes[i]->SetBorderSize(1);
        boxes[i]->SetTextFont(42);
        boxes[i]->SetTextSize(0.06);
        boxes[i]->SetLineWidth(3);
        boxes[i]->SetFillColor(0);
        boxes[i]->SetShadowColor(0);
        boxes[i]->AddText((text_jet_pt).c_str());
    
        pads[i]->cd();
        boxes[i]->Draw("same");
    }

    pads[0]->cd();
    latex.SetTextSize(0.055);
    latex.SetTextAlign(31);
    latex.DrawLatex(0.95, 0.73, (text_photon_pt).c_str());
    latex.DrawLatex(0.95, 0.65, (text_photon_eta).c_str());
    latex.DrawLatex(0.95, 0.57, (text_dphi + ", " + text_jet_eta).c_str());
    latex.DrawLatex(0.95, 0.49, (text_jet_alg).c_str());

    if (!subsets)    latex.SetTextAlign(31);
    if (!subsets)    latex.DrawLatex(0.97, 0.36, "pp 302 pb^{-1}");

    pads[ncols-1]->cd();
    legend_part1.Draw();
    legend_part2.Draw();

    canvas.SaveAs((set + "_final_theory_spectra_pp_" + name + "_log.pdf").c_str());

    canvas.SaveAs((set + "_final_theory_spectra_pp_" + name + "_log.C").c_str());

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return congratulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}