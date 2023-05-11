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
    auto prefix = conf->get<std::string>("prefix");

    auto smeared = conf->get<bool>("smeared");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    auto bpho_pt = sel->get<std::vector<float>>("photon_pt_bounds");
    auto bdr = sel->get<std::vector<float>>("dr_bounds");
    auto bjet_pt = sel->get<std::vector<float>>("jet_pt_bounds"); std::cout << __LINE__ << std::endl;

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();std::cout << __LINE__ << std::endl;

    /* open input files */
    std::vector<TFile*> files(inputs.size(), nullptr);
    zip([&](auto& file, auto const& input) {
        file = new TFile((base + input).data(), "read");
    }, files, inputs);std::cout << __LINE__ << std::endl;

    /* prepare plots */
    auto hb = new pencil();
    hb->category("system", "pp", "aa");

    hb->alias("aa", "PbPb");
    if (smeared) hb->alias("pp", "pp (smeared)");
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
            l->DrawLatexNDC(0.865, 0.42, photon_selections.data());
            l->DrawLatexNDC(0.865, 0.37, jet_selections.data());
        }
    };
std::cout << __LINE__ << std::endl;
    /* get histograms */
    std::vector<history<TH1F>*> hist_inputs(5, nullptr);
    std::vector<history<TH1F>*> syst_inputs(5, nullptr);
std::cout << __LINE__ << std::endl;
    zip([&](auto& hist_input, auto& syst_input, auto const file,
            auto const& tag) {
        hist_input = new history<TH1F>(file, tag + "_base_mean");
        syst_input = new history<TH1F>(file, tag + "_syst_mean");
    }, hist_inputs, syst_inputs, files, tags);
std::cout << __LINE__ << std::endl;
    std::vector<history<TH1F>*> hists(2, nullptr);
    std::vector<history<TH1F>*> systs(2, nullptr);
std::cout << __LINE__ << std::endl;
    auto incl = new interval(""s, 9, 0.f, 9.f);
    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
std::cout << __LINE__ << std::endl;
    for (size_t i = 0; i < hists.size(); ++i) {
        hists[i] = new history<TH1F>("hist"s + to_text(i), "", fincl, 1);
        systs[i] = new history<TH1F>("syst"s + to_text(i), "", fincl, 1);

        title(std::bind(rename_axis, _1, "<#deltaj>"), hists[i]);

        for (int64_t j = 0; j < (*hists[i])[0]->GetNbinsX(); ++j) {
            (*hists[i])[0]->SetBinContent((j + 1)*2, (*hist_inputs[i])[j]->GetBinContent(1));
            (*hists[i])[0]->SetBinError((j + 1)*2, (*hist_inputs[i])[j]->GetBinError(1));

            (*systs[i])[0]->SetBinContent((j + 1)*2, (*syst_inputs[i])[j]->GetBinContent(1));
        }
    }
std::cout << __LINE__ << std::endl;
    /* link histograms, uncertainties */
    std::unordered_map<TH1*, TH1*> links;
    zip([&](auto hist, auto syst) {
        hist->apply([&](TH1* h, int64_t index) {
            links[h] = (*syst)[index]; });
    }, hists, systs);
std::cout << __LINE__ << std::endl;
    std::unordered_map<TH1*, int32_t> colours;
    hists[0]->apply([&](TH1* h) { colours[h] = 1; });
std::cout << __LINE__ << std::endl;
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
std::cout << __LINE__ << std::endl;
    /* prepare papers */
    auto s = new paper(set + "_" + prefix + "_results_ss_mean", hb);
    apply_style(s, "#bf{#scale[1.4]{CMS}}     #sqrt{s_{NN}} = 5.02 TeV"s, "PbPb 1.69 nb^{-1}, pp 302 pb^{-1}"s, 0, 0.1);
    s->accessory(kinematics);
    s->jewellery(box);
std::cout << __LINE__ << std::endl;
    /* draw histograms with uncertainties */
    s->add((*hists[0])[0], "aa");
    s->stack((*hists[1])[0], "pp");
std::cout << __LINE__ << std::endl;
    auto pp_style = [](TH1* h) {
        h->SetLineColor(1);
        h->SetMarkerStyle(25);
        h->SetMarkerSize(0.60);
    };
std::cout << __LINE__ << std::endl;
    auto aa_style = [](TH1* h) {
        h->SetLineColor(1);
        h->SetMarkerStyle(20);
        h->SetMarkerSize(0.60);
    };

    hb->style("pp", pp_style);
    hb->style("aa", aa_style);
    hb->sketch();

    s->draw("pdf");

    in(output, []() {});

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return congratulate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
