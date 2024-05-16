#include "../include/lambdas.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"
#include "TMath.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int gather_theory(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto inputs_selfnorm = conf->get<std::vector<std::string>>("inputs_selfnorm");
    auto inputs_selfnorm_part1 = conf->get<std::vector<std::string>>("inputs_selfnorm_part1");
    auto inputs_selfnorm_part2 = conf->get<std::vector<std::string>>("inputs_selfnorm_part2");
    auto inputs_totnorm = conf->get<std::vector<std::string>>("inputs_totnorm");
    auto inputs_totnorm_part1 = conf->get<std::vector<std::string>>("inputs_totnorm_part1");
    auto inputs_totnorm_part2 = conf->get<std::vector<std::string>>("inputs_totnorm_part2");

    auto labels = conf->get<std::vector<std::string>>("labels");

    std::vector<float> gdr = { 0.00, 0.01, 0.016, 0.024, 0.032, 0.04, 0.05, 0.06, 0.08, 0.1, 0.15, 0.3 };

    /* make histograms */
    auto idr = new interval("#deltaj"s, gdr);
    auto fdr = std::bind(&interval::book<TH1F>, idr, _1, _2, _3);

    int size = labels.size();

    std::vector<history<TH1F>*> hists_pp_selfnorm(size, nullptr);
    std::vector<history<TH1F>*> hists_pp_selfnorm_part1(size, nullptr);
    std::vector<history<TH1F>*> hists_pp_selfnorm_part2(size, nullptr);
    std::vector<history<TH1F>*> hists_pp_totnorm(size, nullptr);
    std::vector<history<TH1F>*> hists_pp_totnorm_part1(size, nullptr);
    std::vector<history<TH1F>*> hists_pp_totnorm_part2(size, nullptr);

    std::vector<history<TH1F>*> hists_aa_selfnorm(size, nullptr);
    std::vector<history<TH1F>*> hists_aa_selfnorm_part1(size, nullptr);
    std::vector<history<TH1F>*> hists_aa_selfnorm_part2(size, nullptr);
    std::vector<history<TH1F>*> hists_aa_totnorm(size, nullptr);
    std::vector<history<TH1F>*> hists_aa_totnorm_part1(size, nullptr);
    std::vector<history<TH1F>*> hists_aa_totnorm_part2(size, nullptr);

    std::vector<history<TH1F>*> hists_ratio_selfnorm(size, nullptr);
    std::vector<history<TH1F>*> hists_ratio_selfnorm_part1(size, nullptr);
    std::vector<history<TH1F>*> hists_ratio_selfnorm_part2(size, nullptr);
    std::vector<history<TH1F>*> hists_ratio_totnorm(size, nullptr);
    std::vector<history<TH1F>*> hists_ratio_totnorm_part1(size, nullptr);
    std::vector<history<TH1F>*> hists_ratio_totnorm_part2(size, nullptr);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    for (int i = 0; i < size; ++i) {
        std::ifstream file_selfnorm(inputs_selfnorm[i].data());
        std::ifstream file_selfnorm_part1(inputs_selfnorm_part1[i].data());
        std::ifstream file_selfnorm_part2(inputs_selfnorm_part2[i].data());
        std::ifstream file_totnorm(inputs_totnorm[i].data());
        std::ifstream file_totnorm_part1(inputs_totnorm_part1[i].data());
        std::ifstream file_totnorm_part2(inputs_totnorm_part2[i].data());

        hists_pp_selfnorm[i] = new history<TH1F>("pp_selfnorm_"s + labels[i] + "_dr", "", fdr, 1);
        hists_pp_selfnorm_part1[i] = new history<TH1F>("pp_selfnorm_"s + labels[i] + "_dr_part1", "", fdr, 1);
        hists_pp_selfnorm_part2[i] = new history<TH1F>("pp_selfnorm_"s + labels[i] + "_dr_part2", "", fdr, 1);
        hists_pp_totnorm[i] = new history<TH1F>("pp_totnorm_"s + labels[i] + "_dr", "", fdr, 1);
        hists_pp_totnorm_part1[i] = new history<TH1F>("pp_totnorm_"s + labels[i] + "_dr_part1", "", fdr, 1);
        hists_pp_totnorm_part2[i] = new history<TH1F>("pp_totnorm_"s + labels[i] + "_dr_part2", "", fdr, 1);

        hists_aa_selfnorm[i] = new history<TH1F>("aa_selfnorm_"s + labels[i] + "_dr", "", fdr, 1);
        hists_aa_selfnorm_part1[i] = new history<TH1F>("aa_selfnorm_"s + labels[i] + "_dr_part1", "", fdr, 1);
        hists_aa_selfnorm_part2[i] = new history<TH1F>("aa_selfnorm_"s + labels[i] + "_dr_part2", "", fdr, 1);
        hists_aa_totnorm[i] = new history<TH1F>("aa_totnorm_"s + labels[i] + "_dr", "", fdr, 1);
        hists_aa_totnorm_part1[i] = new history<TH1F>("aa_totnorm_"s + labels[i] + "_dr_part1", "", fdr, 1);
        hists_aa_totnorm_part2[i] = new history<TH1F>("aa_totnorm_"s + labels[i] + "_dr_part2", "", fdr, 1);

        hists_ratio_selfnorm[i] = new history<TH1F>("ratio_selfnorm_"s + labels[i] + "_dr", "", fdr, 1);
        hists_ratio_selfnorm_part1[i] = new history<TH1F>("ratio_selfnorm_"s + labels[i] + "_dr_part1", "", fdr, 1);
        hists_ratio_selfnorm_part2[i] = new history<TH1F>("ratio_selfnorm_"s + labels[i] + "_dr_part2", "", fdr, 1);
        hists_ratio_totnorm[i] = new history<TH1F>("ratio_totnorm_"s + labels[i] + "_dr", "", fdr, 1);
        hists_ratio_totnorm_part1[i] = new history<TH1F>("ratio_totnorm_"s + labels[i] + "_dr_part1", "", fdr, 1);
        hists_ratio_totnorm_part2[i] = new history<TH1F>("ratio_totnorm_"s + labels[i] + "_dr_part2", "", fdr, 1);

        float bin_center, pp_center, pp_error, aa_down, aa_up, ratio_down, ratio_up;

        for (size_t j = 1; j < gdr.size(); ++j) {  
            file_selfnorm >> bin_center >> pp_center >> pp_error >> aa_up >> aa_down >> ratio_up >> ratio_down;

            (*hists_pp_selfnorm[i])[0]->SetBinContent(j, pp_center);
            (*hists_pp_selfnorm[i])[0]->SetBinError(j, pp_error);

            (*hists_aa_selfnorm[i])[0]->SetBinContent(j, (aa_up + aa_down)/2);
            (*hists_aa_selfnorm[i])[0]->SetBinError(j, (aa_up - aa_down)/2);

            (*hists_ratio_selfnorm[i])[0]->SetBinContent(j, (ratio_up + ratio_down)/2);
            (*hists_ratio_selfnorm[i])[0]->SetBinError(j, (ratio_up - ratio_down)/2);

            file_selfnorm_part1 >> bin_center >> pp_center >> pp_error >> aa_up >> aa_down >> ratio_up >> ratio_down;

            (*hists_pp_selfnorm_part1[i])[0]->SetBinContent(j, pp_center);
            (*hists_pp_selfnorm_part1[i])[0]->SetBinError(j, pp_error);

            (*hists_aa_selfnorm_part1[i])[0]->SetBinContent(j, (aa_up + aa_down)/2);
            (*hists_aa_selfnorm_part1[i])[0]->SetBinError(j, (aa_up - aa_down)/2);

            (*hists_ratio_selfnorm_part1[i])[0]->SetBinContent(j, (ratio_up + ratio_down)/2);
            (*hists_ratio_selfnorm_part1[i])[0]->SetBinError(j, (ratio_up - ratio_down)/2);

            file_selfnorm_part2 >> bin_center >> pp_center >> pp_error >> aa_up >> aa_down >> ratio_up >> ratio_down;

            (*hists_pp_selfnorm_part2[i])[0]->SetBinContent(j, pp_center);
            (*hists_pp_selfnorm_part2[i])[0]->SetBinError(j, pp_error);

            (*hists_aa_selfnorm_part2[i])[0]->SetBinContent(j, (aa_up + aa_down)/2);
            (*hists_aa_selfnorm_part2[i])[0]->SetBinError(j, (aa_up - aa_down)/2);

            (*hists_ratio_selfnorm_part2[i])[0]->SetBinContent(j, (ratio_up + ratio_down)/2);
            (*hists_ratio_selfnorm_part2[i])[0]->SetBinError(j, (ratio_up - ratio_down)/2);

            file_totnorm >> bin_center >> pp_center >> pp_error >> aa_up >> aa_down >> ratio_up >> ratio_down;

            (*hists_pp_totnorm[i])[0]->SetBinContent(j, pp_center);
            (*hists_pp_totnorm[i])[0]->SetBinError(j, pp_error);

            (*hists_aa_totnorm[i])[0]->SetBinContent(j, (aa_up + aa_down)/2);
            (*hists_aa_totnorm[i])[0]->SetBinError(j, (aa_up - aa_down)/2);

            (*hists_ratio_totnorm[i])[0]->SetBinContent(j, (ratio_up + ratio_down)/2);
            (*hists_ratio_totnorm[i])[0]->SetBinError(j, (ratio_up - ratio_down)/2);

            file_totnorm_part1 >> bin_center >> pp_center >> pp_error >> aa_up >> aa_down >> ratio_up >> ratio_down;

            (*hists_pp_totnorm_part1[i])[0]->SetBinContent(j, pp_center);
            (*hists_pp_totnorm_part1[i])[0]->SetBinError(j, pp_error);

            (*hists_aa_totnorm_part1[i])[0]->SetBinContent(j, (aa_up + aa_down)/2);
            (*hists_aa_totnorm_part1[i])[0]->SetBinError(j, (aa_up - aa_down)/2);

            (*hists_ratio_totnorm_part1[i])[0]->SetBinContent(j, (ratio_up + ratio_down)/2);
            (*hists_ratio_totnorm_part1[i])[0]->SetBinError(j, (ratio_up - ratio_down)/2);

            file_totnorm_part2 >> bin_center >> pp_center >> pp_error >> aa_up >> aa_down >> ratio_up >> ratio_down;

            (*hists_pp_totnorm_part2[i])[0]->SetBinContent(j, pp_center);
            (*hists_pp_totnorm_part2[i])[0]->SetBinError(j, pp_error);

            (*hists_aa_totnorm_part2[i])[0]->SetBinContent(j, (aa_up + aa_down)/2);
            (*hists_aa_totnorm_part2[i])[0]->SetBinError(j, (aa_up - aa_down)/2);

            (*hists_ratio_totnorm_part2[i])[0]->SetBinContent(j, (ratio_up + ratio_down)/2);
            (*hists_ratio_totnorm_part2[i])[0]->SetBinError(j, (ratio_up - ratio_down)/2);
        }

        file_selfnorm.close();
        file_selfnorm_part1.close();
        file_selfnorm_part2.close();
        file_totnorm.close();
        file_totnorm_part1.close();
        file_totnorm_part2.close();
   }

    /* save histograms */
    in(output, [&]() {
        for (int i = 0; i < size; ++i) {
            hists_pp_selfnorm[i]->save();
            hists_pp_selfnorm_part1[i]->save();
            hists_pp_selfnorm_part2[i]->save();
            hists_pp_totnorm[i]->save();
            hists_pp_totnorm_part1[i]->save();
            hists_pp_totnorm_part2[i]->save();

            hists_aa_selfnorm[i]->save();
            hists_aa_selfnorm_part1[i]->save();
            hists_aa_selfnorm_part2[i]->save();
            hists_aa_totnorm[i]->save();
            hists_aa_totnorm_part1[i]->save();
            hists_aa_totnorm_part2[i]->save();

            hists_ratio_selfnorm[i]->save();
            hists_ratio_selfnorm_part1[i]->save();
            hists_ratio_selfnorm_part2[i]->save();
            hists_ratio_totnorm[i]->save();
            hists_ratio_totnorm_part1[i]->save();
            hists_ratio_totnorm_part2[i]->save();
        }
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return gather_theory(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
