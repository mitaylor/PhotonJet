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

    auto is_paper = conf->get<bool>("paper");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto heavyion = sel->get<bool>("heavyion");

    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto const jet_eta_abs = sel->get<float>("jet_eta_abs");

    auto const photon_eta_abs = sel->get<float>("photon_eta_abs");
    auto const jet_dr_max = sel->get<float>("jet_dr_max");

    auto bpho_pt = sel->get<std::vector<float>>("photon_pt_bounds");
    auto ptg_range = sel->get<std::vector<float>>("ptg_range");

    auto osg = sel->get<std::vector<int64_t>>("osg");
    auto osg_part1 = sel->get<std::vector<int64_t>>("osg_part1");
    auto osg_part2 = sel->get<std::vector<int64_t>>("osg_part2");

    std::vector<int32_t> drange = { dcent.front(), dcent.back() };
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
    hb->category("type", "total", legend_keys);

    zip([&](auto const& label, auto const& legend) {
        hb->alias(label, legend); }, legend_keys, legends);

    /* lambdas */
    std::function<void(TH1*)> square_ = [](TH1* h) {
        for_contents([](std::array<double, 1> v) {
            return v[0] * v[0]; }, h); };
    std::function<void(TH1*)> sqrt_ = [](TH1* h) {
        for_contents([](std::array<double, 1> v) {
            return std::sqrt(v[0]); }, h); };

    auto kinematics = [&](int64_t index) {
        if (index > 0) {
            auto photon_selections = to_text(bpho_pt[0]) + " < p_{T}^{#gamma} < "s + to_text(bpho_pt[1]) + " GeV, |#eta^{#gamma}| < "s + to_text(photon_eta_abs)  + 
                ", #Delta#phi_{j#gamma} > " + to_text(dphi_min_numerator) + "#pi/"s + to_text(dphi_min_denominator);
            auto jet_selections = "anti-k_{T} R = 0.3, " + to_text(bjet_pt[0]) + " < p_{T}^{jet} < "s + to_text(bjet_pt[1]) + " GeV, |#eta^{jet}| < "s + to_text(jet_eta_abs);

            TLatex* l = new TLatex();
            l->SetTextAlign(31);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.86, 0.41, photon_selections.data());
            l->DrawLatexNDC(0.86, 0.36, jet_selections.data());
        }
    };

    auto blurb = [&](int64_t index) {
        if (index > 0) {
            auto system_tag = (heavyion) ? "PbPb 1.69 nb^{-1}, "s : "pp 302 pb^{-1}, "s;
            system_tag += "#sqrt{s_{NN}} = 5.02 TeV"s;
            auto cms = "#bf{#scale[1.4]{CMS}} #font[52]{Preliminary}"s;

            TLatex* l = new TLatex();
            l->SetTextAlign(11);
            l->SetTextFont(43);
            l->SetTextSize(11);
            l->DrawLatexNDC(0.14, 0.83, cms.data());
            l->DrawLatexNDC(0.14, 0.77, system_tag.data());
        }
    };

    auto hf_info = [&](int64_t index, int64_t cent) {
        if (index > 0) {
            std::string text = "Cent. "s + to_text(dcent[cent + 1]) + " - " + to_text(dcent[cent]) + "%";

            TLatex* l = new TLatex();
            l->SetTextAlign(11);
            l->SetTextFont(43);
            l->SetTextSize(11);

            l->DrawLatexNDC(0.14, 0.73, text.data());
        }
    };

    auto range_info = [&](int64_t index) {
        if (index > 0) {
            std::string text = "Cent. "s + to_text(drange[1]) + " - " + to_text(drange[0]) + "%";

            TLatex* l = new TLatex();
            l->SetTextAlign(11);
            l->SetTextFont(43);
            l->SetTextSize(11);

            l->DrawLatexNDC(0.14, 0.73, text.data());
        }
    };

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    std::string system_tag = "  #sqrt{s_{NN}} = 5.02 TeV"s;
    system_tag += (tag == "aa") ? ", 1.69 nb^{-1}"s : ", 302 pb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}}"s;
    if (!is_paper) cms += " #it{#scale[1.2]{Preliminary}}"s;

    /* calculate variations */
    zip([&](auto const& figure, auto type, auto cols, auto range) {
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

        std::vector<paper*> cs(cols, nullptr);

        for (int i = 0; i < cols; ++i) {
            cs[i] = new paper(set + "_" + tag + "_var"s + stub + "_" + to_text(i), hb);
            apply_style(cs[i], "", "", 0.00001, range);
            cs[i]->divide(2, -1);

            if (heavyion && cols == 4) { cs[i]->accessory(std::bind(hf_info, _1, i)); }
            else if (heavyion) { cs[i]->accessory(range_info); }

            cs[i]->accessory(kinematics);
            cs[i]->accessory(blurb);
            cs[i]->set(paper::flags::logy);
        }

        auto base = new history<TH1F>(f, tag + "_"s + label + stub, "base_"s + tag + "_"s + label + stub);
        title(std::bind(rename_axis, _1, "1/N^{#gamma}dN/d#deltaj"), base);
        std::vector<history<TH1F>*> sets;
        std::vector<history<TH1F>*> batches(inputs.size(), nullptr);

        zip([&](auto& batch, auto file, auto const& label) {
            batch = new history<TH1F>(file, tag + "_" + label + stub, "batch_"s + tag + "_"s + label + stub);
            title(std::bind(rename_axis, _1, "1/N^{#gamma}dN/d#deltaj"), batch);
        }, batches, files, labels);

        auto total = new history<TH1F>(*base, "total");
        title(std::bind(rename_axis, _1, "1/N^{#gamma}dN/d#deltaj"), total);
        total->apply([](TH1* h) { h->Reset("MICES"); });

        for (auto const& batch : batches) {
            batch->add(*base, -1);

            // for (int64_t i = 0; i < batch->size(); ++i) {
            //     std::vector<float> differences;

            //     for (int64_t j = 0; j <= (*batch)[i]->GetNbinsX(); ++j) {
            //         differences.push_back(std::abs((*batch)[i]->GetBinContent(j + 1)));
            //     }

            //     float min = 10000;
            //     float max = 0;
            //     for (auto difference : differences) {
            //         if (difference < min) { min = difference; }
            //         if (difference > max) { max = difference; }
            //     }

            //     printf("%.2f-%.2f ", min, max);
            // }
            // std::cout << std::endl;

            batch->apply(square_);
            // batch->apply(sqrt_);

            // /* apply smoothing */
            // for (int64_t i = 0; i < batch->size(); ++i) {
            //     for (int64_t j = 1; j <= 6; ++j) {
            //     // for (int64_t j = 1; j <= (*batch)[i]->GetNbinsX(); ++j) {
            //         if (j == 1) {
            //             double value = (std::abs((*batch)[i]->GetBinContent(j) + (*batch)[i]->GetBinContent(j + 1))) / 2;
            //             (*batch)[i]->SetBinContent(j, value);
            //         }
            //         else if (j == (*batch)[i]->GetNbinsX()) {
            //             double value = (std::abs((*batch)[i]->GetBinContent(j) + (*batch)[i]->GetBinContent(j - 1))) / 2;
            //             (*batch)[i]->SetBinContent(j, value);
            //         } 
            //         else {
            //             double value = (std::abs((*batch)[i]->GetBinContent(j) + (*batch)[i]->GetBinContent(j - 1) + (*batch)[i]->GetBinContent(j + 1))) / 2;
            //             (*batch)[i]->SetBinContent(j, value);
            //         }
            //     }
            // }

            // batch->apply(square_);
        }

        zip([&](auto const& batch, auto group) {
            if (group == static_cast<int32_t>(sets.size())) {
                sets.push_back(new history<TH1F>(*batch, "set"));
            } else {
                sets[group]->apply([&](TH1* h, int64_t i) {
                    for_contents([](std::array<double, 2> v) -> float {
                        return std::max(v[0], v[1]);
                    }, h, (*batch)[i]); });
            }

            batch->apply(sqrt_);
        }, batches, groups);

        for (auto const& set : sets) {
            total->add(*set, 1);
            set->apply(sqrt_);
        }

        total->apply(sqrt_);

        for (size_t i = 0; i <= sets.size(); ++i) {
            for (int64_t j = 0; j < total->size(); ++j) {
                double avg;

                if (i < sets.size()) {
                    avg = (*sets[i])[j]->Integral("width")/jet_dr_max;
                }
                else {
                    avg = (*total)[j]->Integral("width")/jet_dr_max;
                }

                printf("%.3f ", avg);
            }
            std::cout << std::endl;
        }

        // for (int64_t i = 0; i < total->size(); ++i) {
        //     std::vector<float> differences;

        //     for (int64_t j = 0; j <= (*total)[i]->GetNbinsX(); ++j) {
        //         differences.push_back(std::abs((*total)[i]->GetBinContent(j + 1)));
        //     }

        //     float min = 10000;
        //     float max = 0;
        //     for (auto difference : differences) {
        //         if (difference < min) { min = difference; }
        //         if (difference > max) { max = difference; }
        //     }

        //     printf("%.2f-%.2f ", min, max);
        // }
        // std::cout << std::endl;

        /* add plots */
        for (int i = 0; i < cols; ++i) {
            auto style = [&](TH1* h) { cs[i]->adjust(h, "hist", "l"); };

            auto rename = "total_rename_" + to_text(i);
            TH1F* total_rename = (TH1F*) (*total)[i]->Clone(rename.c_str());

            cs[i]->add((*total)[i], "total"); style((*total)[i]);
            cs[i]->add(total_rename, "total"); style(total_rename);

            zip([&](auto& batch, auto const& label, auto const& plot) {
                if (plot == 1) {
                    cs[i]->stack(1, (*batch)[i], label); style((*batch)[i]);
                }
                else {
                    cs[i]->stack(2, (*batch)[i], label); style((*batch)[i]);
                }
            }, sets, legend_keys, plots);
        }

        zip([&](auto& batch) {
            batch->save(tag);
        }, batches);

        /* save histograms */
        for (auto const& set : sets)
            set->save(tag);

        base->save(tag);
        total->save(tag);

        hb->sketch();

        for (auto c : cs) {
            c->draw("pdf");
        }
    }, figures, types, columns, ranges);

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return obnubilate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}