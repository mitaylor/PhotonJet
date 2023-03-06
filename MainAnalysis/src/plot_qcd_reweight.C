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
#include "TF1.h"

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
    auto pthat = conf->get<std::vector<int32_t>>("pthat");
    auto pthatw = conf->get<std::vector<float>>("pthatw");
    auto vzw = conf->get<std::vector<float>>("vzw");
    auto tag = conf->get<std::string>("tag");

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    auto ipthat = new interval("pthat"s, 100, 0, 1000);
    auto ijetpt = new interval("jet pT (GeV)"s, 100, 0, 500);
    auto injets = new interval("#jets"s, 100, 0, 25);

    auto fpthat = std::bind(&interval::book<TH1F>, ipthat, _1, _2, _3);
    auto fjetpt = std::bind(&interval::book<TH1F>, ijetpt, _1, _2, _3);
    auto fnjets = std::bind(&interval::book<TH1F>, injets, _1, _2, _3);

    auto h_pthat = new history<TH1F>("h_pthat"s, "", fpthat, 1, 1);
    auto h_jetpt = new history<TH1F>("h_jetpt"s, "", fjetpt, 1, 1);
    auto h_njets = new history<TH1F>("h_njets"s, "", fnjets, 1, 1);

    TF1* fweight = new TF1("fweight", "(gaus(0))/(gaus(3))");
    fweight->SetParameters(vzw[0], vzw[1], vzw[2], vzw[3], vzw[4], vzw[5]); 

    /* load input */
    for (auto const& file : files) {
        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(true, false, false, t, { 1, 0, 0, 0, 1, 0, 0 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());
        for (int64_t i = 0; i < nentries; ++i) {
            if (i % 100000 == 0) { printf("entry: %li/%li\n", i, nentries); }

            t->GetEntry(i);

            float weight = fweight->Eval(pjt->vz) * weight_for(pthat, pthatw, pjt->pthat) * pjt->weight;

            (*h_pthat)[0]->Fill(pjt->pthat, weight);
            (*h_njets)[0]->Fill(pjt->ngen, weight);

            for (int64_t j = 0; j < pjt->ngen; ++j) {
                if (std::abs((*pjt->geneta)[j]) < 1.6) {
                    (*h_jetpt)[0]->Fill((*pjt->genpt)[j], weight);
                }
            }
        }
    }

    (*h_pthat)[0]->Scale(1. / (*h_pthat)[0]->Integral());
    (*h_njets)[0]->Scale(1. / (*h_njets)[0]->Integral());
    (*h_jetpt)[0]->Scale(1. / (*h_jetpt)[0]->Integral());

    in(output, [&]() {
        h_pthat->save(tag);
        h_njets->save(tag);
        h_jetpt->save(tag);
    });

    printf("destroying objects..\n");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return plot_qcd(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}