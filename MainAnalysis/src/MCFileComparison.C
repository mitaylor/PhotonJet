#include "TFile.h"
#include "TDirectory.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TChain.h"

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

using namespace std;

double GetAvgRho(vector<double>* evtRho, vector<double>* etaMin, vector<double>* etaMax) {
    double result = 0;
    double count = 0;

    if(etaMin == nullptr)
        return -1;

    int NBin = etaMin->size();
    if(NBin == 0)
        return -1;

    for(int i = 0; i < NBin; i++) {
        double fraction = 1;

        if ((*etaMax)[i-1] > 1.442)
            continue;
        if ((*etaMin)[i+1] < -1.442)
            continue;
        
        if ((*etaMax)[i] > 1.442) {
            fraction = (1.442 - (*etaMax)[i-1]) / ((*etaMax)[i] - (*etaMax)[i-1]);
        }

        if ((*etaMin)[i] < -1.442) {
            fraction = ((*etaMin)[i+1] + 1.442) / ((*etaMin)[i+1] - (*etaMin)[i]);
        }

        count += fraction;
        result += (*evtRho)[i] * fraction;
    }

    if (count > 0) return result/count;
    else return -1;
}

void FormatHistogram(TH1F* hist, int color) {
    hist->SetMarkerColor(color);
    hist->SetLineColor(color);
    hist->SetMarkerSize(0.5);
    hist->SetMarkerStyle(20);
    hist->SetStats(0);
    hist->GetXaxis()->CenterTitle(true);
    hist->GetYaxis()->SetTitle("Normalized Counts");
    hist->GetYaxis()->CenterTitle(true);
}

void FormatHistogram2D(TH2F* hist) {
    hist->SetStats(0);
    hist->GetXaxis()->SetTitle("Eta");
    hist->GetXaxis()->CenterTitle(true);
    hist->GetYaxis()->SetTitle("Ncoll");
    hist->GetYaxis()->CenterTitle(true);
}

void PrintHist(TH1* hist1, TH1* hist2, string title, TCanvas* canvas, TLegend* legend, string filename) {
    hist1->GetXaxis()->SetTitle(title.c_str());
    hist1->Draw("HIST LP");
    hist2->Draw("HIST LP SAME");
    legend->Draw();
    canvas->Print(filename.c_str());
}

void PrintHist2D(TH2* hist, string title, TCanvas* canvas, string filename) {
    hist->GetXaxis()->SetTitle(title.c_str());
    hist->Draw("COLZ");
    canvas->Print(filename.c_str());
    hist->SetMaximum(1);
    hist->SetMinimum(0.0001);
}

void GetFiles(char const* input, vector<string>& files) {
    TSystemDirectory dir(input, input);
    TList *list = dir.GetListOfFiles();

    if (list) {
        TSystemFile *file;
        string fname;
        TIter next(list);
        while ((file = (TSystemFile*) next())) {
            fname = file->GetName();

            if (file->IsDirectory() && (fname.find(".") == string::npos)) {
                string newDir = string(input) + fname + "/";
                GetFiles(newDir.c_str(), files);
            }
            else if ((fname.find(".root") != string::npos)) {
                files.push_back(string(input) + fname);
                cout << files.back() << endl;
            }
        }
    }

    return;
}

void FillChain(TChain& chain, vector<string>& files) {
    for (auto file : files) {
        chain.Add(file.c_str());
    }
}

