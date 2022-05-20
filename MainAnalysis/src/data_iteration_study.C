#include "../include/lambdas.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

#include <string>
#include <vector>
#include <iostream>
#include <cmath>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int data_iteration_study(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto tag = conf->get<std::string>("tag");
    auto input = conf->get<std::string>("input");

    auto base_label = conf->get<std::string>("base_label");
    auto refold_label = conf->get<std::string>("refold_label");

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    TFile* input = new TFile(input.data(), "read");

    auto base = new history<TH1F>(input, tag + "_"s + base_label);

    /* create histograms */
    std::vector<int64_t> iterations {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 35, 40, 45, 50, 55, 60, 65, 70, 80, 90, 100, 125,
        150, 200, 250};

    auto f = [&](int64_t, std::string const& name, std::string const&) {
        return new TH1F(name.data(), ";index;", iterations.back(), 0, iterations.back()); };

    auto chi_square = new memory<TH1F>("chi_square"s, "", f, base->shape());

    for (size_t i = 0; i < iterations.size(); ++i) {
        auto refold = new history<TH1F>(input, tag + "_"s + refold_label + std::to_string(iterations[i]));

        double sum = 0;

        for (int64_t j = 1; j < base->GetNbinsX(); ++j) {
            sum += std::pow(base->GetBinContent(j + 1) - refold->GetBinContent(j + 1), 2);
        }

        chi_square->SetBinContent(iterations[i] + 1, sum);
    }

    in(output, [&]() {
        chi_square->save("test");
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return data_iteration_study(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
