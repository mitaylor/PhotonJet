#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

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
#include "TRandom3.h"

#include <memory>
#include <string>
#include <vector>
#include <ctime>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int combine_populate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto group = conf->get<std::string>("group");
    auto labels = conf->get<std::vector<std::string>>("labels");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    /* load input */
    TFile* f = new TFile((base + input).data(), "read");

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    /* combine nevt information */
    auto jet20 = new history<TH1F>(f, "raw_jet20"s);
    auto jet25 = new history<TH1F>(f, "raw_jet25"s);
    auto jet30 = new history<TH1F>(f, "raw_jet30"s);
    auto jet35 = new history<TH1F>(f, "raw_jet35"s);
    auto jet40 = new history<TH1F>(f, "raw_jet40"s);
    auto jet50 = new history<TH1F>(f, "raw_jet50"s);

    auto mix_jet20 = new history<TH1F>(f, "raw_mix_jet20"s);
    auto mix_jet25 = new history<TH1F>(f, "raw_mix_jet25"s);
    auto mix_jet30 = new history<TH1F>(f, "raw_mix_jet30"s);
    auto mix_jet35 = new history<TH1F>(f, "raw_mix_jet35"s);
    auto mix_jet40 = new history<TH1F>(f, "raw_mix_jet40"s);
    auto mix_jet50 = new history<TH1F>(f, "raw_mix_jet50"s);

    auto signal = new TH2F("signal", "Fractional Jet Area: Raw", 4, 0, 4, 6, 0, 6);
    auto mixed = new TH2F("signal", "Fractional Jet Area: Mixed", 4, 0, 4, 6, 0, 6);

    for (int i = 0; i < 4; ++i) {
        signal->SetBinContent(i + 1, 1, (*jet20)[i]->GetBinContent(1));
        signal->SetBinContent(i + 1, 2, (*jet25)[i]->GetBinContent(1));
        signal->SetBinContent(i + 1, 3, (*jet30)[i]->GetBinContent(1));
        signal->SetBinContent(i + 1, 4, (*jet35)[i]->GetBinContent(1));
        signal->SetBinContent(i + 1, 5, (*jet40)[i]->GetBinContent(1));
        signal->SetBinContent(i + 1, 6, (*jet50)[i]->GetBinContent(1));

        mixed->SetBinContent(i + 1, 1, (*mix_jet20)[i]->GetBinContent(1));
        mixed->SetBinContent(i + 1, 2, (*mix_jet25)[i]->GetBinContent(1));
        mixed->SetBinContent(i + 1, 3, (*mix_jet30)[i]->GetBinContent(1));
        mixed->SetBinContent(i + 1, 4, (*mix_jet35)[i]->GetBinContent(1));
        mixed->SetBinContent(i + 1, 5, (*mix_jet40)[i]->GetBinContent(1));
        mixed->SetBinContent(i + 1, 6, (*mix_jet50)[i]->GetBinContent(1));
    }

    signal->Write();
    mixed->Write();

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return combine_populate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}