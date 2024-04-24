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
#include "TColor.h"

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
    auto groups = conf->get<std::vector<int32_t>>("groups");
    auto plots = conf->get<std::vector<int32_t>>("plots");
    auto legends = conf->get<std::vector<std::string>>("legends");
    auto legend_keys = conf->get<std::vector<std::string>>("legend_keys");

    auto figures = conf->get<std::vector<std::string>>("figures");
    auto types = conf->get<std::vector<int64_t>>("types");
    auto columns = conf->get<std::vector<int32_t>>("columns");
    auto ranges = conf->get<std::vector<float>>("ranges");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto heavyion = sel->get<bool>("heavyion");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");

    auto bpho_pt = sel->get<std::vector<float>>("photon_pt_bounds");
    auto ptg_range = sel->get<std::vector<float>>("ptg_range");

    auto osg = sel->get<std::vector<int64_t>>("osg");
    auto osg_part1 = sel->get<std::vector<int64_t>>("osg_part1");
    auto osg_part2 = sel->get<std::vector<int64_t>>("osg_part2");

    std::vector<float> bjet_pt = {1.0, 1.0};

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

    /* lambdas */
    std::function<void(TH1*)> square_ = [](TH1* h) {
        for_contents([](std::array<double, 1> v) {
            return v[0] * v[0]; }, h); };
    std::function<void(TH1*)> sqrt_ = [](TH1* h) {
        for_contents([](std::array<double, 1> v) {
            return std::sqrt(v[0]); }, h); };

    auto shader = [&](TH1* h, float max) {
        default_formatter(h, 0., max);
        auto col = h->GetLineColor();
        h->SetFillColorAlpha(col, 0.32);
        h->SetLineWidth(1);
    };

    std::vector<int32_t> const colours = {
        TColor::GetColor("#515151"),
        TColor::GetColor("#FF484E"),
        TColor::GetColor("#FF9E48"),
        TColor::GetColor("#FFFA48"),
        TColor::GetColor("#A9FF48"),
        TColor::GetColor("#48FF9E"),
        TColor::GetColor("#48FFFA"),
        TColor::GetColor("#48A9FF"),
        TColor::GetColor("#484EFF"),
        TColor::GetColor("#9E48FF"),
        TColor::GetColor("#F948FF"),
        TColor::GetColor("#FF484E"),
        TColor::GetColor("#FF9E48"),
        TColor::GetColor("#FFFA48"),
        TColor::GetColor("#A9FF48"),
        TColor::GetColor("#9E48FF"),
        TColor::GetColor("#F948FF")
    };

    auto box = [&](TH1* h, int64_t) {
        for (int64_t i = 0; i < h->GetNbinsX(); ++i) {
            TBox *b = new TBox(h->GetBinLowEdge(i + 1),
                                0,
                                h->GetBinWidth(i + 1) + h->GetBinLowEdge(i + 1),
                                h->GetBinContent(i + 1));
            b->SetFillColorAlpha(colours[i], 0.5);
            b->SetLineColor(colours[i]);
            b->Draw("l");
        }
    };

    auto hf_info = [&](int64_t index) {
        info_text(index, 0.73, "Cent. %i - %i%%", dcent, true); };

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
            auto system_tag = (heavyion) ? "PbPb 1.69 nb^{-1}, "s : "pp 302 pb^{-1}, "s;
            system_tag += "#sqrt{s_{NN}} = 5.02 TeV"s;
            auto cms = "#bf{#scale[1.4]{CMS}}"s;

            TLatex* l = new TLatex();
            l->SetTextAlign(11);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.14, 0.83, cms.data());
            l->DrawLatexNDC(0.14, 0.77, system_tag.data());
        }
    };

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    /* calculate variations */
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

        auto stub = "_"s + figure;

        auto c1 = new paper(set + "_" + tag + "_mean_var"s + stub, hb);
        apply_style(c1, "", "", std::bind(shader, _1, 0.004));

        auto base = new history<TH1F>(f, tag + "_"s + label + stub, "base_"s + tag + "_"s + label + stub);
        std::vector<history<TH1F>*> batches(inputs.size(), nullptr);
        zip([&](auto& batch, auto file, auto const& label) {
            batch = new history<TH1F>(file, tag + "_" + label + stub, "batch_"s + tag + "_"s + label + stub);
        }, batches, files, labels);

        auto incl_all = new interval(""s, inputs.size() + 1, 0.f, (float) inputs.size() + 1);
        auto fmean_all = std::bind(&interval::book<TH1F>, incl_all, _1, _2, _3);

        auto incl = new interval(""s, legends.size() + 1, 0.f, (float) legends.size() + 1);
        auto fmean = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);

        auto means = new history<TH1F>("syst_mean"s + stub, "", fmean, base->size());
        auto means_all = new history<TH1F>("syst_mean_all"s + stub, "", fmean_all, base->size());
        auto base_mean_all = new history<TH1F>("base_mean_all"s + stub, "", fmean_all, base->size());

        for (int64_t i = 0; i < base->size(); ++i) {
            for (int64_t j = 0; j < (*base_mean_all)[i]->GetNbinsX(); ++j) {
                (*base_mean_all)[i]->SetBinContent(j + 1, (*base)[i]->GetMean());
                (*base_mean_all)[i]->SetBinError(j + 1, (*base)[i]->GetMeanError());
            }
        }

        /* calculate the mean deviation for each systematic */
        for (size_t k = 0; k < inputs.size(); ++k) {
            title(std::bind(rename_axis, _1, "<#deltaj>"), means);

            for (int64_t i = 0; i < means->size(); ++i) {
                float difference = (*(batches[k]))[i]->GetMean() - (*base_mean_all)[i]->GetBinContent(k + 2);

                (*means_all)[i]->SetBinContent(k + 2, difference * difference);
                (*means_all)[i]->SetBinError(k + 2, 0);

                printf("%.5f ", difference);
            }
            std::cout << std::endl;
        }

        /* calculate the total systematic error */
        for (int64_t i = 0; i < means->size(); ++i) {
            std::vector<double> sets;

            for (size_t k = 0; k < inputs.size(); ++k) {
                auto difference = (*means_all)[i]->GetBinContent(k + 2);
                if (groups[k] == (int32_t) sets.size()) {
                    sets.push_back(difference);
                } else {
                    difference = std::max(sets[groups[k]], difference);
                    sets[groups[k]] = difference;
                }

                (*means)[i]->SetBinContent(groups[k] + 2, std::sqrt(difference));
            }

            float total = 0;
            for (auto set : sets) {
                total += set;
            }

            (*means)[i]->SetBinContent(1, std::sqrt(total));
            (*means)[i]->SetFillColorAlpha(1, 0);
        }

        std::cout << std::endl;
        for (int64_t i = 0; i < means->size(); ++i) {
            float difference = (*means)[i]->GetBinContent(1);
                printf("%.5f ", difference);
        }
        std::cout << std::endl;

        /* set bin labels */
        for (int64_t i = 0; i < means->size(); ++i) {
            for (size_t k = 0; k < labels.size(); ++k) {
                (*means)[i]->GetXaxis()->SetBinLabel(k + 2, legends[k].data());
            }
            (*means)[i]->GetXaxis()->SetBinLabel(1, "total");
            (*means)[i]->GetXaxis()->SetTicks("-");
        }

        /* add plots */
        // auto style1 = [&](TH1* h) { c1->adjust(h, "hist", "f"); };

        means->apply([&](TH1* h) { 
            c1->add(h); // style1(h); 
        });

        /* add info text */
        c1->accessory(hf_info); 
        c1->accessory(kinematics);
        c1->accessory(blurb);
        c1->jewellery(box);

        /* save histograms */
        base_mean_all->save(tag);
        means_all->save(tag);
        means->save(tag);

        hb->sketch();

        c1->draw("pdf");
    }, figures, types);

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return obnubilate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}