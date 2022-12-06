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
    TTreeReaderValue<int> trackEtaMin(trackReader, "nVtx");
    // TTreeReaderValue<vector<double>> trackEtaMax(trackReader, "etaMax");
    // TTreeReaderValue<vector<double>> tracktrack(trackReader, "track");

    TChain evtChain("hiEvtAnalyzer/HiTree");
    FillChain(evtChain, files);
    // evtChain.Add(input);
    TTreeReader evtReader(&evtChain);
    TTreeReaderValue<float> evtNcoll(evtReader, "Ncoll");

    /* create histograms for energy sum plots */
    int nbins = 80;
    float min = 0;
    float max = 300;

    auto trackHist = new TH1F("trackHist", "", nbins, min, max);
    auto trackNcollHist = new TH2F("trackNcollHist", "", nbins, min, max, nbins, 0, 2100);

    int entries = trackChain.GetEntries();

    /* customize energy sum histogram draw options */
    auto legend = new TLegend(0.55, 0.75 ,0.85, 0.85);
    legend->SetTextSize(0.03);
    legend->AddEntry(trackHist, "Extra MC Photon track", "p");

    FormatHistogram(trackHist, 46);
    FormatHistogram2D(trackNcollHist);

    /* read in information from TTrees */
    for (int i = 1; i < entries; ++i) {
        trackReader.Next(); evtReader.Next();

        if (i % (entries / 100) == 0) cout << i << " / " << entries << endl;
            
        auto avg_track = GetAvgtrack(tracktrack.Get(), trackEtaMin.Get(), trackEtaMax.Get());

        trackHist->Fill(avg_track);
        trackNcollHist->Fill(avg_track, *evtNcoll);
    }

    /* scale the histograms */
    trackHist->Scale(1.0/entries);
    trackNcollHist->Scale(1.0/entries);

    string filename = "MCComparison_" + to_string(pthat) + ".pdf";

    auto canvas = new TCanvas("canvas", "", 0 , 0, 500, 500);
    canvas->SetLeftMargin(0.15);
    canvas->SetBottomMargin(0.15);
    canvas->Print(string(filename + "[").c_str());
    canvas->SetLogz(1);
    canvas->Clear();

    PrintHist(photonHist, newPhotonHist, "Extra vs Nominal MC Photon track", canvas, legend, filename);
    PrintHist2D(photonNcollHist, "Extra MC Photon track vs Ncoll", canvas, filename);

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
