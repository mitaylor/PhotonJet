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

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

int congratulate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto inputs = conf->get<std::vector<std::string>>("inputs");
    auto tags = conf->get<std::vector<std::string>>("tags");
    auto figures = conf->get<std::vector<std::string>>("figures");
    auto prefix = conf->get<std::string>("prefix");

    auto smeared = conf->get<bool>("smeared");

    auto min = conf->get<float>("min");
    auto max = conf->get<float>("max");

    auto suffix = conf->get<std::string>("suffix");

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

    std::vector<float> bjet_pt = {1.0, 1.0};

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open input files */
    std::vector<TFile*> files(inputs.size(), nullptr);
    zip([&](auto& file, auto const& input) {
        file = new TFile((base + input).data(), "read");
    }, files, inputs);

    /* load histograms */
    std::vector<std::string> base_stubs(5);
    std::vector<std::string> syst_stubs(5);

    zip([&](auto& base, auto& syst, auto const& tag) {
        base = tag + "_base_mean_raw_sub_";
        syst = tag + "_syst_mean_raw_sub_";
    }, base_stubs, syst_stubs, tags);

    /* prepare plots */
    auto hb = new pencil();
    hb->category("system", "pp", "aa");

    hb->alias("aa", "PbPb");
    hb->alias("pp", "pp");
    if (smeared) hb->alias("pp", "pp (smeared)");

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto photon_selections = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs)  + 
                ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
            auto jet_selections = "anti-k_{T} R = 0.3, " + to_text(bjet_pt[0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.86, 0.65, photon_selections.data());
            l->DrawLatexNDC(0.86, 0.60, jet_selections.data());
        }
    };

    auto blurb = [&](int64_t index) {
        if (index > 0) {
            auto system_tag = "PbPb 1.69 nb^{-1}, pp 302 pb^{-1}"s;
            auto energy = "#sqrt{s_{NN}} = 5.02 TeV"s;
            auto cms = "#bf{#scale[1.4]{CMS}}"s;

            TLatex* l = new TLatex();
            l->SetTextAlign(11);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.14, 0.83, cms.data());
            l->DrawLatexNDC(0.14, 0.77, system_tag.data());
            l->DrawLatexNDC(0.14, 0.73, energy.data());
        }
    };

    /* get histograms */
    zip([&](auto const& figure, auto type) {
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

        std::vector<history<TH1F>*> hist_inputs(5, nullptr);
        std::vector<history<TH1F>*> syst_inputs(5, nullptr);

        zip([&](auto& hist_input, auto& syst_input, auto const file,
                auto const& base_stub, auto const& syst_stub) {
            hist_input = new history<TH1F>(file, base_stub + figure);
            syst_input = new history<TH1F>(file, syst_stub + figure);
        }, hist_inputs, syst_inputs, files, base_stubs, syst_stubs);

        std::vector<history<TH1F>*> hists(2, nullptr);
        std::vector<history<TH1F>*> systs(2, nullptr);

        auto incl = new interval(""s, 9, 0.f, 9.f);
        auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);

        for (size_t i = 0; i < hists.size(); ++i) {
            hists[i] = new history<TH1F>("hist"s + to_text(i), "", fincl, 1);
            systs[i] = new history<TH1F>("syst"s + to_text(i), "", fincl, 1);

            title(std::bind(rename_axis, _1, "<#deltaj>"), hists[i]);
        }

        for (int64_t j = 0; j < hist_inputs[0]->size(); ++j) {
            (*hists[0])[0]->SetBinContent((j + 1)*2, (*hist_inputs[0])[j]->GetBinContent(1));
            (*hists[0])[0]->SetBinError((j + 1)*2, (*hist_inputs[0])[j]->GetBinError(1));

            (*systs[0])[0]->SetBinContent((j + 1)*2, (*syst_inputs[0])[j]->GetBinContent(1));
        }

        for (int64_t j = 0; j < hist_inputs[0]->size(); ++j) {
            (*hists[1])[0]->SetBinContent((j + 1)*2, (*hist_inputs[j + 1])[0]->GetBinContent(1));
            (*hists[1])[0]->SetBinError((j + 1)*2, (*hist_inputs[j + 1])[0]->GetBinError(1));

            (*systs[1])[0]->SetBinContent((j + 1)*2, (*syst_inputs[j + 1])[0]->GetBinContent(1));
        }

        /* link histograms, uncertainties */
        std::unordered_map<TH1*, TH1*> links;
        zip([&](auto hist, auto syst) {
            hist->apply([&](TH1* h, int64_t index) {
                links[h] = (*syst)[index]; });
        }, hists, systs);

        std::unordered_map<TH1*, int32_t> colours;
        hists[0]->apply([&](TH1* h) { colours[h] = 1; });

        /* uncertainty box */
        auto box = [&](TH1* h, int64_t) {
            TGraph* gr = new TGraph();
            gr->SetFillStyle(1001);
            gr->SetFillColorAlpha(colours[h] ? red : blue, 0.48);

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
        };

        (*hists[0])[0]->GetXaxis()->SetBinLabel(1, " ");
        (*hists[0])[0]->GetXaxis()->SetBinLabel(2, "50-90%");
        (*hists[0])[0]->GetXaxis()->SetBinLabel(3, " ");
        (*hists[0])[0]->GetXaxis()->SetBinLabel(4, "30-50%");
        (*hists[0])[0]->GetXaxis()->SetBinLabel(5, " ");
        (*hists[0])[0]->GetXaxis()->SetBinLabel(6, "10-30%");
        (*hists[0])[0]->GetXaxis()->SetBinLabel(7, " ");
        (*hists[0])[0]->GetXaxis()->SetBinLabel(8, "0-10%");
        (*hists[0])[0]->GetXaxis()->SetBinLabel(9, " ");
        (*hists[0])[0]->GetXaxis()->SetTickLength(0);
        (*hists[0])[0]->GetXaxis()->SetLabelSize(0.05);

        /* prepare papers */
        auto s = new paper(set + "_" + prefix + "_results_ss_mean" + suffix, hb);
        apply_style(s, ""s, ""s, min, max);
        s->accessory(kinematics);
        s->accessory(blurb);
        s->jewellery(box);

        /* draw histograms with uncertainties */
        // (*hists[0])[0]->GetYaxis()->SetMaxDigits(1);

        s->add((*hists[0])[0], "aa");
        s->stack((*hists[1])[0], "pp");

        s->adjust((*hists[0])[0], "pe", "plf");
        s->adjust((*hists[1])[0], "pe", "plf");

        auto pp_style = [](TH1* h) {
            h->SetFillColorAlpha(blue, 0.5);
            h->SetLineColor(1);
            h->SetMarkerStyle(25);
            h->SetMarkerSize(0.60);
        };

        auto aa_style = [](TH1* h) {
            h->SetFillColorAlpha(red, 0.5);
            h->SetLineColor(1);
            h->SetMarkerStyle(20);
            h->SetMarkerSize(0.60);
        };

        hb->style("pp", pp_style);
        hb->style("aa", aa_style);
        hb->sketch();

        s->draw("pdf");
    }, figures, types);

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return congratulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
