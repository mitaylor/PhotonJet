#include "TFile.h"
#include "TDirectory.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TChain.h"

#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include "TMath.h"
#include "TH1F.h"
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

void FormatHistogramProf2D(TProfile2D* hist, double max) {
    hist->SetStats(0);
    hist->GetXaxis()->SetTitle("Eta");
    hist->GetXaxis()->CenterTitle(true);
    hist->GetYaxis()->SetTitle("Phi");
    hist->GetYaxis()->CenterTitle(true);
    hist->SetMinimum(0);
    hist->SetMaximum(max);
}

void PrintHist(TH1* hist1, TH1* hist2, string title, TCanvas* canvas, TLegend* legend, string filename) {
    hist1->GetXaxis()->SetTitle(title.c_str());
    hist1->Draw("HIST LP");
    hist2->Draw("HIST LP SAME");
    legend->Draw();

    TLatex* newMean = new TLatex();
    string newMeanText;
    newMean->SetTextFont(43);
    newMean->SetTextSize(12);
    newMean->SetTextColor(30);

    TLatex* oldMean = new TLatex();
    string oldMeanText;
    oldMean->SetTextFont(43);
    oldMean->SetTextSize(12);
    oldMean->SetTextColor(46);

    oldMeanText = "2018 Mean: " + to_string(hist2->GetMean());
    oldMean->DrawLatexNDC(0.6, 0.64, oldMeanText.c_str());
    newMeanText = "2022 Mean: " + to_string(hist1->GetMean());
    newMean->DrawLatexNDC(0.6, 0.60, newMeanText.c_str());
    canvas->Print(filename.c_str());
}

void PrintHistProf2D(TProfile2D* hist1, TProfile2D* hist2, TCanvas* canvas, string filename) {
    hist1->Draw("COLZ");
    canvas->Print(filename.c_str());
    hist2->Draw("COLZ");
    canvas->Print(filename.c_str());
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

int Compare(char const* oldInput, char const* newInput) {
    /* read in 2018 information */
    vector<string> oldFiles;
    GetFiles(oldInput, oldFiles);

    TChain oldPhotonChain("ggHiNtuplizerGED/EventTree");
    FillChain(oldPhotonChain, oldFiles);
    TTreeReader oldPhotonReader(&oldPhotonChain);
    TTreeReaderValue<float> oldPhotonRho(oldPhotonReader, "rho");

    TChain oldRhoChain("hiPuRhoR3Analyzer/t");
    FillChain(oldRhoChain, oldFiles);
    TTreeReader oldRhoReader(&oldRhoChain);
    TTreeReaderValue<vector<double>> oldRhoEtaMin(oldRhoReader, "etaMin");
    TTreeReaderValue<vector<double>> oldRhoEtaMax(oldRhoReader, "etaMax");
    TTreeReaderValue<vector<double>> oldRhoRho(oldRhoReader, "rho");

    /* read in 2022 information */
    vector<string> newFiles;
    GetFiles(newInput, newFiles);

    TChain newPhotonChain("ggHiNtuplizerGED/EventTree");
    FillChain(newPhotonChain, newFiles);
    TTreeReader newPhotonReader(&newPhotonChain);
    TTreeReaderValue<float> newPhotonRho(newPhotonReader, "rho");

    TChain newRhoChain("hiPuRhoR3Analyzer/t");
    FillChain(newRhoChain, newFiles);
    TTreeReader newRhoReader(&newRhoChain);
    TTreeReaderValue<vector<double>> newRhoEtaMin(newRhoReader, "etaMin");
    TTreeReaderValue<vector<double>> newRhoEtaMax(newRhoReader, "etaMax");
    TTreeReaderValue<vector<double>> newRhoRho(newRhoReader, "rho");

    /* create histograms for energy sum plots */
    int nbins = 80;
    float min = 0;
    float max = 400;

    auto oldPhotonHist = new TH1F("oldPhotonHist", "", nbins, min, max);
    auto newPhotonHist = new TH1F("newPhotonHist", "", nbins, min, max);
    auto oldRhoHist = new TH1F("oldRhoHist", "", nbins, min, max);
    auto newRhoHist = new TH1F("newRhoHist", "", nbins, min, max);

    int oldEntries = oldPhotonChain.GetEntries();
    int newEntries = newPhotonChain.GetEntries();

    /* customize energy sum histogram draw options */
    auto legend = new TLegend(0.55, 0.75 ,0.85, 0.85);
    legend->SetTextSize(0.03);
    legend->AddEntry(oldPhotonHist, "Extra MC", "p");
    legend->AddEntry(newPhotonHist, "Nominal MC", "p");

    FormatHistogram(newPhotonHist, 30);
    FormatHistogram(oldPhotonHist, 46);
    FormatHistogram(newRhoHist, 32);
    FormatHistogram(oldRhoHist, 44);

    /* read in information from TTrees */
    for (int i = 1; i < oldEntries; ++i) {
        oldPhotonReader.Next(); oldRhoReader.Next();

        if (i % (oldEntries / 100) == 0) cout << i << " / " << oldEntries << endl;
            
        oldPhotonHist->Fill(*oldPhotonRho);
        oldRhoHist->Fill(GetAvgRho(oldRhoRho, oldRhoEtaMin, oldRhoEtaMax));
    }

    for (int i = 1; i < newEntries; ++i) {
        newPhotonReader.Next(); newRhoReader.Next();

        if (i % (newEntries / 100) == 0) cout << i << " / " << newEntries << endl;

        newPhotonHist->Fill(*newPhotonRho);
        newRhoHist->Fill(GetAvgRho(newRhoRho, newRhoEtaMin, newRhoEtaMax));
    }

    /* scale the histograms */
    oldPhotonHist->Scale(1.0/oldEntries);
    oldRhoHist->Scale(1.0/oldEntries);

    newPhotonHist->Scale(1.0/newEntries);
    newRhoHist->Scale(1.0/newEntries);

    /* plot the caloTower distributions */
    auto canvas = new TCanvas("canvas", "", 0 , 0, 500, 500);
    canvas->SetLeftMargin(0.15);
    canvas->SetBottomMargin(0.15);
    canvas->Print("MCComparison.pdf[");
    canvas->SetLogy(1);
    canvas->Clear();

    PrintHist(oldPhotonHist, newPhotonHist, "Extra vs Nominal MC Photon Rho", canvas, legend, "MCComparison.pdf");
    PrintHist(oldRhoHist, newRhoHist, "Extra vs Nominal MC Calc Rho", canvas, legend, "MCComparison.pdf");
    PrintHist(oldPhotonHist, newPhotonHist, "Extra MC Photon vs Calc Rho", canvas, legend, "MCComparison.pdf");
    PrintHist(newPhotonHist, newRhoHist, "Nominal MC Photon vs Calc Rho", canvas, legend, "MCComparison.pdf");

    canvas->Print("MCComparison.pdf]");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return Compare(argv[1], argv[2]);

    else {
        cout << "ERROR: Please pass two paths for Extra MC and Nominal MC." << endl;
        return -1;
    }
}
