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
    auto types = conf->get<std::vector<std::string>>("types");
    auto prefix = conf->get<std::string>("prefix");

    auto smeared = conf->get<bool>("smeared");

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
    auto ptg_range = sel->get<std::vector<float>>("ptg_range");

    auto osg = sel->get<std::vector<int64_t>>("osg");
    auto osg_part1 = sel->get<std::vector<int64_t>>("osg_part1");
    auto osg_part2 = sel->get<std::vector<int64_t>>("osg_part2");

    std::vector<float> bjet_pt = {1.0, 1.0};

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
    std::vector<std::string> base_stubs(6);
    std::vector<std::string> syst_stubs(6);

    zip([&](auto& base, auto& syst, auto const& tag) {
        base = tag + "_base_" + tag + "_nominal_s_pure_raw_sub_";
        syst = tag + "_total_base_" + tag + "_nominal_s_pure_raw_sub_";
    }, base_stubs, syst_stubs, tags);

    /* prepare plots */
    auto hb = new pencil();
    hb->category("system", "pp", "aa", "ss");

    hb->alias("aa", "PbPb");
    if (smeared) hb->alias("ss", "pp (smeared)");
    else hb->alias("ss", "pp");

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
            l->DrawLatexNDC(0.86, 0.65, photon_selections.data());
            l->DrawLatexNDC(0.86, 0.60, jet_selections.data());
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

    auto blurb_pp = [&](int64_t index) {
        if (index > 0) {
            auto system_tag = "pp 302 pb^{-1}"s;
            auto energy = "#sqrt{s_{NN}} = 5.02 TeV"s;
            auto cms = "#bf{#scale[1.4]{CMS}} #sqrt{s_{NN}} = 5.02 TeV"s;

            TLatex* l = new TLatex();
            l->SetTextAlign(11);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.14, 0.83, cms.data());
            l->DrawLatexNDC(0.14, 0.77, system_tag.data());
        }
    };

    auto blurb_aa = [&](int64_t index) {
        if (index > 0) {
            auto system_tag = "PbPb 1.69 nb^{-1}"s;
            auto energy = "#sqrt{s_{NN}} = 5.02 TeV"s;
            auto cms = "#bf{#scale[1.4]{CMS}} #sqrt{s_{NN}} = 5.02 TeV"s;

            TLatex* l = new TLatex();
            l->SetTextAlign(11);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.14, 0.83, cms.data());
            l->DrawLatexNDC(0.14, 0.77, system_tag.data());
        }
    };

    zip([&](auto const& figure, auto type, auto ymin, auto ymax) {
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


        /* get histograms */
        std::vector<history<TH1F>*> hists(6, nullptr);
        std::vector<history<TH1F>*> systs(6, nullptr);

        zip([&](auto& hist, auto& syst, auto const file,
                auto const& base_stub, auto const& syst_stub) {
            hist = new history<TH1F>(file, base_stub + figure);
            title(std::bind(rename_axis, _1, "1/N^{#gamma}dN^{#gammaj}/d#deltaj"), hist);
            syst = new history<TH1F>(file, syst_stub + figure);
        }, hists, systs, files, base_stubs, syst_stubs);

        for (size_t i = 2; i < files.size(); ++i) {
            std::string name1 = std::to_string(i) + std::string("happy");
            std::string name2 = std::to_string(i) + std::string("sad");
            hists[i]->rename(name1);
            systs[i]->rename(name2);
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

        /* prepare papers */
        auto p = new paper(set + "_" + prefix + "_results_pp_" + figure, hb);
        apply_style(p, ""s, ""s, ymin, ymax);
        p->accessory(std::bind(line_at, _1, 0.f, bdr[0], bdr[1]));
        p->accessory(kinematics);
        p->accessory(blurb_pp);
        p->jewellery(box);
        p->divide(-1, 1);

        auto a = new paper(set + "_" + prefix+ "_results_aa_" + figure, hb);
        apply_style(a, ""s, ""s, ymin, ymax);
        a->accessory(std::bind(line_at, _1, 0.f, bdr[0], bdr[1]));
        a->accessory(kinematics);
        a->accessory(blurb_aa);
        a->jewellery(box);
        if (hists[0]->size() == ihf->size()) { 
            a->accessory(std::bind(aa_hf_info, _1, hists[0])); 
            a->divide(hists[0]->size()/2, -1);
        } else { 
            a->accessory(std::bind(aa_range_info, _1, hists[0])); 
        }

        auto s = new paper(set + "_" + prefix + "_results_ss_" + figure, hb);
        s->accessory(std::bind(line_at, _1, 0.f, bdr[0], bdr[1]));
        s->accessory(kinematics);
        s->accessory(blurb);
        s->jewellery(box);

        if (hists[0]->size() == ihf->size()) { 
            apply_style(s, ""s, ""s, ymin, ymax);
            s->accessory(std::bind(aa_hf_info, _1, hists[0])); 
            s->divide(hists[0]->size()/2, -1);
        } else { 
            apply_style(s, ""s, ""s, ymin, ymax);
            s->accessory(std::bind(aa_range_info, _1, hists[0])); 
        }

        /* draw histograms with uncertainties */
        hists[0]->apply([&](TH1* h) {
            a->add(h, "aa");
            s->add(h, "aa"); 

            a->adjust(h, "pe", "plf");
            s->adjust(h, "pe", "plf");
        });

        hists[1]->apply([&](TH1* h) {
            p->add(h, "pp");
            p->adjust(h, "pe", "plf");
        });

        for (int64_t i = 0; i < hists[0]->size(); ++i) {
            hists[i + 2]->apply([&](TH1* h, int64_t index) {
                s->stack(i + index + 1, h, "ss");
                s->adjust(h, "pe", "plf");
            });
        }

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
        hb->style("ss", pp_style);
        hb->sketch();

        p->draw("pdf");
        a->draw("pdf");
        s->draw("pdf");
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