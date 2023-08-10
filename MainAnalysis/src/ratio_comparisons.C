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
#include "TLatex.h"
#include "TLine.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

int ratio(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input_final = conf->get<std::vector<std::string>>("input_final");
    auto input_pt_weight = conf->get<std::vector<std::string>>("input_pt_weight");
    auto input_no_weight = conf->get<std::vector<std::string>>("input_no_weight");

    auto tags = conf->get<std::vector<std::string>>("tags");
    auto figures = conf->get<std::vector<std::string>>("figures");

    auto xmins = conf->get<std::vector<float>>("xmin");
    auto xmaxs = conf->get<std::vector<float>>("xmax");
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

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open input files */
    std::vector<TFile*> file_final(input_final.size(), nullptr);
    zip([&](auto& file, auto const& input) {
        file = new TFile((base + input).data(), "read");
    }, file_final, input_final);

    std::vector<TFile*> file_pt_weight(input_final.size(), nullptr);
    zip([&](auto& file, auto const& input) {
        file = new TFile((base + input).data(), "read");
    }, file_pt_weight, input_final);

    std::vector<TFile*> file_no_weight(input_final.size(), nullptr);
    zip([&](auto& file, auto const& input) {
        file = new TFile((base + input).data(), "read");
    }, file_no_weight, input_final);

    /* load histograms */
    std::vector<std::string> base_stubs(2);

    zip([&](auto& base, auto const& tag) {
        base = tag + "_nominal_s_pure_raw_sub_";
    }, base_stubs, tags);

    /* prepare plots */
    auto hb = new pencil();
    hb->category("system", "final", "no_weight", "pt_weight");

    hb->alias("final", "PbPb/pp with pT and #deltaj weights");
    hb->alias("pt_weight", "PbPb/pp with response pT weights");
    hb->alias("no_weight", "PbPb/pp with no response weights");

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto aa_hf_info = [&](int64_t index, history<TH1F>* h) {
        stack_text(index, 0.84, 0.04, h, hf_info); };

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

    zip([&](auto const& figure, auto xmin, auto xmax, auto ymin, auto ymax) {
        /* get histograms */ 
        std::vector<history<TH1F>*> hist_final(2, nullptr);
        std::vector<history<TH1F>*> hist_pt_weight(2, nullptr);
        std::vector<history<TH1F>*> hist_no_weight(2, nullptr);

        zip([&](auto& hist, auto const file,
                auto const& base_stub) {
            hist = new history<TH1F>(file, base_stub + figure);
            hist->rename(base_stub + figure + "_final");
            title(std::bind(rename_axis, _1, "PbPb / pp"), hist);
        }, hist_final, file_final, base_stubs);

        zip([&](auto& hist, auto const file,
                auto const& base_stub) {
            hist = new history<TH1F>(file, base_stub + figure);
            hist->rename(base_stub + figure + "_pt_weight");
            title(std::bind(rename_axis, _1, "PbPb / pp"), hist);
        }, hist_pt_weight, file_pt_weight, base_stubs);

        zip([&](auto& hist, auto const file,
                auto const& base_stub) {
            hist = new history<TH1F>(file, base_stub + figure);
            hist->rename(base_stub + figure + "_no_weight");
            title(std::bind(rename_axis, _1, "PbPb / pp"), hist);
        }, hist_no_weight, file_no_weight, base_stubs);

        auto ratio_final = new history<TH1F>(*hist_final[0], "stat"s);
        auto ratio_pt_weight = new history<TH1F>(*hist_pt_weight[0], "stat"s);
        auto ratio_no_weight = new history<TH1F>(*hist_no_weight[0], "stat"s);

        /* take the ratio */
        for (int64_t i = 0; i < hist_final[0]->size(); ++i) {
            for (int64_t j = 1; j <= (*hist_final[0])[0]->GetNbinsX(); ++j) {  
                auto aa_hist = (*hist_final[0])[i];
                auto pp_hist = (*hist_final[1])[0];

                double aa_val = aa_hist->GetBinContent(j);
                double aa_stat_err = aa_hist->GetBinError(j);
                auto aa_stat_err_scale = aa_stat_err/aa_val;

                double pp_val = pp_hist->GetBinContent(j);
                double pp_stat_err = pp_hist->GetBinError(j);
                auto pp_stat_err_scale = pp_stat_err/pp_val;

                auto ratio = aa_val / pp_val;

                aa_stat_err = ratio * std::sqrt(aa_stat_err_scale * aa_stat_err_scale + pp_stat_err_scale * pp_stat_err_scale);

                (*ratio_final)[i]->SetBinContent(j, ratio);
                (*ratio_final)[i]->SetBinError(j, aa_stat_err);
            }
        }

        for (int64_t i = 0; i < hist_pt_weight[0]->size(); ++i) {
            for (int64_t j = 1; j <= (*hist_pt_weight[0])[0]->GetNbinsX(); ++j) {  
                auto aa_hist = (*hist_pt_weight[0])[i];
                auto pp_hist = (*hist_pt_weight[1])[0];

                double aa_val = aa_hist->GetBinContent(j);
                double aa_stat_err = aa_hist->GetBinError(j);
                auto aa_stat_err_scale = aa_stat_err/aa_val;

                double pp_val = pp_hist->GetBinContent(j);
                double pp_stat_err = pp_hist->GetBinError(j);
                auto pp_stat_err_scale = pp_stat_err/pp_val;

                auto ratio = aa_val / pp_val;

                aa_stat_err = ratio * std::sqrt(aa_stat_err_scale * aa_stat_err_scale + pp_stat_err_scale * pp_stat_err_scale);

                (*ratio_pt_weight)[i]->SetBinContent(j, ratio);
                (*ratio_pt_weight)[i]->SetBinError(j, aa_stat_err);
            }
        }

        for (int64_t i = 0; i < hist_no_weight[0]->size(); ++i) {
            for (int64_t j = 1; j <= (*hist_no_weight[0])[0]->GetNbinsX(); ++j) {  
                auto aa_hist = (*hist_no_weight[0])[i];
                auto pp_hist = (*hist_no_weight[1])[0];

                double aa_val = aa_hist->GetBinContent(j);
                double aa_stat_err = aa_hist->GetBinError(j);
                auto aa_stat_err_scale = aa_stat_err/aa_val;

                double pp_val = pp_hist->GetBinContent(j);
                double pp_stat_err = pp_hist->GetBinError(j);
                auto pp_stat_err_scale = pp_stat_err/pp_val;

                auto ratio = aa_val / pp_val;

                aa_stat_err = ratio * std::sqrt(aa_stat_err_scale * aa_stat_err_scale + pp_stat_err_scale * pp_stat_err_scale);

                (*ratio_no_weight)[i]->SetBinContent(j, ratio);
                (*ratio_no_weight)[i]->SetBinError(j, aa_stat_err);
            }
        }

        /* prepare papers */
        auto s = new paper(set + "_ratio_comparison_" + figure, hb);
        s->accessory(std::bind(line_at, _1, 1.f, xmin, xmax));
        s->accessory(kinematics);
        apply_style(s, "#bf{#scale[1.4]{CMS}}     #sqrt{s_{NN}} = 5.02 TeV"s, "PbPb 1.69 nb^{-1}, pp 302 pb^{-1}"s, ymin, ymax);
        s->accessory(std::bind(aa_hf_info, _1, ratio_final)); 
        s->divide(ratio_final->size()/2, -1);
        
        /* draw histograms with uncertainties */
        ratio_final->apply([&](TH1* h) {
            h->GetXaxis()->SetRangeUser(xmin, xmax);
            s->add(h, "final"); 
            s->adjust(h, "pe", "lf");
        });
        
        ratio_pt_weight->apply([&](TH1* h, int64_t index) {
            s->stack(index + 1, h);
            s->adjust(h, "pe", "lf");
        });

        ratio_no_weight->apply([&](TH1* h, int64_t index) {
            s->stack(index + 1, h);
            s->adjust(h, "pe", "lf");
        });

        hb->sketch();

        s->draw("pdf");
    }, figures, xmins, xmaxs, ymins, ymaxs);

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return ratio(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}