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
#include "TLine.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TPad.h"
#include "TGaxis.h"
#include "TLegend.h"
#include "TBox.h"
#include "TGraphAsymmErrors.h"

#include <string>
#include <vector>
#include <cmath>

using namespace std::literals::string_literals;
using namespace std::placeholders;

std::vector<TGraphAsymmErrors> get_graph(std::vector<history<TH1F>*> h, int type)
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
        result[i].SetFillColorAlpha(color[type], 0.60);
        result[i].SetMarkerSize(0);
        result[i].SetLineWidth(2.0);
    }

    return result;
}

void set_pad(TPad &pad, bool log)
{
    pad.SetLeftMargin(0);
    pad.SetTopMargin(0);
    pad.SetRightMargin(0);
    pad.SetBottomMargin(0);
    pad.SetTickx();
    pad.SetTicky();

    if (log)    pad.SetLogx();

    pad.Draw();
}

void set_axis(TGaxis &axis, double sf)
{
    axis.SetLabelFont(42);
    axis.SetLabelSize(0.040/sf);
    axis.SetMaxDigits(6);
    axis.SetNoExponent();
    axis.SetTickLength(0.0);
    axis.Draw();
}

int congratulate() {
    const int ncols = 1; // change

    std::vector<std::vector<float>> bjet_pt(ncols, std::vector<float>(2, 0));

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open input files */
    auto file = new TFile("data/arc/pho_60_rebin3/gather_hybrid.root", "read");

    /* define kinematics and luminosity */
    auto text_photon_pt = "60 < p_{T}^{#gamma} < 200 GeV"s; // change
    auto text_photon_eta = "|#eta^{#gamma}| < 1.44"s;
    auto text_dphi = "#Delta#phi_{j#gamma} > #frac{2#pi}{3}"s; // change
    auto text_jet_alg = "anti-k_{T} R = 0.3"s;
    auto text_jet_eta = "|#eta^{jet}| < 1.6"s;

    std::vector<history<TH1F>*> hists(ncols);

    for (int i = 0; i < ncols; ++i) {
        /* get histograms */
        hists[i] = new history<TH1F>(file, "pp_selfnorm_elastic_wake_dr"); // change
    }

    auto graphs_hists = get_graph(hists, 0);

    /* size canvas */
    double panel_size = 500;
    double padding_width_left = 140;
    double padding_width_right = 50;
    double padding_height = 70;

    double canvas_width = panel_size * ncols + padding_width_left + padding_width_right;
    double canvas_height = panel_size * 1 + padding_height * 2;

    double sf = 1;

    double pad_x0 = padding_width_left / canvas_width;
    double pad_y0 = padding_height / canvas_height;
    double pad_dx = panel_size / canvas_width;
    double pad_dy = panel_size / canvas_height;

    double xmin = 0;
    double xmax = 0.3 * 0.999;

    gStyle->SetLineScalePS(1);

    /* declare canvas, pads, axes, and titles */
    TCanvas canvas("canvas", "", canvas_width, canvas_height);

    TLatex latex;
    latex.SetNDC();

    std::vector<TH2F*> worlds(ncols);
    std::vector<TPad*> pads(ncols);
    std::vector<TGaxis*> axis_x(ncols);
    std::vector<TGaxis*> axis_y(1);

    double ymin = -3.5; // change
    double ymax = 35; // change

    for (int i = 0; i < ncols; ++i) {
        worlds[i] = new TH2F("world", ";;", 100, xmin, xmax, 100, ymin, ymax);
        worlds[i]->SetStats(0);

        pads[i] = new TPad("P1", "", pad_x0 + pad_dx * i, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * (i + 1), pad_y0 + pad_dy * 1, 0);
        
        set_pad(*pads[i], 0);

        axis_x[i] = new TGaxis(pad_x0 + pad_dx * i, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * (i + 1), pad_y0 + pad_dy * 0, xmin, xmax, 510, "S");

        set_axis(*axis_x[i], sf);

        canvas.cd();

        latex.SetTextFont(42);
        latex.SetTextSize(0.06/sf);
        latex.SetTextAlign(22);
        latex.SetTextAngle(0);
        latex.DrawLatex(pad_x0 + pad_dx * (i + 0.5), pad_y0 * 0.4, "#Deltaj");
    }

    axis_y[0] = new TGaxis(pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 1, ymin * 0.999, ymax * 0.999, 510, "S");

    set_axis(*axis_y[0], sf);

    latex.SetTextFont(42);
    latex.SetTextSize(0.06/sf);
    latex.SetTextAlign(22);
    latex.SetTextAngle(90);
    latex.DrawLatex(pad_x0 * 0.4, pad_y0 + pad_dy * 0.5, "#frac{1}{N_{j#gamma}} #frac{dN_{j#gamma}}{d#Deltaj}");
    latex.SetTextAngle(0);

    TGraph line;
    line.SetPoint(0, xmin, 0);
    line.SetPoint(1, xmax, 0);
    line.SetLineStyle(kDashed);

    /* declare legend */
    double legend_y_min = 0;
    double legend_y_max = 0;
    double legend_x_min = 0;
    double legend_x_max = 0;

    legend_y_min = 0.38;
    legend_y_max = 0.45;
    legend_x_min = 0.65;
    legend_x_max = 0.95;

    TLegend legend(legend_x_min, legend_y_min, legend_x_max, legend_y_max);
    legend.SetTextFont(42);
    legend.SetTextSize(0.05);
    legend.SetFillStyle(0);
    legend.SetBorderSize(0);
    legend.AddEntry(&graphs_hists[0], "Hybrid pp (new)", "plf"); // change

    for (int i = 0; i < ncols; i++) {
        pads[i]->cd();

        worlds[i]->Draw("axis");

        graphs_hists[i].Draw("same 3");
        graphs_hists[i].Draw("same lX");

        line.Draw("l");

        auto text_jet_pt = "30 < p_{T}^{jet} < 100 GeV"s; // change
    
        pads[i]->cd();
        latex.SetTextAlign(21);
        latex.SetTextSize(0.06);
        latex.DrawLatex(0.5, 0.9, (text_jet_pt).c_str()); 
    }

    pads[ncols-1]->cd();
    latex.SetTextSize(0.05);
    latex.SetTextAlign(31);
    latex.DrawLatex(0.95, 0.78, (text_photon_pt).c_str());
    latex.DrawLatex(0.95, 0.70, (text_photon_eta).c_str());
    latex.DrawLatex(0.95, 0.62, (text_dphi + ", " + text_jet_eta).c_str());
    latex.DrawLatex(0.95, 0.54, (text_jet_alg).c_str());
    legend.Draw();

    canvas.SaveAs("hybrid_spectra_pp_new.pdf"); // change

    return 0;
}

int main() {
    return congratulate();
}