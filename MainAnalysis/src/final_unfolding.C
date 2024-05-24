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

    static int style[6] = {20, 25, 34, 32, 22, 27};
    static int color[6] = {TColor::GetColor("#5790FC"), 
                           TColor::GetColor("#F89C20"), 
                           TColor::GetColor("#E42536"),
                           TColor::GetColor("#964A8B"),
                           TColor::GetColor("#9C9CA1"),
                           TColor::GetColor("#7A21DD")
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

void set_pad(TPad &pad, bool logx, bool logy, bool logz)
{
    pad.SetLeftMargin(0.15);
    pad.SetTopMargin(0.15);
    pad.SetRightMargin(0.15);
    pad.SetBottomMargin(0.15);
    pad.SetTickx();
    pad.SetTicky();

    if (logx) pad.SetLogx();
    if (logy) pad.SetLogy();
    if (logz) pad.SetLogz();

    pad.Draw();
}

int plot() {
    float const dphi_min_numerator = 2;
    float const dphi_min_denominator = 3;
    float const jet_eta_abs = 1.6;
    float const photon_eta_abs = 1.442;

    std::vector<float> bpho_pt = {60, 200};
    std::vector<float> bdr = {0, 0.2};
    std::vector<float> bjet_pt = {30, 200};

    const int ncols = 2;

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    std::vector<std::string> figures = {"response"s};

    /* open input files */
    for (size_t i = 0; i < figures.size(); ++i) {
        /* define kinematics and luminosity */
        auto text_system = "(5.02 TeV)"s;
        auto text_cms = "CMS #scale[0.8]{#font[52]{Simulation Preliminary}}"s;
        auto text_photon_pt = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV"s;
        auto text_photon_eta = "|#eta^{#gamma}| < "s + to_text(photon_eta_abs);
        auto text_dphi = "#Delta#phi_{j#gamma} > #frac{"s + to_text(dphi_min_numerator) + "#pi}{"s + to_text(dphi_min_denominator) + "}"s;
        auto text_jet_alg = "anti-k_{T} R = 0.3"s;
        auto text_jet_eta = "|#eta^{jet}| < "s + to_text(jet_eta_abs);
        auto text_jet_pt = to_text(bjet_pt[0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV"s;
        auto text_aa = "Pythia8 (CP5) QCD Photon + Hydjet (Drum5F)"s;
        auto text_pp = "Pythia8 (CP5) QCD Photon"s;

        /* styling */
        gStyle->SetLineScalePS(1);
        gStyle->SetPalette(kViridis);
        gStyle->SetOptStat(0);

        /* size canvas */
        double panel_left_margin = 0.15;
        double panel_top_margin = 0.15;
        double panel_right_margin = 0.15;
        double panel_bottom_margin = 0.15;

        double panel_size = 500;
        double panel_width = panel_size / (1 - panel_left_margin - panel_right_margin);
        double panel_height = panel_size / (1 - panel_top_margin - panel_bottom_margin);

        double canvas_width = panel_width * ncols;
        double canvas_height = panel_height;

        double sf = 1;

        double pad_x0 = panel_width * panel_left_margin / canvas_width;
        double pad_y0 = panel_height * panel_bottom_margin / canvas_height;
        double pad_dx = panel_size / canvas_width;
        double pad_dy = panel_size / canvas_height;
        double pad_x1 = panel_width * panel_right_margin / canvas_width;
        double pad_y1 = panel_height * panel_top_margin / canvas_height;

        /* text sizes */
        // double axis_label_size = 0.05/sf;
        double legend_size = 0.05;
        double text_size = 0.04;

        /* declare canvas, pads, axes, and titles */
        TCanvas canvas("canvas", "", canvas_width, canvas_height);

        TLatex latex;
        latex.SetNDC();

        std::vector<TH2F*> worlds(ncols);
        std::vector<TPad*> pads(ncols);

        pads[0] = new TPad("P1", "", pad_x0 * 0 + pad_dx * 0 + pad_x1 * 0, pad_y0 * 0 + pad_dy * 0 + pad_y1 * 0, pad_x0 * 1 + pad_dx * 1 + pad_x1 * 1, pad_y0 * 1 + pad_dy * 1 + pad_y1 * 1, 0);
        pads[1] = new TPad("P1", "", pad_x0 * 1 + pad_dx * 1 + pad_x1 * 1, pad_y0 * 0 + pad_dy * 0 + pad_y1 * 0, pad_x0 * 2 + pad_dx * 2 + pad_x1 * 2, pad_y0 * 1 + pad_dy * 1 + pad_y1 * 1, 0);

        switch (i) {
            case 0:
                // get histograms
                auto file_aa = new TFile("data/arc/pho_60_rebin3/process_vacillate_aa_qcd.root", "read");
                auto file_pp = new TFile("data/arc/pho_60_rebin3/process_vacillate_pp_qcd.root", "read");

                auto hist_aa = new history<TH2F>(file_aa, "aa_c"s);
                auto hist_pp = new history<TH2F>(file_pp, "pp_c"s);

                (*hist_aa)[3]->GetZaxis()->SetTitle("");
                (*hist_pp)[0]->GetZaxis()->SetTitle("");
                (*hist_aa)[3]->GetYaxis()->SetTitle("");
                (*hist_pp)[0]->GetYaxis()->SetTitle("");
                (*hist_aa)[3]->GetXaxis()->SetTitle("");
                (*hist_pp)[0]->GetXaxis()->SetTitle("");

                // declare legends
                TLegend legend_part1(0.65, 0.87, 0.95, 0.95);
                legend_part1.SetTextFont(42);
                legend_part1.SetTextSize(legend_size);
                legend_part1.SetFillStyle(0);
                legend_part1.SetBorderSize(0);
                legend_part1.AddEntry((TObject*) 0, "PbPb MC 0-10%", "");

                TLegend legend_part2(0.65, 0.87, 0.95, 0.95);
                legend_part2.SetTextFont(42);
                legend_part2.SetTextSize(legend_size);
                legend_part2.SetFillStyle(0);
                legend_part2.SetBorderSize(0);
                legend_part2.AddEntry((TObject*) 0, "pp MC", "");

                // set pads
                set_pad(*pads[0], 0, 0, 1);
                set_pad(*pads[1], 0, 0, 1);

                // plot histograms
                pads[0]->cd();
                (*hist_aa)[3]->Draw("colz");
                legend_part1.Draw();
                gPad->SetLogz();
                gPad->SetTicks();

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(31);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.95, 0.13, (text_photon_pt + ", " + text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.95, 0.05, (text_jet_alg + ", " + text_jet_pt + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.05, 0.87, (text_aa).c_str());

                pads[1]->cd();
                (*hist_pp)[0]->Draw("colz");
                legend_part2.Draw();
                gPad->SetLogz();
                gPad->SetTicks();

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(31);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.95, 0.13, (text_photon_pt + ", " + text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.95, 0.05, (text_jet_alg + ", " + text_jet_pt + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.05, 0.87, (text_pp).c_str());

                // x axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(0);
                latex.DrawLatex(pad_x0 * 1 + pad_dx * 0.5 + pad_x1 * 0, pad_y0 * 0.5, "Reconstructed Bin Index");
                latex.DrawLatex(pad_x0 * 2 + pad_dx * 1.5 + pad_x1 * 1, pad_y0 * 0.5, "Reconstructed Bin Index");

                // y axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(90);
                latex.DrawLatex(pad_x0 * 0.4 + pad_dx * 0 + pad_x1 * 0, pad_y0 + pad_dy * 0.5, "Generator Bin Index");
                latex.DrawLatex(pad_x0 * 1.4 + pad_dx * 0 + pad_x1 * 0, pad_y0 + pad_dy * 0.5, "Generator Bin Index");
            break;
        }

        canvas.cd();

        latex.SetTextFont(62);
        latex.SetTextSize(0.07/sf);
        latex.SetTextAlign(11);
        latex.SetTextAngle(0);
        latex.DrawLatex(pad_x0, pad_y0 * 1 + pad_dy * 1 + pad_y1 * 0.1, text_cms.c_str());

        latex.SetTextFont(42);
        latex.SetTextSize(0.045/sf);
        latex.SetTextAlign(31);
        latex.SetTextAngle(0);
        latex.DrawLatex(pad_x0 * 2 + pad_dx * 2 + pad_x1 * 1, pad_y0 * 1 + pad_dy * 1 + pad_y1 * 0.1, text_system.c_str());

        canvas.SaveAs(("unfolding_" + figures[i] + ".pdf").c_str());

        // auto graphs_hists_aa = get_graph(hists_aa, 2, 0);
        // auto graphs_hists_pp = get_graph(hists_pp, 2, 1);

        // TGraph line;
        // if (system == 2 || system == 4)    line.SetPoint(0, xmin, 1);
        // if (system == 2 || system == 4)    line.SetPoint(1, xmax, 1);
        // line.SetLineStyle(kDashed);

        // for (int i = 0; i < ncols; i++) {
        //     worlds[i]->Draw("axis");

        //     line.Draw("l");
        // }
    }

    return 0;
}

int main() {
    return plot();
}