int Compare(char const* oldInput, char const* newInput, int pthat) {
    /* read in 2018 information */
    vector<string> oldFiles;
    GetFiles(oldInput, oldFiles);

    TChain oldPhotonChain("ggHiNtuplizerGED/EventTree");
    FillChain(oldPhotonChain, oldFiles);
    // oldPhotonChain.Add(oldInput);
    TTreeReader oldPhotonReader(&oldPhotonChain);
    TTreeReaderValue<float> oldPhotonRho(oldPhotonReader, "rho");

    TChain oldRhoChain("hiPuRhoR3Analyzer/t");
    FillChain(oldRhoChain, oldFiles);
    // oldRhoChain.Add(oldInput);
    TTreeReader oldRhoReader(&oldRhoChain);
    TTreeReaderValue<vector<double>> oldRhoEtaMin(oldRhoReader, "etaMin");
    TTreeReaderValue<vector<double>> oldRhoEtaMax(oldRhoReader, "etaMax");
    TTreeReaderValue<vector<double>> oldRhoRho(oldRhoReader, "rho");

    TChain oldEvtChain("hiEvtAnalyzer/HiTree");
    FillChain(oldEvtChain, oldFiles);
    // oldEvtChain.Add(oldInput);
    TTreeReader oldEvtReader(&oldEvtChain);
    TTreeReaderValue<float> oldEvtNcoll(oldEvtReader, "Ncoll");

    /* read in 2022 information */
    vector<string> newFiles;
    GetFiles(newInput, newFiles);

    // TChain newPhotonChain("ggHiNtuplizerGED/EventTree");
    TChain newPhotonChain("ggHiNtuplizer/EventTree");
    FillChain(newPhotonChain, newFiles);
    // newPhotonChain.Add(newInput);
    TTreeReader newPhotonReader(&newPhotonChain);
    TTreeReaderValue<float> newPhotonRho(newPhotonReader, "rho");

    // TChain newRhoChain("hiPuRhoR3Analyzer/t");
    TChain newRhoChain("hiPuRhoAnalyzer/t");
    FillChain(newRhoChain, newFiles);
    // newRhoChain.Add(newInput);
    TTreeReader newRhoReader(&newRhoChain);
    TTreeReaderValue<vector<double>> newRhoEtaMin(newRhoReader, "etaMin");
    TTreeReaderValue<vector<double>> newRhoEtaMax(newRhoReader, "etaMax");
    TTreeReaderValue<vector<double>> newRhoRho(newRhoReader, "rho");

    TChain newEvtChain("hiEvtAnalyzer/HiTree");
    FillChain(newEvtChain, newFiles);
    // newEvtChain.Add(newInput);
    TTreeReader newEvtReader(&newEvtChain);
    TTreeReaderValue<float> newEvtNcoll(newEvtReader, "Ncoll");

    /* create histograms for energy sum plots */
    int nbins = 80;
    float min = 0;
    float max = 300;

    auto oldPhotonHist = new TH1F("oldPhotonHist", "", nbins, min, max);
    auto newPhotonHist = new TH1F("newPhotonHist", "", nbins, min, max);
    auto oldRhoHist = new TH1F("oldRhoHist", "", nbins, min, max);
    auto newRhoHist = new TH1F("newRhoHist", "", nbins, min, max);

    auto oldPhotonNcollHist = new TH2F("oldPhotonNcollHist", "", nbins, min, max, nbins, 0, 2100);
    auto oldRhoNcollHist = new TH2F("oldRhoNcollHist", "", nbins, min, max, nbins, 0, 2100);
    auto newPhotonNcollHist = new TH2F("newPhotonNcollHist", "", nbins, min, max, nbins, 0, 2100);
    auto newRhoNcollHist = new TH2F("newRhoNcollHist", "", nbins, min, max, nbins, 0, 2100);

    int oldEntries = oldPhotonChain.GetEntries();
    int newEntries = newPhotonChain.GetEntries();

    /* customize energy sum histogram draw options */
    auto legend = new TLegend(0.55, 0.75 ,0.85, 0.85);
    legend->SetTextSize(0.03);
    legend->AddEntry(oldPhotonHist, "Extra MC Photon Rho", "p");
    legend->AddEntry(newPhotonHist, "Nominal MC Photon Rho", "p");
    legend->AddEntry(oldRhoHist, "Extra MC Analyzer Rho", "p");
    legend->AddEntry(newRhoHist, "Nominal MC Analyzer Rho", "p");

    FormatHistogram(newPhotonHist, 30);
    FormatHistogram(oldPhotonHist, 46);
    FormatHistogram(newRhoHist, 32);
    FormatHistogram(oldRhoHist, 44);

    FormatHistogram2D(oldPhotonNcollHist);
    FormatHistogram2D(oldRhoNcollHist);
    FormatHistogram2D(newPhotonNcollHist);
    FormatHistogram2D(newRhoNcollHist);

    /* read in information from TTrees */
    for (int i = 1; i < oldEntries; ++i) {
        oldPhotonReader.Next(); oldRhoReader.Next(); oldEvtReader.Next();

        if (i % (oldEntries / 100) == 0) cout << i << " / " << oldEntries << endl;
            
        auto avg_rho = GetAvgRho(oldRhoRho.Get(), oldRhoEtaMin.Get(), oldRhoEtaMax.Get());

        oldPhotonHist->Fill(*oldPhotonRho);
        oldRhoHist->Fill(avg_rho);
        oldPhotonNcollHist->Fill(*oldPhotonRho, *oldEvtNcoll);
        oldRhoNcollHist->Fill(avg_rho, *oldEvtNcoll);
    }

    for (int i = 1; i < newEntries; ++i) {
        newPhotonReader.Next(); newRhoReader.Next(); newEvtReader.Next();

        if (i % (newEntries / 100) == 0) cout << i << " / " << newEntries << endl;

        auto avg_rho = GetAvgRho(newRhoRho.Get(), newRhoEtaMin.Get(), newRhoEtaMax.Get());

        newPhotonHist->Fill(*newPhotonRho);
        newRhoHist->Fill(avg_rho);
        newPhotonNcollHist->Fill(*newPhotonRho, *newEvtNcoll);
        newRhoNcollHist->Fill(avg_rho, *newEvtNcoll);
    }

    /* scale the histograms */
    oldPhotonHist->Scale(1.0/oldEntries);
    oldRhoHist->Scale(1.0/oldEntries);
    oldPhotonNcollHist->Scale(1.0/oldEntries);
    oldRhoNcollHist->Scale(1.0/oldEntries);

    newPhotonHist->Scale(1.0/newEntries);
    newRhoHist->Scale(1.0/newEntries);
    newPhotonNcollHist->Scale(1.0/oldEntries);
    newRhoNcollHist->Scale(1.0/oldEntries);

    string filename = "MCComparison_" + to_string(pthat) + ".pdf";

    auto canvas = new TCanvas("canvas", "", 0 , 0, 500, 500);
    canvas->SetLeftMargin(0.15);
    canvas->SetBottomMargin(0.15);
    canvas->Print(string(filename + "[").c_str());
    canvas->SetLogz(1);
    canvas->Clear();

    PrintHist(oldPhotonHist, newPhotonHist, "Extra vs Nominal MC Photon Rho", canvas, legend, filename);
    PrintHist(oldRhoHist, newRhoHist, "Extra vs Nominal MC Calc Rho", canvas, legend, filename);
    PrintHist(oldPhotonHist, oldRhoHist, "Extra MC Photon vs Calc Rho", canvas, legend, filename);
    PrintHist(newPhotonHist, newRhoHist, "Nominal MC Photon vs Calc Rho", canvas, legend, filename);
    PrintHist2D(oldPhotonNcollHist, "Extra MC Photon Rho vs Ncoll", canvas, filename);
    PrintHist2D(oldRhoNcollHist, "Extra MC Calc Rho vs Ncoll", canvas, filename);
    PrintHist2D(newPhotonNcollHist, "Nominal MC Photon Rho vs Ncoll", canvas, filename);
    PrintHist2D(newRhoNcollHist, "Nominal MC Calc Rho vs Ncoll", canvas, filename);

    canvas->Print(string(filename + "]").c_str());

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return Compare(argv[1], argv[2], stoi(argv[3]));

    else {
        cout << "ERROR: Please pass two paths for Extra MC and Nominal MC." << endl;
        return -1;
    }
}
