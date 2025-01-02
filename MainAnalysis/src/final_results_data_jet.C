#include "../include/lambdas.h"
#include "../include/specifics.h"
#include "../include/text.h"
#include "../include/significant.h"

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
#include <fstream>
#include <iomanip>

using namespace std::literals::string_literals;
using namespace std::placeholders;

void hep_data_spectra(std::string hep,
                      std::vector<std::vector<TGraphAsymmErrors>> &graphs_hists_aa, 
                      std::vector<std::vector<TGraphAsymmErrors>> &graphs_systs_aa,
                      std::vector<std::vector<TGraphAsymmErrors>> &graphs_hists_pp,
                      std::vector<std::vector<TGraphAsymmErrors>> &graphs_systs_pp)
{
    std::ofstream out("hep/"s + hep);

    // write x ranges: taking from the first one
    out << "independent_variables:" << std::endl;
    out << "- header: {name: '$p_{T}^{jet}$'}" << std::endl;
    out << "  values:" << std::endl;

    int nbins = graphs_hists_aa[0][0].GetN();

    for (int i = 0; i < nbins; ++i) {
        double x, y, exl, exh;
        graphs_hists_aa[0][0].GetPoint(i, x, y);
        exl = graphs_hists_aa[0][0].GetErrorXlow(i);
        exh = graphs_hists_aa[0][0].GetErrorXhigh(i);

        double low = x - exl;
        double high = x + exh;

        out << "  - {low: " << round(low, -5) << ", high: " << round(high, -5) << "}" << std::endl;
    }

    // write y ranges: loop over graphs
    out << "dependent_variables:" << std::endl;

    // pp results
    out << "- header: {name: '$\\frac{1}{N_{\\gamma}} \\frac{dN_{j\\gamma}}{dp_{T}^{jet}}$'}" << std::endl;
    out << "  qualifiers:" << std::endl;
    out << "  - {name: 'SQRT(S)/NUCLEON', units: 'GEV', value: '5020'}" << std::endl;
    out << "  - {name: 'JET ALGO', value: 'ANTI-KT R = 0.3'}" << std::endl;
    out << "  - {name: '|JET ETA|', value: '< 1.6'}" << std::endl;
    out << "  - {name: 'PHOTON PT', units: 'GEV', value: '60 - 200'}" << std::endl;
    out << "  - {name: '|PHOTON ETA|', value: '< 1.44'}" << std::endl;
    out << "  - {name: '|PHOTON-JET DPHI|', value: '> 2*PI/3'}" << std::endl;
    out << "  - {name: 'JET PT', units: 'GEV', value: '30 - 100'}" << std::endl;
    out << "  - {name: 'RE', value: 'P P --> JET + PHOTON'}" << std::endl;
    out << "  - {name: 'CENTRALITY', value: 'N/A'}" << std::endl;

    out << "  values:" << std::endl;

    for (int k = 0; k < nbins; k++) {
        double x, y, ey, sy;

        graphs_hists_pp[0][0].GetPoint(k, x, y);

        ey = graphs_hists_pp[0][0].GetErrorYhigh(k);
        sy = graphs_systs_pp[0][0].GetErrorYhigh(k);

        y /= 1000;
        ey /= 1000;
        sy /= 1000;

        int d = -10000;
        int digit = 0;

        d = std::max(d, first_sig_digit(ey));
        d = std::max(d, first_sig_digit(sy));

        d = d - 1;

        if (d < 0)          digit = -d;
        if (digit > 100)    digit = 0;

        out << std::fixed << std::setprecision(digit);

        out << "  - value: " << round(y, d) << std::endl;
        out << "    errors:" << std::endl;
        out << "    - {label: stat, symerror: " << round(ey, d) << "}" << std::endl;
        out << "    - {label: syst, symerror: " << round(sy, d) << "}" << std::endl;
    }

    // aa results for all centrality classes
    for (size_t j = 0; j < graphs_hists_aa[0].size(); ++j) {    // npads
        out << "- header: {name: '$\\frac{1}{N_{\\gamma}} \\frac{dN_{j\\gamma}}{dp_{T}^{jet}}$'}" << std::endl;
        out << "  qualifiers:" << std::endl;
        out << "  - {name: 'SQRT(S)/NUCLEON', units: 'GEV', value: '5020'}" << std::endl;
        out << "  - {name: 'JET ALGO', value: 'ANTI-KT R = 0.3'}" << std::endl;
        out << "  - {name: '|JET ETA|', value: '< 1.6'}" << std::endl;
        out << "  - {name: 'PHOTON PT', units: 'GEV', value: '60 - 200'}" << std::endl;
        out << "  - {name: '|PHOTON ETA|', value: '< 1.44'}" << std::endl;
        out << "  - {name: '|PHOTON-JET DPHI|', value: '> 2*PI/3'}" << std::endl;
        out << "  - {name: 'JET PT', units: 'GEV', value: '30 - 100'}" << std::endl;
        out << "  - {name: 'RE', value: 'Pb Pb --> JET + PHOTON'}" << std::endl;

        if (j == 0) out << "  - {name: 'CENTRALITY', value: '50-90%'}" << std::endl;
        if (j == 1) out << "  - {name: 'CENTRALITY', value: '30-50%'}" << std::endl;
        if (j == 2) out << "  - {name: 'CENTRALITY', value: '10-30%'}" << std::endl;
        if (j == 3) out << "  - {name: 'CENTRALITY', value: '0-10%'}" << std::endl;

        out << "  values:" << std::endl;

        for (int k = 0; k < nbins; k++) {
            double x, y, ey, sy;

            graphs_hists_aa[0][j].GetPoint(k, x, y);

            ey = graphs_hists_aa[0][j].GetErrorYhigh(k);
            sy = graphs_systs_aa[0][j].GetErrorYhigh(k);

            y /= 1000;
            ey /= 1000;
            sy /= 1000;

            int d = -10000;
            int digit = 0;

            d = std::max(d, first_sig_digit(ey));
            d = std::max(d, first_sig_digit(sy));

            d = d - 1;

            if (d < 0)          digit = -d;
            if (digit > 100)    digit = 0;

            out << std::fixed << std::setprecision(digit);

            out << "  - value: " << round(y, d) << std::endl;
            out << "    errors:" << std::endl;
            out << "    - {label: stat, symerror: " << round(ey, d) << "}" << std::endl;
            out << "    - {label: syst, symerror: " << round(sy, d) << "}" << std::endl;
        }
    }

    out.close();
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

        (*h)[i]->Scale(1000);
        (*s)[i]->Scale(1000);
    }
}

