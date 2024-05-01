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

void set_values(history<TH1F>* h, history<TH1F>* s, history<TH1F>* h_aa, history<TH1F>* s_aa, history<TH1F>* h_pp, history<TH1F>* s_pp)
{
    for (int i = 0; i < h->size(); ++i) {
        (*s_aa)[i]->Scale(1/(*h_aa)[i]->Integral("width"));
        (*h_aa)[i]->Scale(1/(*h_aa)[i]->Integral("width"));
        (*s_pp)[0]->Scale(1/(*h_pp)[0]->Integral("width"));
        (*h_pp)[0]->Scale(1/(*h_pp)[0]->Integral("width"));

        for (int j = 1; j <= (*h)[i]->GetNbinsX(); ++j) {
            double aa_val = (*h_aa)[i]->GetBinContent(j);
            double aa_stat_err = (*h_aa)[i]->GetBinError(j);
            double aa_syst_err = (*s_aa)[i]->GetBinError(j);
            auto aa_stat_err_scale = aa_stat_err/aa_val;
            auto aa_syst_err_scale = aa_syst_err/aa_val;

            double pp_val = (*h_pp)[0]->GetBinContent(j);
            double pp_stat_err = (*h_pp)[0]->GetBinError(j);
            double pp_syst_err = (*s_pp)[0]->GetBinError(j);
            auto pp_stat_err_scale = pp_stat_err/pp_val;
            auto pp_syst_err_scale = pp_syst_err/pp_val;

            auto ratio = aa_val / pp_val;

            auto stat_err = ratio * std::sqrt(aa_stat_err_scale * aa_stat_err_scale + pp_stat_err_scale * pp_stat_err_scale);
            auto syst_err = ratio * std::sqrt(aa_syst_err_scale * aa_syst_err_scale + pp_syst_err_scale * pp_syst_err_scale);

            (*h)[i]->SetBinContent(j, ratio);
            (*h)[i]->SetBinError(j, stat_err);
            (*s)[i]->SetBinContent(j, ratio);
            (*s)[i]->SetBinError(j, syst_err);
        }
    }
}

void set_values(history<TH1F>* h, history<TH1F>* h_aa, history<TH1F>* h_pp)
{
    for (int i = 0; i < h->size(); ++i) {
        (*h_aa)[i]->Scale(1/(*h_aa)[i]->Integral("width"));
        (*h_pp)[0]->Scale(1/(*h_pp)[0]->Integral("width"));

        for (int j = 1; j <= (*h)[i]->GetNbinsX(); ++j) {
            double aa_val = (*h_aa)[i]->GetBinContent(j);
            double aa_stat_err = (*h_aa)[i]->GetBinError(j);
            auto aa_stat_err_scale = aa_stat_err/aa_val;

            double pp_val = (*h_pp)[0]->GetBinContent(j);
            double pp_stat_err = (*h_pp)[0]->GetBinError(j);
            auto pp_stat_err_scale = pp_stat_err/pp_val;

            auto ratio = aa_val / pp_val;

            auto stat_err = ratio * std::sqrt(aa_stat_err_scale * aa_stat_err_scale + pp_stat_err_scale * pp_stat_err_scale);

            (*h)[i]->SetBinContent(j, ratio);
            (*h)[i]->SetBinError(j, stat_err);
        }
    }
}

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

void format(history<TH1F>* h, history<TH1F>* s, int system)
{
    static int style[3] = {20, 20, 20};
    static int color[3] = {TColor::GetColor("#5790FC"), TColor::GetColor("#E42536"), TColor::GetColor("#9C9C9C")};

    for (int i = 0; i < h->size(); ++i) {
        (*h)[i]->SetMarkerStyle(style[system]);
        (*h)[i]->SetMarkerColor(1);
        (*h)[i]->SetLineColor(1);
        (*h)[i]->SetFillColor(color[system]);
        (*h)[i]->SetMarkerSize(1.5);
        (*h)[i]->SetLineWidth(1.0);

        (*s)[i]->SetMarkerStyle(style[system]);
        (*s)[i]->SetMarkerColor(1);
        (*s)[i]->SetLineColor(1);
        (*s)[i]->SetFillColor(color[system]);
        (*s)[i]->SetFillColorAlpha(color[system], 0.60);
        (*s)[i]->SetMarkerSize(1.5);
        (*s)[i]->SetLineWidth(1.0);
    }
}

