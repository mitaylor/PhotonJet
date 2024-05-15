#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

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

int plot_qcd(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto files = conf->get<std::vector<std::string>>("files");

    auto pthat_w = new TH1F("pthat_w", "pthat", 100, 0, 200);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    for (auto const& file : files) {
        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(true, false, false, t, { 1, 0, 0, 0, 0, 0, 0 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());
        for (int64_t i = 0; i < nentries; ++i) {
            if (i % 100000 == 0) { printf("entry: %li/%li\n", i, nentries); }

            t->GetEntry(i);

            pthat_w->Fill(pjt->pthat, pjt->w);
        }
    }

    pthat_w->Scale(1. / pthat_w->Integral());

    pthat_w->SaveAs(output);

    auto system_tag = "  #sqrt{s_{NN}} = 5.02 TeV"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

    auto hb = new pencil();

    auto p8 = new paper("pthat_spectrum", hb);
    p8->set(paper::flags::logy);
    p8->set(paper::flags::logx);
    apply_style(p8, cms, system_tag);
    p8->add(pthat_w);

    printf("destroying objects..\n");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return plot_qcd(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}