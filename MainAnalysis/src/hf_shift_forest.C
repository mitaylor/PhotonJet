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
                std::cout << files.back() << std::endl;
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

    auto hp_input = conf->get<std::string>("hp_input");
    auto mb_input = conf->get<std::string>("mb_input");

    auto tag = conf->get<std::string>("tag");
    auto pthat = conf->get<std::string>("pthat");

    auto use_energy = conf->get<bool>("use_energy");

    auto const photon_pt_min = conf->get<float>("photon_pt_min");
    auto const photon_eta_abs = conf->get<float>("photon_eta_abs");

    auto const hovere_max = conf->get<float>("hovere_max");
    auto const see_max = conf->get<float>("see_max");
    auto const iso_max = conf->get<float>("iso_max");

    /* read in information */
    std::vector<std::string> hp_files;
    GetFiles(hp_input.c_str(), hp_files);

    TChain hpEvtChain("hiEvtAnalyzer/HiTree");
    FillChain(hpEvtChain, hp_files);
    // hpEvtChain.Add(input);
    TTreeReader hpEvtReader(&hpEvtChain);
    TTreeReaderValue<float> hpWeight(hpEvtReader, "weight");
    TTreeReaderValue<float> hpVz(hpEvtReader, "vz");
    TTreeReaderValue<float> hpNcoll(hpEvtReader, "Ncoll");

    TChain hpPhoChain("ggHiNtuplizerGED/EventTree");
    FillChain(hpPhoChain, hp_files);
    // hpPhoChain.Add(input);
    TTreeReader hpPhoReader(&hpPhoChain);
    TTreeReaderValue<int32_t> hpNPho(hpPhoReader, "nPho");
    TTreeReaderValue<std::vector<float>> hpPhoEt(hpPhoReader, "phoEt");
    TTreeReaderValue<std::vector<float>> hpPhoSCEta(hpPhoReader, "phoSCEta");
    TTreeReaderValue<std::vector<float>> hpPhoHoverE(hpPhoReader, "phoHoverE");
    TTreeReaderValue<std::vector<float>> hpPhoSigmaIEtaIEta_2012(hpPhoReader, "phoSigmaIEtaIEta_2012");
    TTreeReaderValue<std::vector<float>> hpPho_ecalClusterIsoR3(hpPhoReader, "pho_ecalClusterIsoR3");
    TTreeReaderValue<std::vector<float>> hpPho_hcalRechitIsoR3(hpPhoReader, "pho_hcalRechitIsoR3");
    TTreeReaderValue<std::vector<float>> hpPho_trackIsoR3PtCut20(hpPhoReader, "pho_trackIsoR3PtCut20");

    TChain hpPfChain("pfcandAnalyzer/pfTree");
    FillChain(hpPfChain, hp_files);
    // hpPfChain.Add(input);
    TTreeReader hpPfReader(&hpPfChain);
    TTreeReaderValue<std::vector<int>> hpPfId(hpPfReader, "pfId");
    TTreeReaderValue<std::vector<float>> hpPfPt(hpPfReader, "pfPt");
    TTreeReaderValue<std::vector<float>> hpPfEnergy(hpPfReader, "pfEnergy");
    TTreeReaderValue<std::vector<float>> hpPfEta(hpPfReader, "pfEta");
    TTreeReaderValue<std::vector<float>> hpPfPhi(hpPfReader, "pfPhi");

    std::vector<std::string> mb_files;
    GetFiles(mb_input.c_str(), mb_files);

    TChain mbEvtChain("hiEvtAnalyzer/HiTree");
    FillChain(mbEvtChain, mb_files);
    // mbEvtChain.Add(input);
    TTreeReader mbEvtReader(&mbEvtChain);
    TTreeReaderValue<float> mbWeight(mbEvtReader, "weight");
    TTreeReaderValue<float> mbVz(mbEvtReader, "vz");
    TTreeReaderValue<float> mbNcoll(mbEvtReader, "Ncoll");

    TChain mbPhoChain("ggHiNtuplizerGED/EventTree");
    FillChain(mbPhoChain, mb_files);
    // mbPhoChain.Add(input);
    TTreeReader mbPhoReader(&mbPhoChain);
    TTreeReaderValue<int32_t> mbNPho(mbPhoReader, "nPho");
    TTreeReaderValue<std::vector<float>> mbPhoEt(mbPhoReader, "phoEt");
    TTreeReaderValue<std::vector<float>> mbPhoSCEta(mbPhoReader, "phoSCEta");
    TTreeReaderValue<std::vector<float>> mbPhoHoverE(mbPhoReader, "phoHoverE");
    TTreeReaderValue<std::vector<float>> mbPhoSigmaIEtaIEta_2012(mbPhoReader, "phoSigmaIEtaIEta_2012");
    TTreeReaderValue<std::vector<float>> mbPho_ecalClusterIsoR3(mbPhoReader, "pho_ecalClusterIsoR3");
    TTreeReaderValue<std::vector<float>> mbPho_hcalRechitIsoR3(mbPhoReader, "pho_hcalRechitIsoR3");
    TTreeReaderValue<std::vector<float>> mbPho_trackIsoR3PtCut20(mbPhoReader, "pho_trackIsoR3PtCut20");

    TChain mbPfChain("pfcandAnalyzer/pfTree");
    FillChain(mbPfChain, mb_files);
    // mbPfChain.Add(input);
    TTreeReader mbPfReader(&mbPfChain);
    TTreeReaderValue<std::vector<int>> mbPfId(mbPfReader, "pfId");
    TTreeReaderValue<std::vector<float>> mbPfPt(mbPfReader, "pfPt");
    TTreeReaderValue<std::vector<float>> mbPfEnergy(mbPfReader, "pfEnergy");
    TTreeReaderValue<std::vector<float>> mbPfEta(mbPfReader, "pfEta");
    TTreeReaderValue<std::vector<float>> mbPfPhi(mbPfReader, "pfPhi");
    
    /* define histograms */
    int hf_max = use_energy ? 70000 : 7000;

    auto ihf = new interval("HF Energy"s, 100, 0, hf_max);
    auto in = new interval("Ncoll"s, 100, 0, 2100);

    auto mhn = new multival(*ihf, *in);
    auto fhn = std::bind(&multival::book<TH2F>, mhn, _1, _2, _3); // hf:ncoll

    auto hp_hn = new history<TH2F>("hp_hn"s, "Pythia+Hydjet", fhn, 1);
    auto mb_hn = new history<TH2F>("mb_hn"s, "Hydjet", fhn, 1);

    auto fhnp = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";Ncoll;HF Energy;"s + label).data(), 100, 0, 2100, 0, hf_max, "LE"); };

    auto hp_hn_p = new history<TProfile>("hp_hn_p"s, "Pythia+Hydjet", fhnp, 1);
    auto mb_hn_p = new history<TProfile>("mb_hn_p"s, "Hydjet", fhnp, 1);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* read in information from TTrees */
    int entries = hpEvtChain.GetEntries();
    double nphotons = 0;

    for (int64_t i = 0; i < entries/200; ++i) {
        hpEvtReader.Next(); hpPhoReader.Next(); hpPfReader.Next();

        if (i % (entries/2000) == 0) std::cout << i << " / " << entries << std::endl;

        if (i > 5040) std::cout << i << std::endl;

        if (std::abs(*hpVz) > 15) { continue; }
        
        int64_t leading = -1;
        float leading_pt = 0;
        for (int64_t j = 0; j < *hpNPho; ++j) {
            if ((*hpPhoEt)[j] <= photon_pt_min) { continue; }
            if (std::abs((*hpPhoSCEta)[j]) >= photon_eta_abs) { continue; }
            if ((*hpPhoHoverE)[j] > hovere_max) { continue; }

            if ((*hpPhoEt)[j] > leading_pt) {
                leading = j;
                leading_pt = (*hpPhoEt)[j];
            }
        }

        /* require leading photon */
        if (leading < 0) { continue; }
        if ((*hpPhoSigmaIEtaIEta_2012)[leading] > see_max) { continue; }

        /* isolation requirement */
        float isolation = (*hpPho_ecalClusterIsoR3)[leading]
            + (*hpPho_hcalRechitIsoR3)[leading]
            + (*hpPho_trackIsoR3PtCut20)[leading];
        if (isolation > iso_max) { continue; }

        if (leading_pt > 200) { continue; }

        nphotons += *hpWeight;
        float pf_sum = 0;

        if (use_energy) {
            for (size_t j = 0; j < hpPfEnergy->size(); ++j) {
                // if (std::abs((*pjt->hpPfEta)[j]) > 3 && std::abs((*pjt->hpPfEta)[j]) < 5) {
                if ((*hpPfId)[j] >= 6) {
                    pf_sum += (*hpPfEnergy)[j];
                }
            }
        } else {
            for (size_t j = 0; j < hpPfPt->size(); ++j) {
                // if (std::abs((*pjt->hpPfEta)[j]) > 3 && std::abs((*pjt->hpPfEta)[j]) < 5) {
                if ((*hpPfId)[j] >= 6) {
                    pf_sum += (*hpPfPt)[j];
                }
            }
        }

        (*hp_hn)[0]->Fill(pf_sum, *hpNcoll, *hpWeight);
        (*hp_hn_p)[0]->Fill(*hpNcoll, pf_sum, *hpWeight);
    }

    entries = mbEvtChain.GetEntries();
    double nmb = 0;

    for (int64_t i = 0; i < entries/200; ++i) {
        mbEvtReader.Next(); mbPhoReader.Next(); mbPfReader.Next();

        if (i % (entries/200) == 0) std::cout << i << " / " << entries << std::endl;

        if (std::abs(*mbVz) > 15) { continue; }

        nmb += *mbWeight;
        float pf_sum = 0;

        if (use_energy) {
            for (size_t j = 0; j < mbPfEnergy->size(); ++j) {
                // if (std::abs((*pjt->mbPfEta)[j]) > 3 && std::abs((*pjt->mbPfEta)[j]) < 5) {
                if ((*mbPfId)[j] >= 6) {
                    pf_sum += (*mbPfEnergy)[j];
                }
            }
        } else {
            for (size_t j = 0; j < mbPfPt->size(); ++j) {
                // if (std::abs((*pjt->mbPfEta)[j]) > 3 && std::abs((*pjt->mbPfEta)[j]) < 5) {
                if ((*mbPfId)[j] >= 6) {
                    pf_sum += (*mbPfPt)[j];
                }
            }
        }

        (*mb_hn)[0]->Fill(pf_sum, *mbNcoll, *mbWeight);
        (*mb_hn_p)[0]->Fill(*mbNcoll, pf_sum, *mbWeight);
    }

    /* subtract distributions */
    auto hp_hn_h = new TH1F("hp_hn_h", ";;Orange - Purple", 100, 0, 2100);
    auto mb_hn_h = new TH1F("mb_hn_h", ";;Orange - Purple", 100, 0, 2100);

    for (int i = 1; i <= hp_hn_h->GetNbinsX(); ++i) {
        hp_hn_h->SetBinContent(i, (*hp_hn_p)[0]->GetBinContent(i));
        hp_hn_h->SetBinError(i, (*hp_hn_p)[0]->GetBinError(i));
        mb_hn_h->SetBinContent(i, (*mb_hn_p)[0]->GetBinContent(i));
        mb_hn_h->SetBinError(i, (*mb_hn_p)[0]->GetBinError(i));
    }

    if (!(hp_hn_h->GetSumw2N() > 0)) hp_hn_h->Sumw2(true);

    hp_hn_h->Add(mb_hn_h, -1);

    hp_hn_h->SetMaximum(100);
    hp_hn_h->SetMinimum(-100);

    /* fit to a constant function */
    hp_hn_h->Fit("pol0", "", "", 21, 2100);

    auto hp_hn_h_fit = hp_hn_h->GetFunction("pol0");

    auto hp_hn_h_mean = hp_hn_h_fit->GetParameter(0);
    auto hp_hn_h_error = hp_hn_h_fit->GetParError(0);

    std::cout << "HF difference: " << hp_hn_h->GetMean(2) << " - " << mb_hn_h->GetMean(2) << std::endl;
    std::cout << "HF error: " << hp_hn_h->GetMeanError(2) << " - " << mb_hn_h->GetMeanError(2) << std::endl;
    std::cout << "Fit HF difference: " << hp_hn_h_mean << std::endl;
    std::cout << "Fit HF error: " << hp_hn_h_error << std::endl;

    auto hn_fit_info = [&](int64_t index) {
        if (index == 2) {
            char buffer[128] = { '\0' };
            sprintf(buffer, "Mean = %.3f +- %.3f", hp_hn_h_mean, hp_hn_h_error);

            TLatex* hn_mean = new TLatex();
            hn_mean->SetTextFont(43);
            hn_mean->SetTextSize(12);

            hn_mean->DrawLatexNDC(0.2, 0.2, buffer);
        }
    };

    /* draw rho distributions */
    /* draw distributions */
    auto system_tag = "PbPb #sqrt{s_{NN}} = 5.02 TeV"s; 
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Simulation}}"s;
    // cms += "         p_{T}^{#gamma} > 40 GeV";

    auto hb = new pencil();
    hb->category("type", "Pythia+Hydjet", "Hydjet"); 

    auto c3 = new paper(tag + "_pythia_hydjet_hf_v_ncoll", hb); 
    apply_style(c3, cms, system_tag);
    c3->set(paper::flags::logz);
    c3->add((*hp_hn)[0], "Pythia+Hydjet");
    c3->adjust((*hp_hn)[0], "col", "");

    auto c4 = new paper(tag + "_hydjet_hf_v_ncoll", hb);
    apply_style(c4, cms, system_tag);
    c4->set(paper::flags::logz);
    c4->add((*mb_hn)[0], "Hydjet");
    c4->adjust((*mb_hn)[0], "col", "");

    auto c8 = new paper(tag + "_comp_hf_v_ncoll", hb);
    c8->divide(1, -1);
    apply_style(c8, cms, system_tag);
    c8->add((*mb_hn_p)[0], "Hydjet");
    c8->stack((*hp_hn_p)[0], "Pythia+Hydjet");
    c8->add(hp_hn_h);
    c8->accessory(hn_fit_info);

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
    c8->draw("pdf");

    auto fout = new TFile(output, "recreate");

    hp_hn->save();
    mb_hn->save();
    hp_hn_h->Write();

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return hf_shift(argv[1], argv[2]);

    return 0;
}
