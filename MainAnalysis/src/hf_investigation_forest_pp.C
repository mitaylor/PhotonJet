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

    // auto input = conf->get<std::string>("input");
    auto files = conf->get<std::vector<std::string>>("input");
    
    auto tag = conf->get<std::string>("tag");
    auto pthat = conf->get<std::string>("pthat");

    auto const photon_pt_min = conf->get<float>("photon_pt_min");
    auto const photon_eta_abs = conf->get<float>("photon_eta_abs");

    auto const hovere_max = conf->get<float>("hovere_max");
    auto const see_max = conf->get<float>("see_max");
    auto const iso_max = conf->get<float>("iso_max");

    auto dpt = conf->get<std::vector<float>>("pt_diff");

    /* read in information */
    // std::vector<std::string> files;
    // GetFiles(input.c_str(), files);

    TChain evtChain("hiEvtAnalyzer/HiTree");
    FillChain(evtChain, files);
    // evtChain.Add(input);
    TTreeReader evtReader(&evtChain);
    TTreeReaderValue<std::vector<int>> npus(evtReader, "npus");
    TTreeReaderValue<float> weight(evtReader, "weight");
    TTreeReaderValue<float> vz(evtReader, "vz");
    TTreeReaderValue<float> pthat(evtReader, "pthat");

    TChain genChain("HiGenParticleAna/hi");
    FillChain(genChain, files);
    // genChain.Add(input);
    TTreeReader genReader(&genChain);
    TTreeReaderValue<float> ncoll(genReader, "ncoll");
    TTreeReaderValue<int> mult(genReader, "mult");
    TTreeReaderValue<std::vector<float>> energy(genReader, "E");
    TTreeReaderValue<std::vector<float>> eta(genReader, "eta");
    // TTreeReaderValue<std::vector<float>> phi(genReader, "phi");
    // TTreeReaderValue<std::vector<float>> pt(genReader, "pt");
    TTreeReaderValue<std::vector<int>> sube(genReader, "sube");

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
    TTreeReaderValue<std::vector<float>> pfEnergy(pfReader, "pfEnergy");
    TTreeReaderValue<std::vector<float>> pfEta(pfReader, "pfEta");
    TTreeReaderValue<std::vector<float>> pfPhi(pfReader, "pfPhi");

    /* create histograms */
    int max_hf = 70000;
    int max_avg_hf = 2500;

    auto ipt = new interval(dpt);
    auto ihf = new interval("Estimated HF"s, 20, 0, max_avg_hf);
    auto ipthat = new interval("pthat"s, 50, 0, 200);

    auto fhf = std::bind(&interval::book<TH1F>, ihf, _1, _2, _3);
    auto fpthat = std::bind(&interval::book<TH1F>, ipthat, _1, _2, _3);
    auto fnpu = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";nPU;HF Energy;"s + label).data(), 18, 0, 18, 0, max_hf, "LE"); };

    auto h_hf_pf = new history<TH1F>("h_hf_pf"s, "", fhf, ipt->size());
    auto h_npu_hf_pf = new history<TProfile>("npu"s, "", fnpu, 1);
    auto h_pthat = new history<TH1F>("h_pthat"s, "", fpthat, ipt->size());

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* read in information from TTrees */
    int entries = evtChain.GetEntries();

    for (int i = 1; i < entries; ++i) {
        evtReader.Next(); phoReader.Next(); pfReader.Next(); genReader.Next();

        if (i % (entries/200) == 0) std::cout << i << " / " << entries << std::endl;

        if (std::abs(*vz) > 15) { continue; }

        h_pthat[0]->Fill(*pthat, *weight);

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
        if ((*phoSigmaIEtaIEta_2012)[leading] > see_max) { continue; }

        /* isolation requirement */
        float isolation = (*pho_ecalClusterIsoR3)[leading]
            + (*pho_hcalRechitIsoR3)[leading]
            + (*pho_trackIsoR3PtCut20)[leading];
        if (isolation > iso_max) { continue; }

        if (leading_pt > 200) { continue; }
        auto pt_x = ipt->index_for(leading_pt);

        float pf_sum = 0;

        for (size_t j = 0; j < pfEnergy->size(); ++j) {
            if ((*pfId)[j] >= 6) {
                pf_sum += (*pfEnergy)[j];
            }
        }

        if ((*npus)[5] == 0) { 
            (*h_hf_pf)[pt_x]->Fill(pf_sum, *weight);
        }

        (*h_npu_hf_pf)[0]->Fill((*npus)[5], pf_sum, *weight);
    }

    /* save histograms */
    in(output, [&]() {
        h_hf_pf->save(tag);
        h_npu_hf_pf->save(tag);
        h_pthat->save(tag);
    });

    /* plot histograms */
    auto pt_info = [&](int64_t index) {
        info_text(index, 0.75, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    auto mean_info_pu = [&](int64_t index) {
        char buffer[128] = { '\0' };
        sprintf(buffer, "mean: %.3f +- %.3f",
            (*h_hf_pf)[index - 1]->GetMean(1),
            (*h_hf_pf)[index - 1]->GetMeanError(1));

        TLatex* text = new TLatex();
        text->SetTextFont(43);
        text->SetTextSize(12);
        text->DrawLatexNDC(0.54, 0.75, buffer);
    };

    auto hb = new pencil();
    hb->category("type", "PbPb MC", "PP MC");

    auto c1 = new paper(tag + "_" + pthat + "_pthat", hb);
    apply_style(c1, "", "#sqrt{s} = 5.02 TeV"s);
    c4->add((*h_pthat)[0], "PP MC");

    auto c2 = new paper(tag + "_" + pthat + "_selected_estimated_hf_sum", hb);
    apply_style(c2, "", "#sqrt{s} = 5.02 TeV"s);

    c2->accessory(pt_info);
    c2->accessory(mean_info_pu);
    c2->divide(ipt->size(), -1);

    for (int64_t j = 0; j < ipt->size(); ++j) {
        c2->add((*h_hf_pf)[j], "PP MC");
    }

    hb->sketch();
    c2->draw("pdf");

    auto c4 = new paper(tag + "_" + pthat + "_selected_estimated_hf_vs_pu", hb);
    apply_style(c4, "", "#sqrt{s} = 5.02 TeV"s);

    c4->add((*h_npu_hf_pf)[0], "PP MC");

    hb->sketch();
    c4->draw("pdf");

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
