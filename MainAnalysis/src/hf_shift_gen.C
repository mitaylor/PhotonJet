#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/overflow_angles.h"

#include "TChain.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TGaxis.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

void FillChain(TChain* chain, std::vector<std::string>& files) {
    for (auto file : files) {
        chain->Add(file.c_str());
    }
}

int hf_shift(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto hp_input = conf->get<std::vector<std::string>>("hp_input");
    auto mb_input = conf->get<std::vector<std::string>>("mb_input");

    TH1::SetDefaultSumw2();
    TGaxis::SetMaxDigits(3);
    
    auto irho = new interval("#rho"s, 100, 0, 400);
    auto ihf = new interval("HF Energy"s, 100, 0, 7000);
    auto in = new interval("Ncoll"s, 100, 0, 22000);

    auto mhn = new multival(*ihf, *in);
    auto mrn = new multival(*irho, *in);

    auto fhn = std::bind(&multival::book<TH2F>, mhn, _1, _2, _3); // hf:ncoll
    auto frn = std::bind(&multival::book<TH2F>, mrn, _1, _2, _3); // rho:ncoll

    auto hp_hn = new history<TH2F>("hp_hn"s, "Pythia+Hydjet", fhn, 1);
    auto mb_hn = new history<TH2F>("mb_hn"s, "Hydjet", fhn, 1);

    auto hp_rn = new history<TH2F>("hp_rn"s, "Pythia+Hydjet", frn, 1);
    auto mb_rn = new history<TH2F>("mb_rn"s, "Hydjet", frn, 1);

    auto fhnp = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";Ncoll;HF Energy;"s + label).data(), 100, 0, 22000, 0, 7000, "LE"); };
    auto frnp = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";Ncoll;#rho;"s + label).data(), 100, 0, 22000, 0, 400, "LE"); };

    auto hp_hn_p = new history<TProfile>("hp_hn_p"s, "Pythia+Hydjet", fhnp, 1);
    auto mb_hn_p = new history<TProfile>("mb_hn_p"s, "Hydjet", fhnp, 1);

    auto hp_rn_p = new history<TProfile>("hp_rn_p"s, "Pythia+Hydjet", frnp, 1);
    auto mb_rn_p = new history<TProfile>("mb_rn_p"s, "Hydjet", frnp, 1);

    auto hp_evt_dir = new TChain("hiEvtAnalyzer/HiTree");
    FillChain(hp_evt_dir, hp_input);
    TTreeReader hp_evt(hp_evt_dir);
    TTreeReaderValue<float> hp_hf(hp_evt, "hiHF");
    TTreeReaderValue<float> hp_ncoll(hp_evt, "Ncoll");
    TTreeReaderValue<float> hp_weight(hp_evt, "weight");

    auto hp_pho_dir = new TChain("ggHiNtuplizerGED/EventTree");
    FillChain(hp_pho_dir, hp_input);
    TTreeReader hp_pho(hp_pho_dir);
    TTreeReaderValue<float> hp_rho(hp_pho, "rho");

    auto mb_evt_dir = new TChain("hiEvtAnalyzer/HiTree");
    FillChain(mb_evt_dir, mb_input);
    TTreeReader mb_evt(mb_evt_dir);
    TTreeReaderValue<float> mb_hf(mb_evt, "hiHF");
    TTreeReaderValue<float> mb_ncoll(mb_evt, "Ncoll");
    TTreeReaderValue<float> mb_weight(mb_evt, "weight");

    auto mb_pho_dir = new TChain("ggHiNtuplizer/EventTree");
    FillChain(mb_pho_dir, mb_input);
    TTreeReader mb_pho(mb_pho_dir);
    TTreeReaderValue<float> mb_rho(mb_pho, "rho");

    auto nentries = static_cast<int64_t>(hp_evt_dir->GetEntries());

    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 10000 == 0)
            printf("entry: %li/%li\n", i, nentries);

        hp_evt.Next(); hp_pho.Next();

        (*hp_hn)[0]->Fill(*hp_hf, *hp_ncoll, *hp_weight);
        (*hp_rn)[0]->Fill(*hp_rho, *hp_ncoll, *hp_weight);

        (*hp_hn_p)[0]->Fill(*hp_ncoll, *hp_hf, *hp_weight);
        (*hp_rn_p)[0]->Fill(*hp_ncoll, *hp_rho, *hp_weight);

        ++i;
    }

    nentries = static_cast<int64_t>(mb_evt_dir->GetEntries());

    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 10000 == 0)
            printf("entry: %li/%li\n", i, nentries);

        mb_evt.Next(); mb_pho.Next();

        (*mb_hn)[0]->Fill(*mb_hf, *mb_ncoll, *mb_weight);
        (*mb_rn)[0]->Fill(*mb_rho, *mb_ncoll, *mb_weight);

        (*mb_hn_p)[0]->Fill(*mb_ncoll, *mb_hf, *mb_weight);
        (*mb_rn_p)[0]->Fill(*mb_ncoll, *mb_rho, *mb_weight);

        ++i;
    }

    /* subtract distributions */
    auto diff_hn_p = (TH1*) (*hp_hn_p)[0]->Clone();
    auto diff_rn_p = (TH1*) (*hp_rn_p)[0]->Clone();

    diff_hn_p->SetNameTitle("diff_hn_p", ";;Orange - Purple");
    diff_rn_p->SetNameTitle("diff_rn_p", ";;Orange - Purple");

    if (!(diff_hn_p->GetSumw2N() > 0)) diff_hn_p->Sumw2(true);
    if (!(diff_rn_p->GetSumw2N() > 0)) diff_rn_p->Sumw2(true);

    diff_hn_p->Add((*mb_hn_p)[0], -1);
    diff_rn_p->Add((*mb_rn_p)[0], -1);

    diff_hn_p->SetMaximum(1000);
    diff_hn_p->SetMinimum(-1000);
    diff_rn_p->SetMaximum(40);
    diff_rn_p->SetMinimum(-40);

    /* draw distributions */
    auto system_tag = "PbPb #sqrt{s_{NN}} = 5.02 TeV"s; 
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Simulation}}"s;
    // cms += "         p_{T}^{#gamma} > 40 GeV";

    auto hb = new pencil();
    hb->category("type", "Pythia+Hydjet", "Hydjet"); 

    auto c3 = new paper("pythia_hydjet_hf_v_ncoll", hb); 
    apply_style(c3, cms, system_tag);
    c3->set(paper::flags::logz);
    c3->add((*hp_hn)[0], "Pythia+Hydjet");
    c3->adjust((*hp_hn)[0], "col", "");

    auto c4 = new paper("hydjet_hf_v_ncoll", hb);
    apply_style(c4, cms, system_tag);
    c4->set(paper::flags::logz);
    c4->add((*mb_hn)[0], "Hydjet");
    c4->adjust((*mb_hn)[0], "col", "");

    auto c5 = new paper("pythia_hydjet_rho_v_ncoll", hb); 
    apply_style(c5, cms, system_tag);
    c5->set(paper::flags::logz);
    c5->add((*hp_rn)[0], "Pythia+Hydjet");
    c5->adjust((*hp_rn)[0], "col", "");

    auto c6 = new paper("hydjet_rho_v_ncoll", hb);
    apply_style(c6, cms, system_tag);
    c6->set(paper::flags::logz);
    c6->add((*mb_rn)[0], "Hydjet");
    c6->adjust((*mb_rn)[0], "col", "");

    auto c8 = new paper("comp_hf_v_ncoll", hb);
    c8->divide(1, -1);
    apply_style(c8, cms, system_tag);
    c8->add((*mb_hn_p)[0], "Hydjet");
    c8->stack((*hp_hn_p)[0], "Pythia+Hydjet");
    c8->add(diff_hn_p);

    auto c9 = new paper("comp_rho_v_ncoll", hb);
    c9->divide(1, -1);
    apply_style(c9, cms, system_tag);
    c9->add((*mb_rn_p)[0], "Hydjet");
    c9->stack((*hp_rn_p)[0], "Pythia+Hydjet");
    c9->add(diff_rn_p);

    auto hp_style = [](TH1* h) {
        h->SetMarkerStyle(34);
        h->SetMarkerColor(95);
        h->SetMarkerSize(0.60);
    };

    auto mb_style = [](TH1* h) {
        h->SetMarkerStyle(34);
        h->SetMarkerColor(51);
        h->SetMarkerSize(0.60);
    };

    hb->style("Hydjet", mb_style);
    hb->style("Pythia+Hydjet", hp_style);

    hb->sketch();
    c3->draw("pdf");
    c4->draw("pdf");
    c5->draw("pdf");
    c6->draw("pdf");
    c8->draw("pdf");
    c9->draw("pdf");

    // in(output, [&]() {
    //     hp_hn->save();
    //     hp_rn->save();
    //     mb_hn->save();
    //     mb_rn->save();
    // });

    auto fout = new TFile(output, "recreate");

    hp_hn->save();
    hp_rn->save();
    mb_hn->save();
    mb_rn->save();

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return hf_shift(argv[1], argv[2]);

    return 0;
}
