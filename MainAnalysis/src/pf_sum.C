#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
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

int pf_sum(char const* input, char const* output) {
    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    TFile* fin = new TFile(input, "read");
    TTree* tin = (TTree*)fin->Get("pj");
    auto pjt = new pjtree(gen_iso, false, heavyion, tin, { 0, 0, 0, 0, 0, 0, 0, 1, 0 });
    int64_t nentries = static_cast<int64_t>(tin->GetEntries());

    /* create output tree */
    TTree* o("MyTree","Example Tree");

    /* fill output tree */
    for (int64_t i = 0; i < nentries; ++i) {
        if (i % (nentries/200) == 0) { printf("entry: %li/%li\n", i, nentries); }

        tin->GetEntry(i);

        float pfsum = 0;
        for (size_t j = 0; j < pjt->pfEta->size(); ++j) {
            if (std::abs((*pjt->pfEta)[j]) > 3 && std::abs((*pjt->pfEta)[j]) < 5) {
                pfsum += (*pjt->pfE)[j];
            }
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return pf_sum(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
