#include "../include/lambdas.h"
#include "../include/specifics.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TColor.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TLine.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

static auto const data = TColor::GetColor("#5c5c5c");
static int const colors[8] = {
    TColor::GetColor("#ffc9ed"), 
    TColor::GetColor("#b77ea3"), 
    TColor::GetColor("#f7d027"), 
    TColor::GetColor("#e48f1b"), 
    TColor::GetColor("#9cf168"),
    TColor::GetColor("#6ba547"),
    TColor::GetColor("#60ceed"),
    TColor::GetColor("#619ed6"),
    };

static int const markers[8] = { 49, 48, 47, 45, 6, 6, 6, 6 };

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

int theory(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input_aa = conf->get<std::string>("input_aa");
    auto input_pp = conf->get<std::string>("input_pp");

    auto figure = conf->get<std::string>("figure");
    auto type = conf->get<std::string>("type");

    auto theory_inputs_aa = conf->get<std::vector<std::string>>("theory_inputs_aa");
    auto theory_inputs_pp = conf->get<std::vector<std::string>>("theory_inputs_pp");

    auto theory_figures_aa = conf->get<std::vector<std::string>>("theory_figures_aa");
    auto theory_figures_pp = conf->get<std::vector<std::string>>("theory_figures_pp");

    auto theory_tags = conf->get<std::vector<std::string>>("theory_tags");
    auto theory_legends = conf->get<std::vector<std::string>>("theory_legends");

    auto theory_legend_styles = conf->get<std::vector<std::string>>("theory_legend_styles");
    auto theory_plot_styles = conf->get<std::vector<std::string>>("theory_plot_styles");

    auto offset = conf->get<int32_t>("offset");

    auto tag = conf->get<std::string>("tag");
    auto prefix = conf->get<std::string>("prefix");

    auto xmin = conf->get<float>("xmin");
    auto xmax = conf->get<float>("xmax");
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
    auto bjet_pt = sel->get<std::vector<float>>("jet_pt_bounds");

    std::vector<int32_t> dcent = {10, 0};

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open data file */
    TFile* file_aa = new TFile((base + input_aa).data(), "read");
    TFile* file_pp = new TFile((base + input_pp).data(), "read");
std::cout << __LINE__ << std::endl;
    /* load histograms */
    auto hist_aa = new history<TH1F>(file_aa, "aa_base_aa_nominal_s_pure_raw_sub_"s + figure);
    auto syst_aa = new history<TH1F>(file_aa, "aa_total_base_aa_nominal_s_pure_raw_sub_"s + figure);
std::cout << __LINE__ << std::endl;
    auto hist_pp = new history<TH1F>(file_pp, "pp_base_pp_nominal_s_pure_raw_sub_"s + figure);
    auto syst_pp = new history<TH1F>(file_pp, "pp_total_base_pp_nominal_s_pure_raw_sub_"s + figure);
std::cout << __LINE__ << std::endl;
    title(std::bind(rename_axis, _1, "PbPb / pp"), hist_pp);
std::cout << __LINE__ << std::endl;
    file_aa->Close();
    file_pp->Close();
std::cout << __LINE__ << std::endl;
    /* calculate ratio for data */
    auto hist_ratio = new history<TH1F>(*hist_pp, "ratio"s);
    auto syst_ratio = new history<TH1F>(*syst_pp, "ratio"s);
std::cout << __LINE__ << std::endl;
    /* link histograms, uncertainties */
    std::unordered_map<TH1*, TH1*> links;
    hist_aa->apply([&](TH1* h, int64_t index) { links[h] = (*syst_aa)[index]; });
    hist_pp->apply([&](TH1* h, int64_t index) { links[h] = (*syst_pp)[index]; });
    hist_ratio->apply([&](TH1* h, int64_t index) { links[h] = (*syst_ratio)[index]; });
std::cout << __LINE__ << std::endl;
    for (int64_t j = 1; j <= (*hist_ratio)[0]->GetNbinsX(); ++j) {
        auto aa_hist = (*hist_aa)[0];
        auto pp_hist = (*hist_pp)[0];
std::cout << __LINE__ << std::endl;
        double aa_val = aa_hist->GetBinContent(j);
        double aa_stat_err = aa_hist->GetBinError(j);
        double aa_syst_err = links[aa_hist]->GetBinContent(j);
        auto aa_stat_err_scale = aa_stat_err/aa_val;
        auto aa_syst_err_scale = aa_syst_err/aa_val;
std::cout << __LINE__ << std::endl;
        double pp_val = pp_hist->GetBinContent(j);
        double pp_stat_err = pp_hist->GetBinError(j);
        double pp_syst_err = links[pp_hist]->GetBinContent(j);
        auto pp_stat_err_scale = pp_stat_err/pp_val;
        auto pp_syst_err_scale = pp_syst_err/pp_val;
std::cout << __LINE__ << std::endl;
        auto ratio = aa_val / pp_val;
std::cout << __LINE__ << std::endl;
        aa_stat_err = ratio * std::sqrt(aa_stat_err_scale * aa_stat_err_scale + pp_stat_err_scale * pp_stat_err_scale);
        aa_syst_err = ratio * std::sqrt(aa_syst_err_scale * aa_syst_err_scale + pp_syst_err_scale * pp_syst_err_scale);
std::cout << __LINE__ << std::endl;
        (*hist_ratio)[0]->SetBinContent(j, ratio);
        (*hist_ratio)[0]->SetBinError(j, aa_stat_err);
        links[(*hist_ratio)[0]]->SetBinContent(j, aa_syst_err);
    }
std::cout << __LINE__ << std::endl;
    /* get theory predictions */ 
    std::vector<TFile*> theory_files_aa(theory_inputs_aa.size());
    std::vector<TFile*> theory_files_pp(theory_inputs_pp.size());
std::cout << __LINE__ << std::endl;
    std::vector<history<TH1F>*> theory_hists_aa(theory_inputs_aa.size());
    std::vector<history<TH1F>*> theory_hists_pp(theory_inputs_pp.size());
std::cout << __LINE__ << std::endl;
    for (size_t i = 0; i < theory_inputs_aa.size(); ++i) {
        theory_files_aa[i] = new TFile((base + theory_inputs_aa[i]).data(), "read");
        theory_files_pp[i] = new TFile((base + theory_inputs_pp[i]).data(), "read");

        theory_hists_aa[i] = new history<TH1F>(theory_files_aa[i], theory_figures_aa[i]);
        theory_hists_pp[i] = new history<TH1F>(theory_files_pp[i], theory_figures_pp[i]);
    }
std::cout << __LINE__ << std::endl;
    /* uncertainty box */
    auto box = [&](TH1* h, int64_t) {
        if (links.count(h)) {
            TGraph* gr = new TGraph();
            gr->SetFillStyle(1001);
            gr->SetFillColorAlpha(data, 0.48);

            for (int i = 1; i <= h->GetNbinsX(); ++i) {
                if (h->GetBinError(i) == 0) continue;

                double x = h->GetBinCenter(i);
                double width = h->GetBinWidth(i);
                double val = h->GetBinContent(i);
                double err = links[h]->GetBinContent(i);

                gr->SetPoint(0, x - (width / 2), val - err);
                gr->SetPoint(1, x + (width / 2), val - err);
                gr->SetPoint(2, x + (width / 2), val + err);
                gr->SetPoint(3, x - (width / 2), val + err);

                gr->DrawClone("f");
            }
        }
    };
std::cout << __LINE__ << std::endl;
    /* prepare plots */
    auto hb = new pencil();
    hb->category("type", "data", theory_tags);
std::cout << __LINE__ << std::endl;
    zip([&](auto const& tag, auto const& legend) {
        hb->alias(tag, legend); }, theory_tags, theory_legends);

    hb->alias("data", "PbPb/pp");
    std::cout << __LINE__ << std::endl;
    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };
std::cout << __LINE__ << std::endl;
    auto aa_hf_info = [&](int64_t index, history<TH1F>* h) {
        stack_text(index, 0.84, 0.04, h, hf_info); };
std::cout << __LINE__ << std::endl;
    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto photon_selections = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs)  + 
                ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
            auto jet_selections = "anti-k_{T} R = 0.3, " + to_text(bjet_pt[0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.25, photon_selections.data());
            l->DrawLatexNDC(0.865, 0.19, jet_selections.data());
        }
    };
std::cout << __LINE__ << std::endl;
    auto luminosity = [&](int64_t index) {
        if (index > 0) {
            auto values = "PbPb 1.69 nb^{-1}, pp 302 pb^{-1}"s;

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.34, values.data());
        }
    };
std::cout << __LINE__ << std::endl;
    /* prepare papers */
    auto p = new paper(set + "_theory_comparison_" + type + "_" + tag, hb);
    apply_style(p, "#bf{#scale[1.4]{CMS}}"s, "#sqrt{s_{NN}} = 5.02 TeV"s, ymin, ymax);
    p->accessory(std::bind(line_at, _1, 0.f, xmin, xmax));
    p->accessory(kinematics);
    p->accessory(luminosity);
    p->jewellery(box);
    p->accessory(std::bind(aa_hf_info, _1, hist_ratio)); 
    p->divide(-1, 1);
std::cout << __LINE__ << std::endl;
    /* draw histograms with uncertainties */
    p->add((*hist_ratio)[0], "data");
    p->adjust((*hist_ratio)[0], "pe", "plf");
std::cout << __LINE__ << std::endl;
    // for (size_t i = 0; i < theory_inputs.size(); ++i) {
    //     p->stack((*theory_hists[i])[0], theory_tags[i]);
    // }
std::cout << __LINE__ << std::endl;
    auto data_style = [&](TH1* h) {
        h->SetLineColor(1);
        h->SetFillColorAlpha(data, 0.5);
        h->SetMarkerStyle(20);
        h->SetMarkerSize(0.60);

        p->adjust(h, "le", "plf");
    };
std::cout << __LINE__ << std::endl;
    auto theory_style = [&](TH1* h, int i) {
        h->SetMarkerColor(colors[i + offset]);
        h->SetLineColor(colors[i + offset]);
        h->SetFillColorAlpha(colors[i + offset], 0.5);
        h->SetMarkerStyle(markers[i + offset]);
        h->SetMarkerSize(0.60);

        p->adjust(h, theory_plot_styles[i], theory_legend_styles[i]);
    };
std::cout << __LINE__ << std::endl;
    hb->style("data", data_style);

    for (size_t i = 0; i < theory_tags.size(); ++i) {
        hb->style(theory_tags[i], std::bind(theory_style, _1, i));
    }
std::cout << __LINE__ << std::endl;
    hb->sketch();
std::cout << __LINE__ << std::endl;
    p->draw("pdf");

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return theory(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