std::vector<TGraphAsymmErrors> get_graph(std::vector<history<TH1F>*> h, int format, int system)
{
    std::vector<TGraphAsymmErrors> result(h.size());

    static int style[3] = {20, 20, 20};
    static int color[3] = {TColor::GetColor("#5790FC"), TColor::GetColor("#E42536"), TColor::GetColor("#9C9C9C")};

    for (size_t i = 0; i < h.size(); ++i) {
        if (system == 2 || system == 0) {
            for (int j = 1; j <= (*h[i])[3]->GetNbinsX(); ++j) {
                double x = (*h[i])[3]->GetBinCenter(j);
                double dx = (*h[i])[3]->GetBinWidth(j);

                result[i].SetPoint(j - 1, x, (*h[i])[3]->GetBinContent(j));
                result[i].SetPointError(j - 1, dx, dx, (*h[i])[3]->GetBinError(j), (*h[i])[3]->GetBinError(j));
            }
        }
        else {
            for (int j = 1; j <= (*h[i])[0]->GetNbinsX(); ++j) {
                double x = (*h[i])[0]->GetBinCenter(j);
                double dx = (*h[i])[0]->GetBinWidth(j);

                result[i].SetPoint(j - 1, x, (*h[i])[0]->GetBinContent(j));
                result[i].SetPointError(j - 1, dx, dx, (*h[i])[0]->GetBinError(j), (*h[i])[0]->GetBinError(j));
            }
        }

        result[i].SetMarkerStyle(style[format]);
        result[i].SetMarkerColor(1);
        result[i].SetLineColor(1);
        result[i].SetFillColorAlpha(color[format], 0.60);
        result[i].SetMarkerSize(1.5);
        result[i].SetLineWidth(1.0);
    }

    return result;
}

