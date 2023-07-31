#include "../include/lambdas.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int sum_iteration(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto tag = conf->get<std::string>("tag");
    auto input = conf->get<std::string>("input");

    auto base_label = conf->get<std::string>("base_label");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    auto label = conf->get<std::string>("label");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto rpt = sel->get<std::vector<float>>("photon_pt_bounds");

    auto mpthf = new multival(rpt, dhf);

    std::vector<int32_t> drange = { dcent.front(), dcent.back() };

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    TFile* f = new TFile((base + input).data(), "read");

    auto base_fold0 = new history<TH1F>(f, tag + "_"s + base_label + "_sum0_side0");
    auto base_fold1 = new history<TH1F>(f, tag + "_"s + base_label + "_sum0_side1");

    auto base_merge_fold0 = new history<TH1F>(f, tag + "_"s + base_label + "_merge_side0");
    auto base_merge_fold1 = new history<TH1F>(f, tag + "_"s + base_label + "_merge_side1");

    /* create histograms */
    std::vector<int64_t> iterations {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 35, 40, 45, 50, 55, 60, 65, 70, 80, 90, 100, 125, 150, 200, 250};

    auto func = [&](int64_t, std::string const& name, std::string const&) {
        return new TH1F(name.data(), ";iterations;", iterations.back(), 1, iterations.back()); };

    auto sum = new history<TH1F>("sum"s, "", func, base_fold0->size());
    auto sum_merge = new history<TH1F>("sum_merge"s, "", func, 1);

    for (size_t i = 1; i < iterations.size(); ++i) {
        auto refold_fold0 = new history<TH1F>(f, tag + "_"s + base_label + "_sum0_refolded_fold0" + std::to_string(iterations[i]));
        auto refold_fold1 = new history<TH1F>(f, tag + "_"s + base_label + "_sum0_refolded_fold1" + std::to_string(iterations[i]));

        auto refold_merge_fold0 = new history<TH1F>(f, tag + "_"s + base_label + "_merge_refolded_fold0" + std::to_string(iterations[i]));
        auto refold_merge_fold1 = new history<TH1F>(f, tag + "_"s + base_label + "_merge_refolded_fold1" + std::to_string(iterations[i]));

        for (int64_t j = 0; j < base_fold0->size(); ++j) {
            if (!((*refold_fold0)[j]->GetBinError(1) < 10)) { continue; }
            if (!((*refold_fold1)[j]->GetBinError(1) < 10)) { continue; }

            double chi2 = 0;
            double unc = 0;

            for (int64_t k = 1; k < (*base_fold0)[j]->GetNbinsX(); ++k) {
                double diff = (*base_fold0)[j]->GetBinContent(k + 1) - (*refold_fold0)[j]->GetBinContent(k + 1);
                diff += (*base_fold1)[j]->GetBinContent(k + 1) - (*refold_fold1)[j]->GetBinContent(k + 1);
                chi2 += diff * diff;
                unc += (*refold_fold0)[j]->GetBinError(k + 1) + (*refold_fold1)[j]->GetBinError(k + 1);;
            }

            if (!(unc < 20)) { continue; }

            (*sum)[j]->SetBinContent(iterations[i], chi2);
            (*sum)[j]->SetBinError(iterations[i], unc);
        }

        if (!((*refold_merge_fold0)[j]->GetBinError(1) < 10)) { continue; }
        if (!((*refold_merge_fold1)[j]->GetBinError(1) < 10)) { continue; }

        double chi2 = 0;
        double unc = 0;

        for (int64_t k = 1; k < (*base_merge_fold0)[j]->GetNbinsX(); ++k) {
            double diff = (*base_merge_fold0)[j]->GetBinContent(k + 1) - (*refold_merge_fold0)[j]->GetBinContent(k + 1);
            diff += (*base_merge_fold1)[j]->GetBinContent(k + 1) - (*refold_merge_fold1)[j]->GetBinContent(k + 1);
            chi2 += diff * diff;
            unc += (*refold_merge_fold0)[j]->GetBinError(k + 1) + (*refold_merge_fold1)[j]->GetBinError(k + 1);;
        }

        if (!(unc < 20)) { continue; }

        (*sum_merge)[j]->SetBinContent(iterations[i], chi2);
        (*sum_merge)[j]->SetBinError(iterations[i], unc);
    }

    in(output, [&]() {
        sum->save("");
        sum_merge->save("");
    });

    std::vector<int> choice(sum->size(), 1);

    for (int i = 0; i < sum->size(); ++i) {
        double min = 99999999999;

        for (int j = 1; j <= (*sum)[i]->GetNbinsX(); ++j) {
            auto top = (*sum)[i]->GetBinContent(j) + (*sum)[i]->GetBinError(j);

            if (top == 0) { continue; }

            std::cout << top << " ";

            if (top < min) {
                min = top;
                choice[i] = j;
            }
            else {
                break;
            }
        }

        std::cout << std::endl << choice[i] << std::endl;
    }

    auto minimum = [&](int64_t index) {
        char buffer[128] = { '\0' };
        sprintf(buffer, "minimum: %d", choice[index-1]);

        TLatex* l = new TLatex();
        l->SetTextAlign(11);
        l->SetTextFont(43);
        l->SetTextSize(13);
        l->DrawLatexNDC(0.135, 0.17, buffer);
    };

    /* set up figures */
    std::string system_tag = "  #sqrt{s_{NN}} = 5.02 TeV"s;
    system_tag += (tag == "aa") ? ", 1.69 nb^{-1}"s : ", 302 pb^{-1}"s;
    auto cms = "#bf{#scale[1.4]{CMS}}"s;
    cms += " #it{#scale[1.2]{Preliminary}}"s;

    std::function<void(int64_t, float)> pt_info = [&](int64_t x, float pos) {
        info_text(x, pos, "%.0f < p_{T}^{#gamma} < %.0f", rpt, false); };

    std::function<void(int64_t, float)> hf_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", dcent, true); };

    auto pthf_info = [&](int64_t index) {
        stack_text(index, 0.25, 0.04, mpthf, pt_info, hf_info); };

    std::function<void(int64_t, float)> range_info = [&](int64_t x, float pos) {
        info_text(x, pos, "Cent. %i - %i%%", drange, true); };

    auto ptrange_info = [&](int64_t index) {
        stack_text(index, 0.25, 0.04, mpthf, pt_info, range_info); };

    auto hb = new pencil();

    auto p1 = new paper(set + "_iteration_original_" + label, hb);

    p1->divide(sum->size(), -1);
    p1->accessory(pthf_info);
    p1->accessory(minimum);
    apply_style(p1, cms, system_tag);
    p1->set(paper::flags::logx);

    sum->apply([&](TH1* h) { p1->add(h); });

    auto p2 = new paper(set + "_iteration_original_" + label + "_merge", hb);

    p2->accessory(ptrange_info);
    apply_style(p2, cms, system_tag);
    p2->set(paper::flags::logx);

    sum_merge->apply([&](TH1* h) { p2->add(h); });

    hb->sketch();
    p1->draw("pdf");
    p2->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return sum_iteration(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}