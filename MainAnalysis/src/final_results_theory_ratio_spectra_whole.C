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

std::vector<TGraphAsymmErrors> get_graph(std::vector<history<TH1F>*> h, int format, int system, double ncols)
{
    std::vector<TGraphAsymmErrors> result(h.size());

    static int style[3] = {20, 20, 20};
    static int color[3] = {TColor::GetColor("#5790FC"), TColor::GetColor("#E42536"), TColor::GetColor("#9C9C9C")};

    for (size_t i = 0; i < h.size(); ++i) {
        if (system == 1) {
            for (int j = 1; j <= (*h[0])[3]->GetNbinsX(); ++j) {
                double x = (*h[0])[3]->GetBinCenter(j);
                double dx = (*h[0])[3]->GetBinWidth(j)/2;

                result[i].SetPoint(j - 1, x, (*h[0])[3]->GetBinContent(j));
                result[i].SetPointError(j - 1, dx, dx, (*h[0])[3]->GetBinError(j), (*h[0])[3]->GetBinError(j));
            }
        }
        else {
            for (int j = 1; j <= (*h[0])[0]->GetNbinsX(); ++j) {
                double x = (*h[0])[0]->GetBinCenter(j);
                double dx = (*h[0])[0]->GetBinWidth(j)/2;

                result[i].SetPoint(j - 1, x, (*h[0])[0]->GetBinContent(j));
                result[i].SetPointError(j - 1, dx, dx, (*h[0])[0]->GetBinError(j), (*h[0])[0]->GetBinError(j));
            }
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
        result[i].SetLineWidth(15.0 / ncols);
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

    auto input_aa_jewel = conf->get<std::string>("input_aa_jewel");
    auto input_aa_jewel_no_recoil = conf->get<std::string>("input_aa_jewel_no_recoil");
    auto input_aa_pyquen = conf->get<std::string>("input_aa_pyquen");
    auto input_aa_pyquen_no_wide = conf->get<std::string>("input_aa_pyquen_no_wide");
    auto input_aa_hybrid = conf->get<std::string>("input_aa_hybrid");
    auto input_pp_jewel = conf->get<std::string>("input_pp_jewel");
    auto input_pp_pyquen = conf->get<std::string>("input_pp_pyquen");
    auto input_pp_pythia = conf->get<std::string>("input_pp_pythia");
    auto input_pp_hybrid = conf->get<std::string>("input_pp_hybrid");
    auto input_ratio_hybrid = conf->get<std::string>("input_ratio_hybrid");

    auto tag_aa_jewel = conf->get<std::string>("tag_aa_jewel");
    auto tag_aa_jewel_no_recoil = conf->get<std::string>("tag_aa_jewel_no_recoil");
    auto tag_aa_pyquen = conf->get<std::string>("tag_aa_pyquen");
    auto tag_aa_pyquen_no_wide = conf->get<std::string>("tag_aa_pyquen_no_wide");
    auto tag_aa_hybrid_no_elastic_no_wake = conf->get<std::string>("tag_aa_hybrid_no_elastic_no_wake");
    auto tag_aa_hybrid_elastic_no_wake = conf->get<std::string>("tag_aa_hybrid_elastic_no_wake");
    auto tag_aa_hybrid_no_elastic_wake = conf->get<std::string>("tag_aa_hybrid_no_elastic_wake");
    auto tag_aa_hybrid_elastic_wake = conf->get<std::string>("tag_aa_hybrid_elastic_wake");
    auto tag_pp_jewel = conf->get<std::string>("tag_pp_jewel");
    auto tag_pp_pyquen = conf->get<std::string>("tag_pp_pyquen");
    auto tag_pp_pythia = conf->get<std::string>("tag_pp_pythia");
    auto tag_pp_hybrid = conf->get<std::string>("tag_pp_hybrid");
    auto tag_ratio_hybrid_no_elastic_no_wake = conf->get<std::string>("tag_ratio_hybrid_no_elastic_no_wake");
    auto tag_ratio_hybrid_elastic_no_wake = conf->get<std::string>("tag_ratio_hybrid_elastic_no_wake");
    auto tag_ratio_hybrid_no_elastic_wake = conf->get<std::string>("tag_ratio_hybrid_no_elastic_wake");
    auto tag_ratio_hybrid_elastic_wake = conf->get<std::string>("tag_ratio_hybrid_elastic_wake");

    auto figures = conf->get<std::vector<std::string>>("figures");
    auto theory_figures = conf->get<std::vector<std::string>>("theory_figures");

    auto name = conf->get<std::string>("name");
    auto types = conf->get<std::vector<int64_t>>("types");

    auto supplementary = conf->get<bool>("supplementary");

    auto ymins = conf->get<std::vector<float>>("ymins");
    auto ymaxs = conf->get<std::vector<float>>("ymaxs");

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

    const int nsystems = figures.size();
    const int ncols = 2;

    std::vector<std::vector<float>> bjet_pt(nsystems, std::vector<float>(2, 0));

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();
std::cout << __LINE__ << std::endl;
    /* open input files */
    auto file_aa = new TFile((base + input_aa).data(), "read");
    auto file_pp = new TFile((base + input_pp).data(), "read");

    auto file_aa_jewel = new TFile((base + input_aa_jewel).data(), "read");
    auto file_aa_jewel_no_recoil = new TFile((base + input_aa_jewel_no_recoil).data(), "read");
    auto file_aa_pyquen = new TFile((base + input_aa_pyquen).data(), "read");
    auto file_aa_pyquen_no_wide = new TFile((base + input_aa_pyquen_no_wide).data(), "read");
    auto file_aa_hybrid = new TFile((base + input_aa_hybrid).data(), "read");
    auto file_pp_jewel = new TFile((base + input_pp_jewel).data(), "read");
    auto file_pp_pyquen = new TFile((base + input_pp_pyquen).data(), "read");
    auto file_pp_pythia = new TFile((base + input_pp_pythia).data(), "read");
    auto file_pp_hybrid = new TFile((base + input_pp_hybrid).data(), "read");
    auto file_ratio_hybrid = new TFile((base + input_ratio_hybrid).data(), "read");
std::cout << __LINE__ << std::endl;
    /* define kinematics and luminosity */
    auto text_system = "(5.02 TeV)"s;
    auto text_cms = (supplementary) ? "CMS #scale[0.8]{#font[52]{Supplementary}}"s : "CMS"s;
    auto text_photon_pt = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV"s;
    auto text_photon_eta = "|#eta^{#gamma}| < "s + to_text(photon_eta_abs);
    auto text_dphi = "#Delta#phi_{j#gamma} > #frac{"s + to_text(dphi_min_numerator) + "#pi}{"s + to_text(dphi_min_denominator) + "}"s;
    auto text_jet_alg = "anti-k_{T} R = 0.3"s;
    auto text_jet_eta = "|#eta^{jet}| < "s + to_text(jet_eta_abs);

    if (system == 0) text_system = "PbPb 1.69 nb^{-1} " + text_system;
    if (system == 1) text_system = "PbPb 1.69 nb^{-1}, pp 302 pb^{-1} " + text_system;
std::cout << __LINE__ << std::endl;
    std::vector<history<TH1F>*> hists_aa(nsystems);
    std::vector<history<TH1F>*> systs_aa(nsystems);
    std::vector<history<TH1F>*> hists_pp(nsystems);
    std::vector<history<TH1F>*> systs_pp(nsystems);
    std::vector<history<TH1F>*> hists_ratio(nsystems);
    std::vector<history<TH1F>*> systs_ratio(nsystems);

    std::vector<history<TH1F>*> hists_aa_jewel(nsystems);
    std::vector<history<TH1F>*> hists_aa_jewel_no_recoil(nsystems);
    std::vector<history<TH1F>*> hists_aa_pyquen(nsystems);
    std::vector<history<TH1F>*> hists_aa_pyquen_no_wide(nsystems);
    std::vector<history<TH1F>*> hists_aa_hybrid_elastic_no_wake(nsystems);
    std::vector<history<TH1F>*> hists_aa_hybrid_no_elastic_no_wake(nsystems);
    std::vector<history<TH1F>*> hists_aa_hybrid_elastic_wake(nsystems);
    std::vector<history<TH1F>*> hists_aa_hybrid_no_elastic_wake(nsystems);
    std::vector<history<TH1F>*> hists_pp_jewel(nsystems);
    std::vector<history<TH1F>*> hists_pp_pyquen(nsystems);
    std::vector<history<TH1F>*> hists_pp_pythia(nsystems);
    std::vector<history<TH1F>*> hists_pp_hybrid(nsystems);
    std::vector<history<TH1F>*> hists_ratio_jewel(nsystems);
    std::vector<history<TH1F>*> hists_ratio_jewel_no_recoil(nsystems);
    std::vector<history<TH1F>*> hists_ratio_pyquen(nsystems);
    std::vector<history<TH1F>*> hists_ratio_pyquen_no_wide(nsystems);
    std::vector<history<TH1F>*> hists_ratio_hybrid_elastic_no_wake(nsystems);
    std::vector<history<TH1F>*> hists_ratio_hybrid_no_elastic_no_wake(nsystems);
    std::vector<history<TH1F>*> hists_ratio_hybrid_elastic_wake(nsystems);
    std::vector<history<TH1F>*> hists_ratio_hybrid_no_elastic_wake(nsystems);
std::cout << __LINE__ << std::endl;
    for (int i = 0; i < nsystems; ++i) {
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
std::cout << __LINE__ << std::endl;
        /* get histograms */
        hists_aa[i] = new history<TH1F>(file_aa, "aa_base_aa_nominal_s_pure_raw_sub_" + figures[i]);
        systs_aa[i] = new history<TH1F>(file_aa, "aa_total_base_aa_nominal_s_pure_raw_sub_" + figures[i]);
        hists_pp[i] = new history<TH1F>(file_pp, "pp_base_pp_nominal_s_pure_raw_sub_" + figures[i]);
        systs_pp[i] = new history<TH1F>(file_pp, "pp_total_base_pp_nominal_s_pure_raw_sub_" + figures[i]);
        hists_ratio[i] = new history<TH1F>(*hists_aa[i], "hist");
        systs_ratio[i] = new history<TH1F>(*systs_aa[i], "syst");
std::cout << __LINE__ << std::endl;
        hists_aa_jewel[i] = new history<TH1F>(file_aa_jewel, tag_aa_jewel + "_dr" + theory_figures[i]);
        hists_aa_jewel_no_recoil[i] = new history<TH1F>(file_aa_jewel_no_recoil, tag_aa_jewel_no_recoil + "_dr" + theory_figures[i]);
        hists_aa_pyquen[i] = new history<TH1F>(file_aa_pyquen, tag_aa_pyquen + "_dr" + theory_figures[i]);
        hists_aa_pyquen_no_wide[i] = new history<TH1F>(file_aa_pyquen_no_wide, tag_aa_pyquen_no_wide + "_dr" + theory_figures[i]);
        hists_aa_hybrid_elastic_no_wake[i] = new history<TH1F>(file_aa_hybrid, tag_aa_hybrid_elastic_no_wake + "_dr" + theory_figures[i]);
        hists_aa_hybrid_no_elastic_no_wake[i] = new history<TH1F>(file_aa_hybrid, tag_aa_hybrid_no_elastic_no_wake + "_dr" + theory_figures[i]);
        hists_aa_hybrid_elastic_wake[i] = new history<TH1F>(file_aa_hybrid, tag_aa_hybrid_elastic_wake + "_dr" + theory_figures[i]);
        hists_aa_hybrid_no_elastic_wake[i] = new history<TH1F>(file_aa_hybrid, tag_aa_hybrid_no_elastic_wake + "_dr" + theory_figures[i]);
        hists_pp_jewel[i] = new history<TH1F>(file_pp_jewel, tag_pp_jewel + "_dr" + theory_figures[i]);
        hists_pp_pyquen[i] = new history<TH1F>(file_pp_pyquen, tag_pp_pyquen + "_dr" + theory_figures[i]);
        hists_pp_pythia[i] = new history<TH1F>(file_pp_pythia, tag_pp_pythia + theory_figures[i]);
        hists_pp_hybrid[i] = new history<TH1F>(file_pp_hybrid, tag_pp_hybrid + "_dr" + theory_figures[i]);
        hists_ratio_jewel[i] = new history<TH1F>(*hists_aa_jewel[i], "hist_jewel");
        hists_ratio_jewel_no_recoil[i] = new history<TH1F>(*hists_aa_jewel_no_recoil[i], "hist_jewel_no_recoil");
        hists_ratio_pyquen[i] = new history<TH1F>(*hists_aa_pyquen[i], "hist_pyquen");
        hists_ratio_pyquen_no_wide[i] = new history<TH1F>(*hists_aa_pyquen_no_wide[i], "hist_pyquen_no_wide");
        hists_ratio_hybrid_elastic_no_wake[i] = new history<TH1F>(file_ratio_hybrid, tag_ratio_hybrid_elastic_no_wake + "_dr" + theory_figures[i]);
        hists_ratio_hybrid_no_elastic_no_wake[i] = new history<TH1F>(file_ratio_hybrid, tag_ratio_hybrid_no_elastic_no_wake + "_dr" + theory_figures[i]);
        hists_ratio_hybrid_elastic_wake[i] = new history<TH1F>(file_ratio_hybrid, tag_ratio_hybrid_elastic_wake + "_dr" + theory_figures[i]);
        hists_ratio_hybrid_no_elastic_wake[i] = new history<TH1F>(file_ratio_hybrid, tag_ratio_hybrid_no_elastic_wake + "_dr" + theory_figures[i]);

        set_systematics(hists_aa[i], systs_aa[i]);
        set_systematics(hists_pp[i], systs_pp[i]);
        
        if (system == 1) {
            set_values(hists_ratio[i], systs_ratio[i], hists_aa[i], systs_aa[i], hists_pp[i], systs_pp[i]);
            set_values(hists_ratio_jewel[i], hists_aa_jewel[i], hists_pp_jewel[i]);
            set_values(hists_ratio_jewel_no_recoil[i], hists_aa_jewel_no_recoil[i], hists_pp_jewel[i]);
            set_values(hists_ratio_pyquen[i], hists_aa_pyquen[i], hists_pp_pyquen[i]);
            set_values(hists_ratio_pyquen_no_wide[i], hists_aa_pyquen_no_wide[i], hists_pp_pyquen[i]);
        }
    }
std::cout << __LINE__ << std::endl;
    auto graphs_hists_aa = get_graph(hists_aa, 2, 0, ncols);
    auto graphs_systs_aa = get_graph(systs_aa, 2, 0, ncols);
    auto graphs_hists_pp = get_graph(hists_pp, 2, 0, ncols);
    auto graphs_systs_pp = get_graph(systs_pp, 2, 0, ncols);
    auto graphs_hists_ratio = get_graph(hists_ratio, 2, 1, ncols);
    auto graphs_systs_ratio = get_graph(systs_ratio, 2, 1, ncols);
std::cout << __LINE__ << std::endl;
    auto graphs_hists_aa_jewel = get_graph(hists_aa_jewel, 0, ncols);
    auto graphs_hists_aa_jewel_no_recoil = get_graph(hists_aa_jewel_no_recoil, 1, ncols);
    auto graphs_hists_aa_pyquen = get_graph(hists_aa_pyquen, 2, ncols);
    auto graphs_hists_aa_pyquen_no_wide = get_graph(hists_aa_pyquen_no_wide, 3, ncols);
    auto graphs_hists_aa_hybrid_elastic_no_wake = get_graph(hists_aa_hybrid_elastic_no_wake, 5, ncols);
    auto graphs_hists_aa_hybrid_no_elastic_no_wake = get_graph(hists_aa_hybrid_no_elastic_no_wake, 6, ncols);
    auto graphs_hists_aa_hybrid_elastic_wake = get_graph(hists_aa_hybrid_elastic_wake, 7, ncols);
    auto graphs_hists_aa_hybrid_no_elastic_wake = get_graph(hists_aa_hybrid_no_elastic_wake, 8, ncols);
    auto graphs_hists_pp_jewel = get_graph(hists_pp_jewel, 0, ncols);
    auto graphs_hists_pp_pyquen = get_graph(hists_pp_pyquen, 2, ncols);
    auto graphs_hists_pp_pythia = get_graph(hists_pp_pythia, 4, ncols);
    auto graphs_hists_pp_hybrid = get_graph(hists_pp_hybrid, 5, ncols);
    auto graphs_hists_ratio_jewel = get_graph(hists_ratio_jewel, 0, ncols);
    auto graphs_hists_ratio_jewel_no_recoil = get_graph(hists_ratio_jewel_no_recoil, 1, ncols);
    auto graphs_hists_ratio_pyquen = get_graph(hists_ratio_pyquen, 2, ncols);
    auto graphs_hists_ratio_pyquen_no_wide = get_graph(hists_ratio_pyquen_no_wide, 3, ncols);
    auto graphs_hists_ratio_hybrid_elastic_no_wake = get_graph(hists_ratio_hybrid_elastic_no_wake, 5, ncols);
    auto graphs_hists_ratio_hybrid_no_elastic_no_wake = get_graph(hists_ratio_hybrid_no_elastic_no_wake, 6, ncols);
    auto graphs_hists_ratio_hybrid_elastic_wake = get_graph(hists_ratio_hybrid_elastic_wake, 7, ncols);
    auto graphs_hists_ratio_hybrid_no_elastic_wake = get_graph(hists_ratio_hybrid_no_elastic_wake, 8, ncols);

    /* size canvas */
    double panel_size = 500;
    double padding_width_left = 170;
    double padding_width_right = 50;
    double padding_height = 70;

    double canvas_width = panel_size * ncols + padding_width_left + padding_width_right;
    double canvas_height = panel_size * 1 + padding_height * 2;

    double sf = 1;
std::cout << __LINE__ << std::endl;
    double pad_x0 = padding_width_left / canvas_width;
    double pad_y0 = padding_height / canvas_height;
    double pad_dx = panel_size / canvas_width;
    double pad_dy = panel_size / canvas_height;

    double xmin = bdr[0] + 0.003;
    double xmax = bdr[1];

    gStyle->SetLineScalePS(1);
std::cout << __LINE__ << std::endl;
    /* declare canvas, pads, axes, and titles */
    TCanvas canvas("canvas", "", canvas_width, canvas_height);

    TLatex latex;
    latex.SetNDC();

    std::vector<TH2F*> worlds(ncols);
    std::vector<TArrow*> arrows(ncols);
    std::vector<TPad*> pads(ncols);
    std::vector<TGaxis*> axis_x(ncols);
    std::vector<TGaxis*> axis_y(1);
    std::vector<TPaveText*> boxes(nsystems);
std::cout << __LINE__ << std::endl;
    for (int i = 0; i < ncols; ++i) {
        worlds[i] = new TH2F("world", ";;", 100, xmin, xmax, 100, ymins[0], ymaxs[0]);
        worlds[i]->SetStats(0);
std::cout << __LINE__ << std::endl;
        pads[i] = new TPad("P1", "", pad_x0 + pad_dx * i, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * (i + 1), pad_y0 + pad_dy * 1, 0);
        
        set_pad(*pads[i]);

        axis_x[i] = new TGaxis(pad_x0 + pad_dx * i, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * (i + 1), pad_y0 + pad_dy * 0, xmin, xmax, 510, "SG");
std::cout << __LINE__ << std::endl;
        set_axis(*axis_x[i], sf);

        double arrow_y = 0.0;
        
        if (system == 1) arrow_y = graphs_systs_ratio[0].GetPointY(0);
        if (system == 0) arrow_y = graphs_systs_aa[0].GetPointY(0);
std::cout << __LINE__ << std::endl;
        arrows[0] = new TArrow(0.003, arrow_y, 0.0040, arrow_y, 0.02 / ncols, "<|");
        arrows[0]->SetAngle(40);
        arrows[0]->SetLineWidth(1);

        canvas.cd();

        latex.SetTextFont(42);
        latex.SetTextSize(0.07/sf);
        latex.SetTextAlign(22);
        latex.SetTextAngle(0);
        latex.DrawLatex(pad_x0 + pad_dx * (i + 0.5), pad_y0 * 0.5, "#Deltaj");
    }

    axis_y[0] = new TGaxis(pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 1, ymins[0] * 0.999, ymaxs[0] * 0.999, 510, "S");
std::cout << __LINE__ << std::endl;
    set_axis(*axis_y[0], sf);

    latex.SetTextFont(42);
    latex.SetTextSize(0.07/sf);
    latex.SetTextAlign(22);
    latex.SetTextAngle(90);
    if (system == 1)    latex.DrawLatex(pad_x0 * 0.4, pad_y0 + pad_dy * 0.5, "PbPb / pp");
    if (system == 0)    latex.DrawLatex(pad_x0 * 0.4, pad_y0 + pad_dy * 0.5, "#frac{1}{N_{#gamma}} #frac{dN_{j#gamma}}{d#Deltaj}");

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
std::cout << __LINE__ << std::endl;
    TGraph line;
    if (system == 1)    line.SetPoint(0, xmin, 1);
    if (system == 1)    line.SetPoint(1, xmax, 1);
    if (system == 0)    line.SetPoint(0, xmin, 0);
    if (system == 0)    line.SetPoint(1, xmax, 0);
    line.SetLineStyle(kDashed);

    /* declare legend */
    double legend_y_min = 0;
    double legend_y_max = 0;
    double legend_x_min = 0;
    double legend_x_max = 0;

    if (system == 0)    legend_y_min = 0.64;
    if (system == 0)    legend_y_max = 0.85;
    if (system == 0)    legend_x_min = 0.03;
    if (system == 0)    legend_x_max = 0.33;

    if (system == 0)    legend_y_min = 0.55;
    if (system == 0)    legend_y_max = 0.83;
    if (system == 0)    legend_x_min = 0.03;
    if (system == 0)    legend_x_max = 0.33;
std::cout << __LINE__ << std::endl;
    if (system == 1)    legend_y_min = 0.31;
    if (system == 1)    legend_y_max = 0.66;
    if (system == 1)    legend_x_min = 0.03;
    if (system == 1)    legend_x_max = 0.33;

    if (system == 1)    legend_y_min = 0.34;
    if (system == 1)    legend_y_max = 0.62;
    if (system == 1)    legend_x_min = 0.36;
    if (system == 1)    legend_x_max = 0.66;

std::cout << __LINE__ << std::endl;
    TLegend legend_part1(legend_x_min, legend_y_min, legend_x_max, legend_y_max);
    legend_part1.SetTextFont(42);
    legend_part1.SetTextSize(0.055);
    legend_part1.SetFillStyle(0);
    legend_part1.SetBorderSize(0);
    if (system == 0)   legend_part1.AddEntry(&graphs_systs_aa[0], "CMS PbPb", "plf");
    if (system == 0)   legend_part1.AddEntry(&graphs_hists_aa_jewel_no_recoil[0], "JEWEL, no recoil", "lf");
    if (system == 0)   legend_part1.AddEntry(&graphs_hists_aa_jewel[0], "JEWEL, recoil", "lf");
    if (system == 0)   legend_part1.AddEntry(&graphs_hists_aa_hybrid_no_elastic_no_wake[0], "no elastic, no wake", "lf");
    if (system == 0)   legend_part1.AddEntry(&graphs_hists_aa_hybrid_no_elastic_wake[0], "no elastic, wake", "lf");
    if (system == 0)   legend_part1.AddEntry(&graphs_hists_aa_hybrid_elastic_no_wake[0], "elastic, no wake", "lf");
    if (system == 0)   legend_part1.AddEntry(&graphs_hists_aa_hybrid_elastic_wake[0], "elastic, wake", "lf");
    if (system == 1)    legend_part1.AddEntry(&graphs_systs_ratio[0], "CMS data", "plf");
    if (system == 1)    legend_part1.AddEntry(&graphs_hists_ratio_jewel_no_recoil[0], "JEWEL, no recoil", "lf");
    if (system == 1)    legend_part1.AddEntry(&graphs_hists_ratio_jewel[0], "JEWEL, recoil", "lf");
    if (system == 1)    legend_part1.AddEntry(&graphs_hists_ratio_pyquen_no_wide[0], "PYQUEN", "lf");
    if (system == 1)    legend_part1.AddEntry(&graphs_hists_ratio_pyquen[0], "PYQUEN, wide angle", "lf");
    if (system == 1)    legend_part1.AddEntry(&graphs_hists_ratio_hybrid_no_elastic_no_wake[0], "no elastic, no wake", "lf");
    if (system == 1)    legend_part1.AddEntry(&graphs_hists_ratio_hybrid_no_elastic_wake[0], "no elastic, wake", "lf");
    if (system == 1)    legend_part1.AddEntry(&graphs_hists_ratio_hybrid_elastic_no_wake[0], "elastic, no wake", "lf");
    if (system == 1)    legend_part1.AddEntry(&graphs_hists_ratio_hybrid_elastic_wake[0], "elastic, wake", "lf");

    if (system == 0)    legend_y_min = legend_y_max - 0.14;
    if (system == 0)    legend_y_max = legend_y_max;
    if (system == 0)    legend_x_min = legend_x_max + 0.15;
    if (system == 0)    legend_x_max = legend_x_min + 0.30;

    if (system == 1)    legend_y_min = legend_y_min;
    if (system == 1)    legend_y_max = legend_y_min + 0.14;
    if (system == 1)    legend_x_min = legend_x_max;
    if (system == 1)    legend_x_max = legend_x_min + 0.30;
std::cout << __LINE__ << std::endl;
    TLegend legend_part2(legend_x_min, legend_y_min, legend_x_max, legend_y_max);
    legend_part2.SetTextFont(42);
    legend_part2.SetTextSize(0.055);
    legend_part2.SetFillStyle(0);
    legend_part2.SetBorderSize(0);
    if (system == 0)   legend_part2.AddEntry(&graphs_hists_aa_pyquen_no_wide[0], "PYQUEN", "lf");
    if (system == 0)   legend_part2.AddEntry(&graphs_hists_aa_pyquen[0], "PYQUEN, wide angle", "lf");
    if (system == 0)   legend_part2.AddEntry(&graphs_systs_aa[0], "CMS PbPb", "plf");
    if (system == 1)   legend_part2.AddEntry(&graphs_systs_ratio[0], "CMS data", "plf");

    pads[0]->cd();

    worlds[0]->Draw("axis");
std::cout << __LINE__ << std::endl;
    if (system == 1)    graphs_systs_ratio[0].Draw("same 2");
    if (system == 1)    graphs_hists_ratio_hybrid_no_elastic_no_wake[0].Draw("same 3");
    if (system == 1)    graphs_hists_ratio_hybrid_no_elastic_no_wake[0].Draw("same lX");
    if (system == 1)    graphs_hists_ratio_hybrid_no_elastic_wake[0].Draw("same 3");
    if (system == 1)    graphs_hists_ratio_hybrid_no_elastic_wake[0].Draw("same lX");
    if (system == 1)    graphs_hists_ratio_hybrid_elastic_no_wake[0].Draw("same 3");
    if (system == 1)    graphs_hists_ratio_hybrid_elastic_no_wake[0].Draw("same lX");
    if (system == 1)    graphs_hists_ratio_hybrid_elastic_wake[0].Draw("same 3");
    if (system == 1)    graphs_hists_ratio_hybrid_elastic_wake[0].Draw("same lX");
    if (system == 1)    graphs_hists_ratio[0].Draw("same PZ");
std::cout << __LINE__ << std::endl;
    if (system == 0)    graphs_systs_aa[0].Draw("same 2");
    if (system == 0)    graphs_hists_aa_hybrid_no_elastic_wake[0].Draw("same 3");
    if (system == 0)    graphs_hists_aa_hybrid_no_elastic_wake[0].Draw("same lX");
    if (system == 0)    graphs_hists_aa_hybrid_elastic_wake[0].Draw("same 3");
    if (system == 0)    graphs_hists_aa_hybrid_elastic_wake[0].Draw("same lX");
    if (system == 0)    graphs_hists_aa_hybrid_no_elastic_no_wake[0].Draw("same 3");
    if (system == 0)    graphs_hists_aa_hybrid_no_elastic_no_wake[0].Draw("same lX");
    if (system == 0)    graphs_hists_aa_hybrid_elastic_no_wake[0].Draw("same 3");
    if (system == 0)    graphs_hists_aa_hybrid_elastic_no_wake[0].Draw("same lX");
    if (system == 0)    graphs_hists_aa[0].Draw("same PZ");
std::cout << __LINE__ << std::endl;
    line.Draw("l");
    arrows[0]->Draw();
std::cout << __LINE__ << std::endl;
    auto text_jet_pt = to_text(bjet_pt[0][0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[0][1]) + " GeV"s;
    boxes[0] = new TPaveText(0.22, 1 - 0.16, 0.78, 1 - 0.05, "NDC");
    boxes[0]->SetBorderSize(1);
    boxes[0]->SetTextFont(42);
    boxes[0]->SetTextSize(0.06);
    boxes[0]->SetLineWidth(3);
    boxes[0]->SetFillColor(0);
    boxes[0]->SetShadowColor(0);
    boxes[0]->AddText((text_jet_pt).c_str());
    boxes[0]->Draw("same");
std::cout << __LINE__ << std::endl;
    if (system == 0)    latex.SetTextAlign(21);
    if (system == 0)    latex.SetTextSize(0.06);
    if (system == 0)    latex.DrawLatex(0.5, 0.03, "Cent. 0-10%");

    if (system == 1)   latex.SetTextAlign(21);
    if (system == 1)   latex.SetTextSize(0.06);
    if (system == 1)   latex.DrawLatex(0.5, 0.10, "Cent. 0-10%");
std::cout << __LINE__ << std::endl;
    pads[1]->cd();

    worlds[1]->Draw("axis");
std::cout << __LINE__ << std::endl;
    if (system == 1)    graphs_systs_ratio[0].Draw("same 2");
    if (system == 1)    graphs_hists_ratio_pyquen_no_wide[0].Draw("same 3");
    if (system == 1)    graphs_hists_ratio_pyquen_no_wide[0].Draw("same lX");
    if (system == 1)    graphs_hists_ratio_pyquen[0].Draw("same 3");
    if (system == 1)    graphs_hists_ratio_pyquen[0].Draw("same lX");
    if (system == 1)    graphs_hists_ratio_jewel[0].Draw("same 3");
    if (system == 1)    graphs_hists_ratio_jewel[0].Draw("same lX");
    if (system == 1)    graphs_hists_ratio_jewel_no_recoil[0].Draw("same 3");
    if (system == 1)    graphs_hists_ratio_jewel_no_recoil[0].Draw("same lX");
    if (system == 1)    graphs_hists_ratio[0].Draw("same PZ");

    if (system == 0)    graphs_systs_aa[0].Draw("same 2");
    if (system == 0)    graphs_hists_aa_pyquen_no_wide[0].Draw("same 3");
    if (system == 0)    graphs_hists_aa_pyquen_no_wide[0].Draw("same lX");
    if (system == 0)    graphs_hists_aa_pyquen[0].Draw("same 3");
    if (system == 0)    graphs_hists_aa_pyquen[0].Draw("same lX");
    if (system == 0)    graphs_hists_aa_jewel[0].Draw("same 3");
    if (system == 0)    graphs_hists_aa_jewel[0].Draw("same lX");
    if (system == 0)    graphs_hists_aa_jewel_no_recoil[0].Draw("same 3");
    if (system == 0)    graphs_hists_aa_jewel_no_recoil[0].Draw("same lX");
    if (system == 0)    graphs_hists_aa[0].Draw("same PZ");
std::cout << __LINE__ << std::endl;
    line.Draw("l");
    arrows[0]->Draw();
std::cout << __LINE__ << std::endl;
    if (system == 0)    latex.SetTextAlign(21);
    if (system == 0)    latex.SetTextSize(0.06);
    if (system == 0)    latex.DrawLatex(0.5, 0.03, "Cent. 0-10%");

    if (system == 1)   latex.SetTextAlign(21);
    if (system == 1)   latex.SetTextSize(0.06);
    if (system == 1)   latex.DrawLatex(0.5, 0.10, "Cent. 0-10%");
std::cout << __LINE__ << std::endl;
    pads[0]->cd();
    // legend_part1.Draw();
    // if (system == 0)      legend_part2.Draw();
std::cout << __LINE__ << std::endl;
    latex.SetTextSize(0.055);

    if (system == 0)    latex.SetTextAlign(11);
    if (system == 0)    latex.DrawLatex(0.04, 0.85, "HYBRID");

    if (system == 1)    latex.SetTextAlign(11);
    if (system == 1)    latex.DrawLatex(0.36, 0.64, "HYBRID");
std::cout << __LINE__ << std::endl;
    pads[ncols-1]->cd();
    // if (system == 1)      legend_part2.Draw();

    latex.SetTextSize(0.055);
std::cout << __LINE__ << std::endl;
    if (system == 1)    latex.SetTextAlign(21);
    if (system == 1)    latex.DrawLatex(0.49, 0.81, (text_photon_pt + ", " + text_photon_eta).c_str());
    if (system == 1)    latex.DrawLatex(0.49, 0.73, (text_dphi + ", " + text_jet_eta + ", " + text_jet_alg).c_str());

    if (system == 0)   latex.SetTextAlign(31);
    if (system == 0)   latex.DrawLatex(0.95, 0.64, (text_photon_pt).c_str());
    if (system == 0)   latex.DrawLatex(0.95, 0.56, (text_photon_eta).c_str());
    if (system == 0)   latex.DrawLatex(0.95, 0.48, (text_dphi + ", " + text_jet_eta).c_str());
    if (system == 0)   latex.DrawLatex(0.95, 0.40, (text_jet_alg).c_str());
std::cout << __LINE__ << std::endl;
    if (system == 0)    canvas.SaveAs((set + "_final_theory_spectra_aa_" + name + "_log.pdf").c_str());
    if (system == 1)    canvas.SaveAs((set + "_final_theory_ratio_" + name + "_log.pdf").c_str());

    if (system == 0)    canvas.SaveAs((set + "_final_theory_spectra_aa_" + name + "_log.C").c_str());
    if (system == 1)    canvas.SaveAs((set + "_final_theory_ratio_" + name + "_log.C").c_str());
std::cout << __LINE__ << std::endl;
    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return congratulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}