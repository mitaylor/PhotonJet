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

int Compare(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    
    auto entries = conf->get<int64_t>("entries");
    auto tag = conf->get<std::string>("tag");
    auto pthat = conf->get<std::string>("pthat");
    auto type = conf->get<std::string>("type");

    /* selections */
    auto const photon_pt_min = conf->get<float>("photon_pt_min");
    auto const photon_eta_abs = conf->get<float>("photon_eta_abs");
    auto const hovere_max = conf->get<float>("hovere_max");
    auto const iso_max = conf->get<float>("iso_max");

    auto dpt = conf->get<std::vector<float>>("pt_diff");

    /* read in information */
    std::vector<string> files;
    GetFiles(input, files);

    TChain trackChain("ppTrack/trackTree");
    FillChain(trackChain, files);
    // trackChain.Add(input);
    TTreeReader trackReader(&trackChain);
    TTreeReaderValue<int> nVtx(trackReader, "nVtx");

    TChain evtChain("hiEvtAnalyzer/HiTree");
    FillChain(evtChain, files);
    // evtChain.Add(input);
    TTreeReader evtReader(&evtChain);
    TTreeReaderValue<std::vector<int>> npus(evtReader, "npus");
    TTreeReaderValue<float> weight(evtReader, "weight");
    TTreeReaderValue<float> vz(evtReader, "vz");

    TChain phoChain("ggHiNtuplizerGED/EventTree");
    FillChain(phoChain, files);
    // phoChain.Add(input);
    TTreeReader phoReader(&phoChain);
    TTreeReaderValue<int32_t> nPho(phoReader, "nPho");
    TTreeReaderValue<std::vector<float>> phoEt(phoReader, "phoEt");
    TTreeReaderValue<std::vector<float>> phoSCEta(phoReader, "phoSCEta");
    TTreeReaderValue<std::vector<float>> phoHoverE(phoReader, "phoHoverE");
    TTreeReaderValue<std::vector<float>> phoSigmaIEtaIEta_2012(phoReader, "phoSigmaIEtaIEta_2012");
    TTreeReaderValue<std::vector<float>> pho_ecalClusterIsoR3(phoReader, "pho_ecalClusterIsoR3");
    TTreeReaderValue<std::vector<float>> pho_hcalRechitIsoR3(phoReader, "pho_hcalRechitIsoR3");
    TTreeReaderValue<std::vector<float>> pho_trackIsoR3PtCut20(phoReader, "pho_trackIsoR3PtCut20");

    TChain pfChain("pfcandAnalyzer/pfTree");
    FillChain(pfChain, files);
    // pfChain.Add(input);
    TTreeReader pfReader(&pfChain);
    TTreeReaderValue<std::vector<int>> pfId(pfReader, "pfId");
    TTreeReaderValue<std::vector<float>> pfPt(pfReader, "pfPt");

    /* create histograms */
    auto ipt = new interval(dpt);
    auto ihf = new interval("Estimated HF"s, 50, 0, 250);

    auto fhf = std::bind(&interval::book<TH1F>, ihf, _1, _2, _3);
    
    auto fnvtx = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";nVtx;HF Energy;"s + label).data(), 18, 0, 18, 0, 7000, "LE"); };
    auto fnpu = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";nPU;HF Energy;"s + label).data(), 18, 0, 18, 0, 7000, "LE"); };
    auto fpv = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";nPU;nVtx;"s + label).data(), 11, -0.5, 10.5, 0, 16, "LE"); };

    auto hf_v1 = new history<TH1F>("hf_v1"s, "", fhf, ipt->size());
    auto hf_p0 = new history<TH1F>("hf_p0"s, "", fhf, ipt->size());

    auto nvtx = new history<TProfile>("nvtx"s, "", fnvtx, 1);
    auto npu = new history<TProfile>("npu"s, "", fnpu, 1);
    auto npv = new history<TProfile>("npv"s, "", fpv, 1);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* read in information from TTrees */
    int entries = trackChain.GetEntries();

    for (int i = 1; i < entries; ++i) {
        trackReader.Next(); evtReader.Next(); phoReader.Next(); pfReader.Next();

        if (i % (entries/200) == 0) std::cout << i << " / " << entries << std::endl;

        if (std::abs(*vz) > 15) { continue; }

        int64_t leading = -1;
        float leading_pt = 0;

        for (int64_t j = 0; j < *nPho; ++j) {
            if ((*phoEt)[j] <= photon_pt_min) { continue; }
            if (std::abs((*phoSCEta)[j]) >= photon_eta_abs) { continue; }
            if ((*phoHoverE)[j] > hovere_max) { continue; }

            if ((*phoEt)[j] > leading_pt) {
                leading = j;
                leading_pt = (*phoEt)[j];
            }
        }

        /* require leading photon */
        if (leading < 0) { continue; }

        if ((*phoSigmaIEtaIEta_2012)[leading] > 0.02)
            continue;

        /* isolation requirement */
        float isolation = (*pho_ecalClusterIsoR3)[leading]
            + (*pho_hcalRechitIsoR3)[leading]
            + (*pho_trackIsoR3PtCut20)[leading];
        if (isolation > iso_max) { continue; }

        if (leading_pt > 200) { continue; }
        auto pt_x = ipt->index_for(leading_pt);

        float pf_sum = 0;

        for (size_t j = 0; j < pfPt->size(); ++j) {
            // if (std::abs((*pjt->pfEta)[j]) > 3 && std::abs((*pjt->pfEta)[j]) < 5) {
            if ((*pfId)[j] >= 6) {
                pf_sum += (*pfPt)[j];
            }
        }

        if (*nVtx == 1) { 
            (*hf_v1)[pt_x]->Fill(pf_sum, *weight);
        }

        if ((*npus)[5] == 0) { 
            (*hf_p0)[pt_x]->Fill(pf_sum, *weight);
        }

        (*nvtx)[0]->Fill(*nVtx, pf_sum, *weight);
        (*npu)[0]->Fill((*npus)[5], pf_sum, *weight);
        (*npv)[0]->Fill((*npus)[5], *nVtx, *weight);
    }

    /* save histograms */
    in(output, [&]() {
        hf_v1->save(tag);
        hf_p0->save(tag);
        nvtx->save(tag);
        npu->save(tag);
        npv->save(tag);
    });

    /* plot histograms */
    auto pt_info = [&](int64_t index) {
        info_text(index, 0.75, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    auto mean_info_vtx = [&](int64_t index) {
        char buffer[128] = { '\0' };
        sprintf(buffer, "mean: %.3f",
            (*hf_v1)[index - 1]->GetMean(1));

        TLatex* text = new TLatex();
        text->SetTextFont(43);
        text->SetTextSize(12);
        text->DrawLatexNDC(0.54, 0.75, buffer);
    };

    auto mean_info_pu = [&](int64_t index) {
        char buffer[128] = { '\0' };
        sprintf(buffer, "mean: %.3f",
            (*hf_p0)[index - 1]->GetMean(1));

        TLatex* text = new TLatex();
        text->SetTextFont(43);
        text->SetTextSize(12);
        text->DrawLatexNDC(0.54, 0.75, buffer);
    };v

    auto hb = new pencil();
    hb->category("type", "Data", "MC");
    
    auto c1 = new paper(tag + "_" + pthat + "_estimated_hf_nvtx_1", hb);
    apply_style(c1, "", "pp #sqrt{s} = 5.02 TeV"s);

    c1->accessory(pt_info);
    c1->accessory(mean_info_vtx);
    c1->divide(ipt->size(), -1);
    // c1->set(paper::flags::logy);

    for (int64_t j = 0; j < ipt->size(); ++j) {
        c1->add((*hf_v1)[j], type);
    }

    hb->sketch();
    c1->draw("pdf");

    auto c2 = new paper(tag + "_" + pthat + "_estimated_hf_npu_0", hb);
    apply_style(c2, "", "pp #sqrt{s} = 5.02 TeV"s);

    c2->accessory(pt_info);
    c2->accessory(mean_info_pu);
    c2->divide(ipt->size(), -1);

    for (int64_t j = 0; j < ipt->size(); ++j) {
        c2->add((*hf_p0)[j], type);
    }

    hb->sketch();
    c2->draw("pdf");

    auto c3 = new paper(tag + "_" + pthat + "_nvtx_hf", hb);
    apply_style(c3, "", "pp #sqrt{s} = 5.02 TeV"s);

    c3->add((*nvtx)[0], type);

    hb->sketch();
    c3->draw("pdf");

    auto c4 = new paper(tag + "_" + pthat + "_npu_hf", hb);
    apply_style(c4, "", "pp #sqrt{s} = 5.02 TeV"s);

    c4->add((*npu)[0], type);

    hb->sketch();
    c4->draw("pdf");

    auto c5 = new paper(tag + "_" + pthat + "_npv_hf", hb);
    apply_style(c5, "", "pp #sqrt{s} = 5.02 TeV"s);

    c5->add((*npv)[0], type);

    hb->sketch();
    c5->draw("pdf");

    printf("destroying objects..\n");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return Compare(argv[1], argv[2]);

    else {
        return -1;
    }
}