std::vector<std::vector<TGraphAsymmErrors>> get_graph(std::vector<history<TH1F>*> h, int system, double sf)
{
    std::vector<std::vector<TGraphAsymmErrors>> result(h.size(), std::vector<TGraphAsymmErrors>(h[0]->size()));

    static int style[3] = {20, 24, 20};
    static int color[3] = {TColor::GetColor("#5790FC"), TColor::GetColor("#E42536"), TColor::GetColor("#9C9C9C")};

    for (size_t i = 0; i < h.size(); ++i) {
        for (int64_t k = 0; k < h[0]->size(); ++k) {
            for (int j = 1; j <= (*h[i])[k]->GetNbinsX(); ++j) {
                double x = (*h[i])[k]->GetBinCenter(j);
                double dx = (*h[i])[k]->GetBinWidth(j)/2;

                result[i][k].SetPoint(j - 1, x, (*h[i])[k]->GetBinContent(j));
                result[i][k].SetPointError(j - 1, dx, dx, (*h[i])[k]->GetBinError(j), (*h[i])[k]->GetBinError(j));
            }

            result[i][k].SetMarkerStyle(style[system]);
            result[i][k].SetMarkerColor(1);
            result[i][k].SetLineColor(1);
            result[i][k].SetFillColorAlpha(color[system], 0.60);
            result[i][k].SetMarkerSize(1.5*sf);
            result[i][k].SetLineWidth(5*sf);    
        }
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
    axis.SetMoreLogLabels();
    axis.Draw();
}

int congratulate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input_aa = conf->get<std::string>("input_aa");
    auto input_pp = conf->get<std::string>("input_pp");

    auto figures = conf->get<std::vector<std::string>>("figures");
    auto types = conf->get<std::vector<int64_t>>("types");

    auto hep = conf->get<std::string>("hep");

    auto ymins = conf->get<std::vector<float>>("ymins");
    auto ymaxs = conf->get<std::vector<float>>("ymaxs");

    auto supplementary = conf->get<bool>("supplementary");

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

    const int nrows = figures.size();
    const int npads = 4;

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open input files */
    auto file_aa = new TFile((base + input_aa).data(), "read");
    auto file_pp = new TFile((base + input_pp).data(), "read");

    /* define kinematics and luminosity */
    auto text_system = "PbPb 1.69 nb^{-1}, pp 302 pb^{-1} (5.02 TeV)"s;
    auto text_cms = (supplementary) ? "CMS #scale[0.8]{#font[52]{Supplementary}}"s : "CMS"s;
    auto text_photon_pt = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV"s;
    auto text_photon_eta = "|#eta^{#gamma}| < "s + to_text(photon_eta_abs);
    auto text_dphi = "#Delta#phi_{j#gamma} > #frac{"s + to_text(dphi_min_numerator) + "#pi}{"s + to_text(dphi_min_denominator) + "}"s;
    auto text_jet_alg = "anti-k_{T} R = 0.3"s;
    auto text_jet_eta = "|#eta^{jet}| < "s + to_text(jet_eta_abs);
    auto text_jet_pt = to_text(ptg_range[osg[2]]) + " < p_{T}^{jet} < "s + to_text(ptg_range[ptg_range.size() - 1 - osg[3]]) + " GeV"s;

    std::vector<history<TH1F>*> hists_aa(nrows);
    std::vector<history<TH1F>*> systs_aa(nrows);
    std::vector<history<TH1F>*> hists_pp(nrows);
    std::vector<history<TH1F>*> systs_pp(nrows);

    for (int i = 0; i < nrows; ++i) {
        /* get histograms */
        hists_aa[i] = new history<TH1F>(file_aa, "aa_base_aa_nominal_s_pure_raw_sub_" + figures[i]);
        systs_aa[i] = new history<TH1F>(file_aa, "aa_total_base_aa_nominal_s_pure_raw_sub_" + figures[i]);
        hists_pp[i] = new history<TH1F>(file_pp, "pp_base_pp_nominal_s_pure_raw_sub_" + figures[i]);
        systs_pp[i] = new history<TH1F>(file_pp, "pp_total_base_pp_nominal_s_pure_raw_sub_" + figures[i]);

        set_systematics(hists_aa[i], systs_aa[i]);
        set_systematics(hists_pp[i], systs_pp[i]);
    }

    /* size canvas */
    double panel_size = 500;
    double padding_width_left = 170;
    double padding_width_right = 50;
    double padding_height_top = 70;
    double padding_height_bottom = 100;

    double factor_x = 0.72;
    double factor_y = (2 * panel_size + (1 - factor_x) * padding_width_left + (1 - factor_x) * padding_width_right) / 4 / panel_size;

    panel_size *= factor_y;
    padding_height_top *= factor_x;
    padding_height_bottom *= factor_x;
    padding_width_right *= factor_x;
    padding_width_left *= factor_x;

    double canvas_width = panel_size * 4 + padding_width_left + padding_width_right;
    double canvas_height = panel_size * nrows + padding_height_top + padding_height_bottom;

    double sf = (panel_size * nrows + padding_height_top + padding_height_bottom) / (panel_size * 1 + padding_height_top + padding_height_bottom);
    sf = sf * (panel_size + padding_height_top + padding_height_bottom) / (640) / factor_x;

    double pad_x0 = padding_width_left / canvas_width;
    double pad_y0 = padding_height_bottom / canvas_height;
    double pad_dx = panel_size / canvas_width;
    double pad_dy = panel_size / canvas_height;

    double xmin = 30.0001;
    double xmax = 99.999;

    gStyle->SetLineScalePS(1);

    /* get graphs */
    auto graphs_hists_aa = get_graph(hists_aa, 0, factor_x);
    auto graphs_systs_aa = get_graph(systs_aa, 0, factor_x);
    auto graphs_hists_pp = get_graph(hists_pp, 1, factor_x);
    auto graphs_systs_pp = get_graph(systs_pp, 1, factor_x);

    /* declare canvas, pads, axes, and titles */
    TCanvas canvas("canvas", "", canvas_width, canvas_height);

    std::vector<TH2F*> worlds(nrows);
    std::vector<std::vector<TPad*>> pads(nrows, std::vector<TPad*>(npads));
    std::vector<TGaxis*> axis_x(npads);
    std::vector<TGaxis*> axis_y(nrows);

    for (int i = 0; i < nrows; ++i) {
        worlds[i] = new TH2F("world", ";;", 100, xmin, xmax, 100, ymins[i], ymaxs[i]);
        worlds[i]->SetStats(0);

        axis_y[i] = new TGaxis(pad_x0 + pad_dx * 0, pad_y0 + pad_dy * i, pad_x0 + pad_dx * 0, pad_y0 + pad_dy * (i + 1), ymins[i] * 0.999, ymaxs[i] * 0.999, 510, "S");
        
        set_axis(*axis_y[i], sf);

        for (int j = 0; j < npads; ++j) {
            pads[i][j] = new TPad("P", "", pad_x0 + pad_dx * j, pad_y0 + pad_dy * i, pad_x0 + pad_dx * (j + 1), pad_y0 + pad_dy * (i + 1), 0);
            
            set_pad(*pads[i][j]);
        }
    }

    canvas.cd();

    axis_x[0] = new TGaxis(pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 1, pad_y0 + pad_dy * 0, xmin, xmax, 510, "SG");
    axis_x[1] = new TGaxis(pad_x0 + pad_dx * 1, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 2, pad_y0 + pad_dy * 0, xmin, xmax, 510, "SG");
    axis_x[2] = new TGaxis(pad_x0 + pad_dx * 2, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 3, pad_y0 + pad_dy * 0, xmin, xmax, 510, "SG");
    axis_x[3] = new TGaxis(pad_x0 + pad_dx * 3, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 4, pad_y0 + pad_dy * 0, xmin, xmax, 510, "SG");

    set_axis(*axis_x[0], sf);
    set_axis(*axis_x[1], sf);
    set_axis(*axis_x[2], sf);
    set_axis(*axis_x[3], sf);

    TLatex latex;
    latex.SetNDC();

    latex.SetTextFont(42);
    latex.SetTextSize(0.065/sf);
    latex.SetTextAlign(22);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0 + pad_dx * 0.5, pad_y0 * 0.36, "p_{T}^{jet}");
    latex.DrawLatex(pad_x0 + pad_dx * 1.5, pad_y0 * 0.36, "p_{T}^{jet}");
    latex.DrawLatex(pad_x0 + pad_dx * 2.5, pad_y0 * 0.36, "p_{T}^{jet}");
    latex.DrawLatex(pad_x0 + pad_dx * 3.5, pad_y0 * 0.36, "p_{T}^{jet}");

    latex.SetTextFont(42);
    latex.SetTextSize(0.065/sf);
    latex.SetTextAlign(22);
    latex.SetTextAngle(90);
    latex.DrawLatex(pad_x0 * 0.37, pad_y0 + pad_dy * nrows * 0.5, "#frac{1}{N_{#gamma}} #frac{dN_{j#gamma}}{dp_{T}^{jet}}");

    latex.SetTextFont(42);
    latex.SetTextSize(0.04/sf);
    latex.SetTextAlign(31);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0 * 0.95, pad_y0 + pad_dy * nrows * 0.95, "#times 10^{-3}");

    latex.SetTextFont(62);
    latex.SetTextSize(0.07/sf);
    latex.SetTextAlign(11);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0, pad_y0 * (1 + 0.15 * padding_height_top / padding_height_bottom) + pad_dy * nrows, text_cms.c_str());

    latex.SetTextFont(42);
    latex.SetTextSize(0.055/sf);
    latex.SetTextAlign(31);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0 + pad_dx * 4, pad_y0 * (1 + 0.15 * padding_height_top / padding_height_bottom) + pad_dy * nrows, text_system.c_str());

    /* declare legend */
    double legend_y_min = 1 - 0.74 / factor_y * factor_x;
    double legend_y_max = 1 - 0.60 / factor_y * factor_x;
    double legend_x_min = 0.05 / factor_y * factor_x;
    double legend_x_max = 0.35 / factor_y * factor_x;

    TLegend legend(legend_x_min, legend_y_min, legend_x_max, legend_y_max);
    legend.SetTextFont(42);
    legend.SetTextSize(0.055 / factor_y * factor_x);
    legend.SetFillStyle(0);
    legend.SetBorderSize(0);
    legend.AddEntry(&graphs_systs_aa[0][0], "PbPb", "plf");
    legend.AddEntry(&graphs_systs_pp[0][0], "pp", "plf");

    for (int i = 0; i < nrows; i++) {
        for(int j = 0; j < npads; j++)
        {
            pads[i][j]->cd();

            worlds[i]->Draw("axis");

            graphs_systs_aa[i][j].Draw("same 2");
            graphs_systs_pp[i][0].Draw("same 2");
            graphs_hists_aa[i][j].Draw("same PZ");
            graphs_hists_pp[i][0].Draw("same PZ");
        }

        pads[i][0]->cd();
        latex.SetTextAlign(21);
        latex.SetTextSize(0.06 / factor_y * factor_x);
        latex.DrawLatex(0.5, 1 - 0.08 / factor_y * factor_x, "Cent. 50-90%");

        pads[i][1]->cd();
        latex.SetTextAlign(21);
        latex.SetTextSize(0.06 / factor_y * factor_x);
        latex.DrawLatex(0.5, 1 - 0.08 / factor_y * factor_x, "Cent. 30-50%");

        pads[i][2]->cd();
        latex.SetTextAlign(21);
        latex.SetTextSize(0.06 / factor_y * factor_x);
        latex.DrawLatex(0.5, 1 - 0.08 / factor_y * factor_x, "Cent. 10-30%");

        pads[i][3]->cd();
        latex.SetTextAlign(21);
        latex.SetTextSize(0.06 / factor_y * factor_x);
        latex.DrawLatex(0.5, 1 - 0.08 / factor_y * factor_x, "Cent. 0-10%");
    }

    pads[0][0]->cd();
    legend.Draw();

    pads[0][1]->cd();
    latex.SetTextSize(0.055 / factor_y * factor_x);
    latex.SetTextAlign(11);
    latex.DrawLatex(0.05 / factor_y * factor_x, 0.13 / factor_y * factor_x, (text_photon_eta).c_str());
    latex.DrawLatex(0.05 / factor_y * factor_x, 0.05 / factor_y * factor_x, (text_photon_pt).c_str());
    
    pads[0][2]->cd();
    latex.DrawLatex(0.05 / factor_y * factor_x, 0.13 / factor_y * factor_x, (text_jet_alg).c_str());
    latex.DrawLatex(0.05 / factor_y * factor_x, 0.05 / factor_y * factor_x, (text_dphi + ", " + text_jet_eta).c_str());

    pads[0][3]->cd();
    latex.DrawLatex(0.05 / factor_y * factor_x, 0.05 / factor_y * factor_x, (text_jet_pt).c_str());

    canvas.SaveAs((set + "_final_jet_log.pdf").c_str());

    canvas.SaveAs((set + "_final_jet_log.C").c_str());

    in(output, []() {});

    hep_data_spectra(hep, graphs_hists_aa, graphs_systs_aa, graphs_hists_pp, graphs_systs_pp);

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return congratulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}