std::vector<TGraphAsymmErrors> get_graph(std::vector<history<TH1F>*> h, int type)
{
    std::vector<TGraphAsymmErrors> result(h.size());

    static int style[8] = {20, 20, 20, 20, 20, 20, 20, 20};
    static int color[8] = {TColor::GetColor("#4492D8"), 
                           TColor::GetColor("#95DADD"), 
                           TColor::GetColor("#E5631D"),
                           TColor::GetColor("#BB2112"),
                           TColor::GetColor("#FDA82D"),
                           TColor::GetColor("#B9AB74"),
                           TColor::GetColor("#8235B4"),
                           TColor::GetColor("#A86B5B")
                           };

    for (size_t i = 0; i < h.size(); ++i) {
        for (int j = 1; j <= (*h[i])[0]->GetNbinsX(); ++j) {
            double x = (*h[i])[0]->GetBinCenter(j);
            double dx = (*h[i])[0]->GetBinWidth(j);

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

int congratulate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input_aa = conf->get<std::string>("input_aa");
    auto input_pp = conf->get<std::string>("input_pp");

    auto input_aa_jewel = conf->get<std::string>("input_aa_jewel");
    auto input_aa_jewel_no_recoil = conf->get<std::string>("input_aa_jewel_no_recoil");
    auto input_aa_pyquen = conf->get<std::string>("input_aa_pyquen");
    auto input_aa_pyquen_no_wide = conf->get<std::string>("input_aa_pyquen_no_wide");
    auto input_pp_jewel = conf->get<std::string>("input_pp_jewel");
    auto input_pp_pyquen = conf->get<std::string>("input_pp_pyquen");

    auto log = conf->get<bool>("log");

    auto tag_aa_jewel = conf->get<std::string>("tag_aa_jewel");
    auto tag_aa_jewel_no_recoil = conf->get<std::string>("tag_aa_jewel_no_recoil");
    auto tag_aa_pyquen = conf->get<std::string>("tag_aa_pyquen");
    auto tag_aa_pyquen_no_wide = conf->get<std::string>("tag_aa_pyquen_no_wide");
    auto tag_pp_jewel = conf->get<std::string>("tag_pp_jewel");
    auto tag_pp_pyquen = conf->get<std::string>("tag_pp_pyquen");

    auto figures = conf->get<std::vector<std::string>>("figures");
    auto theory_figures = conf->get<std::vector<std::string>>("theory_figures");

    auto name = conf->get<std::string>("name");
    auto types = conf->get<std::vector<int64_t>>("types");

    auto ymins = conf->get<std::vector<float>>("ymins");
    auto ymaxs = conf->get<std::vector<float>>("ymaxs");

    auto subsets = conf->get<bool>("subsets");
    // auto whole = conf->get<bool>("whole");
    auto system = conf->get<int64_t>("system");

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
    auto file_aa = new TFile((base + input_aa).data(), "read");
    auto file_pp = new TFile((base + input_pp).data(), "read");

    auto file_aa_jewel = new TFile((base + input_aa_jewel).data(), "read");
    auto file_aa_jewel_no_recoil = new TFile((base + input_aa_jewel_no_recoil).data(), "read");
    auto file_aa_pyquen = new TFile((base + input_aa_pyquen).data(), "read");
    auto file_aa_pyquen_no_wide = new TFile((base + input_aa_pyquen_no_wide).data(), "read");
    auto file_pp_jewel = new TFile((base + input_pp_jewel).data(), "read");
    auto file_pp_pyquen = new TFile((base + input_pp_pyquen).data(), "read");

    /* define kinematics and luminosity */
    auto text_system = "(5.02 TeV)"s;
    auto text_cms = "CMS"s;
    auto text_photon_pt = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV"s;
    auto text_photon_eta = "|#eta^{#gamma}| < "s + to_text(photon_eta_abs);
    auto text_dphi = "#Delta#phi_{j#gamma} > #frac{"s + to_text(dphi_min_numerator) + "#pi}{"s + to_text(dphi_min_denominator) + "}"s;
    auto text_jet_alg = "anti-k_{T} R = 0.3"s;
    auto text_jet_eta = "|#eta^{jet}| < "s + to_text(jet_eta_abs);

    if (system == 0) text_system = "PbPb 1.69 nb^{-1} " + text_system;
    if (system == 1) text_system = "pp 302 pb^{-1} " + text_system;
    if (system == 2) text_system = "PbPb 1.69 nb^{-1}, pp 302 pb^{-1} " + text_system;

    std::vector<history<TH1F>*> hists_aa(ncols);
    std::vector<history<TH1F>*> systs_aa(ncols);
    std::vector<history<TH1F>*> hists_pp(ncols);
    std::vector<history<TH1F>*> systs_pp(ncols);
    std::vector<history<TH1F>*> hists_ratio(ncols);
    std::vector<history<TH1F>*> systs_ratio(ncols);

    std::vector<history<TH1F>*> hists_aa_jewel(ncols);
    std::vector<history<TH1F>*> hists_aa_jewel_no_recoil(ncols);
    std::vector<history<TH1F>*> hists_aa_pyquen(ncols);
    std::vector<history<TH1F>*> hists_aa_pyquen_no_wide(ncols);
    std::vector<history<TH1F>*> hists_pp_jewel(ncols);
    std::vector<history<TH1F>*> hists_pp_pyquen(ncols);
    std::vector<history<TH1F>*> hists_ratio_jewel(ncols);
    std::vector<history<TH1F>*> hists_ratio_jewel_no_recoil(ncols);
    std::vector<history<TH1F>*> hists_ratio_pyquen(ncols);
    std::vector<history<TH1F>*> hists_ratio_pyquen_no_wide(ncols);

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

        /* get histograms */
        hists_aa[i] = new history<TH1F>(file_aa, "aa_base_aa_nominal_s_pure_raw_sub_" + figures[i]);
        systs_aa[i] = new history<TH1F>(file_aa, "aa_total_base_aa_nominal_s_pure_raw_sub_" + figures[i]);
        hists_pp[i] = new history<TH1F>(file_pp, "pp_base_pp_nominal_s_pure_raw_sub_" + figures[i]);
        systs_pp[i] = new history<TH1F>(file_pp, "pp_total_base_pp_nominal_s_pure_raw_sub_" + figures[i]);
        hists_ratio[i] = new history<TH1F>(*hists_aa[i], "hist");
        systs_ratio[i] = new history<TH1F>(*systs_aa[i], "syst");

        hists_aa_jewel[i] = new history<TH1F>(file_aa_jewel, tag_aa_jewel + "_" + theory_figures[i]);
        hists_aa_jewel_no_recoil[i] = new history<TH1F>(file_aa_jewel_no_recoil, tag_aa_jewel_no_recoil + "_" + theory_figures[i]);
        hists_aa_pyquen[i] = new history<TH1F>(file_aa_pyquen, tag_aa_pyquen + "_" + theory_figures[i]);
        hists_aa_pyquen_no_wide[i] = new history<TH1F>(file_aa_pyquen_no_wide, tag_aa_pyquen_no_wide + "_" + theory_figures[i]);
        hists_pp_jewel[i] = new history<TH1F>(file_pp_jewel, tag_pp_jewel + "_" + theory_figures[i]);
        hists_pp_pyquen[i] = new history<TH1F>(file_pp_pyquen, tag_pp_pyquen + "_" + theory_figures[i]);
        hists_ratio_jewel[i] = new history<TH1F>(*hists_aa_jewel[i], "hist_jewel");
        hists_ratio_jewel_no_recoil[i] = new history<TH1F>(*hists_aa_jewel_no_recoil[i], "hist_jewel_no_recoil");
        hists_ratio_pyquen[i] = new history<TH1F>(*hists_aa_pyquen[i], "hist_pyquen");
        hists_ratio_pyquen_no_wide[i] = new history<TH1F>(*hists_aa_pyquen_no_wide[i], "hist_pyquen_no_wide");

        set_systematics(hists_aa[i], systs_aa[i]);
        set_systematics(hists_pp[i], systs_pp[i]);
        
        if (system == 2) {
            set_values(hists_ratio[i], systs_ratio[i], hists_aa[i], systs_aa[i], hists_pp[i], systs_pp[i]);
            set_values(hists_ratio_jewel[i], hists_aa_jewel[i], hists_pp_jewel[i]);
            set_values(hists_ratio_jewel_no_recoil[i], hists_aa_jewel_no_recoil[i], hists_pp_jewel[i]);
            set_values(hists_ratio_pyquen[i], hists_aa_pyquen[i], hists_pp_pyquen[i]);
            set_values(hists_ratio_pyquen_no_wide[i], hists_aa_pyquen_no_wide[i], hists_pp_pyquen[i]);
        }

        // format(hists_aa[i], systs_aa[i], 2);
        // format(hists_pp[i], systs_pp[i], 2);
        // format(hists_ratio[i], systs_ratio[i], 2);
    }

    auto graphs_hists_aa = get_graph(hists_aa, 2, 0);
    auto graphs_systs_aa = get_graph(systs_aa, 2, 0);
    auto graphs_hists_pp = get_graph(hists_pp, 2, 1);
    auto graphs_systs_pp = get_graph(systs_pp, 2, 1);
    auto graphs_hists_ratio = get_graph(hists_ratio, 2, 2);
    auto graphs_systs_ratio = get_graph(systs_ratio, 2, 2);

    auto graphs_hists_aa_jewel = get_graph(hists_aa_jewel, 0);
    auto graphs_hists_aa_jewel_no_recoil = get_graph(hists_aa_jewel_no_recoil, 1);
    auto graphs_hists_aa_pyquen = get_graph(hists_aa_pyquen, 2);
    auto graphs_hists_aa_pyquen_no_wide = get_graph(hists_aa_pyquen_no_wide, 3);
    auto graphs_hists_pp_jewel = get_graph(hists_pp_jewel, 0);
    auto graphs_hists_pp_pyquen = get_graph(hists_pp_pyquen, 2);
    auto graphs_hists_ratio_jewel = get_graph(hists_ratio_jewel, 0);
    auto graphs_hists_ratio_jewel_no_recoil = get_graph(hists_ratio_jewel_no_recoil, 1);
    auto graphs_hists_ratio_pyquen = get_graph(hists_ratio_pyquen, 2);
    auto graphs_hists_ratio_pyquen_no_wide = get_graph(hists_ratio_pyquen_no_wide, 3);

    /* size canvas */
    double panel_size = 500;
    double padding_width_left = 140;
    double padding_width_right = 50;
    double padding_height = 130;

    double canvas_width = panel_size * ncols + padding_width_left + padding_width_right;
    double canvas_height = panel_size * 1 + padding_height * 2;

    double sf = 1;

    double pad_x0 = padding_width_left / canvas_width;
    double pad_y0 = padding_height / canvas_height;
    double pad_dx = panel_size / canvas_width;
    double pad_dy = panel_size / canvas_height;

    double xmin = (log) ? bdr[0] + 0.003 : bdr[0];
    double xmax = (log) ? bdr[1] : bdr[1] * 0.999;

    /* declare canvas, pads, axes, and titles */
    TCanvas canvas("canvas", "", canvas_width, canvas_height);

    TLatex latex;
    latex.SetNDC();

    std::vector<TH2F*> worlds(ncols);
    std::vector<TPad*> pads(ncols);
    std::vector<TGaxis*> axis_x(ncols);
    std::vector<TGaxis*> axis_y(1);

    for (int i = 0; i < ncols; ++i) {
        worlds[i] = new TH2F("world", ";;", 100, xmin, xmax, 100, ymins[i], ymaxs[i]);
        worlds[i]->SetStats(0);

        pads[i] = new TPad("P1", "", pad_x0 + pad_dx * i, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * (i + 1), pad_y0 + pad_dy * 1, 0);
        
        set_pad(*pads[i], log);

        if (log)    axis_x[i] = new TGaxis(pad_x0 + pad_dx * i, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * (i + 1), pad_y0 + pad_dy * 0, xmin, xmax, 510, "SG");
        if (!log)   axis_x[i] = new TGaxis(pad_x0 + pad_dx * i, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * (i + 1), pad_y0 + pad_dy * 0, xmin, xmax, 510, "S");

        set_axis(*axis_x[i], sf);

        canvas.cd();

        latex.SetTextFont(42);
        latex.SetTextSize(0.05/sf);
        latex.SetTextAlign(22);
        latex.SetTextAngle(0);
        latex.DrawLatex(pad_x0 + pad_dx * (i + 0.5), pad_y0 * 0.5, "#Deltaj");
    }

    axis_y[0] = new TGaxis(pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 1, ymins[0], ymaxs[0] * 0.999, 510, "S");

    set_axis(*axis_y[0], sf);

    latex.SetTextFont(42);
    latex.SetTextSize(0.05/sf);
    latex.SetTextAlign(22);
    latex.SetTextAngle(90);
    if (system == 2)    latex.DrawLatex(pad_x0 * 0.4, pad_y0 + pad_dy * 0.5, "PbPb / pp");
    if (system < 2)     latex.DrawLatex(pad_x0 * 0.4, pad_y0 + pad_dy * 0.5, "#frac{1}{N_{#gamma}} #frac{dN_{j#gamma}}{d#Deltaj}");

    latex.SetTextFont(62);
    latex.SetTextSize(0.05/sf);
    latex.SetTextAlign(11);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0, pad_y0 * 1.15 + pad_dy, text_cms.c_str());

    latex.SetTextFont(42);
    latex.SetTextSize(0.035/sf);
    latex.SetTextAlign(31);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0 + pad_dx * ncols, pad_y0 * 1.15 + pad_dy, text_system.c_str());

    TGraph line;
    if (system == 2)    line.SetPoint(0, xmin, 1);
    if (system == 2)    line.SetPoint(1, xmax, 1);
    if (system < 2)     line.SetPoint(0, xmin, 0);
    if (system < 2)     line.SetPoint(1, xmax, 0);
    line.SetLineStyle(kDashed);

    /* declare legend */
    double legend_y_min = 0;
    double legend_y_max = 0;
    double legend_x_min = 0;
    double legend_x_max = 0;

    if (system == 2)    legend_y_min = (subsets) ? 0.48 : 0.48;
    if (system == 2)    legend_y_max = (subsets) ? 0.83 : 0.83;
    if (system == 2)    legend_x_min = (subsets) ? 0.05 : 0.05;
    if (system == 2)    legend_x_max = (subsets) ? 0.35 : 0.35;

    if (system == 1)    legend_y_min = (subsets) ? 0.64 : 0.24;
    if (system == 1)    legend_y_max = (subsets) ? 0.85 : 0.45;
    if (system == 1)    legend_x_min = (subsets) ? 0.65 : 0.65;
    if (system == 1)    legend_x_max = (subsets) ? 0.95 : 0.95;

    if (system == 0)    legend_y_min = (subsets) ? 0.40 : 0.15;
    if (system == 0)    legend_y_max = (subsets) ? 0.75 : 0.5;
    if (system == 0)    legend_x_min = (subsets) ? 0.4 : 0.4;
    if (system == 0)    legend_x_max = (subsets) ? 0.7 : 0.7;

    TLegend legend(legend_x_min, legend_y_min, legend_x_max, legend_y_max);
    legend.SetTextFont(42);
    legend.SetTextSize(0.05);
    legend.SetFillStyle(0);
    legend.SetBorderSize(0);
    if (system == 2)    legend.AddEntry(&graphs_systs_ratio[0], "CMS data", "plf");
    if (system == 2)    legend.AddEntry(&graphs_hists_ratio_jewel[0], "JEWEL, recoil", "lf");
    if (system == 2)    legend.AddEntry(&graphs_hists_ratio_jewel_no_recoil[0], "JEWEL, no recoil", "lf");
    if (system == 2)    legend.AddEntry(&graphs_hists_ratio_pyquen_no_wide[0], "PYQUEN", "lf");
    if (system == 2)    legend.AddEntry(&graphs_hists_ratio_pyquen[0], "PYQUEN, wide angle rad.", "lf");
    if (system == 0)    legend.AddEntry(&graphs_systs_aa[0], "CMS data", "plf");
    if (system == 0)    legend.AddEntry(&graphs_hists_aa_jewel[0], "JEWEL, recoil", "lf");
    if (system == 0)    legend.AddEntry(&graphs_hists_aa_jewel_no_recoil[0], "JEWEL, no recoil", "lf");
    if (system == 0)    legend.AddEntry(&graphs_hists_aa_pyquen_no_wide[0], "PYQUEN", "lf");
    if (system == 0)    legend.AddEntry(&graphs_hists_aa_pyquen[0], "PYQUEN, wide angle rad.", "lf");
    if (system == 1)    legend.AddEntry(&graphs_systs_pp[0], "CMS data", "plf");
    if (system == 1)    legend.AddEntry(&graphs_hists_pp_jewel[0], "JEWEL", "lf");
    if (system == 1)    legend.AddEntry(&graphs_hists_pp_pyquen[0], "PYQUEN", "lf");

    for (int i = 0; i < ncols; i++) {
        pads[i]->cd();

        worlds[i]->Draw("axis");

        if (system == 2)    graphs_hists_ratio_pyquen_no_wide[i].Draw("same 3");
        if (system == 2)    graphs_hists_ratio_pyquen_no_wide[i].Draw("same lX");
        if (system == 2)    graphs_hists_ratio_pyquen[i].Draw("same 3");
        if (system == 2)    graphs_hists_ratio_pyquen[i].Draw("same lX");
        if (system == 2)    graphs_hists_ratio_jewel[i].Draw("same 3");
        if (system == 2)    graphs_hists_ratio_jewel[i].Draw("same lX");
        if (system == 2)    graphs_hists_ratio_jewel_no_recoil[i].Draw("same 3");
        if (system == 2)    graphs_hists_ratio_jewel_no_recoil[i].Draw("same lX");
        if (system == 2)    graphs_systs_ratio[i]->Draw("same e2");
        if (system == 2)    graphs_hists_ratio[i]->Draw("same");

        if (system == 0)    graphs_hists_aa_jewel[i].Draw("same 3");
        if (system == 0)    graphs_hists_aa_jewel[i].Draw("same lX");
        if (system == 0)    graphs_hists_aa_jewel_no_recoil[i].Draw("same 3");
        if (system == 0)    graphs_hists_aa_jewel_no_recoil[i].Draw("same lX");
        if (system == 0)    graphs_hists_aa_pyquen_no_wide[i].Draw("same 3");
        if (system == 0)    graphs_hists_aa_pyquen_no_wide[i].Draw("same lX");
        if (system == 0)    graphs_hists_aa_pyquen[i].Draw("same 3");
        if (system == 0)    graphs_hists_aa_pyquen[i].Draw("same lX");
        if (system == 0)    graphs_systs_aa[i]->Draw("same e2");
        if (system == 0)    graphs_hists_aa[i]->Draw("same");

        if (system == 1)    graphs_hists_pp_jewel[i].Draw("same 3");
        if (system == 1)    graphs_hists_pp_jewel[i].Draw("same lX");
        if (system == 1)    graphs_hists_pp_pyquen[i].Draw("same 3");
        if (system == 1)    graphs_hists_pp_pyquen[i].Draw("same lX");
        if (system == 1)    graphs_systs_pp[i]->Draw("same e2");
        if (system == 1)    graphs_hists_pp[i]->Draw("same");

        line.Draw("l");

        auto text_jet_pt = to_text(bjet_pt[i][0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[i][1]) + " GeV"s;
    
        pads[i]->cd();
        latex.SetTextAlign(21);
        latex.SetTextSize(0.06);
        latex.DrawLatex(0.5, 0.9, (text_jet_pt).c_str());

        if (system == 0)    latex.SetTextAlign(11);
        if (system == 0)    latex.SetTextSize(0.06);
        if (system == 0)    latex.DrawLatex(0.05, 0.78, "Cent. 0-10%");

        if (system == 2)    latex.SetTextAlign(11);
        if (system == 2)    latex.SetTextSize(0.06);
        if (system == 2)    latex.DrawLatex(0.05, 0.05, "Cent. 0-10%");
    }

    pads[0]->cd();
    legend.Draw();

    pads[ncols-1]->cd();
    latex.SetTextSize(0.05);
    latex.SetTextAlign(31);
    latex.DrawLatex(0.95, 0.78, (text_photon_pt).c_str());
    latex.DrawLatex(0.95, 0.70, (text_photon_eta).c_str());
    latex.DrawLatex(0.95, 0.62, (text_dphi + ", " + text_jet_eta).c_str());
    latex.DrawLatex(0.95, 0.54, (text_jet_alg).c_str());

    if (system == 2 && log)    canvas.SaveAs((set + "_final_theory_ratio_" + name + "_log.pdf").c_str());
    if (system == 1 && log)    canvas.SaveAs((set + "_final_theory_spectra_pp_" + name + "_log.pdf").c_str());
    if (system == 0 && log)    canvas.SaveAs((set + "_final_theory_spectra_aa_" + name + "_log.pdf").c_str());

    if (system == 2 && !log)   canvas.SaveAs((set + "_final_theory_ratio_" + name + ".pdf").c_str());
    if (system == 1 && !log)   canvas.SaveAs((set + "_final_theory_spectra_pp_" + name + ".pdf").c_str());
    if (system == 0 && !log)   canvas.SaveAs((set + "_final_theory_spectra_aa_" + name + ".pdf").c_str());

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return congratulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}