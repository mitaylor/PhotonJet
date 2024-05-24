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

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    std::vector<std::string> figures = {"binning"s, "response"s, "flattened_input"s, "purity"s, "efficiency"s, "mse_bayes_mc"s, "mse_bayes_flat"s, "mse_svd_mc"s, "mse_svd_flat"s, "bottom_line"s};

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
        double canvas_height = panel_height;

        double sf = 1;

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
        std::vector<TPad*> pads(ncols);

        pads[0] = new TPad("P1", "", pad_x0 * 0 + pad_dx * 0 + pad_x1 * 0, pad_y0 * 0 + pad_dy * 0 + pad_y1 * 0, pad_x0 * 1 + pad_dx * 1 + pad_x1 * 1, pad_y0 * 1 + pad_dy * 1 + pad_y1 * 1, 0);
        pads[1] = new TPad("P1", "", pad_x0 * 1 + pad_dx * 1 + pad_x1 * 1, pad_y0 * 0 + pad_dy * 0 + pad_y1 * 0, pad_x0 * 2 + pad_dx * 2 + pad_x1 * 2, pad_y0 * 1 + pad_dy * 1 + pad_y1 * 1, 0);

        switch (i) {
            case 0: // binning
            {
                gStyle->SetPalette(kBird);

                // get histograms
                auto file_aa = new TFile("data/arc/pho_60_rebin1/plot_binning_aa_qcd_raw.root", "read");
                auto file_pp = new TFile("data/arc/pho_60_rebin1/plot_binning_pp_qcd_raw.root", "read");

                auto hist_aa = new history<TH2F>(file_aa, "hist2d_sub_d_hf"s);
                auto hist_pp = new history<TH2F>(file_pp, "hist2d_sub_d_hf"s);

                (*hist_aa)[3]->GetZaxis()->SetTitle("");
                (*hist_pp)[0]->GetZaxis()->SetTitle("");
                (*hist_aa)[3]->GetYaxis()->SetTitle("");
                (*hist_pp)[0]->GetYaxis()->SetTitle("");
                (*hist_aa)[3]->GetXaxis()->SetTitle("");
                (*hist_pp)[0]->GetXaxis()->SetTitle("");

                (*hist_aa)[3]->SetMinimum(0.5);
                (*hist_pp)[0]->SetMinimum(0.5);
                (*hist_aa)[3]->SetMaximum(3000);
                (*hist_pp)[0]->SetMaximum(3000);

                // set pads
                set_pad(*pads[0], 0, 0, 1);
                set_pad(*pads[1], 0, 0, 1);

                // plot histograms
                pads[0]->cd();

                gPad->SetLogz();
                gPad->SetTicks();

                (*hist_aa)[3]->Draw("colz");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.24, (text_photon_pt + ", " + text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.18, 0.18, (text_jet_alg + ", " + text_jet_pt + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "PbPb 0-10%");

                pads[1]->cd();
                
                gPad->SetLogz();
                gPad->SetTicks();

                (*hist_pp)[0]->Draw("colz");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.24, (text_photon_pt + ", " + text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.18, 0.18, (text_jet_alg + ", " + text_jet_pt + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_pp).c_str());
                latex.DrawLatex(0.18, 0.74, "pp");

                // x axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(0);
                latex.DrawLatex(pad_x0 * 1 + pad_dx * 0.5 + pad_x1 * 0, pad_y0 * 0.5, "#Deltaj");
                latex.DrawLatex(pad_x0 * 2 + pad_dx * 1.5 + pad_x1 * 1, pad_y0 * 0.5, "#Deltaj");

                // y axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(90);
                latex.DrawLatex(pad_x0 * 0.4 + pad_dx * 0 + pad_x1 * 0, pad_y0 + pad_dy * 0.5, "p_{T}^{jet}");
                latex.DrawLatex(pad_x0 * 1.4 + pad_dx * 1 + pad_x1 * 1, pad_y0 + pad_dy * 0.5, "p_{T}^{jet}");

                 // z axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size/2);
                latex.SetTextAlign(12);
                latex.SetTextAngle(0);
                latex.DrawLatex(pad_x0 * 1 + pad_dx * 1 + pad_x1 * 0.15, pad_y0 * 0.8, "Counts");
                latex.DrawLatex(pad_x0 * 2 + pad_dx * 2 + pad_x1 * 1.15, pad_y0 * 0.8, "Counts");
                break;
            }
            case 1: // response matrix
            {
                gStyle->SetPalette(kInvertedDarkBodyRadiator);

                // get histograms
                auto file_aa = new TFile("data/arc/pho_60_rebin1/process_vacillate_aa_qcd.root", "read");
                auto file_pp = new TFile("data/arc/pho_60_rebin1/process_vacillate_pp_qcd.root", "read");

                auto hist_aa = new history<TH2F>(file_aa, "aa_c"s);
                auto hist_pp = new history<TH2F>(file_pp, "pp_c"s);

                (*hist_aa)[3]->GetZaxis()->SetTitle("");
                (*hist_pp)[0]->GetZaxis()->SetTitle("");
                (*hist_aa)[3]->GetYaxis()->SetTitle("");
                (*hist_pp)[0]->GetYaxis()->SetTitle("");
                (*hist_aa)[3]->GetXaxis()->SetTitle("");
                (*hist_pp)[0]->GetXaxis()->SetTitle("");

                (*hist_aa)[3]->SetMinimum(0.1);
                (*hist_pp)[0]->SetMinimum(0.1);
                (*hist_aa)[3]->SetMaximum(1000);
                (*hist_pp)[0]->SetMaximum(1000);

                // set pads
                set_pad(*pads[0], 0, 0, 1);
                set_pad(*pads[1], 0, 0, 1);

                // plot histograms
                pads[0]->cd();

                gPad->SetLogz();
                gPad->SetTicks();

                (*hist_aa)[3]->Draw("colz");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.24, (text_photon_pt + ", " + text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.18, 0.18, (text_jet_alg + ", " + text_jet_pt + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "PbPb 0-10%");

                pads[1]->cd();
                
                gPad->SetLogz();
                gPad->SetTicks();

                (*hist_pp)[0]->Draw("colz");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.24, (text_photon_pt + ", " + text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.18, 0.18, (text_jet_alg + ", " + text_jet_pt + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_pp).c_str());
                latex.DrawLatex(0.18, 0.74, "pp");

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
                latex.DrawLatex(pad_x0 * 1.4 + pad_dx * 1 + pad_x1 * 1, pad_y0 + pad_dy * 0.5, "Generator Bin Index");

                 // z axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size/2);
                latex.SetTextAlign(12);
                latex.SetTextAngle(0);
                latex.DrawLatex(pad_x0 * 1 + pad_dx * 1 + pad_x1 * 0.1, pad_y0 * 0.8, "Counts");
                latex.DrawLatex(pad_x0 * 2 + pad_dx * 2 + pad_x1 * 1.1, pad_y0 * 0.8, "Counts");
                break;
            }
            case 2: // flattened inputs
            {
                // get histograms
                auto file_aa = new TFile("data/arc/pho_60_rebin1/accumulate_aa_qcd_mebs_nonclosure.root", "read");
                auto file_pp = new TFile("data/arc/pho_60_rebin1/accumulate_pp_qcd.root", "read");

                auto hist_aa = new history<TH1F>(file_aa, "aa_qcd_mebs_nonclosure_s_pure_raw_sub_pjet_u_dr_jpt_sum0"s);
                auto hist_pp = new history<TH1F>(file_pp, "pp_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0"s);

                (*hist_aa)[3]->GetYaxis()->SetTitle("");
                (*hist_pp)[0]->GetYaxis()->SetTitle("");
                (*hist_aa)[3]->GetXaxis()->SetTitle("");
                (*hist_pp)[0]->GetXaxis()->SetTitle("");

                (*hist_aa)[3]->SetMinimum(0);
                (*hist_pp)[0]->SetMinimum(0);
                (*hist_aa)[3]->SetMaximum(0.05);
                (*hist_pp)[0]->SetMaximum(0.05);

                // set pads
                set_pad(*pads[0], 0, 0, 0);
                set_pad(*pads[1], 0, 0, 0);

                // plot histograms
                pads[0]->cd();

                gPad->SetTicks();

                (*hist_aa)[3]->Draw();

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.64, (text_photon_pt + ", " + text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.18, 0.58, (text_jet_alg + ", " + text_jet_pt + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "PbPb 0-10%");

                pads[1]->cd();
                
                gPad->SetTicks();

                (*hist_pp)[0]->Draw();

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.64, (text_photon_pt + ", " + text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.18, 0.58, (text_jet_alg + ", " + text_jet_pt + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_pp).c_str());
                latex.DrawLatex(0.18, 0.74, "pp");

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
                latex.DrawLatex(pad_x0 * 0.3 + pad_dx * 0 + pad_x1 * 0, pad_y0 + pad_dy * 0.5, "Counts / N_{#gamma}");
                latex.DrawLatex(pad_x0 * 1.3 + pad_dx * 1 + pad_x1 * 1, pad_y0 + pad_dy * 0.5, "Counts / N_{#gamma}");
                break;
            }
            case 3: // purity correction
            {
                // get histograms
                auto file_aa = new TFile("data/arc/pho_60_rebin1/process_vacillate_aa_qcd.root", "read");
                auto file_pp = new TFile("data/arc/pho_60_rebin1/process_vacillate_pp_qcd.root", "read");

                auto hist_aa = new history<TH1F>(file_aa, "aa_r_eff"s);
                auto hist_pp = new history<TH1F>(file_pp, "pp_r_eff"s);

                auto hist_aa_reco = new history<TH1F>(file_aa, "aa_r"s);
                auto hist_pp_reco = new history<TH1F>(file_pp, "pp_r"s);

                auto hist_aa_proj = new history<TH1F>(file_aa, "aa_proj_r"s);
                auto hist_pp_proj = new history<TH1F>(file_pp, "pp_proj_r"s);

                auto graph_aa = new TGraphAsymmErrors((*hist_aa_proj)[3], (*hist_aa_reco)[3], "cl=0.683 b(1,1) mode");
                auto graph_pp = new TGraphAsymmErrors((*hist_pp_proj)[0], (*hist_pp_reco)[0], "cl=0.683 b(1,1) mode");

                for (int j = 0; j < (*hist_aa)[3]->GetNbinsX(); ++j) {
                    (*hist_aa)[3]->SetBinContent(j + 1, graph_aa->GetPointY(j));
                    (*hist_aa)[3]->SetBinError(j + 1, graph_aa->GetErrorY(j));

                    (*hist_pp)[0]->SetBinContent(j + 1, graph_pp->GetPointY(j));
                    (*hist_pp)[0]->SetBinError(j + 1, graph_pp->GetErrorY(j));
                }

                (*hist_aa)[3]->GetYaxis()->SetTitle("");
                (*hist_pp)[0]->GetYaxis()->SetTitle("");
                (*hist_aa)[3]->GetXaxis()->SetTitle("");
                (*hist_pp)[0]->GetXaxis()->SetTitle("");

                (*hist_aa)[3]->SetMinimum(0);
                (*hist_pp)[0]->SetMinimum(0);
                (*hist_aa)[3]->SetMaximum(1.2);
                (*hist_pp)[0]->SetMaximum(1.2);

                // set pads
                set_pad(*pads[0], 0, 0, 0);
                set_pad(*pads[1], 0, 0, 0);

                // set line
                TGraph line;
                line.SetPoint(0, 0, 1);
                line.SetPoint(1, (*hist_aa)[3]->GetNbinsX(), 1);
                line.SetLineStyle(kDashed);

                // plot histograms
                pads[0]->cd();

                gPad->SetTicks();

                (*hist_aa)[3]->Draw();
                line.Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.24, (text_photon_pt + ", " + text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.18, 0.18, (text_jet_alg + ", " + text_jet_pt + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "PbPb 0-10%");

                pads[1]->cd();
                
                gPad->SetTicks();

                (*hist_pp)[0]->Draw();
                line.Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.24, (text_photon_pt + ", " + text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.18, 0.18, (text_jet_alg + ", " + text_jet_pt + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_pp).c_str());
                latex.DrawLatex(0.18, 0.74, "pp");

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
                latex.DrawLatex(pad_x0 * 0.4 + pad_dx * 0 + pad_x1 * 0, pad_y0 + pad_dy * 0.5, "Bin Purity");
                latex.DrawLatex(pad_x0 * 1.4 + pad_dx * 1 + pad_x1 * 1, pad_y0 + pad_dy * 0.5, "Bin Purity");
                break;
            }
            case 4: // efficiency correction
            {
                // get histograms
                auto file_aa = new TFile("data/arc/pho_60_rebin1/process_vacillate_aa_qcd.root", "read");
                auto file_pp = new TFile("data/arc/pho_60_rebin1/process_vacillate_pp_qcd.root", "read");

                auto hist_aa = new history<TH1F>(file_aa, "aa_g_eff"s);
                auto hist_pp = new history<TH1F>(file_pp, "pp_g_eff"s);

                auto hist_aa_gen = new history<TH1F>(file_aa, "aa_g"s);
                auto hist_pp_gen = new history<TH1F>(file_pp, "pp_g"s);

                auto hist_aa_proj = new history<TH1F>(file_aa, "aa_proj_g"s);
                auto hist_pp_proj = new history<TH1F>(file_pp, "pp_proj_g"s);

                auto graph_aa = new TGraphAsymmErrors((*hist_aa_proj)[3], (*hist_aa_gen)[3], "cl=0.683 b(1,1) mode");
                auto graph_pp = new TGraphAsymmErrors((*hist_pp_proj)[0], (*hist_pp_gen)[0], "cl=0.683 b(1,1) mode");

                for (int j = 0; j < (*hist_aa)[3]->GetNbinsX(); ++j) {
                    (*hist_aa)[3]->SetBinContent(j + 1, graph_aa->GetPointY(j));
                    (*hist_aa)[3]->SetBinError(j + 1, graph_aa->GetErrorY(j));

                    (*hist_pp)[0]->SetBinContent(j + 1, graph_pp->GetPointY(j));
                    (*hist_pp)[0]->SetBinError(j + 1, graph_pp->GetErrorY(j));
                }

                (*hist_aa)[3]->GetYaxis()->SetTitle("");
                (*hist_pp)[0]->GetYaxis()->SetTitle("");
                (*hist_aa)[3]->GetXaxis()->SetTitle("");
                (*hist_pp)[0]->GetXaxis()->SetTitle("");

                (*hist_aa)[3]->SetMinimum(0);
                (*hist_pp)[0]->SetMinimum(0);
                (*hist_aa)[3]->SetMaximum(1.2);
                (*hist_pp)[0]->SetMaximum(1.2);

                // set pads
                set_pad(*pads[0], 0, 0, 0);
                set_pad(*pads[1], 0, 0, 0);

                // set line
                TGraph line;
                line.SetPoint(0, 0, 1);
                line.SetPoint(1, (*hist_aa)[3]->GetNbinsX(), 1);
                line.SetLineStyle(kDashed);

                // plot histograms
                pads[0]->cd();

                gPad->SetTicks();

                (*hist_aa)[3]->Draw();
                line.Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.24, (text_photon_pt + ", " + text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.18, 0.18, (text_jet_alg + ", " + text_jet_pt + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "PbPb 0-10%");

                pads[1]->cd();
                
                gPad->SetTicks();

                (*hist_pp)[0]->Draw();
                line.Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.24, (text_photon_pt + ", " + text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.18, 0.18, (text_jet_alg + ", " + text_jet_pt + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_pp).c_str());
                latex.DrawLatex(0.18, 0.74, "pp");

                // x axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(0);
                latex.DrawLatex(pad_x0 * 1 + pad_dx * 0.5 + pad_x1 * 0, pad_y0 * 0.5, "Generator Bin Index");
                latex.DrawLatex(pad_x0 * 2 + pad_dx * 1.5 + pad_x1 * 1, pad_y0 * 0.5, "Generator Bin Index");

                // y axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(90);
                latex.DrawLatex(pad_x0 * 0.4 + pad_dx * 0 + pad_x1 * 0, pad_y0 + pad_dy * 0.5, "Bin Efficiency");
                latex.DrawLatex(pad_x0 * 1.4 + pad_dx * 1 + pad_x1 * 1, pad_y0 + pad_dy * 0.5, "Bin Efficiency");
                break;
            }
            case 5: // mse, jewel, bayes, mc
            {
                // get histograms
                auto file_aa = new TFile("data/arc/pho_60_rebin1/regularization_aa_bayes_jewel_pp_mc.root", "read");
                auto file_pp = new TFile("data/arc/pho_60_rebin1/regularization_pp_bayes_jewel_pp_mc.root", "read");

                auto hist_aa_mse = new history<TH1F>(file_aa, "aa_mse"s);
                auto hist_pp_mse = new history<TH1F>(file_pp, "pp_mse"s);

                auto hist_aa_bias = new history<TH1F>(file_aa, "aa_bias"s);
                auto hist_pp_bias = new history<TH1F>(file_pp, "pp_bias"s);

                auto hist_aa_variance = new history<TH1F>(file_aa, "aa_variance"s);
                auto hist_pp_variance = new history<TH1F>(file_pp, "pp_variance"s);

                (*hist_aa_mse)[3]->GetYaxis()->SetTitle("");
                (*hist_pp_mse)[0]->GetYaxis()->SetTitle("");
                (*hist_aa_mse)[3]->GetXaxis()->SetTitle("");
                (*hist_pp_mse)[0]->GetXaxis()->SetTitle("");

                (*hist_aa_mse)[3]->SetMinimum(1E-10);
                (*hist_pp_mse)[0]->SetMinimum(1E-10);
                (*hist_aa_mse)[3]->SetMaximum(1E-2);
                (*hist_pp_mse)[0]->SetMaximum(1E-2);

                set_format((*hist_aa_mse)[3], 0);
                set_format((*hist_pp_mse)[0], 0);
                set_format((*hist_aa_bias)[3], 1);
                set_format((*hist_pp_bias)[0], 1);
                set_format((*hist_aa_variance)[3], 2);
                set_format((*hist_pp_variance)[0], 2);

                // set legends
                auto legend_part1 = new TLegend(0.6, 0.2, 0.8, 0.34);
                legend_part1->SetTextFont(42);
                legend_part1->SetTextSize(legend_size);
                legend_part1->SetFillStyle(0);
                legend_part1->SetBorderSize(0);
                legend_part1->AddEntry((*hist_aa_mse)[3], "MSE", "l");
                legend_part1->AddEntry((*hist_aa_bias)[3], "Bias^{2}", "l");
                legend_part1->AddEntry((*hist_aa_variance)[3], "Variance", "l");

                auto legend_part2 = new TLegend(0.6, 0.2, 0.8, 0.34);
                legend_part2->SetTextFont(42);
                legend_part2->SetTextSize(legend_size);
                legend_part2->SetFillStyle(0);
                legend_part2->SetBorderSize(0);
                legend_part2->AddEntry((*hist_pp_mse)[0], "MSE", "l");
                legend_part2->AddEntry((*hist_pp_bias)[0], "Bias^{2}", "l");
                legend_part2->AddEntry((*hist_pp_variance)[0], "Variance", "l");

                // set pads
                set_pad(*pads[0], 0, 1, 0);
                set_pad(*pads[1], 0, 1, 0);

                // plot histograms
                pads[0]->cd();

                gPad->SetTicks();
                gPad->SetLogy();

                (*hist_aa_mse)[3]->Draw();
                (*hist_aa_bias)[3]->Draw("same");
                (*hist_aa_variance)[3]->Draw("same");

                legend_part1->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.31, "Source: Jewel pp");
                latex.DrawLatex(0.18, 0.26, "Algorithm: D'Agostini");
                latex.DrawLatex(0.18, 0.21, "Prior: MC");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(21);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.5, 0.18,("Iteration = " + std::to_string((*hist_aa_mse)[3]->GetMinimumBin())).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "PbPb 0-10%");

                pads[1]->cd();
                
                gPad->SetTicks();
                gPad->SetLogy();

                (*hist_pp_mse)[0]->Draw();
                (*hist_pp_bias)[0]->Draw("same");
                (*hist_pp_variance)[0]->Draw("same");

                legend_part2->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.31, "Source: Jewel pp");
                latex.DrawLatex(0.18, 0.26, "Algorithm: D'Agostini");
                latex.DrawLatex(0.18, 0.21, "Prior: MC");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(21);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.5, 0.18,("Iteration = " + std::to_string((*hist_pp_mse)[0]->GetMinimumBin())).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_pp).c_str());
                latex.DrawLatex(0.18, 0.74, "pp");

                // x axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(0);
                latex.DrawLatex(pad_x0 * 1 + pad_dx * 0.5 + pad_x1 * 0, pad_y0 * 0.5, "Iteration");
                latex.DrawLatex(pad_x0 * 2 + pad_dx * 1.5 + pad_x1 * 1, pad_y0 * 0.5, "Iteration");

                // y axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(90);
                latex.DrawLatex(pad_x0 * 0.4 + pad_dx * 0 + pad_x1 * 0, pad_y0 + pad_dy * 0.5, "Bin Averaged Value");
                latex.DrawLatex(pad_x0 * 1.4 + pad_dx * 1 + pad_x1 * 1, pad_y0 + pad_dy * 0.5, "Bin Averaged Value");
                break;
            }
            case 6: // mse, jewel, bayes, flat
            {
                // get histograms
                auto file_aa = new TFile("data/arc/pho_60_rebin1/regularization_aa_bayes_jewel_pp_flat.root", "read");
                auto file_pp = new TFile("data/arc/pho_60_rebin1/regularization_pp_bayes_jewel_pp_flat.root", "read");

                auto hist_aa_mse = new history<TH1F>(file_aa, "aa_mse"s);
                auto hist_pp_mse = new history<TH1F>(file_pp, "pp_mse"s);

                auto hist_aa_bias = new history<TH1F>(file_aa, "aa_bias"s);
                auto hist_pp_bias = new history<TH1F>(file_pp, "pp_bias"s);

                auto hist_aa_variance = new history<TH1F>(file_aa, "aa_variance"s);
                auto hist_pp_variance = new history<TH1F>(file_pp, "pp_variance"s);

                (*hist_aa_mse)[3]->GetYaxis()->SetTitle("");
                (*hist_pp_mse)[0]->GetYaxis()->SetTitle("");
                (*hist_aa_mse)[3]->GetXaxis()->SetTitle("");
                (*hist_pp_mse)[0]->GetXaxis()->SetTitle("");

                (*hist_aa_mse)[3]->SetMinimum(1E-10);
                (*hist_pp_mse)[0]->SetMinimum(1E-10);
                (*hist_aa_mse)[3]->SetMaximum(1E-2);
                (*hist_pp_mse)[0]->SetMaximum(1E-2);

                set_format((*hist_aa_mse)[3], 0);
                set_format((*hist_pp_mse)[0], 0);
                set_format((*hist_aa_bias)[3], 1);
                set_format((*hist_pp_bias)[0], 1);
                set_format((*hist_aa_variance)[3], 2);
                set_format((*hist_pp_variance)[0], 2);

                // set legends
                auto legend_part1 = new TLegend(0.6, 0.2, 0.8, 0.34);
                legend_part1->SetTextFont(42);
                legend_part1->SetTextSize(legend_size);
                legend_part1->SetFillStyle(0);
                legend_part1->SetBorderSize(0);
                legend_part1->AddEntry((*hist_aa_mse)[3], "MSE", "l");
                legend_part1->AddEntry((*hist_aa_bias)[3], "Bias^{2}", "l");
                legend_part1->AddEntry((*hist_aa_variance)[3], "Variance", "l");

                auto legend_part2 = new TLegend(0.6, 0.2, 0.8, 0.34);
                legend_part2->SetTextFont(42);
                legend_part2->SetTextSize(legend_size);
                legend_part2->SetFillStyle(0);
                legend_part2->SetBorderSize(0);
                legend_part2->AddEntry((*hist_pp_mse)[0], "MSE", "l");
                legend_part2->AddEntry((*hist_pp_bias)[0], "Bias^{2}", "l");
                legend_part2->AddEntry((*hist_pp_variance)[0], "Variance", "l");

                // set pads
                set_pad(*pads[0], 0, 1, 0);
                set_pad(*pads[1], 0, 1, 0);

                // plot histograms
                pads[0]->cd();

                gPad->SetTicks();
                gPad->SetLogy();

                (*hist_aa_mse)[3]->Draw();
                (*hist_aa_bias)[3]->Draw("same");
                (*hist_aa_variance)[3]->Draw("same");

                legend_part1->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.31, "Source: Jewel pp");
                latex.DrawLatex(0.18, 0.26, "Algorithm: D'Agostini");
                latex.DrawLatex(0.18, 0.21, "Prior: Flat");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(21);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.5, 0.18,("Iteration = " + std::to_string((*hist_aa_mse)[3]->GetMinimumBin())).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "PbPb 0-10%");

                pads[1]->cd();
                
                gPad->SetTicks();
                gPad->SetLogy();

                (*hist_pp_mse)[0]->Draw();
                (*hist_pp_bias)[0]->Draw("same");
                (*hist_pp_variance)[0]->Draw("same");

                legend_part2->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.31, "Source: Jewel pp");
                latex.DrawLatex(0.18, 0.26, "Algorithm: D'Agostini");
                latex.DrawLatex(0.18, 0.21, "Prior: Flat");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(21);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.5, 0.18,("Iteration = " + std::to_string((*hist_pp_mse)[0]->GetMinimumBin())).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_pp).c_str());
                latex.DrawLatex(0.18, 0.74, "pp");

                // x axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(0);
                latex.DrawLatex(pad_x0 * 1 + pad_dx * 0.5 + pad_x1 * 0, pad_y0 * 0.5, "Iteration");
                latex.DrawLatex(pad_x0 * 2 + pad_dx * 1.5 + pad_x1 * 1, pad_y0 * 0.5, "Iteration");

                // y axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(90);
                latex.DrawLatex(pad_x0 * 0.4 + pad_dx * 0 + pad_x1 * 0, pad_y0 + pad_dy * 0.5, "Bin Averaged Value");
                latex.DrawLatex(pad_x0 * 1.4 + pad_dx * 1 + pad_x1 * 1, pad_y0 + pad_dy * 0.5, "Bin Averaged Value");
                break;
            }
            case 7: // mse, jewel, svd, mc
            {
                // get histograms
                auto file_aa = new TFile("data/arc/pho_60_rebin1/regularization_aa_svd_jewel_pp_mc.root", "read");
                auto file_pp = new TFile("data/arc/pho_60_rebin1/regularization_pp_svd_jewel_pp_mc.root", "read");

                auto hist_aa_mse = new history<TH1F>(file_aa, "aa_mse"s);
                auto hist_pp_mse = new history<TH1F>(file_pp, "pp_mse"s);

                auto hist_aa_bias = new history<TH1F>(file_aa, "aa_bias"s);
                auto hist_pp_bias = new history<TH1F>(file_pp, "pp_bias"s);

                auto hist_aa_variance = new history<TH1F>(file_aa, "aa_variance"s);
                auto hist_pp_variance = new history<TH1F>(file_pp, "pp_variance"s);

                (*hist_aa_mse)[3]->GetYaxis()->SetTitle("");
                (*hist_pp_mse)[0]->GetYaxis()->SetTitle("");
                (*hist_aa_mse)[3]->GetXaxis()->SetTitle("");
                (*hist_pp_mse)[0]->GetXaxis()->SetTitle("");

                (*hist_aa_mse)[3]->SetMinimum(1E-10);
                (*hist_pp_mse)[0]->SetMinimum(1E-10);
                (*hist_aa_mse)[3]->SetMaximum(1E-2);
                (*hist_pp_mse)[0]->SetMaximum(1E-2);

                set_format((*hist_aa_mse)[3], 0);
                set_format((*hist_pp_mse)[0], 0);
                set_format((*hist_aa_bias)[3], 1);
                set_format((*hist_pp_bias)[0], 1);
                set_format((*hist_aa_variance)[3], 2);
                set_format((*hist_pp_variance)[0], 2);

                // set legends
                auto legend_part1 = new TLegend(0.6, 0.2, 0.8, 0.34);
                legend_part1->SetTextFont(42);
                legend_part1->SetTextSize(legend_size);
                legend_part1->SetFillStyle(0);
                legend_part1->SetBorderSize(0);
                legend_part1->AddEntry((*hist_aa_mse)[3], "MSE", "l");
                legend_part1->AddEntry((*hist_aa_bias)[3], "Bias^{2}", "l");
                legend_part1->AddEntry((*hist_aa_variance)[3], "Variance", "l");

                auto legend_part2 = new TLegend(0.6, 0.2, 0.8, 0.34);
                legend_part2->SetTextFont(42);
                legend_part2->SetTextSize(legend_size);
                legend_part2->SetFillStyle(0);
                legend_part2->SetBorderSize(0);
                legend_part2->AddEntry((*hist_pp_mse)[0], "MSE", "l");
                legend_part2->AddEntry((*hist_pp_bias)[0], "Bias^{2}", "l");
                legend_part2->AddEntry((*hist_pp_variance)[0], "Variance", "l");

                // set pads
                set_pad(*pads[0], 0, 1, 0);
                set_pad(*pads[1], 0, 1, 0);

                // plot histograms
                pads[0]->cd();

                gPad->SetTicks();
                gPad->SetLogy();

                (*hist_aa_mse)[3]->Draw();
                (*hist_aa_bias)[3]->Draw("same");
                (*hist_aa_variance)[3]->Draw("same");

                legend_part1->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.31, "Source: Jewel pp");
                latex.DrawLatex(0.18, 0.26, "Algorithm: SVD");
                latex.DrawLatex(0.18, 0.21, "Prior: MC");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(21);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.5, 0.18,("k_{reg} = " + std::to_string((*hist_aa_mse)[3]->GetMinimumBin())).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "PbPb 0-10%");

                pads[1]->cd();
                
                gPad->SetTicks();
                gPad->SetLogy();

                (*hist_pp_mse)[0]->Draw();
                (*hist_pp_bias)[0]->Draw("same");
                (*hist_pp_variance)[0]->Draw("same");

                legend_part2->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.31, "Source: Jewel pp");
                latex.DrawLatex(0.18, 0.26, "Algorithm: SVD");
                latex.DrawLatex(0.18, 0.21, "Prior: MC");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(21);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.5, 0.18,("k_{reg} = " + std::to_string((*hist_pp_mse)[0]->GetMinimumBin())).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_pp).c_str());
                latex.DrawLatex(0.18, 0.74, "pp");

                // x axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(0);
                latex.DrawLatex(pad_x0 * 1 + pad_dx * 0.5 + pad_x1 * 0, pad_y0 * 0.5, "k_{reg}");
                latex.DrawLatex(pad_x0 * 2 + pad_dx * 1.5 + pad_x1 * 1, pad_y0 * 0.5, "k_{reg}");

                // y axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(90);
                latex.DrawLatex(pad_x0 * 0.4 + pad_dx * 0 + pad_x1 * 0, pad_y0 + pad_dy * 0.5, "Bin Averaged Value");
                latex.DrawLatex(pad_x0 * 1.4 + pad_dx * 1 + pad_x1 * 1, pad_y0 + pad_dy * 0.5, "Bin Averaged Value");
                break;
            }
            case 8: // mse, jewel, svd, flat
            {
                // get histograms
                auto file_aa = new TFile("data/arc/pho_60_rebin1/regularization_aa_svd_jewel_pp_flat.root", "read");
                auto file_pp = new TFile("data/arc/pho_60_rebin1/regularization_pp_svd_jewel_pp_flat.root", "read");

                auto hist_aa_mse = new history<TH1F>(file_aa, "aa_mse"s);
                auto hist_pp_mse = new history<TH1F>(file_pp, "pp_mse"s);

                auto hist_aa_bias = new history<TH1F>(file_aa, "aa_bias"s);
                auto hist_pp_bias = new history<TH1F>(file_pp, "pp_bias"s);

                auto hist_aa_variance = new history<TH1F>(file_aa, "aa_variance"s);
                auto hist_pp_variance = new history<TH1F>(file_pp, "pp_variance"s);

                (*hist_aa_mse)[3]->GetYaxis()->SetTitle("");
                (*hist_pp_mse)[0]->GetYaxis()->SetTitle("");
                (*hist_aa_mse)[3]->GetXaxis()->SetTitle("");
                (*hist_pp_mse)[0]->GetXaxis()->SetTitle("");

                (*hist_aa_mse)[3]->SetMinimum(1E-10);
                (*hist_pp_mse)[0]->SetMinimum(1E-10);
                (*hist_aa_mse)[3]->SetMaximum(1E-2);
                (*hist_pp_mse)[0]->SetMaximum(1E-2);

                set_format((*hist_aa_mse)[3], 0);
                set_format((*hist_pp_mse)[0], 0);
                set_format((*hist_aa_bias)[3], 1);
                set_format((*hist_pp_bias)[0], 1);
                set_format((*hist_aa_variance)[3], 2);
                set_format((*hist_pp_variance)[0], 2);

                // set legends
                auto legend_part1 = new TLegend(0.6, 0.2, 0.8, 0.34);
                legend_part1->SetTextFont(42);
                legend_part1->SetTextSize(legend_size);
                legend_part1->SetFillStyle(0);
                legend_part1->SetBorderSize(0);
                legend_part1->AddEntry((*hist_aa_mse)[3], "MSE", "l");
                legend_part1->AddEntry((*hist_aa_bias)[3], "Bias^{2}", "l");
                legend_part1->AddEntry((*hist_aa_variance)[3], "Variance", "l");

                auto legend_part2 = new TLegend(0.6, 0.2, 0.8, 0.34);
                legend_part2->SetTextFont(42);
                legend_part2->SetTextSize(legend_size);
                legend_part2->SetFillStyle(0);
                legend_part2->SetBorderSize(0);
                legend_part2->AddEntry((*hist_pp_mse)[0], "MSE", "l");
                legend_part2->AddEntry((*hist_pp_bias)[0], "Bias^{2}", "l");
                legend_part2->AddEntry((*hist_pp_variance)[0], "Variance", "l");

                // set pads
                set_pad(*pads[0], 0, 1, 0);
                set_pad(*pads[1], 0, 1, 0);

                // plot histograms
                pads[0]->cd();

                gPad->SetTicks();
                gPad->SetLogy();

                (*hist_aa_mse)[3]->Draw();
                (*hist_aa_bias)[3]->Draw("same");
                (*hist_aa_variance)[3]->Draw("same");

                legend_part1->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.31, "Source: Jewel pp");
                latex.DrawLatex(0.18, 0.26, "Algorithm: SVD");
                latex.DrawLatex(0.18, 0.21, "Prior: Flat");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(21);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.5, 0.18,("k_{reg} = " + std::to_string((*hist_aa_mse)[3]->GetMinimumBin())).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "PbPb 0-10%");

                pads[1]->cd();
                
                gPad->SetTicks();
                gPad->SetLogy();

                (*hist_pp_mse)[0]->Draw();
                (*hist_pp_bias)[0]->Draw("same");
                (*hist_pp_variance)[0]->Draw("same");

                legend_part2->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.31, "Source: Jewel pp");
                latex.DrawLatex(0.18, 0.26, "Algorithm: SVD");
                latex.DrawLatex(0.18, 0.21, "Prior: Flat");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(21);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.5, 0.18,("k_{reg} = " + std::to_string((*hist_pp_mse)[0]->GetMinimumBin())).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_pp).c_str());
                latex.DrawLatex(0.18, 0.74, "pp");

                // x axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(0);
                latex.DrawLatex(pad_x0 * 1 + pad_dx * 0.5 + pad_x1 * 0, pad_y0 * 0.5, "k_{reg}");
                latex.DrawLatex(pad_x0 * 2 + pad_dx * 1.5 + pad_x1 * 1, pad_y0 * 0.5, "k_{reg}");

                // y axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(90);
                latex.DrawLatex(pad_x0 * 0.4 + pad_dx * 0 + pad_x1 * 0, pad_y0 + pad_dy * 0.5, "Bin Averaged Value");
                latex.DrawLatex(pad_x0 * 1.4 + pad_dx * 1 + pad_x1 * 1, pad_y0 + pad_dy * 0.5, "Bin Averaged Value");
                break;
            }
            case 9: // bottom line test
            {
                // get histograms
                auto file_aa = new TFile("data/arc/pho_60_rebin1/bottom_line_test_aa_jewel_pp.root", "read");
                auto file_pp = new TFile("data/arc/pho_60_rebin1/bottom_line_test_pp_jewel_pp.root", "read");

                auto hist_aa_before = new history<TH1F>(file_aa, "chi2_before_dj"s);
                auto hist_pp_before = new history<TH1F>(file_pp, "chi2_before_dj"s);

                auto hist_aa_after = new history<TH1F>(file_aa, "chi2_after_dj"s);
                auto hist_pp_after = new history<TH1F>(file_pp, "chi2_after_dj"s);

                (*hist_aa_before)[3]->GetYaxis()->SetTitle("");
                (*hist_pp_before)[0]->GetYaxis()->SetTitle("");
                (*hist_aa_before)[3]->GetXaxis()->SetTitle("");
                (*hist_pp_before)[0]->GetXaxis()->SetTitle("");

                (*hist_aa_before)[3]->SetMinimum(1E3);
                (*hist_pp_before)[0]->SetMinimum(1E4);
                (*hist_aa_before)[3]->SetMaximum(1E4);
                (*hist_pp_before)[0]->SetMaximum(1E5);

                set_format((*hist_aa_before)[3], 0);
                set_format((*hist_pp_before)[0], 0);
                set_format((*hist_aa_after)[3], 1);
                set_format((*hist_pp_after)[0], 1);

                // set legends
                auto legend_part1 = new TLegend(0.6, 0.25, 0.8, 0.34);
                legend_part1->SetTextFont(42);
                legend_part1->SetTextSize(legend_size);
                legend_part1->SetFillStyle(0);
                legend_part1->SetBorderSize(0);
                legend_part1->AddEntry((*hist_aa_before)[3], "#chi^{2}_{smeared}", "l");
                legend_part1->AddEntry((*hist_aa_after)[3], "#chi^{2}_{unfolded}", "l");

                auto legend_part2 = new TLegend(0.6, 0.25, 0.8, 0.34);
                legend_part2->SetTextFont(42);
                legend_part2->SetTextSize(legend_size);
                legend_part2->SetFillStyle(0);
                legend_part2->SetBorderSize(0);
                legend_part2->AddEntry((*hist_pp_before)[0], "#chi^{2}_{smeared}", "l");
                legend_part2->AddEntry((*hist_pp_after)[0], "#chi^{2}_{unfolded}", "l");

                // set pads
                set_pad(*pads[0], 0, 1, 0);
                set_pad(*pads[1], 0, 1, 0);

                // plot histograms
                pads[0]->cd();

                gPad->SetTicks();
                gPad->SetLogy();

                (*hist_aa_before)[3]->Draw("p");
                (*hist_aa_after)[3]->Draw("same p");

                legend_part1->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.31, "Source: Jewel pp");
                latex.DrawLatex(0.18, 0.26, "Algorithm: D'Agostini");
                latex.DrawLatex(0.18, 0.21, "Prior: MC");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.64, (text_photon_pt + ", " + text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.18, 0.58, (text_jet_alg + ", " + text_jet_pt + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_aa).c_str());
                latex.DrawLatex(0.18, 0.74, "PbPb 0-10%");

                pads[1]->cd();
                
                gPad->SetTicks();
                gPad->SetLogy();

                (*hist_pp_before)[0]->Draw("p");
                (*hist_pp_after)[0]->Draw("same p");

                legend_part2->Draw("same");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.31, "Source: Jewel pp");
                latex.DrawLatex(0.18, 0.26, "Algorithm: D'Agostini");
                latex.DrawLatex(0.18, 0.21, "Prior: MC");

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.64, (text_photon_pt + ", " + text_photon_eta + ", " + text_dphi).c_str());
                latex.DrawLatex(0.18, 0.58, (text_jet_alg + ", " + text_jet_pt + ", " + text_jet_eta).c_str());

                latex.SetTextFont(42);
                latex.SetTextSize(text_size);
                latex.SetTextAlign(11);
                latex.SetTextAngle(0);
                latex.DrawLatex(0.18, 0.80, (text_pp).c_str());
                latex.DrawLatex(0.18, 0.74, "pp");

                // x axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(0);
                latex.DrawLatex(pad_x0 * 1 + pad_dx * 0.5 + pad_x1 * 0, pad_y0 * 0.5, "k_{reg}");
                latex.DrawLatex(pad_x0 * 2 + pad_dx * 1.5 + pad_x1 * 1, pad_y0 * 0.5, "k_{reg}");

                // y axis label
                canvas.cd();
                latex.SetTextFont(42);
                latex.SetTextSize(axis_label_size);
                latex.SetTextAlign(22);
                latex.SetTextAngle(90);
                latex.DrawLatex(pad_x0 * 0.4 + pad_dx * 0 + pad_x1 * 0, pad_y0 + pad_dy * 0.5, "");
                latex.DrawLatex(pad_x0 * 1.4 + pad_dx * 1 + pad_x1 * 1, pad_y0 + pad_dy * 0.5, "");
                break;
            }
        }

        canvas.cd();

        latex.SetTextFont(62);
        latex.SetTextSize(0.05/sf);
        latex.SetTextAlign(11);
        latex.SetTextAngle(0);
        latex.DrawLatex(pad_x0, pad_y0 * 1 + pad_dy * 1 + pad_y1 * 0.2, text_cms.c_str());

        latex.SetTextFont(42);
        latex.SetTextSize(0.045/sf);
        latex.SetTextAlign(31);
        latex.SetTextAngle(0);
        latex.DrawLatex(pad_x0 * 2 + pad_dx * 2 + pad_x1 * 1, pad_y0 * 1 + pad_dy * 1 + pad_y1 * 0.2, text_system.c_str());

        canvas.SaveAs(("unfolding_" + figures[i] + ".pdf").c_str());
    }

    return 0;
}

int main() {
    return plot();
}