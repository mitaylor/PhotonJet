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

using namespace std::literals::string_literals;
using namespace std::placeholders;

void format(history<TH1F>* h, history<TH1F>* s, int system)
{
    static int style[2] = {21, 25};
    // ff7366 4dff7c
    static int color[2] = {TColor::GetColor("#EC8379"), TColor::GetColor("#71DB8D")};

    for (int i = 0; i < h->size(); ++i) {
        for (int j = 1; j <= (*h)[i]->GetNbinsX(); ++j) {
            double val = (*h)[i]->GetBinContent(j);
            double err = (*s)[i]->GetBinContent(j);

            (*s)[i]->SetBinContent(j, val);
            (*s)[i]->SetBinError(j, err);
        }

        (*h)[i]->SetMarkerStyle(style[system]);
        (*h)[i]->SetMarkerColor(1);
        (*h)[i]->SetLineColor(1);
        (*h)[i]->SetFillColor(color[system]);
        (*h)[i]->SetMarkerSize(2.2);
        (*h)[i]->SetLineWidth(2.0);

        (*s)[i]->SetMarkerStyle(style[system]);
        (*s)[i]->SetMarkerColor(1);
        (*s)[i]->SetLineColor(1);
        (*s)[i]->SetFillColor(color[system]);
        (*s)[i]->SetFillColorAlpha(color[system], 0.25);
        (*s)[i]->SetMarkerSize(2.2);
        (*s)[i]->SetLineWidth(2.0);
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

void set_axis(TGaxis &axis)
{
    axis.SetLabelFont(42);
    axis.SetLabelSize(0.040);
    axis.SetMaxDigits(6);
    axis.SetNoExponent();
    axis.Draw();
}

int congratulate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input_aa = conf->get<std::string>("input_aa");
    auto input_pp = conf->get<std::string>("input_pp");
std::cout << __LINE__ << std::endl;
    auto figures = conf->get<std::vector<std::string>>("figures");
    auto types = conf->get<std::vector<int64_t>>("types");

    auto ymins = conf->get<std::vector<float>>("ymins");
    auto ymaxs = conf->get<std::vector<float>>("ymaxs");
std::cout << __LINE__ << std::endl;
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");
std::cout << __LINE__ << std::endl;
    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");
std::cout << __LINE__ << std::endl;
    auto bpho_pt = sel->get<std::vector<float>>("photon_pt_bounds");
    auto bdr = sel->get<std::vector<float>>("dr_bounds");
    auto ptg_range = sel->get<std::vector<float>>("ptg_range");
std::cout << __LINE__ << std::endl;
    auto osg = sel->get<std::vector<int64_t>>("osg");
    auto osg_part1 = sel->get<std::vector<int64_t>>("osg_part1");
    auto osg_part2 = sel->get<std::vector<int64_t>>("osg_part2");

    std::vector<float> bjet_pt = {1.0, 1.0};
std::cout << __LINE__ << std::endl;
    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();
std::cout << __LINE__ << std::endl;
    /* open input files */
    auto file_aa = new TFile((base + input_aa).data(), "read");
    auto file_pp = new TFile((base + input_pp).data(), "read");
std::cout << __LINE__ << std::endl;
    zip([&](auto const& figure, auto type, auto ymin, auto ymax) {
        /* define kinematics and luminosity*/
        switch (type) {
        case 1:
            bjet_pt[0] = ptg_range[osg_part1[2]];
            bjet_pt[1] = ptg_range[ptg_range.size() - 1 - osg_part1[3]];std::cout << __LINE__ << std::endl;
            break;
        case 2:
            bjet_pt[0] = ptg_range[osg_part2[2]];
            bjet_pt[1] = ptg_range[ptg_range.size() - 1 - osg_part2[3]];std::cout << __LINE__ << std::endl;
            break;
        default:
            bjet_pt[0] = ptg_range[osg[2]];
            bjet_pt[1] = ptg_range[ptg_range.size() - 1 - osg[3]];std::cout << __LINE__ << std::endl;
        }
std::cout << __LINE__ << std::endl;
        auto text_system = "#sqrt{s_{NN}} = 5.02 TeV, PbPb 1.69 nb^{-1}, pp 302 pb^{-1}"s;
        auto text_cms = "#bf{#scale[1.4]{CMS}}"s;
        auto text_photon_pt = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV"s;
        auto text_photon_eta = "|#eta^{#gamma}| < "s + to_text(photon_eta_abs);
        auto text_dphi = "#Delta#phi_{j#gamma} > "s + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
        auto text_jet_alg = "anti-k_{T} R = 0.3"s;
        auto text_jet_pt = to_text(bjet_pt[0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV"s;
        auto text_jet_eta = "|#eta^{jet}| < "s + to_text(jet_eta_abs);
std::cout << __LINE__ << std::endl;
        /* get histograms */
        auto hist_aa = new history<TH1F>(file_aa, "aa_base_aa_nominal_s_pure_raw_sub_" + figure);
        auto syst_aa = new history<TH1F>(file_aa, "aa_total_base_aa_nominal_s_pure_raw_sub_" + figure);
        auto hist_pp = new history<TH1F>(file_pp, "pp_base_pp_nominal_s_pure_raw_sub_" + figure);
        auto syst_pp = new history<TH1F>(file_pp, "pp_total_base_pp_nominal_s_pure_raw_sub_" + figure);
        std::cout << __LINE__ << std::endl;
        format(hist_aa, syst_aa, 0);
        format(hist_pp, syst_pp, 1);
std::cout << __LINE__ << std::endl;
        /* size canvas */
        double panel_size = 500;
        double padding_width_left = 140;
        double padding_width_right = 50;
        double padding_height = 130;
std::cout << __LINE__ << std::endl;
        double canvas_width = panel_size * 4 + padding_width_left + padding_width_right;
        double canvas_height = panel_size * 1 + padding_height * 2;

        double pad_x0 = padding_width_left / canvas_width;
        double pad_y0 = padding_height / canvas_height;
        double pad_dx = panel_size / canvas_width;
        double pad_dy = panel_size / canvas_height;
std::cout << __LINE__ << std::endl;
        double xmin = bdr[0];
        double xmax = bdr[1];

        /* declare canvas, pads, axes, and titles */
        TCanvas canvas("canvas", "", canvas_width, canvas_height);
std::cout << __LINE__ << std::endl;
        TH2F world("world", ";;", 100, xmin, xmax, 100, ymin, ymax);
        world.SetStats(0);

        const int npads = 4;
        TPad *pads[npads];
std::cout << __LINE__ << std::endl;
        pads[0] = new TPad("P1", "", pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 1, pad_y0 + pad_dy * 1, 0);
        pads[1] = new TPad("P2", "", pad_x0 + pad_dx * 1, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 2, pad_y0 + pad_dy * 1, 0);
        pads[2] = new TPad("P3", "", pad_x0 + pad_dx * 2, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 3, pad_y0 + pad_dy * 1, 0);
        pads[3] = new TPad("P4", "", pad_x0 + pad_dx * 3, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 4, pad_y0 + pad_dy * 1, 0);
std::cout << __LINE__ << std::endl;
        set_pad(*pads[0]);
        set_pad(*pads[1]);
        set_pad(*pads[2]);
        set_pad(*pads[3]);

        canvas.cd();
std::cout << __LINE__ << std::endl;
        TGaxis axis_x1(pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 1, pad_y0 + pad_dy * 0, xmin, xmax, 510, "");
        TGaxis axis_x2(pad_x0 + pad_dx * 1, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 2, pad_y0 + pad_dy * 0, xmin, xmax, 510, "");
        TGaxis axis_x3(pad_x0 + pad_dx * 2, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 3, pad_y0 + pad_dy * 0, xmin, xmax, 510, "");
        TGaxis axis_x4(pad_x0 + pad_dx * 3, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 4, pad_y0 + pad_dy * 0, xmin, xmax, 510, "");
        TGaxis axis_y1(pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 0, pad_x0 + pad_dx * 0, pad_y0 + pad_dy * 1, ymin, ymax, 510, "");
std::cout << __LINE__ << std::endl;
        set_axis(axis_x1);
        set_axis(axis_x2);
        set_axis(axis_x3);
        set_axis(axis_x4);
        set_axis(axis_y1);

        TLatex latex;
        latex.SetNDC();

        latex.SetTextFont(42);
        latex.SetTextSize(0.045);
        latex.SetTextAlign(22);
        latex.SetTextAngle(0);
        latex.DrawLatex(pad_x0 + pad_dx * 0.5, pad_y0 * 0.4, "#deltaj");
        latex.DrawLatex(pad_x0 + pad_dx * 1.5, pad_y0 * 0.4, "#deltaj");
        latex.DrawLatex(pad_x0 + pad_dx * 2.5, pad_y0 * 0.4, "#deltaj");
        latex.DrawLatex(pad_x0 + pad_dx * 3.5, pad_y0 * 0.4, "#deltaj");

        latex.SetTextFont(42);
        latex.SetTextSize(0.045);
        latex.SetTextAlign(22);
        latex.SetTextAngle(90);
        latex.DrawLatex(pad_x0 * 0.2, pad_y0 + pad_dy * 0.5, "#frac{1}{N_{#gamma}#frac{dN_{j#gamma}}{d#deltaj}");

        latex.SetTextFont(62);
        latex.SetTextSize(0.045);
        latex.SetTextAlign(11);
        latex.SetTextAngle(0);
        latex.DrawLatex(pad_x0, pad_y0 * 1.15 + pad_dy * 1, text_cms.c_str());

        latex.SetTextFont(42);
        latex.SetTextSize(0.035);
        latex.SetTextAlign(31);
        latex.SetTextAngle(0);
        latex.DrawLatex(pad_x0 + pad_dx * 4, pad_y0 * 1.15 + pad_dy * 1, text_system.c_str());
        
        TGraph line;
        line.SetPoint(0, xmin, 0);
        line.SetPoint(1, xmax, 0);
        line.SetLineStyle(kDashed);

        /* declare legend */
        TLegend legend(0.05, 0.75, 0.5, 0.95);
        legend.SetTextFont(42);
        legend.SetTextSize(0.07);
        legend.SetFillStyle(0);
        legend.SetBorderSize(0);
        legend.AddEntry((*syst_aa)[0], "PbPb", "plf");
        legend.AddEntry((*syst_pp)[0], "pp", "plf");

        for(int i = 0; i < npads; i++)
        {
            pads[i]->cd();

            world.Draw("axis");
            (*syst_aa)[i]->Draw("same e2");
            (*syst_pp)[0]->Draw("same e2");
            (*hist_aa)[i]->Draw("same");
            (*hist_pp)[0]->Draw("same");
            line.Draw("l");
        }

        pads[0]->cd();
        latex.SetTextAlign(31);
        latex.SetTextSize(0.07);
        latex.DrawLatex(0.95, 0.8, "Cent. 50-90%");
        legend.Draw();

        pads[1]->cd();
        latex.SetTextAlign(31);
        latex.SetTextSize(0.07);
        latex.DrawLatex(0.95, 0.8, "Cent. 30-50%");

        pads[2]->cd();
        latex.SetTextAlign(31);
        latex.SetTextSize(0.07);
        latex.DrawLatex(0.95, 0.8, "Cent. 10-30%");

        pads[3]->cd();
        latex.SetTextAlign(31);
        latex.SetTextSize(0.07);
        latex.DrawLatex(0.95, 0.8, "Cent. 0-10%");
        latex.DrawLatex(0.95, 0.7, (text_photon_pt + ", " + text_photon_eta).c_str());
        latex.DrawLatex(0.95, 0.6, (text_dphi + ", " + text_jet_alg).c_str());
        latex.DrawLatex(0.95, 0.5, (text_jet_pt + ", " + text_jet_eta).c_str());

        canvas.SaveAs((set + "_" + "_final_spectra_" + figure + ".pdf").c_str());
    }, figures, types, ymins, ymaxs);

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return congratulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}