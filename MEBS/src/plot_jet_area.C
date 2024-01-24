#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"
#include "../git/history/include/memory.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"
#include "TStyle.h"

#include <memory>
#include <string>
#include <vector>
#include <ctime>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int combine_populate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto tag = conf->get<std::string>("tag");

    auto dhf = conf->get<std::vector<float>>("hf_diff");

    auto sel = new configurer(selections);

    auto base = sel->get<std::string>("base");

    auto dpt = sel->get<std::vector<float>>("photon_pt_diff");

    auto mpthf = new multival(dpt, dhf);

    /* load input */
    TFile* f = new TFile((base + input).data(), "read");

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    /* combine nevt information */
    auto jet20 = new history<TH1F>(f, "raw_njet20"s);
    auto jet25 = new history<TH1F>(f, "raw_njet25"s);
    auto jet30 = new history<TH1F>(f, "raw_njet30"s);
    auto jet35 = new history<TH1F>(f, "raw_njet35"s);

    auto mix_jet20 = new history<TH1F>(f, "raw_mix_njet20"s);
    auto mix_jet25 = new history<TH1F>(f, "raw_mix_njet25"s);
    auto mix_jet30 = new history<TH1F>(f, "raw_mix_njet30"s);
    auto mix_jet35 = new history<TH1F>(f, "raw_mix_njet35"s);

    auto signal = new TH2F("signal", "Fractional Jet Area: Raw", 4, 0, 4, 4, 0, 4);
    auto mixed = new TH2F("mixed", "Fractional Jet Area: Mixed", 4, 0, 4, 4, 0, 4);

    for (int i = 0; i < 4; ++i) {
        auto index = mpthf->index_for(x{0, i});

        signal->SetBinContent(i + 1, 1, (*jet20)[index]->GetBinContent(1));
        signal->SetBinContent(i + 1, 2, (*jet25)[index]->GetBinContent(1));
        signal->SetBinContent(i + 1, 3, (*jet30)[index]->GetBinContent(1));
        signal->SetBinContent(i + 1, 4, (*jet35)[index]->GetBinContent(1));

        mixed->SetBinContent(i + 1, 1, (*mix_jet20)[index]->GetBinContent(1));
        mixed->SetBinContent(i + 1, 2, (*mix_jet25)[index]->GetBinContent(1));
        mixed->SetBinContent(i + 1, 3, (*mix_jet30)[index]->GetBinContent(1));
        mixed->SetBinContent(i + 1, 4, (*mix_jet35)[index]->GetBinContent(1));

    }

    signal->Write();
    mixed->Write();

    signal->SetMinimum(0);
    signal->SetMaximum(0.15);

    mixed->SetMinimum(0);
    mixed->SetMaximum(0.15);

    signal->GetXaxis()->SetBinLabel(1, "50-90%");
    signal->GetXaxis()->SetBinLabel(2, "30-50%");
    signal->GetXaxis()->SetBinLabel(3, "10-30%");
    signal->GetXaxis()->SetBinLabel(4, "0-10%");

    mixed->GetXaxis()->SetBinLabel(1, "50-90%");
    mixed->GetXaxis()->SetBinLabel(2, "30-50%");
    mixed->GetXaxis()->SetBinLabel(3, "10-30%");
    mixed->GetXaxis()->SetBinLabel(4, "0-10%");

    signal->GetYaxis()->SetBinLabel(1, "> 20 GeV");
    signal->GetYaxis()->SetBinLabel(2, "> 25 GeV");
    signal->GetYaxis()->SetBinLabel(3, "> 30 GeV");
    signal->GetYaxis()->SetBinLabel(4, "> 35 GeV");

    mixed->GetYaxis()->SetBinLabel(1, "> 20 GeV");
    mixed->GetYaxis()->SetBinLabel(2, "> 25 GeV");
    mixed->GetYaxis()->SetBinLabel(3, "> 30 GeV");
    mixed->GetYaxis()->SetBinLabel(4, "> 35 GeV");

    signal->GetZaxis()->SetLabelSize(0.02);
    mixed->GetZaxis()->SetLabelSize(0.02);

    auto hb = new pencil();

    gStyle->SetPaintTextFormat(".3f");

    auto c1 = new paper("jet_area_signal_"s + tag, hb);
    apply_style(c1, "", "");
    c1->add(signal);
    c1->adjust(signal, "text colz", "");

    auto c2 = new paper("jet_area_mixed_"s + tag, hb);
    apply_style(c2, "", "");
    c2->add(mixed);
    c2->adjust(mixed, "text colz", "");

    hb->sketch();

    c1->draw("pdf");
    c2->draw("pdf");

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return combine_populate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}