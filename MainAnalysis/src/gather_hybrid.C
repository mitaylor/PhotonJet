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

template <typename... T>
void scale_bin_width(T*... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1., "width"); }), 0)... };
}

template <typename... T>
void normalise_to_unity(T*&... args) {
    (void)(int [sizeof...(T)]) { (args->apply([](TH1* obj) {
        obj->Scale(1. / obj->Integral("width")); }), 0)... };
}

int gather_theory(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto inputs = conf->get<std::vector<std::string>>("inputs");
    auto labels = conf->get<std::vector<std::string>>("labels");

    std::vector<float> gdr = { 0.00, 0.02, 0.04, 0.06, 0.08, 0.10, 0.125, 0.20, 0.30 };

    /* make histograms */
    auto idr = new interval("#deltaj"s, gdr);
    auto fdr = std::bind(&interval::book<TH1F>, idr, _1, _2, _3);

    std::vector<history<TH1F>*> pp_hists(inputs.size(), nullptr);
    std::vector<history<TH1F>*> aa_hists(inputs.size(), nullptr);
    std::vector<history<TH1F>*> ratio_hists(inputs.size(), nullptr);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    for (size_t i = 0; i < inputs.size(); ++i) {
        std::cout << inputs[i] << std::endl;
        ifstream f(inputs[i].data());

        pp_hists[i] = new history<TH1F>("pp_dr_"s + labels[i], "", fdr, 1);
        aa_hists[i] = new history<TH1F>("aa_dr_"s + labels[i], "", fdr, 1);
        ratio_hists[i] = new history<TH1F>("ratio_dr_"s + labels[i], "", fdr, 1);

        float bin_center;
        float pp_center, pp_error;
        float aa_down, aa_up;
        float ratio_down, ratio_up;

        for (int64_t j = 1; j <= (*aa_hists[i])[0]->GetNbinsX(); ++j) {  
            f >> bin_center >> pp_center >> pp_error >> aa_up >> aa_down >> ratio_up >> ratio_down;

            auto pp_hist = (*pp_hists[i])[0];
            auto aa_hist = (*aa_hists[i])[0];
            auto ratio_hist = (*ratio_hists[i])[0];

            pp_hist->SetBinContent(j, pp_center);
            pp_hist->SetBinError(j, pp_error);

            aa_hist->SetBinContent(j, (aa_up + aa_down)/2);
            aa_hist->SetBinError(j, (aa_up - aa_down)/2);

            ratio_hist->SetBinContent(j, (ratio_up + ratio_down)/2);
            ratio_hist->SetBinError(j, (ratio_up - ratio_down)/2);
        }

        f->Close();
   }

    /* save histograms */
    in(output, [&]() {
        for (size_t i = 0; i < trees.size(); ++i) {
            hists[i]->save();
        }
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return gather_theory(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
