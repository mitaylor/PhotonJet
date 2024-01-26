#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
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

int mebs_inputs(char const* input, char const* output) {
    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    TFile* fin = new TFile(input, "read");
    TTree* tin = (TTree*)fin->Get("pj");
    auto pjt = new pjtree(false, false, true, tin, { 1, 1, 1, 1, 1, 0, 1, 0, 0} );
    int64_t nentries = static_cast<int64_t>(tin->GetEntries());

    /* create output tree */
    TFile* fout = new TFile(output, "recreate");
    TTree* tout = new TTree("pj", "photon-jet");

    float vz;
    float pfSum;
    float hiHF;
    float nref;
    vector<float> jtptCor;
    vector<float> jtpt;
    vector<float> jteta;
    vector<float> jtphi;
    vector<float> WTAeta;
    vector<float> WTAphi;

    tout->Branch("vz", &vz);
    tout->Branch("pfSum", &pfSum);
    tout->Branch("hiHF", &hiHF);
    tout->Branch("nref", &nref);
    tout->Branch("jtptCor", &jtptCor);
    tout->Branch("jtpt", &jtpt);
    tout->Branch("jteta", &jteta);
    tout->Branch("jtphi", &jtphi);
    tout->Branch("WTAeta", &WTAeta);
    tout->Branch("WTAphi", &WTAphi);

    /* fill output tree */
    for (int64_t i = 0; i < nentries; ++i) {
        if (i % (nentries/200) == 0) { printf("entry: %li/%li\n", i, nentries); }

        tin->GetEntry(i);

        jtptCor.clear();
        jtpt.clear();
        jteta.clear();
        jtphi.clear();
        WTAeta.clear();
        WTAphi.clear();

        pfSum = 0;
        for (size_t j = 0; j < pjt->pfEta->size(); ++j) {
            if (std::abs((*pjt->pfEta)[j]) > 3 && std::abs((*pjt->pfEta)[j]) < 5) {
                pfSum += (*pjt->pfE)[j];
            }
        }

        vz = pjt->vz;
        hiHF = pjt->hiHF;
        nref = pjt->nref;

        for (size_t j = 0; j < nref; ++j) {
            if (std::abs((*pjt->jteta)[j]) >= 2) { continue; }

            jtptCor.push_back((*pjt->jtptCor)[j]);
            jtpt.push_back((*pjt->jtpt)[j]);
            jteta.push_back((*pjt->jteta)[j]);
            jtphi.push_back((*pjt->jtphi)[j]);
            WTAeta.push_back((*pjt->WTAeta)[j]);
            WTAphi.push_back((*pjt->WTAphi)[j]);
        }

        tout->Fill();
    }

    fout->Write("", TObject::kOverwrite);
    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return mebs_inputs(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
