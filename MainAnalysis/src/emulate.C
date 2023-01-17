#include "../include/lambdas.h"
#include "../include/pjtree.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"

#include "TChain.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TLatex.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int emulate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto data = conf->get<std::vector<std::string>>("data");
    auto mc = conf->get<std::vector<std::string>>("mc");
    auto pthat15 = conf->get<std::vector<std::string>>("pthat15");

    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");
    
    auto rvz = conf->get<std::vector<float>>("vz_range");

    TH1::SetDefaultSumw2();

    /* merged gen inputs */
    TChain* tcomb = new TChain("pj");
    for (auto const& f : mc)   tcomb->Add(f.data());

    /* merged base inputs */
    TChain* tbase = new TChain("pj");
    for (auto const& f : pthat15)   tbase->Add(f.data());

    /* get entries in each pthat bin */
    float pthats[7] = { 15, 30, 50, 80, 120, 170, 999999 };
    int count = 6; // number of pthats

    TH1F* hcomb = new TH1F("npthats", "", count, pthats);
    TH1F* hbase = new TH1F("npthats_model", "", count, pthats);

    for (auto const& file : pthat15) {
        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(true, false, false, t, { 1, 0, 0, 0, 0, 0, 0, 0 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        for (int64_t i = 0; i < nentries; ++i) {
            t->GetEntry(i);
            hbase->Fill(pjt->pthat, pjt->weight);
        }
    }

    /* iterate through combined chain */
    for (auto const& file : mc) {
        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(true, false, false, t, { 1, 0, 0, 0, 0, 0, 0, 0 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        for (int64_t i = 0; i < nentries; ++i) {
            t->GetEntry(i);
            hcomb->Fill(pjt->pthat, pjt->weight);
        }
    }

    /* loop over and assign pthat weights */
    auto incl = new interval(""s, 1L, 0., 1.);
    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto pthatw = new history<TH1F>("pthat"s, "", fincl, count);

    for(int i = 0; i < count; ++i) {
        auto weight = hbase->GetBinContent(i+1) / hcomb->GetBinContent(i+1);
        (*pthatw)[i]->SetBinContent(1, weight);

        printf("[%f, %f]: %E\n", pthats[i], pthats[i + 1], weight);
    }

    printf("\n");

    /* calculate vz weights */
    auto ivz = new interval("v_{z} (cm)"s, rvz[0], rvz[1], rvz[2]);
    auto fvz = std::bind(&interval::book<TH1F>, ivz, _1, _2, _3);

    auto vz = new history<TH1F>("vz"s, "", fvz, 3L);

    TChain* tdata = new TChain("pj");
    for (auto const& f : data)   tdata->Add(f.data());

    tdata->Project((*vz)[0]->GetName(), "vz");
    tcomb->Project((*vz)[1]->GetName(), "vz");

    (*vz)[0]->Scale(1. / (*vz)[0]->Integral());
    (*vz)[1]->Scale(1. / (*vz)[1]->Integral());

    (*vz)[0]->Fit("gaus", "LMQ", "", -15, 15);
    (*vz)[1]->Fit("gaus", "LMQ", "", -15, 15);

    auto fdata = (*vz)[0]->GetFunction("gaus");
    printf("data: (%f, %f, %f)\n", fdata->GetParameter(0),
        fdata->GetParameter(1), fdata->GetParameter(2));

    auto fcomb = (*vz)[1]->GetFunction("gaus");
    printf("gen: (%f, %f, %f)\n", fcomb->GetParameter(0),
        fcomb->GetParameter(1), fcomb->GetParameter(2));

    (*vz)[2]->Divide((*vz)[0], (*vz)[1]);

    TF1* fweight = new TF1("fweight", "(gaus(0))/(gaus(3))", -15, 15);
    fweight->SetParameters(
        fdata->GetParameter(0), fdata->GetParameter(1), fdata->GetParameter(2),
        fcomb->GetParameter(0), fcomb->GetParameter(1), fcomb->GetParameter(2)
    );

    /* draw vertex distributions */
    auto hb = new pencil();
    hb->category("system", "data", "mc");

    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;

    auto ratio_style = [&](TH1* h, int64_t index) {
        if (index == 3) {
            fweight->Draw("same");
            h->SetAxisRange(0.4, 1.6, "Y");
        }
    };

    auto c1 = new paper(tag + "_vz", hb);
    apply_style(c1, cms, system_tag, 0., 0.04);
    c1->jewellery(ratio_style);
    c1->divide(3, -1);

    c1->add((*vz)[0], "data");
    c1->add((*vz)[1], "mc");
    c1->add((*vz)[2]);

    hb->sketch();
    c1->draw("pdf");

    /* save output */
    in(output, [&]() {
        pthatw->save(tag);
        vz->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return emulate(argv[1], argv[2]);

    return 0;
}
