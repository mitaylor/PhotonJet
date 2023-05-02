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

static auto const red = TColor::GetColor("#f2777a");
static auto const blue = TColor::GetColor("#6699cc");

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

    auto xmins = conf->get<std::vector<float>>("xmin");
    auto xmaxs = conf->get<std::vector<float>>("xmax");
    auto ymins = conf->get<std::vector<float>>("ymin");
    auto ymaxs = conf->get<std::vector<float>>("ymax");
    auto oflows = conf->get<std::vector<bool>>("oflow");

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

    auto aa_info = [&](int64_t index, history<TH1F>* h) {
        stack_text(index, 0.73, 0.04, h, hf_info); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto photon_selections = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs)  + 
                ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
            auto jet_selections = "anti-k_{T} R = 0.3, " + to_text(bjet_pt[0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.42, photon_selections.data());
            l->DrawLatexNDC(0.865, 0.37, jet_selections.data());
        }
    };

    zip([&](auto const& figure, auto xmin, auto xmax, auto ymin, auto ymax,
            auto integral) {
        /* get histograms */
        std::vector<history<TH1F>*> hists(6, nullptr);
        std::vector<history<TH1F>*> systs(6, nullptr);

        zip([&](auto& hist, auto& syst, auto const file,
                auto const& base_stub, auto const& syst_stub) {
            hist = new history<TH1F>(file, base_stub + figure);
            title(std::bind(rename_axis, _1, "1/N^{#gammaj}dN/d#deltaj"), hist);
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

        /* minor adjustments */
        if (integral) { xmin = convert_pi(xmin); xmax = convert_pi(xmax); }

        /* prepare papers */
        auto p = new paper(set + "_" + prefix + "_results_pp_" + figure, hb);
        apply_style(p, "#bf{#scale[1.4]{CMS}}     #sqrt{s} = 5.02 TeV"s, "pp 302 pb^{-1}"s, ymin, ymax);
        p->accessory(std::bind(line_at, _1, 0.f, xmin, xmax));
        p->accessory(kinematics);
        p->jewellery(box);
        p->divide(-1, 1);

        auto a = new paper(set + "_" + prefix+ "_results_aa_" + figure, hb);
        apply_style(a, "#bf{#scale[1.4]{CMS}}     #sqrt{s_{NN}} = 5.02 TeV"s, "PbPb 1.6 nb^{-1}"s, ymin, ymax);
        a->accessory(std::bind(line_at, _1, 0.f, xmin, xmax));
        a->accessory(std::bind(aa_info, _1, hists[0]));
        a->accessory(kinematics);
        a->jewellery(box);
        a->divide(ihf->size()/2, -1);

        auto s = new paper(set + "_" + prefix + "_results_ss_" + figure, hb);
        apply_style(s, "#bf{#scale[1.4]{CMS}}     #sqrt{s_{NN}} = 5.02 TeV"s, "PbPb 1.69 nb^{-1}, pp 302 pb^{-1}"s, ymin, ymax);
        s->accessory(std::bind(line_at, _1, 0.f, xmin, xmax));
        s->accessory(std::bind(aa_info, _1, hists[0]));
        s->accessory(kinematics);
        s->jewellery(box);
        s->divide(ihf->size()/2, -1);

        /* draw histograms with uncertainties */
        hists[0]->apply([&](TH1* h) { a->add(h, "aa"); s->add(h, "aa"); });
        hists[1]->apply([&](TH1* h) { p->add(h, "pp"); });

        for (int64_t i = 0; i < 4; ++i) {
            hists[i + 2]->apply([&](TH1* h, int64_t index) {
                s->stack(i + index + 1, h, "ss");
            });
        }

        auto pp_style = [](TH1* h) {
            h->SetLineColor(1);
            h->SetMarkerStyle(25);
            h->SetMarkerSize(0.60);
        };

        auto aa_style = [](TH1* h) {
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
    }, figures, xmins, xmaxs, ymins, ymaxs, oflows);

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return congratulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
