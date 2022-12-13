#include "../include/lambdas.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "TFile.h"
#include "TDirectory.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TChain.h"
#include "TStyle.h"

#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include "TMath.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TProfile2D.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

void GetFiles(char const* input, std::vector<std::string>& files) {
    TSystemDirectory dir(input, input);
    TList *list = dir.GetListOfFiles();

    if (list) {
        TSystemFile *file;
        std::string fname;
        TIter next(list);
        while ((file = (TSystemFile*) next())) {
            fname = file->GetName();

            if (file->IsDirectory() && (fname.find(".") == std::string::npos)) {
                std::string newDir = std::string(input) + fname + "/";
                GetFiles(newDir.c_str(), files);
            }
            else if ((fname.find(".root") != std::string::npos)) {
                files.push_back(std::string(input) + fname);
            }
        }
    }

    return;
}

void FillChain(TChain& chain, std::vector<std::string>& files) {
    for (auto file : files) {
        chain.Add(file.c_str());
    }
}

int hf_shift(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto aa_input = conf->get<std::string>("aa_input");
    auto pp_input = conf->get<std::string>("pp_input");

    auto tag = conf->get<std::string>("tag");

    auto const photon_pt_min = conf->get<float>("photon_pt_min");
    auto const photon_eta_abs = conf->get<float>("photon_eta_abs");

    auto const hovere_max = conf->get<float>("hovere_max");
    auto const see_max = conf->get<float>("see_max");
    auto const iso_max = conf->get<float>("iso_max");

    /* read in information */
    std::vector<std::string> aa_files;
    GetFiles(aa_input.c_str(), aa_files);

    TChain aaEvtChain("hiEvtAnalyzer/HiTree");
    FillChain(aaEvtChain, aa_files);
    // aaEvtChain.Add(input);
    TTreeReader aaEvtReader(&aaEvtChain);
    TTreeReaderValue<float> aaWeight(aaEvtReader, "weight");
    TTreeReaderValue<float> aaVz(aaEvtReader, "vz");
    TTreeReaderValue<float> aaNcoll(aaEvtReader, "Ncoll");

    TChain aaPhoChain("ggHiNtuplizerGED/EventTree");
    FillChain(aaPhoChain, aa_files);
    // aaPhoChain.Add(input);
    TTreeReader aaPhoReader(&aaPhoChain);
    TTreeReaderValue<int32_t> aaNPho(aaPhoReader, "nPho");
    TTreeReaderValue<std::vector<float>> aaPhoEt(aaPhoReader, "phoEt");
    TTreeReaderValue<std::vector<float>> aaPhoSCEta(aaPhoReader, "phoSCEta");
    TTreeReaderValue<std::vector<float>> aaPhoHoverE(aaPhoReader, "phoHoverE");
    TTreeReaderValue<std::vector<float>> aaPhoSigmaIEtaIEta_2012(aaPhoReader, "phoSigmaIEtaIEta_2012");
    TTreeReaderValue<std::vector<float>> aaPho_ecalClusterIsoR3(aaPhoReader, "pho_ecalClusterIsoR3");
    TTreeReaderValue<std::vector<float>> aaPho_hcalRechitIsoR3(aaPhoReader, "pho_hcalRechitIsoR3");
    TTreeReaderValue<std::vector<float>> aaPho_trackIsoR3PtCut20(aaPhoReader, "pho_trackIsoR3PtCut20");

    TChain aaPfChain("pfcandAnalyzer/pfTree");
    FillChain(aaPfChain, aa_files);
    // aaPfChain.Add(input);
    TTreeReader aaPfReader(&aaPfChain);
    TTreeReaderValue<std::vector<int>> aaPfId(aaPfReader, "pfId");
    TTreeReaderValue<std::vector<float>> aaPfPt(aaPfReader, "pfPt");
    TTreeReaderValue<std::vector<float>> aaPfEnergy(aaPfReader, "pfEnergy");
    TTreeReaderValue<std::vector<float>> aaPfEta(aaPfReader, "pfEta");
    TTreeReaderValue<std::vector<float>> aaPfPhi(aaPfReader, "pfPhi");

    std::vector<std::string> pp_files;
    GetFiles(pp_input.c_str(), pp_files);

    TChain ppEvtChain("hiEvtAnalyzer/HiTree");
    FillChain(ppEvtChain, pp_files);
    // ppEvtChain.Add(input);
    TTreeReader ppEvtReader(&ppEvtChain);
    TTreeReaderValue<float> ppWeight(ppEvtReader, "weight");
    TTreeReaderValue<float> ppVz(ppEvtReader, "vz");
    TTreeReaderValue<std::vector<int>> ppNPU(ppEvtReader, "npus");

    TChain ppPhoChain("ggHiNtuplizerGED/EventTree");
    FillChain(ppPhoChain, pp_files);
    // ppPhoChain.Add(input);
    TTreeReader ppPhoReader(&ppPhoChain);
    TTreeReaderValue<int32_t> ppNPho(ppPhoReader, "nPho");
    TTreeReaderValue<std::vector<float>> ppPhoEt(ppPhoReader, "phoEt");
    TTreeReaderValue<std::vector<float>> ppPhoSCEta(ppPhoReader, "phoSCEta");
    TTreeReaderValue<std::vector<float>> ppPhoHoverE(ppPhoReader, "phoHoverE");
    TTreeReaderValue<std::vector<float>> ppPhoSigmaIEtaIEta_2012(ppPhoReader, "phoSigmaIEtaIEta_2012");
    TTreeReaderValue<std::vector<float>> ppPho_ecalClusterIsoR3(ppPhoReader, "pho_ecalClusterIsoR3");
    TTreeReaderValue<std::vector<float>> ppPho_hcalRechitIsoR3(ppPhoReader, "pho_hcalRechitIsoR3");
    TTreeReaderValue<std::vector<float>> ppPho_trackIsoR3PtCut20(ppPhoReader, "pho_trackIsoR3PtCut20");

    TChain ppPfChain("pfcandAnalyzer/pfTree");
    FillChain(ppPfChain, pp_files);
    // ppPfChain.Add(input);
    TTreeReader ppPfReader(&ppPfChain);
    TTreeReaderValue<std::vector<int>> ppPfId(ppPfReader, "pfId");
    TTreeReaderValue<std::vector<float>> ppPfPt(ppPfReader, "pfPt");
    TTreeReaderValue<std::vector<float>> ppPfEnergy(ppPfReader, "pfEnergy");
    TTreeReaderValue<std::vector<float>> ppPfEta(ppPfReader, "pfEta");
    TTreeReaderValue<std::vector<float>> ppPfPhi(ppPfReader, "pfPhi");
    
    /* define histograms */
    auto ieta = new interval("PF Eta"s, 30, -5, 5);
    auto iphi = new interval("PF Phi"s, 30, -3.15, 3.15);
    auto ienergy = new interval("PF Energy"s, 50, 0, 1400);
    auto ipt = new interval("PF Pt"s, 50, 0, 180);

    auto feta = std::bind(&interval::book<TH1F>, ieta, _1, _2, _3);
    auto fphi = std::bind(&interval::book<TH1F>, iphi, _1, _2, _3);
    auto fenergy = std::bind(&interval::book<TH1F>, ienergy, _1, _2, _3);
    auto fpt = std::bind(&interval::book<TH1F>, ipt, _1, _2, _3);

    auto aa_eta = new history<TH1F>("aa_eta"s, "", feta, 1);
    auto aa_phi = new history<TH1F>("aa_phi"s, "", fphi, 1);
    auto aa_energy = new history<TH1F>("aa_energy"s, "", fenergy, 1);
    auto aa_pt = new history<TH1F>("aa_pt"s, "", fpt, 1);

    auto pp_eta = new history<TH1F>("pp_eta"s, "", feta, 1);
    auto pp_phi = new history<TH1F>("pp_phi"s, "", fphi, 1);
    auto pp_energy = new history<TH1F>("pp_energy"s, "", fenergy, 1);
    auto pp_pt = new history<TH1F>("pp_pt"s, "", fpt, 1);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* read in information from TTrees */
    int entries = aaEvtChain.GetEntries();
    entries = (entries > 1000000 ? 1000000 : entries);
    double naa = 0;

    for (int64_t i = 0; i < entries; ++i) {
        aaEvtReader.Next(); aaPhoReader.Next(); aaPfReader.Next();

        if (i % (entries/200) == 0) std::cout << i << " / " << entries << std::endl;

        if (std::abs(*aaVz) > 15) { continue; }
        if (*aaNcoll > 1) { continue; }
        
        int64_t leading = -1;
        float leading_pt = 0;
        for (int64_t j = 0; j < *aaNPho; ++j) {
            if ((*aaPhoEt)[j] <= photon_pt_min) { continue; }
            if (std::abs((*aaPhoSCEta)[j]) >= photon_eta_abs) { continue; }
            if ((*aaPhoHoverE)[j] > hovere_max) { continue; }

            if ((*aaPhoEt)[j] > leading_pt) {
                leading = j;
                leading_pt = (*aaPhoEt)[j];
            }
        }

        /* require leading photon */
        if (leading < 0) { continue; }
        if ((*aaPhoSigmaIEtaIEta_2012)[leading] > see_max) { continue; }

        /* isolation requirement */
        float isolation = (*aaPho_ecalClusterIsoR3)[leading]
            + (*aaPho_hcalRechitIsoR3)[leading]
            + (*aaPho_trackIsoR3PtCut20)[leading];
        if (isolation > iso_max) { continue; }

        if (leading_pt > 200) { continue; }

        naa += *aaWeight;

        for (size_t j = 0; j < aaPfEnergy->size(); ++j) {
            (*aa_eta)[0]->Fill((*aaPfEta)[j], *aaWeight);
            (*aa_phi)[0]->Fill((*aaPfPhi)[j], *aaWeight);
            (*aa_energy)[0]->Fill((*aaPfEnergy)[j], *aaWeight);
            (*aa_pt)[0]->Fill((*aaPfPt)[j], *aaWeight);
        }
    }

    entries = ppEvtChain.GetEntries();
    entries = (entries > 100000 ? 100000 : entries);
    double npp = 0;

    for (int64_t i = 0; i < entries; ++i) {
        ppEvtReader.Next(); ppPhoReader.Next(); ppPfReader.Next();

        if (i % (entries/200) == 0) std::cout << i << " / " << entries << std::endl;

        if (std::abs(*ppVz) > 15) { continue; }
        if ((*ppNPU)[5] != 0) { continue; }

        int64_t leading = -1;
        float leading_pt = 0;
        for (int64_t j = 0; j < *ppNPho; ++j) {
            if ((*ppPhoEt)[j] <= photon_pt_min) { continue; }
            if (std::abs((*ppPhoSCEta)[j]) >= photon_eta_abs) { continue; }
            if ((*ppPhoHoverE)[j] > hovere_max) { continue; }

            if ((*ppPhoEt)[j] > leading_pt) {
                leading = j;
                leading_pt = (*ppPhoEt)[j];
            }
        }

        /* require leading photon */
        if (leading < 0) { continue; }
        if ((*ppPhoSigmaIEtaIEta_2012)[leading] > see_max) { continue; }

        /* isolation requirement */
        float isolation = (*ppPho_ecalClusterIsoR3)[leading]
            + (*ppPho_hcalRechitIsoR3)[leading]
            + (*ppPho_trackIsoR3PtCut20)[leading];
        if (isolation > iso_max) { continue; }

        if (leading_pt > 200) { continue; }

        npp += *ppWeight;

        for (size_t j = 0; j < ppPfEnergy->size(); ++j) {
            (*pp_eta)[0]->Fill((*ppPfEta)[j], *ppWeight);
            (*pp_phi)[0]->Fill((*ppPfPhi)[j], *ppWeight);
            (*pp_energy)[0]->Fill((*ppPfEnergy)[j], *ppWeight);
            (*pp_pt)[0]->Fill((*ppPfPt)[j], *ppWeight);
        }
    }

    /* normalize distributions */
    (*aa_eta)[0]->Scale(1/naa);
    (*aa_phi)[0]->Scale(1/naa);
    (*aa_energy)[0]->Scale(1/naa);
    (*aa_pt)[0]->Scale(1/naa);

    (*pp_eta)[0]->Scale(1/naa);
    (*pp_phi)[0]->Scale(1/naa);
    (*pp_energy)[0]->Scale(1/naa);
    (*pp_pt)[0]->Scale(1/naa);

    /* draw distributions */
    auto system_tag = "PbPb #sqrt{s_{NN}} = 5.02 TeV"s; 
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Simulation}}"s;
    // cms += "         p_{T}^{#gamma} > 40 GeV";

    auto hb = new pencil();
    hb->category("type", "PbPb", "PP"); 

    auto c1 = new paper(tag + "_eta", hb); 
    apply_style(c1, cms, system_tag);
    c1->add((*aa_eta)[0], "PbPb");
    c1->stack((*pp_eta)[0], "PP");

    auto c2 = new paper(tag + "_phi", hb); 
    apply_style(c2, cms, system_tag);
    c2->add((*aa_phi)[0], "PbPb");
    c2->stack((*pp_phi)[0], "PP");

    auto c3 = new paper(tag + "_energy", hb); 
    apply_style(c3, cms, system_tag);
    c3->set(paper::flags::logy);
    c3->add((*aa_energy)[0], "PbPb");
    c3->stack((*pp_energy)[0], "PP");

    auto c4 = new paper(tag + "_pt", hb); 
    apply_style(c4, cms, system_tag);
    c4->set(paper::flags::logy);
    c4->add((*aa_pt)[0], "PbPb");
    c4->stack((*pp_pt)[0], "PP");

    auto aa_style = [](TH1* h) {
        h->SetMarkerStyle(34);
        h->SetMarkerColor(95);
        h->SetMarkerSize(0.60);
    };

    auto pp_style = [](TH1* h) {
        h->SetMarkerStyle(34);
        h->SetMarkerColor(51);
        h->SetMarkerSize(0.60);
    };

    hb->style("PbPb", pp_style);
    hb->style("PP", aa_style);

    hb->sketch();
    c1->draw("pdf");
    c2->draw("pdf");
    c3->draw("pdf");
    c4->draw("pdf");

    auto fout = new TFile(output, "recreate");

    aa_eta->save();
    aa_phi->save();
    aa_energy->save();
    aa_pt->save();
    pp_eta->save();
    pp_phi->save();
    pp_energy->save();
    pp_pt->save();

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return hf_shift(argv[1], argv[2]);

    return 0;
}
