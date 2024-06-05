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

#include "TUnfoldBinning.h"
#include "TUnfoldDensity.h"

#include <string>
#include <vector>
#include <cmath>

using namespace std::literals::string_literals;
using namespace std::placeholders;

void set_format(TH1F* h, int type)
{
    static int style[6] = {20, 25, 34, 32, 22, 27};
    static int color[6] = {TColor::GetColor("#5790FC"), 
                           TColor::GetColor("#F89C20"), 
                           TColor::GetColor("#E42536"),
                           TColor::GetColor("#964A8B"),
                           TColor::GetColor("#9C9CA1"),
                           TColor::GetColor("#7A21DD")
                           };


    h->SetMarkerStyle(style[type]);
    h->SetMarkerColor(color[type]);
    h->SetLineColor(color[type]);
    h->SetMarkerSize(1);
    h->SetLineWidth(2.0);

    return;
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
    const int nrows = 2;

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    std::vector<std::string> figures = {"comparison"s, "closure_mc_bayes_mc"s};

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
        double canvas_height = panel_height * nrows;

        double sf = 2;

        double pad_x0 = panel_width * panel_left_margin / canvas_width;
        double pad_y0 = panel_height * panel_bottom_margin / canvas_height;
        double pad_dx = panel_size / canvas_width;
        double pad_dy = panel_size / canvas_height;
        double pad_x1 = panel_width * panel_right_margin / canvas_width;
        double pad_y1 = panel_height * panel_top_margin / canvas_height;

        /* text sizes */
        double axis_label_size = 0.05/sf;
        double legend_size = 0.035;
        double text_size = 0.035;

        /* declare canvas, pads, axes, and titles */
        TCanvas canvas("canvas", "", canvas_width, canvas_height);

        TLatex latex;
        latex.SetNDC();

        std::vector<TH2F*> worlds(ncols);
        std::vector<TPad*> pads(ncols * nrows);

        pads[0] = new TPad("P1", "", pad_x0 * 0 + pad_dx * 0 + pad_x1 * 0, pad_y0 * 0 + pad_dy * 0 + pad_y1 * 0, pad_x0 * 1 + pad_dx * 1 + pad_x1 * 1, pad_y0 * 1 + pad_dy * 1 + pad_y1 * 1, 0);
        pads[1] = new TPad("P2", "", pad_x0 * 1 + pad_dx * 1 + pad_x1 * 1, pad_y0 * 0 + pad_dy * 0 + pad_y1 * 0, pad_x0 * 2 + pad_dx * 2 + pad_x1 * 2, pad_y0 * 1 + pad_dy * 1 + pad_y1 * 1, 0);
        pads[2] = new TPad("P3", "", pad_x0 * 0 + pad_dx * 0 + pad_x1 * 0, pad_y0 * 1 + pad_dy * 1 + pad_y1 * 1, pad_x0 * 1 + pad_dx * 1 + pad_x1 * 1, pad_y0 * 2 + pad_dy * 2 + pad_y1 * 2, 0);
        pads[3] = new TPad("P4", "", pad_x0 * 1 + pad_dx * 1 + pad_x1 * 1, pad_y0 * 1 + pad_dy * 1 + pad_y1 * 1, pad_x0 * 2 + pad_dx * 2 + pad_x1 * 2, pad_y0 * 2 + pad_dy * 2 + pad_y1 * 2, 0);

        switch (i) {
            case 0: // folded theory comparison
            {
                // get histograms
                auto file_aa_pyquen_pp = new TFile("data/arc/pho_60_rebin1/compare_unfolding_inputs_aa_pyquen_pp.root", "read");
                auto file_pp_pyquen_pp = new TFile("data/arc/pho_60_rebin1/compare_unfolding_inputs_pp_pyquen_pp.root", "read");

                auto file_aa_pyquen_aa = new TFile("data/arc/pho_60_rebin1/compare_unfolding_inputs_aa_pyquen_aa.root", "read");
                auto file_pp_pyquen_aa = new TFile("data/arc/pho_60_rebin1/compare_unfolding_inputs_pp_pyquen_aa.root", "read");

                auto file_aa_jewel_pp = new TFile("data/arc/pho_60_rebin1/compare_unfolding_inputs_aa_jewel_pp.root", "read");
                auto file_pp_jewel_pp = new TFile("data/arc/pho_60_rebin1/compare_unfolding_inputs_pp_jewel_pp.root", "read");

                auto file_aa_jewel_aa = new TFile("data/arc/pho_60_rebin1/compare_unfolding_inputs_aa_jewel_aa.root", "read");
                auto file_pp_jewel_aa = new TFile("data/arc/pho_60_rebin1/compare_unfolding_inputs_pp_jewel_aa.root", "read");

                auto hist_aa_pyquen_pp_fold0 = new history<TH1F>(file_aa_pyquen_pp, "input_theory_reco_fold0"s);
                auto hist_pp_pyquen_pp_fold0 = new history<TH1F>(file_pp_pyquen_pp, "input_theory_reco_fold0"s);

                auto hist_aa_pyquen_aa_fold0 = new history<TH1F>(file_aa_pyquen_aa, "input_theory_reco_fold0"s);
                auto hist_pp_pyquen_aa_fold0 = new history<TH1F>(file_pp_pyquen_aa, "input_theory_reco_fold0"s);
                
                auto hist_aa_jewel_pp_fold0 = new history<TH1F>(file_aa_jewel_pp, "input_theory_reco_fold0"s);
                auto hist_pp_jewel_pp_fold0 = new history<TH1F>(file_pp_jewel_pp, "input_theory_reco_fold0"s);

                auto hist_aa_jewel_aa_fold0 = new history<TH1F>(file_aa_jewel_aa, "input_theory_reco_fold0"s);
                auto hist_pp_jewel_aa_fold0 = new history<TH1F>(file_pp_jewel_aa, "input_theory_reco_fold0"s);

                auto hist_aa_pythia_fold0 = new history<TH1F>(file_aa_jewel_aa, "input_mc_create_reco_fold0"s);
                auto hist_pp_pythia_fold0 = new history<TH1F>(file_pp_jewel_aa, "input_mc_create_reco_fold0"s);

                auto hist_aa_pyquen_pp_fold1 = new history<TH1F>(file_aa_pyquen_pp, "input_theory_reco_fold1"s);
                auto hist_pp_pyquen_pp_fold1 = new history<TH1F>(file_pp_pyquen_pp, "input_theory_reco_fold1"s);

                auto hist_aa_pyquen_aa_fold1 = new history<TH1F>(file_aa_pyquen_aa, "input_theory_reco_fold1"s);
                auto hist_pp_pyquen_aa_fold1 = new history<TH1F>(file_pp_pyquen_aa, "input_theory_reco_fold1"s);
                
                auto hist_aa_jewel_pp_fold1 = new history<TH1F>(file_aa_jewel_pp, "input_theory_reco_fold1"s);
                auto hist_pp_jewel_pp_fold1 = new history<TH1F>(file_pp_jewel_pp, "input_theory_reco_fold1"s);

                auto hist_aa_jewel_aa_fold1 = new history<TH1F>(file_aa_jewel_aa, "input_theory_reco_fold1"s);
                auto hist_pp_jewel_aa_fold1 = new history<TH1F>(file_pp_jewel_aa, "input_theory_reco_fold1"s);

                auto hist_aa_pythia_fold1 = new history<TH1F>(file_aa_jewel_aa, "input_mc_create_reco_fold1"s);
                auto hist_pp_pythia_fold1 = new history<TH1F>(file_pp_jewel_aa, "input_mc_create_reco_fold1"s);

                (*hist_aa_pyquen_pp_fold0)[3]->GetYaxis()->SetTitle("");
                (*hist_pp_pyquen_pp_fold0)[0]->GetYaxis()->SetTitle("");
                (*hist_aa_pyquen_pp_fold0)[3]->GetXaxis()->SetTitle("");
                (*hist_pp_pyquen_pp_fold0)[0]->GetXaxis()->SetTitle("");
                (*hist_aa_pyquen_pp_fold1)[3]->GetYaxis()->SetTitle("");
                (*hist_pp_pyquen_pp_fold1)[0]->GetYaxis()->SetTitle("");
                (*hist_aa_pyquen_pp_fold1)[3]->GetXaxis()->SetTitle("");
                (*hist_pp_pyquen_pp_fold1)[0]->GetXaxis()->SetTitle("");

                (*hist_aa_pyquen_pp_fold0)[3]->SetMinimum(0);
                (*hist_pp_pyquen_pp_fold0)[0]->SetMinimum(0);
                (*hist_aa_pyquen_pp_fold0)[3]->SetMaximum(25);
                (*hist_pp_pyquen_pp_fold0)[0]->SetMaximum(25);
                (*hist_aa_pyquen_pp_fold1)[3]->SetMinimum(0);
                (*hist_pp_pyquen_pp_fold1)[0]->SetMinimum(0);
                (*hist_aa_pyquen_pp_fold1)[3]->SetMaximum(0.03);
                (*hist_pp_pyquen_pp_fold1)[0]->SetMaximum(0.03);

                set_format((*hist_aa_pyquen_pp_fold0)[3], 0);
                set_format((*hist_pp_pyquen_pp_fold0)[0], 0);
                set_format((*hist_aa_pyquen_pp_fold1)[3], 0);
                set_format((*hist_pp_pyquen_pp_fold1)[0], 0);
                set_format((*hist_aa_pyquen_aa_fold0)[3], 1);
                set_format((*hist_pp_pyquen_aa_fold0)[0], 1);
                set_format((*hist_aa_pyquen_aa_fold1)[3], 1);
                set_format((*hist_pp_pyquen_aa_fold1)[0], 1);
                set_format((*hist_aa_jewel_pp_fold0)[3], 2);
                set_format((*hist_pp_jewel_pp_fold0)[0], 2);
                set_format((*hist_aa_jewel_pp_fold1)[3], 2);
                set_format((*hist_pp_jewel_pp_fold1)[0], 2);
                set_format((*hist_aa_jewel_aa_fold0)[3], 3);
                set_format((*hist_pp_jewel_aa_fold0)[0], 3);
                set_format((*hist_aa_jewel_aa_fold1)[3], 3);
                set_format((*hist_pp_jewel_aa_fold1)[0], 3);
                set_format((*hist_aa_pythia_fold0)[3], 4);
                set_format((*hist_pp_pythia_fold0)[0], 4);
                set_format((*hist_aa_pythia_fold1)[3], 4);
                set_format((*hist_pp_pythia_fold1)[0], 4);

                // set legend_part1s
                auto legend_part1 = new TLegend(0.5, 0.5, 0.7, 0.73);
                legend_part1->SetTextFont(42);
                legend_part1->SetTextSize(legend_size);
                legend_part1->SetFillStyle(0);
                legend_part1->SetBorderSize(0);
                legend_part1->AddEntry((*hist_aa_pyquen_pp_fold0)[3], "Pyquen pp", "lp");
                legend_part1->AddEntry((*hist_aa_pyquen_aa_fold0)[3], "Pyquen PbPb 0-10%", "lp");
                legend_part1->AddEntry((*hist_aa_jewel_pp_fold0)[3], "Jewel pp", "lp");
                legend_part1->AddEntry((*hist_aa_jewel_aa_fold0)[3], "Jewel PbPb 0-10%", "lp");
                legend_part1->AddEntry((*hist_aa_pythia_fold0)[3], "Pythia PbPb 0-10%", "lp");

                auto legend_part2 = new TLegend(0.5, 0.5, 0.7, 0.73);
                legend_part2->SetTextFont(42);
                legend_part2->SetTextSize(legend_size);
                legend_part2->SetFillStyle(0);
                legend_part2->SetBorderSize(0);
                legend_part2->AddEntry((*hist_pp_pyquen_pp_fold0)[0], "Pyquen pp", "lp");
                legend_part2->AddEntry((*hist_pp_pyquen_aa_fold0)[0], "Pyquen PbPb 0-10%", "lp");
                legend_part2->AddEntry((*hist_pp_jewel_pp_fold0)[0], "Jewel pp", "lp");
                legend_part2->AddEntry((*hist_pp_jewel_aa_fold0)[0], "Jewel PbPb 0-10%", "lp");
                legend_part2->AddEntry((*hist_pp_pythia_fold0)[0], "Pythia pp", "lp");

                // set pads
                set_pad(*pads[0], 0, 0, 0);
                set_pad(*pads[1], 0, 0, 0);
                set_pad(*pads[2], 0, 0, 0);
                set_pad(*pads[3], 0, 0, 0);

                // plot histograms
                pads[0]->cd();

                gPad->SetTicks();

                (*hist_aa_pyquen_pp_fold0)[3]->Draw();
                (*hist_aa_pyquen_aa_fold0)[3]->Draw("same");
                (*hist_aa_jewel_pp_fold0)[3]->Draw("same");
                (*hist_aa_jewel_aa_fold0)[3]->Draw("same");
                (*hist_aa_pythia_fold0)[3]->Draw("same");

                legend_part1->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(31);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.82, 0.45, (text_photon_pt).c_str());
                latex.DrawLatex(0.82, 0.39, (text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.82, 0.33, (text_jet_pt).c_str());
                latex.DrawLatex(0.82, 0.27, (text_jet_alg + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "Smeared to PbPb 0-10% reco");

                pads[1]->cd();
                
                gPad->SetTicks();

                (*hist_pp_pyquen_pp_fold0)[0]->Draw();
                (*hist_pp_pyquen_aa_fold0)[0]->Draw("same");
                (*hist_pp_jewel_pp_fold0)[0]->Draw("same");
                (*hist_pp_jewel_aa_fold0)[0]->Draw("same");
                (*hist_pp_pythia_fold0)[0]->Draw("same");

                legend_part2->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(31);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.82, 0.45, (text_photon_pt).c_str());
                latex.DrawLatex(0.82, 0.39, (text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.82, 0.33, (text_jet_pt).c_str());
                latex.DrawLatex(0.82, 0.27, (text_jet_alg + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_pp).c_str());
                latex.DrawLatex(0.18, 0.74, "Smeared to pp reco");

                pads[2]->cd();

                gPad->SetTicks();

                (*hist_aa_pyquen_pp_fold1)[3]->Draw();
                (*hist_aa_pyquen_aa_fold1)[3]->Draw("same");
                (*hist_aa_jewel_pp_fold1)[3]->Draw("same");
                (*hist_aa_jewel_aa_fold1)[3]->Draw("same");
                (*hist_aa_pythia_fold1)[3]->Draw("same");

                legend_part1->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(31);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.82, 0.45, (text_photon_pt).c_str());
                latex.DrawLatex(0.82, 0.39, (text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.82, 0.33, (text_jet_pt).c_str());
                latex.DrawLatex(0.82, 0.27, (text_jet_alg + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "Smeared to PbPb 0-10% reco");

                pads[3]->cd();

                gPad->SetTicks();

                (*hist_pp_pyquen_pp_fold1)[0]->Draw();
                (*hist_pp_pyquen_aa_fold1)[0]->Draw("same");
                (*hist_pp_jewel_pp_fold1)[0]->Draw("same");
                (*hist_pp_jewel_aa_fold1)[0]->Draw("same");
                (*hist_pp_pythia_fold1)[0]->Draw("same");

                legend_part2->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(31);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.82, 0.45, (text_photon_pt).c_str());
                latex.DrawLatex(0.82, 0.39, (text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.82, 0.33, (text_jet_pt).c_str());
                latex.DrawLatex(0.82, 0.27, (text_jet_alg + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "Smeared to pp reco");

                // x axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(0);
                latex.DrawLatex(pad_x0 * 1 + pad_dx * 0.5 + pad_x1 * 0, pad_y0 * 0.5 + pad_dy * 0 + pad_y1 * 0, "#Deltaj");
                latex.DrawLatex(pad_x0 * 2 + pad_dx * 1.5 + pad_x1 * 1, pad_y0 * 0.5 + pad_dy * 0 + pad_y1 * 0, "#Deltaj");
                latex.DrawLatex(pad_x0 * 1 + pad_dx * 0.5 + pad_x1 * 0, pad_y0 * 1.5 + pad_dy * 1 + pad_y1 * 1, "p_{T}^{jet}");
                latex.DrawLatex(pad_x0 * 2 + pad_dx * 1.5 + pad_x1 * 1, pad_y0 * 1.5 + pad_dy * 1 + pad_y1 * 1, "p_{T}^{jet}");

                // y axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(90);
                latex.DrawLatex(pad_x0 * 0.3 + pad_dx * 0 + pad_x1 * 0, pad_y0 * 1 + pad_dy * 0.5 + pad_y1 * 0, "1/N^{#gamma} dN/d#Deltaj");
                latex.DrawLatex(pad_x0 * 1.3 + pad_dx * 1 + pad_x1 * 1, pad_y0 * 1 + pad_dy * 0.5 + pad_y1 * 0, "1/N^{#gamma} dN/d#Deltaj");
                latex.DrawLatex(pad_x0 * 0.3 + pad_dx * 0 + pad_x1 * 0, pad_y0 * 2 + pad_dy * 1.5 + pad_y1 * 1, "1/N^{#gamma} dN/dp_{T}^{jet}");
                latex.DrawLatex(pad_x0 * 1.3 + pad_dx * 1 + pad_x1 * 1, pad_y0 * 2 + pad_dy * 1.5 + pad_y1 * 1, "1/N^{#gamma} dN/dp_{T}^{jet}");
                break;
            }
            case 1: // closure MC, D'Agostini, MC prior
            {
                // get histograms
                auto file_aa = new TFile("data/arc/pho_60_rebin1/compare_unfolding_closure_gen_aa_pythia_mebs_nonclosure_mc.root", "read");
                auto file_pp = new TFile("data/arc/pho_60_rebin1/compare_unfolding_closure_gen_pp_pythia_mc.root", "read");

                auto hist_aa_unfolded_fold0 = new history<TH1F>(file_aa, "unfolded_bayes_fold0"s);
                auto hist_pp_unfolded_fold0 = new history<TH1F>(file_pp, "unfolded_bayes_fold0"s);

                auto hist_aa_unfolded_fold1 = new history<TH1F>(file_aa, "unfolded_bayes_fold1"s);
                auto hist_pp_unfolded_fold1 = new history<TH1F>(file_pp, "unfolded_bayes_fold1"s);

                auto hist_aa_gen_fold0 = new history<TH1F>(file_aa, "gen_bayes_fold0"s);
                auto hist_pp_gen_fold0 = new history<TH1F>(file_pp, "gen_bayes_fold0"s);
                
                auto hist_aa_gen_fold1 = new history<TH1F>(file_aa, "gen_bayes_fold1"s);
                auto hist_pp_gen_fold1 = new history<TH1F>(file_pp, "gen_bayes_fold1"s);

                (*hist_aa_unfolded_fold0)[3]->GetYaxis()->SetTitle("");
                (*hist_pp_unfolded_fold0)[0]->GetYaxis()->SetTitle("");
                (*hist_aa_unfolded_fold1)[3]->GetXaxis()->SetTitle("");
                (*hist_pp_unfolded_fold1)[0]->GetXaxis()->SetTitle("");
                (*hist_aa_gen_fold0)[3]->GetYaxis()->SetTitle("");
                (*hist_pp_gen_fold0)[0]->GetYaxis()->SetTitle("");
                (*hist_aa_gen_fold1)[3]->GetXaxis()->SetTitle("");
                (*hist_pp_gen_fold1)[0]->GetXaxis()->SetTitle("");

                (*hist_aa_unfolded_fold0)[3]->SetMinimum(0);
                (*hist_pp_unfolded_fold0)[0]->SetMinimum(0);
                (*hist_aa_unfolded_fold0)[3]->SetMaximum(25);
                (*hist_pp_unfolded_fold0)[0]->SetMaximum(25);
                (*hist_aa_unfolded_fold1)[3]->SetMinimum(0);
                (*hist_pp_unfolded_fold1)[0]->SetMinimum(0);
                (*hist_aa_unfolded_fold1)[3]->SetMaximum(0.03);
                (*hist_pp_unfolded_fold1)[0]->SetMaximum(0.03);

                set_format((*hist_aa_unfolded_fold0)[3], 0);
                set_format((*hist_pp_unfolded_fold0)[0], 0);
                set_format((*hist_aa_unfolded_fold1)[3], 0);
                set_format((*hist_pp_unfolded_fold1)[0], 0);
                set_format((*hist_aa_gen_fold0)[3], 1);
                set_format((*hist_pp_gen_fold0)[0], 1);
                set_format((*hist_aa_gen_fold1)[3], 1);
                set_format((*hist_pp_gen_fold1)[0], 1);

                // set legends
                auto legend = new TLegend(0.18, 0.18, 0.38, 0.27);
                legend->SetTextFont(42);
                legend->SetTextSize(legend_size);
                legend->SetFillStyle(0);
                legend->SetBorderSize(0);
                legend->AddEntry((*hist_aa_unfolded_fold0)[3], "Truth", "lp");
                legend->AddEntry((*hist_aa_gen_fold0)[3], "Unfolded", "lp");

                // set pads
                set_pad(*pads[0], 0, 0, 0);
                set_pad(*pads[1], 0, 0, 0);
                set_pad(*pads[2], 0, 0, 0);
                set_pad(*pads[3], 0, 0, 0);

                // plot histograms
                pads[0]->cd();

                gPad->SetTicks();

                (*hist_aa_unfolded_fold0)[3]->Draw();
                (*hist_aa_gen_fold0)[3]->Draw("same");

                legend->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.4, 0.54, (text_photon_pt).c_str());
                latex.DrawLatex(0.4, 0.48, (text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.4, 0.42, (text_jet_pt).c_str());
                latex.DrawLatex(0.4, 0.36, (text_jet_alg + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "PbPb 0-10%");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.4, 0.74, "Source: MC");
                latex.DrawLatex(0.4, 0.68, "Algorithm: D'Agostini");
                latex.DrawLatex(0.4, 0.62, "Prior: MC");

                pads[1]->cd();
                
                gPad->SetTicks();

                (*hist_pp_unfolded_fold0)[0]->Draw();
                (*hist_pp_gen_fold0)[0]->Draw("same");

                legend->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.4, 0.54, (text_photon_pt).c_str());
                latex.DrawLatex(0.4, 0.48, (text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.4, 0.42, (text_jet_pt).c_str());
                latex.DrawLatex(0.4, 0.36, (text_jet_alg + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_pp).c_str());
                latex.DrawLatex(0.18, 0.74, "pp");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.4, 0.74, "Source: MC");
                latex.DrawLatex(0.4, 0.68, "Algorithm: D'Agostini");
                latex.DrawLatex(0.4, 0.62, "Prior: MC");

                pads[2]->cd();

                gPad->SetTicks();

                (*hist_aa_unfolded_fold1)[3]->Draw();
                (*hist_aa_gen_fold1)[3]->Draw("same");

                legend->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.4, 0.66, (text_photon_pt).c_str());
                latex.DrawLatex(0.4, 0.60, (text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.4, 0.54, (text_jet_pt).c_str());
                latex.DrawLatex(0.4, 0.48, (text_jet_alg + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "PbPb 0-10%");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.66, "Source: MC");
                latex.DrawLatex(0.18, 0.60, "Algorithm: D'Agostini");
                latex.DrawLatex(0.18, 0.54, "Prior: MC");

                pads[3]->cd();

                gPad->SetTicks();

                (*hist_pp_unfolded_fold1)[0]->Draw();
                (*hist_pp_gen_fold1)[0]->Draw("same");

                legend->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.4, 0.66, (text_photon_pt).c_str());
                latex.DrawLatex(0.4, 0.60, (text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.4, 0.54, (text_jet_pt).c_str());
                latex.DrawLatex(0.4, 0.48, (text_jet_alg + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "pp");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.66, "Source: MC");
                latex.DrawLatex(0.18, 0.60, "Algorithm: D'Agostini");
                latex.DrawLatex(0.18, 0.54, "Prior: MC");

                // x axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(0);
                latex.DrawLatex(pad_x0 * 1 + pad_dx * 0.5 + pad_x1 * 0, pad_y0 * 0.5 + pad_dy * 0 + pad_y1 * 0, "#Deltaj");
                latex.DrawLatex(pad_x0 * 2 + pad_dx * 1.5 + pad_x1 * 1, pad_y0 * 0.5 + pad_dy * 0 + pad_y1 * 0, "#Deltaj");
                latex.DrawLatex(pad_x0 * 1 + pad_dx * 0.5 + pad_x1 * 0, pad_y0 * 1.5 + pad_dy * 1 + pad_y1 * 1, "p_{T}^{jet}");
                latex.DrawLatex(pad_x0 * 2 + pad_dx * 1.5 + pad_x1 * 1, pad_y0 * 1.5 + pad_dy * 1 + pad_y1 * 1, "p_{T}^{jet}");

                // y axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(90);
                latex.DrawLatex(pad_x0 * 0.3 + pad_dx * 0 + pad_x1 * 0, pad_y0 * 1 + pad_dy * 0.5 + pad_y1 * 0, "1/N^{#gamma} dN/d#Deltaj");
                latex.DrawLatex(pad_x0 * 1.3 + pad_dx * 1 + pad_x1 * 1, pad_y0 * 1 + pad_dy * 0.5 + pad_y1 * 0, "1/N^{#gamma} dN/d#Deltaj");
                latex.DrawLatex(pad_x0 * 0.3 + pad_dx * 0 + pad_x1 * 0, pad_y0 * 2 + pad_dy * 1.5 + pad_y1 * 1, "1/N^{#gamma} dN/dp_{T}^{jet}");
                latex.DrawLatex(pad_x0 * 1.3 + pad_dx * 1 + pad_x1 * 1, pad_y0 * 2 + pad_dy * 1.5 + pad_y1 * 1, "1/N^{#gamma} dN/dp_{T}^{jet}");
                break;
            }
        }

        canvas.cd();

        latex.SetTextFont(62);
        latex.SetTextSize(0.05/sf);
        latex.SetTextAlign(11);
        latex.SetTextAngle(0);
        latex.DrawLatex(pad_x0, pad_y0 * 2 + pad_dy * 2 + pad_y1 * 1.2, text_cms.c_str());

        latex.SetTextFont(42);
        latex.SetTextSize(0.045/sf);
        latex.SetTextAlign(31);
        latex.SetTextAngle(0);
        latex.DrawLatex(pad_x0 * 2 + pad_dx * 2 + pad_x1 * 1, pad_y0 * 2 + pad_dy * 2 + pad_y1 * 1.2, text_system.c_str());

        canvas.SaveAs(("unfolding_" + figures[i] + ".pdf").c_str());
    }

    return 0;
}

int main() {
    return plot();
}