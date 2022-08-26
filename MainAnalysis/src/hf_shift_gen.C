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
#include "TLatex.h"

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
    auto mb_input = conf->get<std::string>("mb_input");

    TH1::SetDefaultSumw2();
    
    auto irho = new interval("#rho"s, 100, 0, 400);
    auto ihf = new interval("HF Energy"s, 100, 0, 7000);
    auto imul = new interval("Multiplicity"s, 100, 0, 22000);

    auto mmh = new multival(*imul, *ihf);
    auto mhm = new multival(*ihf, *imul);
    auto mrm = new multival(*irho, *imul);

    auto fmh = std::bind(&multival::book<TH2F>, mmh, _1, _2, _3); // multiplicity:hf
    auto fhm = std::bind(&multival::book<TH2F>, mhm, _1, _2, _3); // hf:multiplicity (scaled by subid)
    auto frm = std::bind(&multival::book<TH2F>, mrm, _1, _2, _3); // rho:multiplicity (scaled by subid)

    auto hp_mh = new history<TH2F>("hp_mh"s, "Pythia+Hydjet", fmh, 1);
    auto mb_mh = new history<TH2F>("mb_mh"s, "Hydjet", fmh, 1);

    auto hp_hm = new history<TH2F>("hp_hm"s, "Pythia+Hydjet", fhm, 1);
    auto mb_hm = new history<TH2F>("mb_hm"s, "Hydjet", fhm, 1);

    auto hp_rm = new history<TH2F>("hp_rm"s, "Pythia+Hydjet", frm, 1);
    auto mb_rm = new history<TH2F>("mb_rm"s, "Hydjet", frm, 1);

    auto fmhp = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";Multiplicity;HF Energy;"s + label).data(), 100, 0, 22000, 0, 7000, "LE"); };
    auto fhmp = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";HF Energy;Multiplicity (UE);"s + label).data(), 100, 0, 400, 0, 7000, "LE"); };
    auto frmp = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";#rho;Multiplicity (UE);"s + label).data(), 100, 0, 400, 0, 7000, "LE"); };

    auto hp_mh_p = new history<TProfile>("hp_mh_p"s, "Pythia+Hydjet", fmhp, 1);
    auto mb_mh_p = new history<TProfile>("mb_mh_p"s, "Hydjet", fmhp, 1);

    auto hp_hm_p = new history<TProfile>("hp_hm_p"s, "Pythia+Hydjet", fhmp, 1);
    auto mb_hm_p = new history<TProfile>("mb_hm_p"s, "Hydjet", fhmp, 1);

    auto hp_rm_p = new history<TProfile>("hp_rm_p"s, "Pythia+Hydjet", frmp, 1);
    auto mb_rm_p = new history<TProfile>("mb_rm_p"s, "Hydjet", frmp, 1);

    auto hp_gen_dir = new TChain("HiGenParticleAna/hi");
    FillChain(hp_gen_dir, hp_input);
    TTreeReader hp_gen(hp_gen_dir);
    TTreeReaderValue<std::vector<int>> hp_subid(hp_gen, "sube");
    TTreeReaderValue<int> hp_mult(hp_gen, "mult");

    auto hp_evt_dir = new TChain("hiEvtAnalyzer/HiTree");
    FillChain(hp_evt_dir, hp_input);
    TTreeReader hp_evt(hp_evt_dir);
    TTreeReaderValue<float> hp_hf(hp_evt, "hiHF");
    TTreeReaderValue<float> hp_weight(hp_evt, "weight");

    auto hp_pho_dir = new TChain("ggHiNtuplizer/EventTree");
    FillChain(hp_pho_dir, hp_input);
    TTreeReader hp_pho(hp_pho_dir);
    TTreeReaderValue<float> hp_rho(hp_pho, "rho");

    auto mb_gen_dir = new TChain("HiGenParticleAna/hi");
    FillChain(mb_gen_dir, mb_input);
    TTreeReader mb_gen(mb_gen_dir);
    TTreeReaderValue<std::vector<int>> mb_subid(mb_gen, "sube");
    TTreeReaderValue<int> mb_mult(mb_gen, "mult");

    auto mb_evt_dir = new TChain("hiEvtAnalyzer/HiTree");
    FillChain(mb_evt_dir, mb_input);
    TTreeReader mb_evt(mb_evt_dir);
    TTreeReaderValue<float> mb_hf(mb_evt, "hiHF");
    TTreeReaderValue<float> mb_weight(mb_evt, "weight");

    auto mb_pho_dir = new TChain("ggHiNtuplizer/EventTree");
    FillChain(mb_pho_dir, mb_input);
    TTreeReader mb_pho(mb_pho_dir);
    TTreeReaderValue<float> mb_rho(mb_pho, "rho");

    auto nentries = static_cast<int64_t>(hp_gen_dir->GetEntries());

    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 10000 == 0)
            printf("entry: %li/%li\n", i, nentries);

        hp_gen.Next(); hp_evt.Next(); hp_pho.Next();

        double hp_subid_weight = 0;
        for (int64_t j = 0; j < *hp_mult; ++j) {
            hp_subid_weight += (*hp_subid)[j];
        }
        hp_subid_weight /= *hp_mult;

        (*hp_mh)[0]->Fill(*hp_mult, *hp_hf, *hp_weight);
        (*hp_hm)[0]->Fill(*hp_hf, (*hp_mult) * hp_subid_weight, *hp_weight);
        (*hp_rm)[0]->Fill(*hp_rho, (*hp_mult) * hp_subid_weight, *hp_weight);

        (*hp_mh_p)[0]->Fill(*hp_mult, *hp_hf, *hp_weight);
        (*hp_hm_p)[0]->Fill(*hp_hf, (*hp_mult) * hp_subid_weight, *hp_weight);
        (*hp_rm_p)[0]->Fill(*hp_rho, (*hp_mult) * hp_subid_weight, *hp_weight);

        ++i;
    }

    nentries = static_cast<int64_t>(mb_gen_dir->GetEntries());

    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 10000 == 0)
            printf("entry: %li/%li\n", i, nentries);

        mb_gen.Next(); mb_evt.Next(); mb_pho.Next();

        double mb_subid_weight = 0;
        for (int64_t j = 0; j < *mb_mult; ++j) {
            mb_subid_weight += (*mb_subid)[j];
        }
        mb_subid_weight /= *mb_mult;

        (*mb_mh)[0]->Fill(*mb_mult, *mb_hf, *mb_weight);
        (*mb_hm)[0]->Fill(*mb_hf, (*mb_mult) * mb_subid_weight, *mb_weight);
        (*mb_rm)[0]->Fill(*mb_rho, (*mb_mult) * mb_subid_weight, *mb_weight);

        (*mb_mh_p)[0]->Fill(*mb_mult, *mb_hf, *mb_weight);
        (*mb_hm_p)[0]->Fill(*mb_hf, (*mb_mult) * mb_subid_weight, *mb_weight);
        (*mb_rm_p)[0]->Fill(*mb_rho, (*mb_mult) * mb_subid_weight, *mb_weight);

        ++i;
    }

    /* draw rho distributions */
    auto system_tag = "PbPb #sqrt{s_{NN}} = 5.02 TeV"s; 
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Simulation}}"s;
    // cms += "         p_{T}^{#gamma} > 40 GeV";

    auto hb = new pencil();
    hb->category("type", "Pythia+Hydjet", "Hydjet"); 
    
    auto c1 = new paper("pythia_hydjet_mult_v_hf", hb); 
    apply_style(c1, cms, system_tag);
    c1->set(paper::flags::logz);
    c1->add((*hp_mh)[0], "Pythia+Hydjet");
    c1->adjust((*hp_mh)[0], "col", "");

    auto c2 = new paper("hydjet_mult_v_hf", hb);
    apply_style(c2, cms, system_tag);
    c2->set(paper::flags::logz);
    c2->add((*mb_mh)[0], "Hydjet");
    c2->adjust((*mb_mh)[0], "col", "");

    auto c3 = new paper("pythia_hydjet_hf_v_scaled_mult", hb); 
    apply_style(c3, cms, system_tag);
    c3->set(paper::flags::logz);
    c3->add((*hp_hm)[0], "Pythia+Hydjet");
    c3->adjust((*hp_hm)[0], "col", "");

    auto c4 = new paper("hydjet_hf_v_scaled_mult", hb);
    apply_style(c4, cms, system_tag);
    c4->set(paper::flags::logz);
    c4->add((*mb_hm)[0], "Hydjet");
    c4->adjust((*mb_hm)[0], "col", "");

    auto c5 = new paper("pythia_hydjet_rho_v_scaled_mult", hb); 
    apply_style(c5, cms, system_tag);
    c5->set(paper::flags::logz);
    c5->add((*hp_rm)[0], "Pythia+Hydjet");
    c5->adjust((*hp_rm)[0], "col", "");

    auto c6 = new paper("hydjet_rho_v_scaled_mult", hb);
    apply_style(c6, cms, system_tag);
    c6->set(paper::flags::logz);
    c6->add((*mb_rm)[0], "Hydjet");
    c6->adjust((*mb_rm)[0], "col", "");

    auto c7 = new paper("comp_mult_v_hf", hb);
    apply_style(c7, cms, system_tag);
    c7->add((*mb_mh_p)[0], "Hydjet");
    c7->stack((*hp_mh_p)[0], "Pythia+Hydjet");

    auto c8 = new paper("comp_hf_v_scaled_mult", hb);
    apply_style(c8, cms, system_tag);
    c8->add((*mb_hm_p)[0], "Hydjet");
    c8->stack((*hp_hm_p)[0], "Pythia+Hydjet");

    auto c9 = new paper("comp_rho_v_scaled_mult", hb);
    apply_style(c9, cms, system_tag);
    c9->add((*mb_rm_p)[0], "Hydjet");
    c9->stack((*hp_rm_p)[0], "Pythia+Hydjet");

    auto style = [](TH1* h) {
        h->SetMarkerSize(0.10);
    };

    hb->style("Hydjet", style);
    hb->style("Pythia+Hydjet", style);

    hb->sketch();
    c1->draw("pdf");
    c2->draw("pdf");
    c3->draw("pdf");
    c4->draw("pdf");
    c5->draw("pdf");
    c6->draw("pdf");

    /* save output */
    in(output, [&]() {
        hp_mh->save();
        hp_hm->save();
        hp_rm->save();
        mb_mh->save();
        mb_hm->save();
        mb_rm->save();
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return hf_shift(argv[1], argv[2]);

    return 0;
}
