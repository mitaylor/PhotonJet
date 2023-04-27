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

int chi_square_itertaion(char const* config, char const* selections, char const* output) {
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

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    TFile* f = new TFile((base + input).data(), "read");

    auto base0 = new history<TH1F>(f, tag + "_"s + base_label + "_side0");
    auto base1 = new history<TH1F>(f, tag + "_"s + base_label + "_side1");

    /* create histograms */
    std::vector<int64_t> iterations {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 35, 40, 45, 50, 55, 60, 65, 70, 80, 90, 100, 125, 150, 200, 250};

    auto func = [&](int64_t, std::string const& name, std::string const&) {
        return new TH1F(name.data(), ";iterations;", iterations.back(), 0, iterations.back()); };

    auto chi_square = new history<TH1F>("chi_square"s, "", func, base0->size());

    for (size_t i = 0; i < iterations.size(); ++i) {
        auto refold0 = new history<TH1F>(f, tag + "_"s + base_label + "_refold0_iteration" + std::to_string(iterations[i]));
        auto refold1 = new history<TH1F>(f, tag + "_"s + base_label + "_refold1_iteration" + std::to_string(iterations[i]));

        for (int64_t j = 0; j < base0->size(); ++j) {
            if (!((*refold0)[j]->GetBinError(1) < 10)) { continue; }
            if (!((*refold1)[j]->GetBinError(1) < 10)) { continue; }

            double sum = 0;
            double unc = 0;

            for (int64_t k = 1; k < (*base0)[j]->GetNbinsX(); ++k) {
                double diff = (*base0)[j]->GetBinContent(k + 1) - (*refold0)[j]->GetBinContent(k + 1);
                diff += (*base1)[j]->GetBinContent(k + 1) - (*refold1)[j]->GetBinContent(k + 1);
                sum += diff * diff;
                unc += (*refold0)[j]->GetBinError(k + 1) + (*refold1)[j]->GetBinError(k + 1);;
            }

            if (!(unc < 20)) { continue; }

            (*chi_square)[j]->SetBinContent(iterations[i] + 1, sum);
            (*chi_square)[j]->SetBinError(iterations[i] + 1, 0);
        }
    }

    in(output, [&]() {
        chi_square->save("test");
    });

    std::vector<int> choice(chi_square->size(), 1);

    for (int i = 0; i < chi_square->size(); ++i) {
        double min = 99999999999;
        double total = 0;
        int max_iteration = 1;

        for (int j = 0; j < (*chi_square)[i]->GetNbinsX(); ++j) {
            auto top = (*chi_square)[i]->GetBinContent(j + 1) + (*chi_square)[i]->GetBinError(j + 1);

            if (top == 0) { continue; }

            if (top * 0.98 < min) {
                total += top;
                min = top;
                max_iteration = j;
            }
            else {
                break;
            }
        }

        total -= min * max_iteration;
        std::cout << std::endl << max_iteration << " " << total << " ";

        double sum = 0;

        for (int j = 0; j <= max_iteration; ++j) {
            sum += (*chi_square)[i]->GetBinContent(j + 1) + (*chi_square)[i]->GetBinError(j + 1) - min;

            if (j > 0 && sum/total < 0.9) {
                min = j;
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

    auto hb = new pencil();
    auto p1 = new paper(set + "_" + label + "_chi_square", hb);

    p1->divide(chi_square->size(), -1);
    p1->accessory(pthf_info);
    p1->accessory(minimum);
    apply_style(p1, cms, system_tag);
    p1->set(paper::flags::logx);

    chi_square->apply([&](TH1* h) { p1->add(h); });

    hb->sketch();
    p1->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return chi_square_itertaion(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}