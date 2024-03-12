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
#include "TMatrixT.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename T>
T* null(int64_t, std::string const&, std::string const&) {
    return nullptr;
}

int regularization(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto tag = conf->get<std::string>("tag");
    auto label = conf->get<std::string>("label");
    auto prior = conf->get<std::string>("prior");
    auto algorithm = conf->get<std::string>("algorithm");

    auto filenames = conf->get<std::vector<std::string>>("filenames");
    
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    std::vector<TFile*> files(filenames.size(), nullptr);

    zip([&](auto& file, auto const& filename) {
        file = new TFile(("unfolded/Theory/"s + set + "/"s + algorithm + "/"s + prior + "/kErrors/" + filename).data(), "read");
    }, files, filenames);

    /* prepare output from pre-unfolded data */
    TFile* fout = new TFile(output, "recreate");

    /* prepare the mse */
    auto mse = new history<TH1F>("mse", "", null<TH1F>, (int64_t) afters.size());

    /* extract chosen histograms */
    for (size_t j = 0; j < files.size(); ++j) {
        auto HMSE = (TH1F*) files[j]->Get("HMSE");
        (*mse)[j] = HUnfoldedBayes;
    }

    /* rename histograms */
    mse->rename(tag + "_mse"s);
    
    /* save histograms */
    mse->save();

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return regularization(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
