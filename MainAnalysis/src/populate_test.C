#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

#include <memory>
#include <string>
#include <vector>
#include <ctime>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int populate(char const* config) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto frequency = conf->get<int64_t>("frequency");

    /* exclude most peripheral events */
    auto jetpt = new TH1F("jet pt", "", 500, -500, 500);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    TFile* f = new TFile(input.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto pjt = new pjtree(true, false, t, { 1, 1, 1, 1, 1, 0 });

    printf("iterate..\n");

    int64_t nentries = static_cast<int64_t>(t->GetEntries());
    for (int64_t i = 0; i < nentries; ++i) {
        if (i % frequency == 0) { printf("entry: %li/%li\n", i, nentries); }

        t->GetEntry(i);

        for (int64_t j = 0; j < pjt->nref; ++j) {
            auto jet_pt = (*pjt->jtpt)[j];
            jetpt->Fill(jet_pt);
        }
    }

    jetpt->SaveAs("temp.root");
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 2)
        return populate(argv[1]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}