#include "../include/lambdas.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TLatex.h"

#include <functional>
#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename... T>
void title(std::function<void(TH1*)> f, T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply(f), 0)... };
}

int obnubilate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto ref = conf->get<std::string>("ref");
    auto label = conf->get<std::string>("label");
    auto tag = conf->get<std::string>("tag");
    auto system = conf->get<std::string>("system");

    auto inputs = conf->get<std::vector<std::string>>("inputs");
    auto labels = conf->get<std::vector<std::string>>("labels");
    auto plots = conf->get<std::vector<int32_t>>("plots");
    auto legends = conf->get<std::vector<std::string>>("legends");
    auto legend_keys = conf->get<std::vector<std::string>>("legend_keys");
    auto figures = conf->get<std::vector<std::string>>("figures");
    auto ranges = conf->get<std::vector<float>>("ranges");
    auto groups = conf->get<std::vector<int32_t>>("groups");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff"); std::cout << __LINE__ << std::endl;

    auto is_paper = conf->get<bool>("paper");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    auto bpho_pt = sel->get<std::vector<float>>("photon_pt_bounds");
    auto bjet_pt = sel->get<std::vector<float>>("jet_pt_bounds"); std::cout << __LINE__ << std::endl;

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* open input files */
    TFile* f = new TFile((base + ref).data(), "read");

    std::vector<TFile*> files(inputs.size(), nullptr);
    zip([&](auto& file, auto const& input) {
        file = new TFile((base + input).data(), "read");
    }, files, inputs);

    /* prepare plots */
    auto hb = new pencil();
    hb->category("type", "total", legend_keys);

    zip([&](auto const& label, auto const& legend) {
        hb->alias(label, legend); }, legend_keys, legends);
 std::cout << __LINE__ << std::endl;
    /* lambdas */
    std::function<void(TH1*)> square_ = [](TH1* h) {
        for_contents([](std::array<double, 1> v) {
            return v[0] * v[0]; }, h); };
    std::function<void(TH1*)> sqrt_ = [](TH1* h) {
        for_contents([](std::array<double, 1> v) {
            return std::sqrt(v[0]); }, h); };
 std::cout << __LINE__ << std::endl;
    auto shader = [&](TH1* h, float max) {
        default_formatter(h, 0., max);
        auto col = h->GetLineColor();
        h->SetFillColorAlpha(col, 0.32);
        h->SetLineWidth(1);
    };

    auto hf_info = [&](int64_t index) {
        info_text(index, 0.75, "Cent. %i - %i%%", dcent, true); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto photon_selections = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs)  + 
                ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
            auto jet_selections = "anti-k_{T} R = 0.3, " + to_text(bjet_pt[0]) + "p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(13);
            l->DrawLatexNDC(0.865, 0.41, photon_selections.data());
            l->DrawLatexNDC(0.865, 0.37, jet_selections.data());
        }
    };
 std::cout << __LINE__ << std::endl;
    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    std::string system_tag = "  #sqrt{s_{NN}} = 5.02 TeV"s;
    system_tag += (tag == "aa") ? ", 1.69 nb^{-1}"s : ", 302 pb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}}"s;
    if (!is_paper) cms += " #it{#scale[1.2]{Preliminary}}"s;
 std::cout << __LINE__ << std::endl;
    /* calculate variations */
    zip([&](auto const& figure, auto range) {
        auto stub = "_"s + figure;

        auto c1 = new paper(set + "_" + tag + "_mean_var"s + stub, hb);
        apply_style(c1, cms, system_tag, std::bind(shader, _1, range));

        auto c2 = new paper(set + "_" + tag + "_mean_var_unfolding"s + stub, hb);
        apply_style(c2, cms, system_tag, std::bind(shader, _1, range));
 std::cout << __LINE__ << std::endl;
        auto base = new history<TH1F>(f, tag + "_"s + label + stub, "base_"s + tag + "_"s + label + stub);

        std::vector<history<TH1F>*> sets;
 std::cout << __LINE__ << std::endl;
        std::vector<history<TH1F>*> means(inputs.size(), nullptr);
        auto incl = new interval("Centrality"s, 4, 0.f, 4.f);
        auto fmean = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
 std::cout << __LINE__ << std::endl;
        auto base_mean = new history<TH1F>("mean_base"s, "", fmean, 1);
        title(std::bind(rename_axis, _1, "<#deltaj>"), base_mean);
 std::cout << __LINE__ << std::endl;
        for (int64_t i = 0; i < base->size(); ++i) {
            (*base_mean)[0]->SetBinContent(i + 1, (*base)[i]->GetMean());
            (*base_mean)[0]->SetBinError(i + 1, (*base)[i]->GetMeanError());
        }
 std::cout << __LINE__ << std::endl;
        std::vector<history<TH1F>*> batches(inputs.size(), nullptr);
        zip([&](auto& batch, auto file, auto const& label) {
            batch = new history<TH1F>(file, tag + "_" + label + stub, "batch_"s + tag + "_"s + label + stub); std::cout << __LINE__ << std::endl;
        }, batches, files, labels);
 std::cout << __LINE__ << std::endl;
        auto total = new history<TH1F>(*base_mean, "total");
        title(std::bind(rename_axis, _1, "<#deltaj>"), total);
        total->apply([](TH1* h) { h->Reset("MICES"); });
 std::cout << __LINE__ << std::endl;
        for (size_t k = 0; k < batches.size(); ++k) {
            means.push_back(new history<TH1F>("mean_"s + to_text(k), "", fmean, 1)); std::cout << __LINE__ << std::endl;
            title(std::bind(rename_axis, _1, "<#deltaj>"), means[k]);
 std::cout << __LINE__ << std::endl;
            for (int64_t i = 0; i < batches[k]->size(); ++i) {
                (*(means[k]))[0]->SetBinContent(i + 1, (*(batches[k]))[i]->GetMean());
                (*(means[k]))[0]->SetBinError(i + 1, (*(batches[k]))[i]->GetMeanError());

                float difference = (*(means[k]))[0]->GetBinContent(i + 1) - (*base_mean)[0]->GetBinContent(i + 1);
                printf("%.2f ", difference);
            }
            std::cout << std::endl;
 std::cout << __LINE__ << std::endl;
            means[k]->add(*base_mean, -1);
            means[k]->apply(square_);
        }
 std::cout << __LINE__ << std::endl;
        zip([&](auto const& mean, auto group) {
            if (group == static_cast<int32_t>(sets.size())) {
                sets.push_back(new history<TH1F>(*mean, "set"));
            } else {
                sets[group]->apply([&](TH1* h, int64_t i) {
                    for_contents([](std::array<double, 2> v) -> float {
                        return std::max(v[0], v[1]);
                    }, h, (*mean)[i]); });
            }

            mean->apply(sqrt_);
        }, means, groups);
 std::cout << __LINE__ << std::endl;
        for (auto const& set : sets)
            total->add(*set, 1);
 std::cout << __LINE__ << std::endl;
        total->apply(sqrt_);
 std::cout << __LINE__ << std::endl;
        for (int64_t i = 0; i < total->size(); ++i) {
            float difference = (*total)[0]->GetBinContent(i + 1) - (*base_mean)[0]->GetBinContent(i + 1);
                printf("%.2f ", difference);
        }
        std::cout << std::endl;
 std::cout << __LINE__ << std::endl;
        /* add plots */
        auto style1 = [&](TH1* h) { c1->adjust(h, "hist", "f"); };
        auto style2 = [&](TH1* h) { c2->adjust(h, "hist", "f"); };
 std::cout << __LINE__ << std::endl;
        total->apply([&](TH1* h) { 
            c1->add(h, "total"); style1(h); 
            c2->add(h, "total"); style2(h); 
        });
 std::cout << __LINE__ << std::endl;
        zip([&](auto& mean, auto const& label, auto const& plot) {
            if (plot == 1) {
                mean->apply([&](TH1* h, int64_t index) {
                    c1->stack(index + 1, h, label); style1(h);
                });
            }
            else {
                mean->apply([&](TH1* h, int64_t index) {
                    c2->stack(index + 1, h, label); style2(h);
                });
            }

            mean->save(tag);
        }, means, legend_keys, plots);
 std::cout << __LINE__ << std::endl;
        /* add info text */
        c1->accessory(hf_info); 
        c1->accessory(kinematics);
        c2->accessory(hf_info); 
        c2->accessory(kinematics);
 std::cout << __LINE__ << std::endl;
        /* save histograms */
        for (auto const& set : sets)
            set->save(tag);
 std::cout << __LINE__ << std::endl;
        base->save(tag);
        total->save(tag);
 std::cout << __LINE__ << std::endl;
        hb->sketch();

        c1->draw("pdf");
        c2->draw("pdf");
    }, figures, ranges);
 std::cout << __LINE__ << std::endl;
    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return obnubilate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}