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
    auto pthat_tag = conf->get<std::string>("pthat");

    auto dpt = conf->get<std::vector<float>>("pt_diff");

    /* read in information */
    // std::vector<std::string> files;
    // GetFiles(input.c_str(), files);

    TChain evtChain("hiEvtAnalyzer/HiTree");
    FillChain(evtChain, files);
    // evtChain.Add(input);
    TTreeReader evtReader(&evtChain);
    TTreeReaderValue<float> weight(evtReader, "weight");
    TTreeReaderValue<float> vz(evtReader, "vz");
    TTreeReaderValue<float> pthat(evtReader, "pthat");

    TChain genChain("HiGenParticleAna/hi");
    FillChain(genChain, files);
    // genChain.Add(input);
    TTreeReader genReader(&genChain);
    TTreeReaderValue<float> ncoll(genReader, "ncoll");
    // TTreeReaderValue<int> mult(genReader, "mult");
    // TTreeReaderValue<std::vector<float>> energy(genReader, "E");
    TTreeReaderValue<std::vector<float>> eta(genReader, "eta");
    // TTreeReaderValue<std::vector<float>> phi(genReader, "phi");
    TTreeReaderValue<std::vector<float>> pt(genReader, "pt");
    TTreeReaderValue<std::vector<int>> sube(genReader, "sube");

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
    auto iphoton = new interval(dpt);
    auto ienergy = new interval("HF Energy Sum"s, 20, 0, 2500);
    auto ipt = new interval("HF Pt Sum"s, 20, 0, 150);
    auto ipthat = new interval("pthat"s, 200, 0, 200);
    auto ieta = new interval("Eta", 20, -5, 5);

    auto fpt = std::bind(&interval::book<TH1F>, ipt, _1, _2, _3);
    auto fenergy = std::bind(&interval::book<TH1F>, ienergy, _1, _2, _3);
    auto fpthat = std::bind(&interval::book<TH1F>, ipthat, _1, _2, _3);
    auto fnpu = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";Ncoll;HF Energy;"s + label).data(), 18, 0, 18, 0, 70000, "LE"); };
    auto feta = std::bind(&interval::book<TH1F>, ieta, _1, _2, _3);

    auto h_hf_pf_energy_selected = new history<TH1F>("h_hf_pf_energy_selected"s, "", fenergy, iphoton->size());
    auto h_hf_pf_pt_selected = new history<TH1F>("h_hf_pf_pt_selected"s, "", fpt, iphoton->size());
    auto h_hf_pf_eta_selected = new history<TH1F>("h_hf_pf_eta_selected"s, "", feta, 1);
    auto h_npu_hf_pf_energy_selected = new history<TProfile>("h_npu_hf_pf_energy_selected"s, "", fnpu, 1);
    auto h_pthat_selected = new history<TH1F>("h_pthat_selected"s, "", fpthat, 1);
    auto h_hf_gen_energy_selected = new history<TH1F>("h_hf_gen_energy_selected"s, "", fenergy, 1);
    auto h_hf_gen_pt_selected = new history<TH1F>("h_hf_gen_pt_selected"s, "", fpt, 1);
    auto h_hf_gen_eta_selected = new history<TH1F>("h_hf_gen_eta_selected"s, "", feta, 1);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* read in information from TTrees */
    int entries = evtChain.GetEntries();

    for (int i = 1; i < entries; ++i) {
        evtReader.Next(); pfReader.Next(); genReader.Next();

        if (i % (entries/200) == 0) std::cout << i << " / " << entries << std::endl;

        if (std::abs(*vz) > 15) { continue; }

        float gen_energy_sum = 0;
        float gen_pt_sum = 0;

        if (*ncoll == 0) { 
            for (size_t j = 0; j < pt->size(); ++j) {
                if (std::abs((*eta)[j]) > 3 && std::abs((*eta)[j]) < 5  && (*pt)[j] > 0.4) {
                    gen_energy_sum += (*pt)[j] * std::cosh((*eta)[j]);
                    gen_pt_sum += (*pt)[j];
                }
        }

        auto pt_x = ipt->index_for(200);

        float pf_energy_sum = 0;
        float pf_pt_sum = 0;

        for (size_t j = 0; j < pfPt->size(); ++j) {
            (*h_hf_pf_eta_selected)[0]->Fill((*pfEta)[j], *weight);

            if ((*pfId)[j] >= 6) {
                pf_energy_sum += (*pfEnergy)[j];
                pf_pt_sum += (*pfPt)[j];
            }
        }

        if (*ncoll == 0) { 
            (*h_hf_pf_energy_selected)[pt_x]->Fill(pf_energy_sum, *weight);
            (*h_hf_pf_pt_selected)[pt_x]->Fill(pf_pt_sum, *weight);
            (*h_pthat_selected)[0]->Fill(*pthat, *weight);
            (*h_hf_gen_energy_selected)[0]->Fill(gen_energy_sum, *weight);
            (*h_hf_gen_pt_selected)[0]->Fill(gen_pt_sum, *weight);
            for (size_t j = 0; j < pt->size(); ++j) {
                if ((*pt)[j] > 0.4) {
                    (*h_hf_gen_eta_selected)[0]->Fill((*eta)[j], *weight);
                }
            }
        }

        (*h_npu_hf_pf_energy_selected)[0]->Fill(*ncoll, pf_energy_sum, *weight);
    }

    /* save histograms */
    in(output, [&]() {
        h_hf_pf_energy_selected->save(tag);
        h_hf_pf_pt_selected->save(tag);
        h_hf_pf_eta_selected->save(tag);
        h_npu_hf_pf_energy_selected->save(tag);
        h_pthat_selected->save(tag);
        h_hf_gen_energy_selected->save(tag);
        h_hf_gen_pt_selected->save(tag);
        h_hf_gen_eta_selected->save(tag);
    });

    /* plot histograms */
    auto pt_info = [&](int64_t index) {
        info_text(index, 0.75, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    auto mean_info = [&](history<TH1F>* h, int64_t index) {
        char buffer[128] = { '\0' };
        sprintf(buffer, "mean: %.3f +- %.3f",
            (*h)[index - 1]->GetMean(1),
            (*h)[index - 1]->GetMeanError(1));

        TLatex* text = new TLatex();
        text->SetTextFont(43);
        text->SetTextSize(12);
        text->DrawLatexNDC(0.54, 0.75, buffer);
    };

    auto avg_incremental_gain = [&](int64_t index) {
        float avg_gain = 0;

        for (int i = 1; i < 6; ++i) {
            avg_gain += (*h_npu_hf_pf_energy_selected)[index - 1]->GetBinContent(i+1) 
                - (*h_npu_hf_pf_energy_selected)[index - 1]->GetBinContent(i);
        }

        avg_gain /= 5;

        char buffer[128] = { '\0' };
        sprintf(buffer, "avg gain: %.3f", avg_gain);

        TLatex* text = new TLatex();
        text->SetTextFont(43);
        text->SetTextSize(12);
        text->DrawLatexNDC(0.54, 0.75, buffer);
    };
    

    auto hb = new pencil();
    hb->category("type", "PbPb MC", "PP MC");

    auto c2 = new paper(tag + "_" + pthat_tag + "_selected_pthat", hb);
    apply_style(c2, "", "#sqrt{s} = 5.02 TeV"s);
    c2->accessory(std::bind(mean_info, h_pthat_selected, _1));
    c2->add((*h_pthat_selected)[0], "PbPb MC");
    hb->sketch();
    c2->draw("pdf");

    auto c3 = new paper(tag + "_" + pthat_tag + "_selected_pf_hf_energy", hb);
    apply_style(c3, "", "#sqrt{s} = 5.02 TeV"s);
    c3->accessory(pt_info);
    c3->accessory(std::bind(mean_info, h_hf_pf_energy_selected, _1));
    c3->divide(iphoton->size(), -1);
    for (int64_t j = 0; j < iphoton->size(); ++j) {
        c3->add((*h_hf_pf_energy_selected)[j], "PbPb MC");
    }
    hb->sketch();
    c3->draw("pdf");

    auto c30 = new paper(tag + "_" + pthat_tag + "_selected_pf_eta", hb);
    apply_style(c30, "", "#sqrt{s} = 5.02 TeV"s);
    c30->accessory(pt_info);
    c30->accessory(std::bind(mean_info, h_hf_pf_eta_selected, _1));
    c30->add((*h_hf_pf_eta_selected)[0], "PbPb MC");
    hb->sketch();
    c30->draw("pdf");

    auto c31 = new paper(tag + "_" + pthat_tag + "_selected_pf_hf_pt", hb);
    apply_style(c31, "", "#sqrt{s} = 5.02 TeV"s);
    c31->accessory(pt_info);
    c31->accessory(std::bind(mean_info, h_hf_pf_pt_selected, _1));
    c31->divide(iphoton->size(), -1);
    for (int64_t j = 0; j < iphoton->size(); ++j) {
        c31->add((*h_hf_pf_pt_selected)[j], "PbPb MC");
    }
    hb->sketch();
    c31->draw("pdf");

    auto c4 = new paper(tag + "_" + pthat_tag + "_selected_pf_hf_energy_vs_pu", hb);
    apply_style(c4, "", "#sqrt{s} = 5.02 TeV"s);
    c4->accessory(avg_incremental_gain);
    c4->add((*h_npu_hf_pf_energy_selected)[0], "PbPb MC");
    hb->sketch();
    c4->draw("pdf");

    auto c5 = new paper(tag + "_" + pthat_tag + "_selected_gen_hf_energy", hb);
    apply_style(c5, "", "#sqrt{s} = 5.02 TeV"s);
    c5->accessory(std::bind(mean_info, h_hf_gen_energy_selected, _1));
    c5->add((*h_hf_gen_energy_selected)[0], "PbPb MC");
    hb->sketch();
    c5->draw("pdf");

    auto c50 = new paper(tag + "_" + pthat_tag + "_selected_gen_eta", hb);
    apply_style(c50, "", "#sqrt{s} = 5.02 TeV"s);
    c50->accessory(std::bind(mean_info, h_hf_gen_eta_selected, _1));
    c50->add((*h_hf_gen_eta_selected)[0], "PbPb MC");
    hb->sketch();
    c50->draw("pdf");

    auto c51 = new paper(tag + "_" + pthat_tag + "_selected_gen_hf_pt", hb);
    apply_style(c51, "", "#sqrt{s} = 5.02 TeV"s);
    c51->accessory(std::bind(mean_info, h_hf_gen_pt_selected, _1));
    c51->add((*h_hf_gen_pt_selected)[0], "PbPb MC");
    hb->sketch();
    c51->draw("pdf");

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
