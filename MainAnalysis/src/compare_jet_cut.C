#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TH1F.h"
#include "TLatex.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int narrate(char const* config, char const* selections) {
    auto conf = new configurer(config);

    auto input_aa = conf->get<std::string>("input_aa");
    auto input_pp = conf->get<std::string>("input_pp");

    auto label = conf->get<std::string>("label");

    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    /* selections */
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    auto rptg = sel->get<std::vector<float>>("ptg_range");

    auto osg = sel->get<std::vector<int64_t>>("osg");

    /* load history objects */
    auto ihf = new interval(dhf);

    TFile* f_aa = new TFile((base + input_aa).data(), "read");
    TFile* f_pp = new TFile((base + input_pp).data(), "read"); 

    std::vector<int64_t> cut = {1, 2, 3, 4, 5};

    std::vector<history<TH1F>*> aa_unfolded_fold0(cut.size(), nullptr);
    std::vector<history<TH1F>*> aa_unfolded_fold1(cut.size(), nullptr);
    std::vector<history<TH1F>*> pp_0_unfolded_fold0(cut.size(), nullptr);
    std::vector<history<TH1F>*> pp_0_unfolded_fold1(cut.size(), nullptr);
    std::vector<history<TH1F>*> pp_1_unfolded_fold0(cut.size(), nullptr);
    std::vector<history<TH1F>*> pp_1_unfolded_fold1(cut.size(), nullptr);
    std::vector<history<TH1F>*> pp_2_unfolded_fold0(cut.size(), nullptr);
    std::vector<history<TH1F>*> pp_2_unfolded_fold1(cut.size(), nullptr);
    std::vector<history<TH1F>*> pp_3_unfolded_fold0(cut.size(), nullptr);
    std::vector<history<TH1F>*> pp_3_unfolded_fold1(cut.size(), nullptr);
    
    for (int64_t i = 0; i < (int64_t) cut.size(); ++i) { 
        aa_unfolded_fold0[i] = new history<TH1F>(f_aa, "aa_"s + label + "_" + std::to_string(i) + "_fold0"s);
        aa_unfolded_fold1[i] = new history<TH1F>(f_aa, "aa_"s + label + "_" + std::to_string(i) + "_fold1"s);

        pp_0_unfolded_fold0[i] = new history<TH1F>(f_pp, "pp_"s + label + "_" + std::to_string(i) + "_fold0"s);
        pp_0_unfolded_fold0[i]->rename(label + "_" + std::to_string(i) + "_" + std::to_string(0) + "_fold0"s);
        pp_0_unfolded_fold1[i] = new history<TH1F>(f_pp, "pp_"s + label + "_" + std::to_string(i) + "_fold1"s);
        pp_0_unfolded_fold1[i]->rename(label + "_" + std::to_string(i) + "_" + std::to_string(0) + "_fold1"s);

        pp_1_unfolded_fold0[i] = new history<TH1F>(f_pp, "pp_"s + label + "_" + std::to_string(i) + "_fold0"s);
        pp_1_unfolded_fold0[i]->rename(label + "_" + std::to_string(i) + "_" + std::to_string(1) + "_fold0"s);
        pp_1_unfolded_fold1[i] = new history<TH1F>(f_pp, "pp_"s + label + "_" + std::to_string(i) + "_fold1"s);
        pp_1_unfolded_fold1[i]->rename(label + "_" + std::to_string(i) + "_" + std::to_string(1) + "_fold1"s);

        pp_2_unfolded_fold0[i] = new history<TH1F>(f_pp, "pp_"s + label + "_" + std::to_string(i) + "_fold0"s);
        pp_2_unfolded_fold0[i]->rename(label + "_" + std::to_string(i) + "_" + std::to_string(2) + "_fold0"s);
        pp_2_unfolded_fold1[i] = new history<TH1F>(f_pp, "pp_"s + label + "_" + std::to_string(i) + "_fold1"s);
        pp_2_unfolded_fold1[i]->rename(label + "_" + std::to_string(i) + "_" + std::to_string(2) + "_fold1"s);

        pp_3_unfolded_fold0[i] = new history<TH1F>(f_pp, "pp_"s + label + "_" + std::to_string(i) + "_fold0"s);
        pp_3_unfolded_fold0[i]->rename(label + "_" + std::to_string(i) + "_" + std::to_string(3) + "_fold0"s);
        pp_3_unfolded_fold1[i] = new history<TH1F>(f_pp, "pp_"s + label + "_" + std::to_string(i) + "_fold1"s);
        pp_3_unfolded_fold1[i]->rename(label + "_" + std::to_string(i) + "_" + std::to_string(3) + "_fold1"s);
    }

    auto hb = new pencil();
    hb->category("system", "pp", "aa");

    hb->alias("aa", "PbPb");

    auto p1 = new paper(set + "_compare_jet_cut_dr", hb);
    apply_style(p1, "#bf{#scale[1.4]{CMS}}     #sqrt{s_{NN}} = 5.02 TeV"s, "PbPb 1.69 nb^{-1}, pp 302 pb^{-1}"s, -2, 27);
    p1->divide(ihf->size(), -1);

    auto p2 = new paper(set + "_compare_jet_cut_jpt", hb);
    apply_style(p2, "#bf{#scale[1.4]{CMS}}     #sqrt{s_{NN}} = 5.02 TeV"s, "PbPb 1.69 nb^{-1}, pp 302 pb^{-1}"s, 0, 0.03);
    p2->divide(ihf->size(), -1);

    for (int64_t i = 0; i < (int64_t) cut.size(); ++i) { 
        aa_unfolded_fold0[i]->apply([&](TH1* h) { p1->add(h, "aa"); });
        aa_unfolded_fold1[i]->apply([&](TH1* h) { p2->add(h, "aa"); });

        // pp_0_unfolded_fold0[i]->apply([&](TH1* h) { p1->stack(h, "pp"); });
        // pp_0_unfolded_fold1[i]->apply([&](TH1* h) { p2->stack(h, "pp"); });

        pp_0_unfolded_fold0[i]->apply([&](TH1* h, int64_t index) { p1->stack(i*4 + index + 1, h, "pp"); });
        pp_0_unfolded_fold1[i]->apply([&](TH1* h, int64_t index) { p2->stack(i*4 + index + 1, h, "pp"); });

        pp_1_unfolded_fold0[i]->apply([&](TH1* h, int64_t index) { p1->stack(i*4 + index + 2, h, "pp"); });
        pp_1_unfolded_fold1[i]->apply([&](TH1* h, int64_t index) { p2->stack(i*4 + index + 2, h, "pp"); });

        pp_2_unfolded_fold0[i]->apply([&](TH1* h, int64_t index) { p1->stack(i*4 + index + 3, h, "pp"); });
        pp_2_unfolded_fold1[i]->apply([&](TH1* h, int64_t index) { p2->stack(i*4 + index + 3, h, "pp"); });

        pp_3_unfolded_fold0[i]->apply([&](TH1* h, int64_t index) { p1->stack(i*4 + index + 4, h, "pp"); });
        pp_3_unfolded_fold1[i]->apply([&](TH1* h, int64_t index) { p2->stack(i*4 + index + 4, h, "pp"); });
    }
    
    hb->sketch();

    p1->draw("pdf");
    p2->draw("pdf");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return narrate(argv[1], argv[2]);

    printf("usage: %s [config] [selections]\n", argv[0]);
    return 1;
}
