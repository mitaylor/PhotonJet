#include "../include/lambdas.h"
#include "../include/specifics.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"


#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TColor.h"
#include "TFile.h"
#include "TH1.h"
#include "TLatex.h"
#include "TLine.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

static auto const red = TColor::GetColor("#ff6666");
static auto const blue = TColor::GetColor("#6666ff");
static auto const grey = TColor::GetColor("#5c5c5c");

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

int ratio(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto inputs = conf->get<std::vector<std::string>>("inputs");
    auto tags = conf->get<std::vector<std::string>>("tags");
    auto figures = conf->get<std::vector<std::string>>("figures");
    auto prefix = conf->get<std::string>("prefix");

    auto ymins = conf->get<std::vector<float>>("ymin");
    auto ymaxs = conf->get<std::vector<float>>("ymax");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

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

    auto ihf = new interval(dhf);

    std::vector<int32_t> drange = { dcent.front(), dcent.back() };

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open input files */
    std::vector<TFile*> files(inputs.size(), nullptr);
    zip([&](auto& file, auto const& input) {
        file = new TFile((base + input).data(), "read");
    }, files, inputs);

    /* load histograms */
    std::vector<std::string> base_stubs(2);
    std::vector<std::string> syst_stubs(2);

    zip([&](auto& base, auto& syst, auto const& tag) {
        base = tag + "_base_" + tag + "_nominal_s_pure_raw_sub_";
        syst = tag + "_total_base_" + tag + "_nominal_s_pure_raw_sub_";
    }, base_stubs, syst_stubs, tags);

    /* prepare plots */
    auto hb = new pencil();
    hb->category("system", "r");

    hb->alias("r", "PbPb/pp");

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    std::function<void(int64_t, float)> range_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", drange, true); };

    auto aa_hf_info = [&](int64_t index, history<TH1F>* h) {
        stack_text(index, 0.73, 0.04, h, hf_info); };

    auto aa_range_info = [&](int64_t index, history<TH1F>* h) {
        stack_text(index, 0.73, 0.04, h, range_info); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto photon_selections = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs)  + 
                ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
            auto jet_selections = "anti-k_{T} R = 0.3, " + to_text(bjet_pt[0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.865, 0.25, photon_selections.data());
            l->DrawLatexNDC(0.865, 0.20, jet_selections.data());
        }
    };

    auto blurb = [&](int64_t index) {
        if (index > 0) {
            auto system_tag = "PbPb 1.69 nb^{-1}, pp 302 pb^{-1}"s;
            auto cms = "#bf{#scale[1.4]{CMS}} #sqrt{s_{NN}} = 5.02 TeV"s;

            TLatex* l = new TLatex();
            l->SetTextAlign(11);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.14, 0.83, cms.data());
            l->DrawLatexNDC(0.14, 0.77, system_tag.data());
        }
    };

    zip([&](auto const& figure, auto ymin, auto ymax) {
        /* get histograms */ 
        std::vector<history<TH1F>*> hists(2, nullptr);
        std::vector<history<TH1F>*> systs(2, nullptr);

        zip([&](auto& hist, auto& syst, auto const file,
                auto const& base_stub, auto const& syst_stub) {
            hist = new history<TH1F>(file, base_stub + figure);
            title(std::bind(rename_axis, _1, "PbPb / pp"), hist);
            syst = new history<TH1F>(file, syst_stub + figure);

            for (int64_t i = 0; i < hist->size(); ++i) {
                (*hist)[i]->Scale(1/(*hist)[i]->Integral("width"));
                (*syst)[i]->Scale(1/(*hist)[i]->Integral("width"));
            }
        }, hists, systs, files, base_stubs, syst_stubs);

        /* link histograms, uncertainties */
        std::unordered_map<TH1*, TH1*> links;
        zip([&](auto hist, auto syst) {
            hist->apply([&](TH1* h, int64_t index) {
                links[h] = (*syst)[index]; });
        }, hists, systs);

        auto ratio_stat = new history<TH1F>(*hists[0], "stat"s);
        auto ratio_syst = new history<TH1F>(*hists[0], "syst"s);

        /* take the ratio */
        for (int64_t i = 0; i < hists[0]->size(); ++i) {
            std::cout << "Chi2 " << (*hists[0])[i]->Chi2Test((*hists[1])[0], "UU P") << std::endl;
            std::cout << "K " << (*hists[0])[i]->KolmogorovTest((*hists[1])[0], "UU P") << std::endl;

            for (int64_t j = 1; j <= (*hists[0])[0]->GetNbinsX(); ++j) {  
                auto aa_hist = (*hists[0])[i];
                auto pp_hist = (*hists[1])[0];

                double aa_val = aa_hist->GetBinContent(j);
                double aa_stat_err = aa_hist->GetBinError(j);
                double aa_syst_err = links[aa_hist]->GetBinContent(j);
                auto aa_stat_err_scale = aa_stat_err/aa_val;
                auto aa_syst_err_scale = aa_syst_err/aa_val;

                double pp_val = pp_hist->GetBinContent(j);
                double pp_stat_err = pp_hist->GetBinError(j);
                double pp_syst_err = links[pp_hist]->GetBinContent(j);
                auto pp_stat_err_scale = pp_stat_err/pp_val;
                auto pp_syst_err_scale = pp_syst_err/pp_val;

                auto ratio = aa_val / pp_val;

                aa_stat_err = ratio * std::sqrt(aa_stat_err_scale * aa_stat_err_scale + pp_stat_err_scale * pp_stat_err_scale);
                aa_syst_err = ratio * std::sqrt(aa_syst_err_scale * aa_syst_err_scale + pp_syst_err_scale * pp_syst_err_scale);

                (*ratio_stat)[i]->SetBinContent(j, ratio);
                (*ratio_stat)[i]->SetBinError(j, aa_stat_err);
                (*ratio_syst)[i]->SetBinContent(j, ratio);
                (*ratio_syst)[i]->SetBinError(j, aa_syst_err);
            }
        }

        /* prepare papers */
        auto s = new paper(set + "_" + prefix + "_ratio_" + figure, hb);
        s->accessory(std::bind(line_at, _1, 1.f, bdr[0], bdr[1]));
        s->accessory(kinematics);
        s->accessory(blurb);

        if (ratio_stat->size() == ihf->size()) { 
            apply_style(s, "#bf{#scale[1.4]{CMS}}     #sqrt{s_{NN}} = 5.02 TeV"s, "PbPb 1.69 nb^{-1}, pp 302 pb^{-1}"s, ymin, ymax);
            s->accessory(std::bind(aa_hf_info, _1, ratio_stat)); 
            s->divide(ratio_stat->size()/2, -1);
        } else { 
            apply_style(s, "#bf{#scale[1.4]{CMS}}"s, "#sqrt{s_{NN}} = 5.02 TeV"s, ymin, ymax);
            s->accessory(std::bind(aa_range_info, _1, ratio_stat)); 
        }

        /* draw histograms with uncertainties */
        ratio_syst->apply([&](TH1* h) {
            h->GetXaxis()->SetRangeUser(bdr[0], bdr[1]);
            s->add(h, "r"); 

            s->adjust(h, "e2", "plf");
            h->SetFillColorAlpha(grey, 0.5);
            h->SetLineColor(1);
            h->SetMarkerStyle(20);
            h->SetMarkerSize(0.60);
        });

        ratio_stat->apply([&](TH1* h, int64_t index) {
            s->stack(index + 1, h);

            s->adjust(h, "pe", "lf");
            h->SetLineColor(1);
            h->SetMarkerStyle(20);
            h->SetMarkerSize(0.60);
        });

        hb->sketch();

        s->draw("pdf");
    }, figures, ymins, ymaxs);

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return ratio(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}