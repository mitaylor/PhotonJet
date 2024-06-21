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

std::vector<TGraphAsymmErrors> get_graph(history<TH1F>* h, int type)
{
    std::vector<TGraphAsymmErrors> result(h->size());

    static int color[12] = { TColor::GetColor("#515151"),
                            TColor::GetColor("#FF484E"),
                            TColor::GetColor("#FF9E48"),
                            TColor::GetColor("#bdb933"),
                            TColor::GetColor("#A9FF48"),
                            TColor::GetColor("#48FF9E"),
                            TColor::GetColor("#48FFFA"),
                            TColor::GetColor("#48A9FF"),
                            TColor::GetColor("#484EFF"),
                            TColor::GetColor("#9E48FF"),
                            TColor::GetColor("#F948FF"),
                            TColor::GetColor("#F734C6")
                           };

    for (int i = 0; i < h->size(); ++i) {
        for (int j = 1; j <= (*h)[i]->GetNbinsX(); ++j) {
            double x = (*h)[i]->GetBinCenter(j);
            double dx = (*h)[i]->GetBinWidth(j)/2;

            result[i].SetPoint(j - 1, x, (*h)[i]->GetBinContent(j));
            result[i].SetPointError(j - 1, dx, dx, (*h)[i]->GetBinError(j), (*h)[i]->GetBinError(j));
        }

        result[i].SetLineColor(color[type]);
        result[i].SetMarkerSize(0);
        result[i].SetLineWidth(5.0);
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

    // auto tag_data = conf->get<std::string>("tag_data");
    auto tag_total = conf->get<std::string>("tag_total");
    auto tag_ele = conf->get<std::string>("tag_ele");
    auto tag_purity = conf->get<std::string>("tag_purity");
    auto tag_iso = conf->get<std::string>("tag_iso");
    auto tag_es = conf->get<std::string>("tag_es");
    auto tag_jec = conf->get<std::string>("tag_jec");
    auto tag_jer = conf->get<std::string>("tag_jer");
    auto tag_prior = conf->get<std::string>("tag_prior");
    auto tag_stat = conf->get<std::string>("tag_stat");
    auto tag_reg = conf->get<std::string>("tag_reg");
    auto tag_mebs = conf->get<std::string>("tag_mebs");
    auto tag_cent = conf->get<std::string>("tag_cent");

    auto suffix = conf->get<std::string>("suffix");
    auto name = conf->get<std::string>("name");

    auto type = conf->get<int>("type");

    auto ymin = conf->get<float>("ymin");
    auto ymax = conf->get<float>("ymax");

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

    const int ncols = 5;
    const int nrows = 2;

    std::vector<float> bjet_pt(2, 0);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open input files */
    auto file_aa = new TFile((base + input_aa).data(), "read");
    auto file_pp = new TFile((base + input_pp).data(), "read");

    /* define jet pT bounds */
    switch (type) {
    case 1:
        bjet_pt[0] = ptg_range[osg_part1[2]];
        bjet_pt[1] = ptg_range[ptg_range.size() - 1 - osg_part1[3]];
        break;
    case 2:
        bjet_pt[0] = ptg_range[osg_part2[2]];
        bjet_pt[1] = ptg_range[ptg_range.size() - 1 - osg_part2[3]];
        break;
    default:
        bjet_pt[0] = ptg_range[osg[2]];
        bjet_pt[1] = ptg_range[ptg_range.size() - 1 - osg[3]];
    }

    if (suffix == "none") suffix = "";

    /* define kinematics and luminosity */
    auto text_system = "PbPb 1.69 nb^{-1}, pp 302 pb^{-1} (5.02 TeV)"s;
    auto text_cms = "CMS #scale[0.8]{#font[52]{Preliminary}}"s;
    auto text_photon_pt = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV"s;
    auto text_photon_eta = "|#eta^{#gamma}| < "s + to_text(photon_eta_abs);
    auto text_dphi = "#Delta#phi_{j#gamma} > #frac{"s + to_text(dphi_min_numerator) + "#pi}{"s + to_text(dphi_min_denominator) + "}"s;
    auto text_jet_alg = "anti-k_{T} R = 0.3"s;
    auto text_jet_eta = "|#eta^{jet}| < "s + to_text(jet_eta_abs);

    /* get histograms */
    // auto data_aa = new history<TH1F>(file_aa, "aa_base_aa_" + tag_data + suffix);
    auto syst_aa = new history<TH1F>(file_aa, "aa_total_base_aa_" + tag_total + suffix);
    auto ele_aa = new history<TH1F>(file_aa, "aa_" + tag_ele + suffix);
    auto purity_aa = new history<TH1F>(file_aa, "aa_" + tag_purity + suffix);
    auto iso_aa = new history<TH1F>(file_aa, "aa_" + tag_iso + suffix);
    auto es_aa = new history<TH1F>(file_aa, "aa_" + tag_es + suffix);
    auto jec_aa = new history<TH1F>(file_aa, "aa_" + tag_jec + suffix);
    auto jer_aa = new history<TH1F>(file_aa, "aa_" + tag_jer + suffix);
    auto prior_aa = new history<TH1F>(file_aa, "aa_" + tag_prior + suffix);
    auto stat_aa = new history<TH1F>(file_aa, "aa_" + tag_stat + suffix);
    auto reg_aa = new history<TH1F>(file_aa, "aa_" + tag_reg + suffix);
    auto mebs_aa = new history<TH1F>(file_aa, "aa_" + tag_mebs + suffix);
    auto cent_aa = new history<TH1F>(file_aa, "aa_" + tag_cent + suffix);

    // auto data_pp = new history<TH1F>(file_pp, "pp_base_pp_" + tag_data + suffix);
    auto syst_pp = new history<TH1F>(file_pp, "pp_total_base_pp_" + tag_total + suffix);
    std::cout << "pp_total_base_pp_" + tag_total + suffix << " " << (*syst_pp)[0]->GetBinContent(0) << std::endl;
    auto ele_pp = new history<TH1F>(file_pp, "pp_" + tag_ele + suffix);
    auto purity_pp = new history<TH1F>(file_pp, "pp_" + tag_purity + suffix);
    auto iso_pp = new history<TH1F>(file_pp, "pp_" + tag_iso + suffix);
    auto es_pp = new history<TH1F>(file_pp, "pp_" + tag_es + suffix);
    auto jec_pp = new history<TH1F>(file_pp, "pp_" + tag_jec + suffix);
    auto jer_pp = new history<TH1F>(file_pp, "pp_" + tag_jer + suffix);
    auto prior_pp = new history<TH1F>(file_pp, "pp_" + tag_prior + suffix);
    auto stat_pp = new history<TH1F>(file_pp, "pp_" + tag_stat + suffix);
    auto reg_pp = new history<TH1F>(file_pp, "pp_" + tag_reg + suffix);

    auto graphs_syst_aa = get_graph(syst_aa, 0);
    auto graphs_ele_aa = get_graph(ele_aa, 1);
    auto graphs_purity_aa = get_graph(purity_aa, 2);
    auto graphs_iso_aa = get_graph(iso_aa, 3);
    auto graphs_es_aa = get_graph(es_aa, 4);
    auto graphs_jec_aa = get_graph(jec_aa, 5);
    auto graphs_jer_aa = get_graph(jer_aa, 6);
    auto graphs_prior_aa = get_graph(prior_aa, 7);
    auto graphs_stat_aa = get_graph(stat_aa, 8);
    auto graphs_reg_aa = get_graph(reg_aa, 9);
    auto graphs_mebs_aa = get_graph(mebs_aa, 10);
    auto graphs_cent_aa = get_graph(cent_aa, 11);

    auto graphs_syst_pp = get_graph(syst_pp, 0);
    auto graphs_ele_pp = get_graph(ele_pp, 1);
    auto graphs_purity_pp = get_graph(purity_pp, 2);
    auto graphs_iso_pp = get_graph(iso_pp, 3);
    auto graphs_es_pp = get_graph(es_pp, 4);
    auto graphs_jec_pp = get_graph(jec_pp, 5);
    auto graphs_jer_pp = get_graph(jer_pp, 6);
    auto graphs_prior_pp = get_graph(prior_pp, 7);
    auto graphs_stat_pp = get_graph(stat_pp, 8);
    auto graphs_reg_pp = get_graph(reg_pp, 9);

    /* size canvas */
    double panel_size = 350;
    double padding_width_left = 140;
    double padding_width_right = 50;
    double padding_height = 70;

    double canvas_width = panel_size * ncols + padding_width_left + padding_width_right;
    double canvas_height = panel_size * nrows + padding_height * 2;

    double sf = 1140.0/840.0;

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

    std::vector<TH2F*> worlds(ncols * nrows);
    std::vector<TPad*> pads(ncols * nrows);
    std::vector<TGaxis*> axis_x(ncols);
    std::vector<TGaxis*> axis_y(nrows);
    std::vector<TArrow*> arrows(ncols * nrows);

    for (int i = 0; i < ncols; ++i) {
        for (int j = 0; j < nrows; ++j) {
            worlds[i + ncols*j] = new TH2F("world", ";;", 100, xmin, xmax, 100, ymin, ymax);
            worlds[i + ncols*j]->SetStats(0);

            pads[i + ncols*j] = new TPad("P", "", pad_x0 + pad_dx * i, pad_y0 + pad_dy * j, pad_x0 + pad_dx * (i + 1), pad_y0 + pad_dy * (j + 1), 0);
            
            set_pad(*pads[i + ncols*j]);

            if (j == 0) axis_x[i] = new TGaxis(pad_x0 + pad_dx * i, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * (i + 1), pad_y0 + pad_dy * 0, xmin, xmax, 510, "SG");
            if (i == 0) axis_y[j] = new TGaxis(pad_x0 + pad_dx * 0, pad_y0 + pad_dy * j, pad_x0 + pad_dx * 0, pad_y0 + pad_dy * (j + 1), ymin, ymax, 510, "SG");

            set_axis(*axis_x[i], sf);
            set_axis(*axis_y[j], sf);

            double arrow_y = 0.0;

            if (i == 0) arrow_y = graphs_syst_pp[0].GetPointY(0);
            if (i == 1) arrow_y = graphs_syst_aa[0].GetPointY(0);
            if (i == 2) arrow_y = graphs_syst_aa[1].GetPointY(0);
            if (i == 3) arrow_y = graphs_syst_aa[2].GetPointY(0);
            if (i == 4) arrow_y = graphs_syst_aa[3].GetPointY(0);
        
            arrows[i + ncols*j] = new TArrow(0.003, arrow_y, 0.0040, arrow_y, 0.02 / ncols, "<|");
            arrows[i + ncols*j]->SetAngle(40);
            arrows[i + ncols*j]->SetLineWidth(1);

            canvas.cd();

            if (j == 0) latex.SetTextFont(42);
            if (j == 0) latex.SetTextSize(0.07/sf);
            if (j == 0) latex.SetTextAlign(22);
            if (j == 0) latex.SetTextAngle(0);
            if (j == 0) latex.DrawLatex(pad_x0 + pad_dx * (i + 0.5), pad_y0 * 0.5, "#Deltaj");

            if (i == 0) latex.SetTextFont(42);
            if (i == 0) latex.SetTextSize(0.07/sf);
            if (i == 0) latex.SetTextAlign(22);
            if (i == 0) latex.SetTextAngle(90);
            if (i == 0) latex.DrawLatex(pad_x0 * 0.4, pad_y0 + pad_dy * (j + 0.5), "#frac{1}{N_{#gamma}} #frac{dN_{j#gamma}}{d#Deltaj}");
        }
    }

    latex.SetTextFont(62);
    latex.SetTextSize(0.07/sf);
    latex.SetTextAlign(11);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0, pad_y0 * 1.15 + pad_dy * nrows, text_cms.c_str());

    latex.SetTextFont(42);
    latex.SetTextSize(0.045/sf);
    latex.SetTextAlign(31);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0 + pad_dx * ncols, pad_y0 * 1.15 + pad_dy * nrows, text_system.c_str());

    /* declare legend */
    double legend_y_min = 0.6;
    double legend_y_max = 0.83;
    double legend_x_min = 0.6;
    double legend_x_max = 0.9;

    TLegend legend(legend_x_min, legend_y_min, legend_x_max, legend_y_max);
    legend.SetTextFont(42);
    legend.SetTextSize(0.03);
    legend.SetFillStyle(0);
    legend.SetBorderSize(0);
    legend.AddEntry(&graphs_syst_aa[0], "Total", "l");
    legend.AddEntry(&graphs_ele_aa[0], "Electron Rejection", "l");
    legend.AddEntry(&graphs_purity_aa[0], "Purity", "l");
    legend.AddEntry(&graphs_iso_aa[0], "Photon Isolation", "l");
    legend.AddEntry(&graphs_es_aa[0], "Photon Energy Scale", "l");
    legend.AddEntry(&graphs_jec_aa[0], "Jet Energy Correction", "l");
    legend.AddEntry(&graphs_jer_aa[0], "Jet Energy Resolution", "l");
    legend.AddEntry(&graphs_prior_aa[0], "Unfolding Prior", "l");
    legend.AddEntry(&graphs_stat_aa[0], "Response Matrix Statistics", "l");
    legend.AddEntry(&graphs_reg_aa[0], "Unfolding Regularization", "l");
    legend.AddEntry(&graphs_mebs_aa[0], "Mixed-Event Background Subtraction", "l");
    legend.AddEntry(&graphs_cent_aa[0], "Centrality", "l");
    
    pads[0]->cd();
    worlds[0]->Draw("axis");
    arrows[0]->Draw();
    graphs_syst_pp[0].Draw("same l");
    graphs_purity_pp[0].Draw("same l");
    graphs_es_pp[0].Draw("same l");
    graphs_jer_pp[0].Draw("same l");
    graphs_stat_pp[0].Draw("same l");

    pads[5]->cd();
    worlds[5]->Draw("axis");
    arrows[5]->Draw();
    graphs_syst_pp[0].Draw("same l");
    graphs_ele_pp[0].Draw("same l");
    graphs_iso_pp[0].Draw("same l");
    graphs_jec_pp[0].Draw("same l");
    graphs_prior_pp[0].Draw("same l");
    graphs_reg_pp[0].Draw("same l");

    pads[1]->cd();
    worlds[1]->Draw("axis");
    arrows[1]->Draw();
    graphs_syst_aa[0].Draw("same l");
    graphs_purity_aa[0].Draw("same l");
    graphs_es_aa[0].Draw("same l");
    graphs_jer_aa[0].Draw("same l");
    graphs_stat_aa[0].Draw("same l");
    graphs_mebs_aa[0].Draw("same l");

    pads[6]->cd();
    worlds[6]->Draw("axis");
    arrows[6]->Draw();
    graphs_syst_aa[0].Draw("same l");
    graphs_ele_aa[0].Draw("same l");
    graphs_iso_aa[0].Draw("same l");
    graphs_jec_aa[0].Draw("same l");
    graphs_prior_aa[0].Draw("same l");
    graphs_reg_aa[0].Draw("same l");
    graphs_cent_aa[0].Draw("same l");

    pads[2]->cd();
    worlds[2]->Draw("axis");
    arrows[2]->Draw();
    graphs_syst_aa[1].Draw("same l");
    graphs_purity_aa[1].Draw("same l");
    graphs_es_aa[1].Draw("same l");
    graphs_jer_aa[1].Draw("same l");
    graphs_stat_aa[1].Draw("same l");
    graphs_mebs_aa[1].Draw("same l");

    pads[7]->cd();
    worlds[7]->Draw("axis");
    arrows[7]->Draw();
    graphs_syst_aa[1].Draw("same l");
    graphs_ele_aa[1].Draw("same l");
    graphs_iso_aa[1].Draw("same l");
    graphs_jec_aa[1].Draw("same l");
    graphs_prior_aa[1].Draw("same l");
    graphs_reg_aa[1].Draw("same l");
    graphs_cent_aa[1].Draw("same l");

    pads[3]->cd();
    worlds[3]->Draw("axis");
    arrows[3]->Draw();
    graphs_syst_aa[2].Draw("same l");
    graphs_purity_aa[2].Draw("same l");
    graphs_es_aa[2].Draw("same l");
    graphs_jer_aa[2].Draw("same l");
    graphs_stat_aa[2].Draw("same l");
    graphs_mebs_aa[2].Draw("same l");

    pads[8]->cd();
    worlds[8]->Draw("axis");
    arrows[8]->Draw();
    graphs_syst_aa[2].Draw("same l");
    graphs_ele_aa[2].Draw("same l");
    graphs_iso_aa[2].Draw("same l");
    graphs_jec_aa[2].Draw("same l");
    graphs_prior_aa[2].Draw("same l");
    graphs_reg_aa[2].Draw("same l");
    graphs_cent_aa[2].Draw("same l");

    pads[4]->cd();
    worlds[4]->Draw("axis");
    arrows[4]->Draw();
    graphs_syst_aa[3].Draw("same l");
    graphs_purity_aa[3].Draw("same l");
    graphs_es_aa[3].Draw("same l");
    graphs_jer_aa[3].Draw("same l");
    graphs_stat_aa[3].Draw("same l");
    graphs_mebs_aa[3].Draw("same l");

    pads[9]->cd();
    worlds[9]->Draw("axis");
    arrows[9]->Draw();
    graphs_syst_aa[3].Draw("same l");
    graphs_ele_aa[3].Draw("same l");
    graphs_iso_aa[3].Draw("same l");
    graphs_jec_aa[3].Draw("same l");
    graphs_prior_aa[3].Draw("same l");
    graphs_reg_aa[3].Draw("same l");
    graphs_cent_aa[3].Draw("same l");

    // auto text_jet_pt = to_text(bjet_pt[i][0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[i][1]) + " GeV"s;

    // latex.SetTextAlign(21);
    // latex.SetTextSize(0.06);
    // latex.DrawLatex(0.5, 0.9, (text_jet_pt).c_str());

    // latex.SetTextAlign(21);
    // latex.SetTextSize(0.06);
    // latex.DrawLatex(0.5, 0.12, "Cent. 0-10%");

    // latex.SetTextSize(0.05);
    // latex.SetTextAlign(31);
    // latex.DrawLatex(0.95, 0.78, (text_photon_pt).c_str());
    // latex.DrawLatex(0.95, 0.70, (text_photon_eta).c_str());
    // latex.DrawLatex(0.95, 0.62, (text_dphi + ", " + text_jet_eta).c_str());
    // latex.DrawLatex(0.95, 0.54, (text_jet_alg).c_str());

    canvas.SaveAs((set + "_final_data_systematics_" + name + "_log.pdf").c_str());

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return congratulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}