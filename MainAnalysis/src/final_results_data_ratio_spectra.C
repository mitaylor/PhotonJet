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
#include "TArrow.h"
#include "TBox.h"
#include "TGraphAsymmErrors.h"

#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>

using namespace std::literals::string_literals;
using namespace std::placeholders;

void hep_data_spectra(std::string hep, bool subsets,
                      std::vector<std::vector<TGraphAsymmErrors>> &graphs_hists_aa, 
                      std::vector<std::vector<TGraphAsymmErrors>> &graphs_systs_aa,
                      std::vector<std::vector<TGraphAsymmErrors>> &graphs_hists_pp,
                      std::vector<std::vector<TGraphAsymmErrors>> &graphs_systs_pp)
{
    std::ofstream out("hep/"s + hep);
    int nrows = (subsets) ? 2 : 1;

    // write x ranges: taking from the first one
    out << "independent_variables:" << std::endl;
    out << "- header: {name: '$\\Delta j$'}" << std::endl;
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

    for (int i = nrows - 1; i >= 0; --i) {           // nrows
        // pp results
        out << "- header: {name: '$\\frac{1}{N_{\\gamma}} \\frac{dN_{j\\gamma}}{d\\Delta j}$'}" << std::endl;
        out << "  qualifiers:" << std::endl;
        out << "  - {name: 'SQRT(S)/NUCLEON', units: 'GEV', value: '5020'}" << std::endl;
        out << "  - {name: 'JET ALGO', value: 'ANTI-KT R = 0.3'}" << std::endl;
        out << "  - {name: '|JET ETA|', value: '< 1.6'}" << std::endl;
        out << "  - {name: 'PHOTON PT', units: 'GEV', value: '60 - 200'}" << std::endl;
        out << "  - {name: '|PHOTON ETA|', value: '< 1.44'}" << std::endl;
        out << "  - {name: '|PHOTON-JET DPHI|', value: '> 2*PI/3'}" << std::endl;

        if (subsets && i == 1)  out << "  - {name: 'JET PT', units: 'GEV', value: '30 - 60'}" << std::endl;
        if (subsets && i == 0)  out << "  - {name: 'JET PT', units: 'GEV', value: '60 - 100'}" << std::endl;
        if (!subsets)           out << "  - {name: 'JET PT', units: 'GEV', value: '30 - 100'}" << std::endl;

        out << "  - {name: 'RE', value: 'P P --> JET + PHOTON'}" << std::endl;
        out << "  - {name: 'CENTRALITY', value: 'N/A'}" << std::endl;

        out << "  values:" << std::endl;

        for (int k = 0; k < nbins; k++) {
            double x, y, ey, sy;

            graphs_hists_pp[i][0].GetPoint(k, x, y);

            ey = graphs_hists_pp[i][0].GetErrorYhigh(k);
            sy = graphs_systs_pp[i][0].GetErrorYhigh(k);

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
            out << "- header: {name: '$\\frac{1}{N_{\\gamma}} \\frac{dN_{j\\gamma}}{d\\Delta j}$'}" << std::endl;
            out << "  qualifiers:" << std::endl;
            out << "  - {name: 'SQRT(S)/NUCLEON', units: 'GEV', value: '5020'}" << std::endl;
            out << "  - {name: 'JET ALGO', value: 'ANTI-KT R = 0.3'}" << std::endl;
            out << "  - {name: '|JET ETA|', value: '< 1.6'}" << std::endl;
            out << "  - {name: 'PHOTON PT', units: 'GEV', value: '60 - 200'}" << std::endl;
            out << "  - {name: '|PHOTON ETA|', value: '< 1.44'}" << std::endl;
            out << "  - {name: '|PHOTON-JET DPHI|', value: '> 2*PI/3'}" << std::endl;

            if (subsets && i == 1)  out << "  - {name: 'JET PT', units: 'GEV', value: '30 - 60'}" << std::endl;
            if (subsets && i == 0)  out << "  - {name: 'JET PT', units: 'GEV', value: '60 - 100'}" << std::endl;
            if (!subsets)           out << "  - {name: 'JET PT', units: 'GEV', value: '30 - 100'}" << std::endl;

            out << "  - {name: 'RE', value: 'Pb Pb --> JET + PHOTON'}" << std::endl;

            if (j == 0) out << "  - {name: 'CENTRALITY', value: '50-90%'}" << std::endl;
            if (j == 1) out << "  - {name: 'CENTRALITY', value: '30-50%'}" << std::endl;
            if (j == 2) out << "  - {name: 'CENTRALITY', value: '10-30%'}" << std::endl;
            if (j == 3) out << "  - {name: 'CENTRALITY', value: '0-10%'}" << std::endl;

            out << "  values:" << std::endl;

            for (int k = 0; k < nbins; k++) {
                double x, y, ey, sy;

                graphs_hists_aa[i][j].GetPoint(k, x, y);

                ey = graphs_hists_aa[i][j].GetErrorYhigh(k);
                sy = graphs_systs_aa[i][j].GetErrorYhigh(k);

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
    }

    out.close();
}

void hep_data_ratio(std::string hep, bool subsets,
                    std::vector<std::vector<TGraphAsymmErrors>> &graphs_hists_ratio, 
                    std::vector<std::vector<TGraphAsymmErrors>> &graphs_systs_ratio)
{
    std::ofstream out("hep/"s + hep);
    int nrows = (subsets) ? 2 : 1;

    // write x ranges: taking from the first one
    out << "independent_variables:" << std::endl;
    out << "- header: {name: '$\\Delta j$'}" << std::endl;
    out << "  values:" << std::endl;

    int nbins = graphs_hists_ratio[0][0].GetN();

    for (int i = 0; i < nbins; ++i) {
        double x, y, exl, exh;
        graphs_hists_ratio[0][0].GetPoint(i, x, y);
        exl = graphs_hists_ratio[0][0].GetErrorXlow(i);
        exh = graphs_hists_ratio[0][0].GetErrorXhigh(i);

        double low = x - exl;
        double high = x + exh;

        out << "  - {low: " << round(low, -5) << ", high: " << round(high, -5) << "}" << std::endl;
    }

    // write y ranges: loop over graphs
    out << "dependent_variables:" << std::endl;

    for (int i = nrows - 1; i >= 0; --i) {           // nrows
        // ratio results for all centrality classes
        for (size_t j = 0; j < graphs_hists_ratio[0].size(); ++j) {    // npads
            out << "- header: {name: 'PbPb / pp'}" << std::endl;
            out << "  qualifiers:" << std::endl;
            out << "  - {name: 'SQRT(S)/NUCLEON', units: 'GEV', value: '5020'}" << std::endl;
            out << "  - {name: 'JET ALGO', value: 'ANTI-KT R = 0.3'}" << std::endl;
            out << "  - {name: '|JET ETA|', value: '< 1.6'}" << std::endl;
            out << "  - {name: 'PHOTON PT', units: 'GEV', value: '60 - 200'}" << std::endl;
            out << "  - {name: '|PHOTON ETA|', value: '< 1.44'}" << std::endl;
            out << "  - {name: '|PHOTON-JET DPHI|', value: '> 2*PI/3'}" << std::endl;

            if (subsets && i == 1)  out << "  - {name: 'JET PT', units: 'GEV', value: '30 - 60'}" << std::endl;
            if (subsets && i == 0)  out << "  - {name: 'JET PT', units: 'GEV', value: '60 - 100'}" << std::endl;
            if (!subsets)           out << "  - {name: 'JET PT', units: 'GEV', value: '30 - 100'}" << std::endl;

            if (j == 0) out << "  - {name: 'CENTRALITY', value: '50-90%'}" << std::endl;
            if (j == 1) out << "  - {name: 'CENTRALITY', value: '30-50%'}" << std::endl;
            if (j == 2) out << "  - {name: 'CENTRALITY', value: '10-30%'}" << std::endl;
            if (j == 3) out << "  - {name: 'CENTRALITY', value: '0-10%'}" << std::endl;

            out << "  values:" << std::endl;

            for (int k = 0; k < nbins; k++) {
                double x, y, ey, sy;

                graphs_hists_ratio[i][j].GetPoint(k, x, y);

                ey = graphs_hists_ratio[i][j].GetErrorYhigh(k);
                sy = graphs_systs_ratio[i][j].GetErrorYhigh(k);

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
    }

    out.close();
}

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
            result[i][k].SetLineWidth(3*sf);    
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
    axis.Draw();
}

int congratulate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input_aa = conf->get<std::string>("input_aa");
    auto input_pp = conf->get<std::string>("input_pp");

    auto figures = conf->get<std::vector<std::string>>("figures");
    auto name = conf->get<std::string>("name");
    auto types = conf->get<std::vector<int64_t>>("types");

    auto hep = conf->get<std::string>("hep");

    auto ymins = conf->get<std::vector<float>>("ymins");
    auto ymaxs = conf->get<std::vector<float>>("ymaxs");

    auto ratio = conf->get<bool>("ratio");
    auto spectra = conf->get<bool>("spectra");

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
    auto osg_part1 = sel->get<std::vector<int64_t>>("osg_part1");
    auto osg_part2 = sel->get<std::vector<int64_t>>("osg_part2");

    const int nrows = figures.size();
    const int npads = 4;

    std::vector<std::vector<float>> bjet_pt(nrows, std::vector<float>(2, 0));

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

    std::vector<history<TH1F>*> hists_aa(nrows);
    std::vector<history<TH1F>*> systs_aa(nrows);
    std::vector<history<TH1F>*> hists_pp(nrows);
    std::vector<history<TH1F>*> systs_pp(nrows);
    std::vector<history<TH1F>*> hists_ratio(nrows);
    std::vector<history<TH1F>*> systs_ratio(nrows);

    for (int i = 0; i < nrows; ++i) {
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

        set_systematics(hists_aa[i], systs_aa[i]);
        set_systematics(hists_pp[i], systs_pp[i]);
        
        if (ratio)      set_values(hists_ratio[i], systs_ratio[i], hists_aa[i], systs_aa[i], hists_pp[i], systs_pp[i]);
    }

    /* size canvas */
    double panel_size = 500;
    double padding_width_left = 170;
    double padding_width_right = 50;
    double padding_height = 70;

    double factor_x = 0.72;
    double factor_y = (2 * panel_size + (1 - factor_x) * padding_width_left + (1 - factor_x) * padding_width_right) / 4 / panel_size;

    panel_size *= factor_y;
    padding_height *= factor_x;
    padding_width_right *= factor_x;
    padding_width_left *= factor_x;

    double canvas_width = panel_size * 4 + padding_width_left + padding_width_right;
    double canvas_height = panel_size * nrows + padding_height * 2;

    double sf = (panel_size * nrows + padding_height * 2) / (panel_size * 1 + padding_height * 2);
    sf = sf * (panel_size + padding_height * 2) / (640) / factor_x;

    double pad_x0 = padding_width_left / canvas_width;
    double pad_y0 = padding_height / canvas_height;
    double pad_dx = panel_size / canvas_width;
    double pad_dy = panel_size / canvas_height;

    double xmin = bdr[0] + 0.003;
    double xmax = bdr[1];

    gStyle->SetLineScalePS(1);

    /* get graphs */
    auto graphs_hists_aa = get_graph(hists_aa, 0, factor_x);
    auto graphs_systs_aa = get_graph(systs_aa, 0, factor_x);
    auto graphs_hists_pp = get_graph(hists_pp, 1, factor_x);
    auto graphs_systs_pp = get_graph(systs_pp, 1, factor_x);
    auto graphs_hists_ratio = get_graph(hists_ratio, 2, factor_x);
    auto graphs_systs_ratio = get_graph(systs_ratio, 2, factor_x);

    /* declare canvas, pads, axes, and titles */
    TCanvas canvas("canvas", "", canvas_width, canvas_height);

    std::vector<TH2F*> worlds(nrows);
    std::vector<std::vector<TArrow*>> arrows_ratio(nrows, std::vector<TArrow*>(npads));
    std::vector<std::vector<TArrow*>> arrows_pp(nrows, std::vector<TArrow*>(npads));
    std::vector<std::vector<TArrow*>> arrows_aa(nrows, std::vector<TArrow*>(npads));
    std::vector<std::vector<TPad*>> pads(nrows, std::vector<TPad*>(npads));
    std::vector<TGaxis*> axis_x(npads);
    std::vector<TGaxis*> axis_y(nrows);

    TBox* jet_box = new TBox(0, 0, 0.5, 0.06 / factor_y * factor_x * 1.2);
    jet_box->SetBBoxCenterX(0.5);
    jet_box->SetBBoxCenterY(1 - 0.2 / factor_y * factor_x);
    jet_box->SetLineColor(1); 

    for (int i = 0; i < nrows; ++i) {
        worlds[i] = new TH2F("world", ";;", 100, xmin, xmax, 100, ymins[i], ymaxs[i]);
        worlds[i]->SetStats(0);

        axis_y[i] = new TGaxis(pad_x0 + pad_dx * 0, pad_y0 + pad_dy * i, pad_x0 + pad_dx * 0, pad_y0 + pad_dy * (i + 1), ymins[i] * 0.999, ymaxs[i] * 0.999, 510, "S");
        
        set_axis(*axis_y[i], sf);

        for (int j = 0; j < npads; ++j) {
            pads[i][j] = new TPad("P", "", pad_x0 + pad_dx * j, pad_y0 + pad_dy * i, pad_x0 + pad_dx * (j + 1), pad_y0 + pad_dy * (i + 1), 0);
            
            set_pad(*pads[i][j]);

            double arrow_y_pp = 0.0;
            double arrow_y_aa = 0.0;
            double arrow_y_ratio = 0.0;

            if (spectra)    arrow_y_pp = graphs_systs_pp[i][0].GetPointY(0);
            if (spectra)    arrow_y_aa = graphs_systs_aa[i][j].GetPointY(0);
            if (ratio)      arrow_y_ratio = graphs_systs_ratio[i][j].GetPointY(0);

            if (spectra)    arrows_pp[i][j] = new TArrow(0.003, arrow_y_pp, 0.0040, arrow_y_pp, 0.02 / npads / factor_y * factor_x, "<|");
            if (spectra)    arrows_pp[i][j]->SetAngle(40);
            if (spectra)    arrows_pp[i][j]->SetLineWidth(1);
            if (spectra)    arrows_aa[i][j] = new TArrow(0.003, arrow_y_aa, 0.0040, arrow_y_aa, 0.02 / npads / factor_y * factor_x, "<|");
            if (spectra)    arrows_aa[i][j]->SetAngle(40);
            if (spectra)    arrows_aa[i][j]->SetLineWidth(1);
            if (ratio)      arrows_ratio[i][j] = new TArrow(0.003, arrow_y_ratio, 0.0040, arrow_y_ratio, 0.02 / npads / factor_y * factor_x, "<|");
            if (ratio)      arrows_ratio[i][j]->SetAngle(40);
            if (ratio)      arrows_ratio[i][j]->SetLineWidth(1);
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
    latex.SetTextSize(0.07/sf);
    latex.SetTextAlign(22);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0 + pad_dx * 0.5, pad_y0 * 0.5, "#Deltaj");
    latex.DrawLatex(pad_x0 + pad_dx * 1.5, pad_y0 * 0.5, "#Deltaj");
    latex.DrawLatex(pad_x0 + pad_dx * 2.5, pad_y0 * 0.5, "#Deltaj");
    latex.DrawLatex(pad_x0 + pad_dx * 3.5, pad_y0 * 0.5, "#Deltaj");

    latex.SetTextFont(42);
    latex.SetTextSize(0.07/sf);
    latex.SetTextAlign(22);
    latex.SetTextAngle(90);
    if (ratio)      latex.DrawLatex(pad_x0 * 0.4, pad_y0 + pad_dy * nrows * 0.5, "PbPb / pp");
    if (spectra)    latex.DrawLatex(pad_x0 * 0.4, pad_y0 + pad_dy * nrows * 0.5, "#frac{1}{N_{#gamma}} #frac{dN_{j#gamma}}{d#Deltaj}");

    latex.SetTextFont(62);
    latex.SetTextSize(0.07/sf);
    latex.SetTextAlign(11);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0, pad_y0 * 1.15 + pad_dy * nrows, text_cms.c_str());

    latex.SetTextFont(42);
    latex.SetTextSize(0.055/sf);
    latex.SetTextAlign(31);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0 + pad_dx * 4, pad_y0 * 1.15 + pad_dy * nrows, text_system.c_str());

    TGraph line;
    if (ratio)      line.SetPoint(0, xmin, 1);
    if (ratio)      line.SetPoint(1, xmax, 1);
    if (spectra)    line.SetPoint(0, xmin, 0);
    if (spectra)    line.SetPoint(1, xmax, 0);
    line.SetLineStyle(kDashed);

    /* declare legend */
    double legend_y_min = (ratio) ? 1 - 0.34 / factor_y * factor_x : 1 - 0.41 / factor_y * factor_x;
    double legend_y_max = (ratio) ? 1 - 0.27 / factor_y * factor_x : 1 - 0.27 / factor_y * factor_x;
    double legend_x_min = (ratio) ? 0.05 / factor_y * factor_x : 0.50 / factor_y * factor_x;
    double legend_x_max = (ratio) ? 0.35 / factor_y * factor_x : 0.80 / factor_y * factor_x;

    TLegend legend(legend_x_min, legend_y_min, legend_x_max, legend_y_max);
    legend.SetTextFont(42);
    legend.SetTextSize(0.05 / factor_y * factor_x);
    legend.SetFillStyle(0);
    legend.SetBorderSize(0);
    if (ratio)      legend.AddEntry(&graphs_systs_ratio[0][0], "PbPb/pp", "plf");
    if (spectra)    legend.AddEntry(&graphs_systs_aa[0][0], "PbPb", "plf");
    if (spectra)    legend.AddEntry(&graphs_systs_pp[0][0], "pp", "plf");

    for (int i = 0; i < nrows; i++) {
        for(int j = 0; j < npads; j++)
        {
            pads[i][j]->cd();

            worlds[i]->Draw("axis");

            if (ratio)      graphs_systs_ratio[i][j].Draw("same 2");
            if (ratio)      graphs_hists_ratio[i][j].Draw("same PZ");
            if (spectra)    graphs_systs_aa[i][j].Draw("same 2");
            if (spectra)    graphs_systs_pp[i][0].Draw("same 2");
            if (spectra)    graphs_hists_aa[i][j].Draw("same PZ");
            if (spectra)    graphs_hists_pp[i][0].Draw("same PZ");

            line.Draw("l");

            if (ratio)      arrows_ratio[i][j]->Draw();
            if (spectra)    arrows_aa[i][j]->Draw();
            if (spectra)    arrows_pp[i][j]->Draw();
        }

        auto text_jet_pt = to_text(bjet_pt[i][0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[i][1]) + " GeV"s;
    
        pads[i][0]->cd();
        latex.SetTextAlign(22);
        latex.SetTextSize(0.06 / factor_y * factor_x);
        latex.DrawLatex(0.5, 1 - 0.2 / factor_y * factor_x, (text_jet_pt).c_str());
        jet_box->Draw("l")

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

    pads[nrows-1][0]->cd();
    if (nrows == 2 || !ratio) legend.Draw();

    pads[0][0]->cd();
    latex.SetTextSize(0.05 / factor_y * factor_x);
    latex.SetTextAlign(11);

    if (spectra)    latex.DrawLatex(0.05 / factor_y * factor_x, 1 - 0.55 / factor_y * factor_x, (text_photon_eta).c_str());
    if (spectra)    latex.DrawLatex(0.05 / factor_y * factor_x, 1 - 0.63 / factor_y * factor_x, (text_photon_pt).c_str());
    if (ratio)      latex.DrawLatex(0.05 / factor_y * factor_x, 1 - 0.32 / factor_y * factor_x, (text_photon_pt).c_str());
    if (ratio)      latex.DrawLatex(0.05 / factor_y * factor_x, 1 - 0.4 / factor_y * factor_x, (text_photon_eta).c_str());

    pads[0][1]->cd();
    if (spectra)    latex.DrawLatex(0.05 / factor_y * factor_x, 1 - 0.55 / factor_y * factor_x, (text_jet_alg).c_str());
    if (spectra)    latex.DrawLatex(0.05 / factor_y * factor_x, 1 - 0.63 / factor_y * factor_x, (text_dphi + ", " + text_jet_eta).c_str());
    if (ratio)      latex.DrawLatex(0.05 / factor_y * factor_x, 1 - 0.32 / factor_y * factor_x, (text_dphi + ", " + text_jet_eta).c_str());
    if (ratio)      latex.DrawLatex(0.05 / factor_y * factor_x, 1 - 0.4 / factor_y * factor_x, (text_jet_alg).c_str());

    pads[0][2]->cd();
    if (nrows != 2 && ratio) legend.Draw();

    if (ratio)      canvas.SaveAs((set + "_final_ratio_" + name + "_log.pdf").c_str());
    if (spectra)    canvas.SaveAs((set + "_final_spectra_" + name + "_log.pdf").c_str());

    if (ratio)      canvas.SaveAs((set + "_final_ratio_" + name + "_log.C").c_str());
    if (spectra)    canvas.SaveAs((set + "_final_spectra_" + name + "_log.C").c_str());

    in(output, []() {});

    if (ratio)      hep_data_ratio(hep, nrows == 2, graphs_hists_ratio, graphs_systs_ratio);
    if (spectra)    hep_data_spectra(hep, nrows == 2, graphs_hists_aa, graphs_systs_aa, graphs_hists_pp, graphs_systs_pp);

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return congratulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}