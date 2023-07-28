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

    auto rptg = sel->get<std::vector<float>>("ptg_range");
    auto rdrg = sel->get<std::vector<float>>("drg_range");

    auto osg = sel->get<std::vector<int64_t>>("osg");

    auto rpt = sel->get<std::vector<float>>("photon_pt_bounds");

    auto mpthf = new multival(rpt, dhf);

    auto idrg = new interval("#deltaj"s, rdrg);
    auto iptg = new interval("p_{T}^{j}"s, rptg);

    auto mg = new multival(*idrg, *iptg);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    TFile* f = new TFile((base + input).data(), "read");
    
    auto preunfold = new history<TH1F>(f, tag + "_"s + base_label + "_sum0"s);

    /* create histograms */
    std::vector<int64_t> iterations {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25};

    auto func = [&](int64_t, std::string const& name, std::string const&) {
        return new TH1F(name.data(), ";iterations;", iterations.back(), 1, iterations.back()); };

    auto sum = new history<TH1F>("sum"s, "", func, preunfold->size());
    auto sum_stat = new history<TH1F>("sum_stat"s, "", func, preunfold->size());
    auto sum_diff = new history<TH1F>("sum_diff"s, "", func, preunfold->size());

    auto sum_merge = new history<TH1F>("sum_merge"s, "", func, 1);
    auto sum_stat_merge = new history<TH1F>("sum_stat_merge"s, "", func, 1);
    auto sum_diff_merge = new history<TH1F>("sum_diff_merge"s, "", func, 1);

    for (size_t i = 1; i < iterations.size(); ++i) {
        auto unfold = new history<TH1F>(f, tag + "_"s + base_label + "_sum0_unfolded" + std::to_string(iterations[i]));
        auto unfold_merge = new history<TH1F>(f, tag + "_"s + base_label + "_merge_unfolded" + std::to_string(iterations[i]));

        auto unfold_prev = new history<TH1F>(f, tag + "_"s + base_label + "_sum0_unfolded" + std::to_string(iterations[i-1]));
        auto unfold_prev_merge = new history<TH1F>(f, tag + "_"s + base_label + "_merge_unfolded" + std::to_string(iterations[i-1]));

        for (int64_t j = 0; j < unfold->size(); ++j) {
            if (!((*unfold)[j]->GetBinError(1) < 1000)) { continue; }

            double s_stat = 0;
            double s_diff = 0;

            for (int64_t k = 1; k <= (*unfold)[j]->GetNbinsX(); ++k) {
                auto indices = mg->indices_for(k);
                auto shape = mg->shape();

                if (indices[0] < osg[0]) { continue; }
                if (indices[1] < osg[2]) {continue; }
                if (indices[0] > (shape[0] - osg[1])) {continue; }
                if (indices[1] > (shape[1] - osg[3])) {continue; }

                auto stat = (*unfold)[j]->GetBinError(k);
                auto diff = (*unfold)[j]->GetBinContent(k) - (*unfold_prev)[j]->GetBinContent(k);

                s_stat += std::abs(stat * stat);
                s_diff += std::abs(diff * diff);
            }

            (*sum)[j]->SetBinContent(iterations[i] + 1, s_stat + s_diff);
            (*sum)[j]->SetBinError(iterations[i] + 1, 0);

            (*sum_stat)[j]->SetBinContent(iterations[i] + 1, s_stat);
            (*sum_stat)[j]->SetBinError(iterations[i] + 1, 0);

            (*sum_diff)[j]->SetBinContent(iterations[i] + 1, s_diff);
            (*sum_diff)[j]->SetBinError(iterations[i] + 1, 0);
        }

        if (!((*unfold_merge)[0]->GetBinError(1) < 1000)) { continue; }

        double s_stat = 0;
        double s_diff = 0;

        for (int64_t k = 1; k <= (*unfold_merge)[0]->GetNbinsX(); ++k) {
            auto indices = mg->indices_for(k);
            auto shape = mg->shape();

            if (indices[0] < osg[0]) { continue; }
            if (indices[1] < osg[2]) {continue; }
            if (indices[0] > (shape[0] - osg[1])) {continue; }
            if (indices[1] > (shape[1] - osg[3])) {continue; }

            auto stat = (*unfold_merge)[0]->GetBinError(k);
            auto diff = (*unfold_merge)[0]->GetBinContent(k) - (*unfold_prev_merge)[0]->GetBinContent(k);

            s_stat += stat * stat;
            s_diff += diff * diff;
        }

        (*sum_merge)[0]->SetBinContent(iterations[i] + 1, s_stat + s_diff);
        (*sum_merge)[0]->SetBinError(iterations[i] + 1, 0);
    
        (*sum_stat_merge)[0]->SetBinContent(iterations[i] + 1, s_stat);
        (*sum_stat_merge)[0]->SetBinError(iterations[i] + 1, 0);

        (*sum_diff_merge)[0]->SetBinContent(iterations[i] + 1, s_diff);
        (*sum_diff_merge)[0]->SetBinError(iterations[i] + 1, 0);
    }

    in(output, [&]() {
        sum->save("");
        sum_merge->save("");
    });

    std::vector<int> choice(sum->size(), 1);

    for (int i = 0; i < sum->size(); ++i) {
        double min = 99999999999;

        for (int j = 0; j < (*sum)[i]->GetNbinsX(); ++j) {
            auto top = (*sum)[i]->GetBinContent(j + 1);

            if (top == 0) { continue; }

            std::cout << top << " ";

            if (top < min) {
                min = top;
                choice[i] = j + 1;
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
        l->DrawLatexNDC(0.135, 0.77, buffer);
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
        stack_text(index, 0.85, 0.04, mpthf, pt_info, hf_info); };

    auto hb = new pencil();

    hb->category("type", "stat", "diff", "total");

    hb->alias("total", "Total");
    hb->alias("stat", "#sigma^{2}_{stat}");
    hb->alias("diff", "#sigma^{2}_{iter}");

    auto p1 = new paper(set + "_sum_" + label, hb);

    p1->divide(sum->size(), -1);
    p1->accessory(pthf_info);
    p1->accessory(minimum);
    apply_style(p1, cms, system_tag);
    // p1->set(paper::flags::logy);

    for (int64_t i = 0; i < preunfold->size(); ++i) {
        p1->add((*sum)[i], "total");
        p1->stack((*sum_stat)[i], "stat");
        p1->stack((*sum_diff)[i], "diff");
    }

    hb->sketch();
    p1->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return sum_iteration(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}