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

using namespace std;

void FormatHistogram(TH1* hist, int color) {
    hist->SetMarkerColor(color);
    hist->SetLineColor(color);
    hist->SetMarkerSize(1.0);
    hist->SetMarkerStyle(20);
    // hist->SetStats(0);
    hist->GetXaxis()->CenterTitle(true);
    hist->GetYaxis()->CenterTitle(true);
}

void FormatHistogram2D(TH2* hist) {
    hist->GetXaxis()->CenterTitle(true);
    hist->GetYaxis()->CenterTitle(true);
    hist->SetStats(0);
}

void PrintHist1D(TH1* hist, string xlabel, string ylabel, TCanvas* canvas, TLegend* legend, string filename) {
    hist->GetXaxis()->SetTitle(xlabel.c_str());
    hist->GetXaxis()->SetTitle(ylabel.c_str());
    // hist->Draw("HIST LP");
    hist->Draw();
    legend->Draw();
    canvas->Print(filename.c_str());
}

void PrintHist2D(TH2* hist, string xlabel, string ylabel, TCanvas* canvas, string filename) {
    hist->GetXaxis()->SetTitle(xlabel.c_str());
    hist->GetXaxis()->SetTitle(ylabel.c_str());
    hist->Draw("COLZ");
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

int Compare(char const* input, int pthat) {
    /* read in 2018 information */
    vector<string> files;
    GetFiles(input, files);

    // nPho phoEt phoSCEta phoHoverE phoSigmaIEtaIEta_2012 pho_ecalClusterIsoR3 pho_hcalRechitIsoR3 pho_trackIsoR3PtCut20 phoEta phoPhi
    // pfPt pfPt nVtx weight

    TChain trackChain("ppTrack/trackTree");
    FillChain(trackChain, files);
    // trackChain.Add(input);
    TTreeReader trackReader(&trackChain);
    TTreeReaderValue<int> trackNVtx(trackReader, "nVtx");

    TChain evtChain("hiEvtAnalyzer/HiTree");
    FillChain(evtChain, files);
    // evtChain.Add(input);
    TTreeReader evtReader(&evtChain);
    TTreeReaderValue<vector<int>> evtNPU(evtReader, "npus");

    /* create histograms for energy sum plots */

    // auto trackHist = new TH1F("trackHist", "", nbins, min, max);
    auto histVtxPU2D = new TH2F("histVtxPU", ";nVtx;nPU", 14, 0, 14, 14, 0, 14);
    auto histVtxPU1D = new TProfile("histVtxPU1D", ";nVtx;nPU", 11, -0.5, 10.5, 0, 16, "PE");

    int entries = trackChain.GetEntries();

    /* customize energy sum histogram draw options */
    auto legend = new TLegend(0.15, 0.75 ,0.45, 0.85);
    legend->SetTextSize(0.03);
    legend->AddEntry(histVtxPU1D, "PP PU vs nVtx", "p");

    FormatHistogram2D(histVtxPU2D);
    FormatHistogram(histVtxPU1D, 30);

    /* read in information from TTrees */
    for (int i = 1; i < entries; ++i) {
        trackReader.Next(); evtReader.Next();

        if (i % (entries / 200) == 0) cout << i << " / " << entries << endl;

        histVtxPU2D->Fill(*trackNVtx, (*evtNPU)[5]);
        histVtxPU1D->Fill(*trackNVtx, (*evtNPU)[5]);
    }

    /* fit the histograms */
    histVtxPU1D->Fit("pol1");
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1);

    /* plot the histograms */
    string filename = "NVtx_vs_NPU_" + to_string(pthat) + ".pdf";

    auto canvas = new TCanvas("canvas", "", 0 , 0, 500, 500);
    canvas->SetLeftMargin(0.15);
    canvas->SetBottomMargin(0.15);
    canvas->Print(string(filename + "[").c_str());
    // canvas->SetLogz(1);
    canvas->Clear();

    // PrintHist(photonHist, newPhotonHist, "Extra vs Nominal MC Photon track", canvas, legend, filename);
    PrintHist2D(histVtxPU2D, "nVtx", "nPU", canvas, filename);
    PrintHist1D(histVtxPU1D, "nVtx", "nPU", canvas, legend, filename);

    canvas->Print(string(filename + "]").c_str());

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return Compare(argv[1], stoi(argv[2]));

    else {
        cout << "ERROR: Please pass two paths for Extra MC and Nominal MC." << endl;
        return -1;
    }
}
