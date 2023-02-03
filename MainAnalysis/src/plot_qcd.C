#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"

#include <memory>
#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

float weight_for(std::vector<int32_t> const& divisions,
                 std::vector<float> const& weights, float value) {
    int64_t index = -1;
    for (auto edge : divisions)
        if (value > edge)
            ++index;

    return weights[index];
}

int populate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto files = conf->get<std::vector<std::string>>("files");

    auto pthat_w = new TH1F("pthat_w", "pthat", 100, 0, 200);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    std::vector<float> original_weights = { 9.972652E-01, 2.132830E-01, 6.679997E-02, 1.784052E-02, 8.292187E-03, 1.131320E-03 };
    std::vector<float> new_weights = { 9.970141E-01, 2.132282E-01, 6.680332E-02, 1.783156E-02, 8.286595E-03, 1.130233E-03 };
    std::vector<int32_t> pthat = { 0, 30, 50, 80, 120, 170 };

    /* load input */
    for (auto const& file : files) {
        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(true, false, false, t, { 1, 0, 0, 0, 0, 0, 0 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());
        for (int64_t i = 0; i < nentries; ++i) {
            if (i % 100000 == 0) { printf("entry: %li/%li\n", i, nentries); }

            t->GetEntry(i);

            pthat_w->Fill(pjt->pthat, pjt->w * weight_for(pthat, new_weights, pjt->pthat) / weight_for(pthat, original_weights, pjt->pthat));
        }
    }

    pthat_w->SaveAs(output);

    printf("destroying objects..\n");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return populate(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}