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

#include <string>
#include <vector>
#include <cmath>

using namespace std::literals::string_literals;
using namespace std::placeholders;

void set_values(history<TH1F>* h_mean, history<TH1F>* s_mean, history<TH1F>* h, history<TH1F>* s, int system)
{
    for (int i = 0; i < 4; ++i) {
        if (system == 0) {
            (*h_mean)[0]->SetBinContent((i + 1) * 2, (*h)[i]->GetBinContent(1));
            (*h_mean)[0]->SetBinError((i + 1) * 2, (*h)[i]->GetBinError(1));

            (*s_mean)[0]->SetBinContent((i + 1) * 2, (*s)[i]->GetBinContent(1));
            (*s_mean)[0]->SetBinError((i + 1) * 2, (*s)[i]->GetBinError(1));
        }
        if (system == 1) {
            (*h_mean)[0]->SetBinContent((i + 1) * 2, (*h)[0]->GetBinContent(1));
            (*h_mean)[0]->SetBinError((i + 1) * 2, (*h)[0]->GetBinError(1));

            (*s_mean)[0]->SetBinContent((i + 1) * 2, (*s)[0]->GetBinContent(1));
            (*s_mean)[0]->SetBinError((i + 1) * 2, (*s)[0]->GetBinError(1));
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
    static int style[3] = {20, 24, 20};
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

void set_pad(TPad &pad)
{
    pad.SetLeftMargin(0);
    pad.SetTopMargin(0);
    pad.SetRightMargin(0);
    pad.SetBottomMargin(0);
    pad.SetTickx();
    pad.SetTicky();
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

void set_world(TH2F* world) {
    world->GetXaxis()->SetBinLabel(1, " ");
    world->GetXaxis()->SetBinLabel(2, "50-90%");
    world->GetXaxis()->SetBinLabel(3, " ");
    world->GetXaxis()->SetBinLabel(4, "30-50%");
    world->GetXaxis()->SetBinLabel(5, " ");
    world->GetXaxis()->SetBinLabel(6, "10-30%");
    world->GetXaxis()->SetBinLabel(7, " ");
    world->GetXaxis()->SetBinLabel(8, "0-10%");
    world->GetXaxis()->SetBinLabel(9, " ");
    world->GetXaxis()->SetTickLength(0);
    world->GetXaxis()->SetLabelSize(0.05);
}

int congratulate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input_aa = conf->get<std::string>("input_aa");
    auto input_pp = conf->get<std::string>("input_pp");

    auto figures = conf->get<std::vector<std::string>>("figures");
    auto name = conf->get<std::string>("name");
    auto types = conf->get<std::vector<int64_t>>("types");

    auto ymins = conf->get<std::vector<float>>("ymins");
    auto ymaxs = conf->get<std::vector<float>>("ymaxs");

    // auto subsets = conf->get<bool>("subsets");
    // auto whole = conf->get<bool>("whole");

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

    /* define kinematics and luminosity */
    auto text_energy = "#sqrt{s_{NN}} = 5.02 TeV"s;
    auto text_system = "PbPb 1.69 nb^{-1}, pp 302 pb^{-1}"s;
    auto text_cms = "CMS"s;
    auto text_photon_pt = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV"s;
    auto text_photon_eta = "|#eta^{#gamma}| < "s + to_text(photon_eta_abs);
    auto text_dphi = "#Delta#phi_{j#gamma} > #frac{"s + to_text(dphi_min_numerator) + "#pi}{"s + to_text(dphi_min_denominator) + "}"s;
    auto text_jet_alg = "anti-k_{T} R = 0.3"s;
    auto text_jet_eta = "|#eta^{jet}| < "s + to_text(jet_eta_abs);

    std::vector<history<TH1F>*> hists_aa(ncols);
    std::vector<history<TH1F>*> systs_aa(ncols);
    std::vector<history<TH1F>*> hists_pp(ncols);
    std::vector<history<TH1F>*> systs_pp(ncols);
    std::vector<history<TH1F>*> hists_means_aa(ncols);
    std::vector<history<TH1F>*> systs_means_aa(ncols);
    std::vector<history<TH1F>*> hists_means_pp(ncols);
    std::vector<history<TH1F>*> systs_means_pp(ncols);

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
        auto incl = new interval(""s, 9, 0.f, 9.f);
        auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);

        hists_aa[i] = new history<TH1F>(file_aa, "aa_base_mean_raw_sub_"s + figures[i]);
        systs_aa[i] = new history<TH1F>(file_aa, "aa_syst_mean_raw_sub_"s + figures[i]);
        hists_pp[i] = new history<TH1F>(file_pp, "pp_base_mean_raw_sub_"s + figures[i]);
        systs_pp[i] = new history<TH1F>(file_pp, "pp_syst_mean_raw_sub_"s + figures[i]);
        hists_means_aa[i] = new history<TH1F>("aa_base_mean_raw_sub_"s + figures[i] + "_mean"s, "", fincl, 1);
        systs_means_aa[i] = new history<TH1F>("aa_syst_mean_raw_sub_"s + figures[i] + "_mean"s, "", fincl, 1);
        hists_means_pp[i] = new history<TH1F>("pp_base_mean_raw_sub_"s + figures[i] + "_mean"s, "", fincl, 1);
        systs_means_pp[i] = new history<TH1F>("pp_syst_mean_raw_sub_"s + figures[i] + "_mean"s, "", fincl, 1);

        set_systematics(hists_aa[i], systs_aa[i]);
        set_systematics(hists_pp[i], systs_pp[i]);
        
        set_values(hists_means_aa[i], systs_means_aa[i], hists_aa[i], systs_aa[i], 0);
        set_values(hists_means_pp[i], systs_means_pp[i], hists_pp[i], systs_pp[i], 1);

        format(hists_means_aa[i], systs_means_aa[i], 0);
        format(hists_means_pp[i], systs_means_pp[i], 1);
    }

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

    /* declare canvas, pads, axes, and titles */
    TCanvas canvas("canvas", "", canvas_width, canvas_height);

    std::vector<TH2F*> worlds(ncols);
    std::vector<TPad*> pads(ncols);
    std::vector<TGaxis*> axis_x(ncols);
    std::vector<TGaxis*> axis_y(1);

    for (int i = 0; i < ncols; ++i) {
        worlds[i] = new TH2F("world", ";;", 9, 0.f, 9.f, 100, ymins[i], ymaxs[i]);
        worlds[i]->SetStats(0);
        set_world(worlds[i]);

        pads[i] = new TPad("P1", "", pad_x0 + pad_dx * i, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * (i + 1), pad_y0 + pad_dy * 1, 0);
        
        set_pad(*pads[i]);

        axis_x[i] = new TGaxis(pad_x0 + pad_dx * i, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * (i + 1), pad_y0 + pad_dy * 0, 0, 9, 510, "S");
        
        set_axis(*axis_x[i], sf);
    }

    canvas.cd();

    axis_y[0] = new TGaxis(pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 1, ymins[0], ymaxs[0] * 0.999, 510, "S");

    set_axis(*axis_y[0], sf);

    TLatex latex;
    latex.SetNDC();

    // latex.SetTextFont(42);
    // latex.SetTextSize(0.055/sf);
    // latex.SetTextAlign(22);
    // latex.SetTextAngle(0);
    // latex.DrawLatex(pad_x0 + pad_dx * 0.5, pad_y0 * 0.5, "#Deltaj");
    // latex.DrawLatex(pad_x0 + pad_dx * 1.5, pad_y0 * 0.5, "#Deltaj"); // fix to have good x axis labels

    latex.SetTextFont(42);
    latex.SetTextSize(0.055/sf);
    latex.SetTextAlign(22);
    latex.SetTextAngle(90);
    latex.DrawLatex(pad_x0 * 0.3, pad_y0 + pad_dy * 0.5, "<#Deltaj>");

    latex.SetTextFont(62);
    latex.SetTextSize(0.07/sf);
    latex.SetTextAlign(11);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0, pad_y0 * 1.15 + pad_dy, text_cms.c_str());

    latex.SetTextFont(42);
    latex.SetTextSize(0.06/sf);
    latex.SetTextAlign(31);
    latex.SetTextAngle(0);
    latex.DrawLatex(pad_x0 + pad_dx * ncols, pad_y0 * 1.15 + pad_dy, text_energy.c_str());

    /* declare legend */
    auto legend_y_min = 0.65;
    auto legend_y_max = 0.85;
    auto legend_x_min = 0.05;
    auto legend_x_max = 0.4;

    TLegend legend(legend_x_min, legend_y_min, legend_x_max, legend_y_max);
    legend.SetTextFont(42);
    legend.SetTextSize(0.07);
    legend.SetFillStyle(0);
    legend.SetBorderSize(0);
    legend.AddEntry((*systs_means_aa[0])[0], "PbPb", "plf");
    legend.AddEntry((*systs_means_pp[0])[0], "pp", "plf");

    for (int i = 0; i < ncols; i++) {
        pads[i]->cd();

        worlds[i]->Draw("axis");

        (*systs_means_aa[i])[0]->Draw("same e2");
        (*systs_means_pp[i])[0]->Draw("same e2");
        (*hists_means_aa[i])[0]->Draw("same");
        (*hists_means_pp[i])[0]->Draw("same");

        auto text_jet_pt = to_text(bjet_pt[i][0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[i][1]) + " GeV"s;
    
        pads[i]->cd();
        latex.SetTextAlign(21);
        latex.SetTextSize(0.07);
        latex.DrawLatex(0.5, 0.05, (text_jet_pt).c_str());
    }

    pads[0]->cd();
    legend.Draw();

    pads[ncols-1]->cd();
    latex.SetTextSize(0.07);
    latex.SetTextAlign(31);
    latex.DrawLatex(0.95, 0.9, (text_system).c_str());

    latex.SetTextSize(0.07);
    latex.SetTextAlign(31);
    latex.DrawLatex(0.95, 0.38, (text_photon_pt).c_str());
    latex.DrawLatex(0.95, 0.28, (text_photon_eta).c_str());

    latex.SetTextSize(0.07);
    latex.SetTextAlign(31);
    latex.DrawLatex(0.95, 0.38, (text_dphi + ", " + text_jet_eta).c_str());
    latex.DrawLatex(0.95, 0.28, (text_jet_alg).c_str());

    canvas.SaveAs((set + "_final_means_" + name + ".pdf").c_str());

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return congratulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}