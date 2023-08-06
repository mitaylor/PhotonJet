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

int gather_theory(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto trees = conf->get<std::vector<std::string>>("trees");

    /* selections */
    auto sel = new configurer(selections);

    auto photon_pt_bounds = sel->get<std::vector<float>>("photon_pt_bounds");
    auto jet_pt_bounds = sel->get<std::vector<float>>("jet_pt_bounds");
    
    auto const dphi_min_numerator = sel->get<float>("dphi_min_numerator");
    auto const dphi_min_denominator = sel->get<float>("dphi_min_denominator");

    auto gdr = sel->get<std::vector<float>>("drg_range");

    /* make histograms */
    auto idr = new interval("#deltaj"s, gdr);
    auto fdr = std::bind(&interval::book<TH1F>, idr, _1, _2, _3);

    std::vector<history<TH1F>*> hists(trees.size(), nullptr);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    TFile* f = new TFile(input.data(), "read");

    for (size_t i = 0; i < trees.size(); ++i) {
        std::cout << trees[i] << std::endl;

        TTree* t = (TTree*)f->Get(trees[i].c_str());
        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        hists[i] = new history<TH1F>("dr_"s + trees[i], "", fdr, 1);

        double photonPt;
        double jetPt;
        double dphi;
        double dj;
        double weight;

        t->SetBranchAddress("photonPt", &photonPt);
        t->SetBranchAddress("jetPt", &jetPt);
        t->SetBranchAddress("dphi", &dphi);
        t->SetBranchAddress("dj", &dj);
        t->SetBranchAddress("weight", &weight);

        for (int64_t j = 0; j < nentries; ++j) {
            t->GetEntry(j);

            if ((float) photonPt < photon_pt_bounds[0]) { continue; }
            if ((float) photonPt > photon_pt_bounds[1]) { continue; }

            if ((float) jetPt < jet_pt_bounds[0]) { continue; }
            if ((float) jetPt > jet_pt_bounds[1]) { continue; }

            if ((float) dphi < dphi_min_numerator/dphi_min_denominator * TMath::Pi()) { continue; }

            (*hists[i])[0]->Fill((float) dj, (float) weight);
        }

        scale_bin_width(hists[i]);
        normalise_to_unity(hists[i]);
    }

    f->Close();

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
