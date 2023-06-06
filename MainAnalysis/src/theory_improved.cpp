#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "TH1D.h"
#include "TGraph.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TFile.h"
#include "TGaxis.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TColor.h"
#include "TBox.h"
#include "TGraphAsymmErrors.h"

#include "CommandLine.h"

#define R02 0
#define R03 1
#define R04 2
#define R06 3
#define R08 4
#define R10 5

#define C0 0
#define C1 1
#define C2 2
#define C3 3

int Color[5] = {-1, -1, -1, -1, -1};

int main(int argc, char *argv[]);
void SetPad(TPad &P);
void SetAxis(TGaxis &A, bool Pres);
void GetRAA(TH1D &H, string FileName, int R, int C, bool Sys = false);
void Rebin(TH1D &H1, TH1D &H2);
void DrawLumiTAA(double Lumi, double TAA[4], double XMin, double XMax, string type);
vector<TGraphAsymmErrors> GetTheoryGraphs(string TheoryFileName, int BaseColor);

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string TheoryType = CL.Get("TheoryType", ""); // "", "All", "Generators", "MC", "Calculations", "LBT", "None"
   bool DoData = CL.GetBool("Data", true);
   bool Pres = CL.GetBool("Pres", true);
   bool Prelim = CL.GetBool("Prelim", true);
   
   bool Do0to10All = (TheoryType == "All");
   bool Do0to10Gen = (TheoryType == "Generators");      // Jewel, Pyquen
   bool Do0to10MC = (TheoryType == "MC");               // Hybrid, Martini, LBT
   bool Do0to10Calc = (TheoryType == "Calculations");   // Ivan, Felix, Yang-Ting, Yacine
   
   bool Do0to10 = Do0to10All || Do0to10Gen || Do0to10MC || Do0to10Calc;
   bool Do10to30 = (TheoryType == "10to30");            // Ivan, Jewel, Martini
   bool Do30to50 = (TheoryType == "30to50");            // Ivan, Jewel, Martini
   bool Do50to90 = (TheoryType == "50to90");            // Jewel
   
   bool DoTheory = Do0to10 || Do10to30 || Do30to50 || Do50to90;
   bool OnlyData = DoData && !DoTheory;

   // Preamble: sizing
   int PanelSize = 500;
   int PaddingWidthL = Pres ? 140 : 100;
   int PaddingWidthR = 50;
   int PaddingHeight = Pres ? 130 : 100;
   int CanvasWidth = PanelSize * 3 + PaddingWidthL + PaddingWidthR;
   int CanvasHeight = PanelSize * 2 + PaddingHeight * 2;

   double PadX0 = (double)PaddingWidthL / CanvasWidth;
   double PadY0 = (double)PaddingHeight / CanvasHeight;
   double PadDX = (double)PanelSize / CanvasWidth;
   double PadDY = (double)PanelSize / CanvasHeight;

   double XMin = 150;
   double XMax = 1500;
   double YMin = 0.0;
   double YMax = 1.199;

   string File = "RAA_Smooth.root";

   double Lumi = 0.023;
   double TAA[4] = {0.018646, 0.025341, 0.051233, 0.100719};

   // Start declaring canvas, pad, axis, and axis title (as TLatex)
   TCanvas Canvas("Canvas", "", CanvasWidth, CanvasHeight);

   TH2D HWorld("HWorld", ";;", 100, XMin, XMax, 100, YMin, YMax);
   HWorld.SetStats(0);

   const int NPad = 6;
   TPad *P[NPad];
   P[0] = new TPad("P1", "", PadX0 + PadDX * 0, PadY0 + PadDY * 1, PadX0 + PadDX * 1, PadY0 + PadDY * 2, 0);
   P[1] = new TPad("P2", "", PadX0 + PadDX * 1, PadY0 + PadDY * 1, PadX0 + PadDX * 2, PadY0 + PadDY * 2, 0);
   P[2] = new TPad("P3", "", PadX0 + PadDX * 2, PadY0 + PadDY * 1, PadX0 + PadDX * 3, PadY0 + PadDY * 2, 0);
   P[3] = new TPad("P4", "", PadX0 + PadDX * 0, PadY0 + PadDY * 0, PadX0 + PadDX * 1, PadY0 + PadDY * 1, 0);
   P[4] = new TPad("P5", "", PadX0 + PadDX * 1, PadY0 + PadDY * 0, PadX0 + PadDX * 2, PadY0 + PadDY * 1, 0);
   P[5] = new TPad("P6", "", PadX0 + PadDX * 2, PadY0 + PadDY * 0, PadX0 + PadDX * 3, PadY0 + PadDY * 1, 0);
   
   SetPad(*P[0]);
   SetPad(*P[1]);
   SetPad(*P[2]);
   SetPad(*P[3]);
   SetPad(*P[4]);
   SetPad(*P[5]);
  
   Canvas.cd();

   TGaxis AxisX1(PadX0 + PadDX * 0, PadY0 + PadDY * 0, PadX0 + PadDX * 1, PadY0 + PadDY * 0, XMin, XMax, 510, "G");
   TGaxis AxisX2(PadX0 + PadDX * 1, PadY0 + PadDY * 0, PadX0 + PadDX * 2, PadY0 + PadDY * 0, XMin, XMax, 510, "G");
   TGaxis AxisX3(PadX0 + PadDX * 2, PadY0 + PadDY * 0, PadX0 + PadDX * 3, PadY0 + PadDY * 0, XMin, XMax, 510, "G");
   TGaxis AxisY1(PadX0 + PadDX * 0, PadY0 + PadDY * 0, PadX0 + PadDX * 0, PadY0 + PadDY * 1, YMin, YMax, 510, "");
   TGaxis AxisY2(PadX0 + PadDX * 0, PadY0 + PadDY * 1, PadX0 + PadDX * 0, PadY0 + PadDY * 2, YMin, YMax, 510, "");

   SetAxis(AxisX1, Pres);
   SetAxis(AxisX2, Pres);
   SetAxis(AxisX3, Pres);
   SetAxis(AxisY1, Pres);
   SetAxis(AxisY2, Pres);

   TLatex Latex;
   Latex.SetNDC();

   Latex.SetTextFont(42);
   if(Pres) Latex.SetTextSize(0.045);
   else     Latex.SetTextSize(0.030);
   Latex.SetTextAlign(22);
   Latex.SetTextAngle(0);
   Latex.DrawLatex(PadX0 + PadDX * 1.5, PadY0 * 0.40, "p_{T}^{jet} (GeV)");

   Latex.SetTextFont(42);
   if(Pres) Latex.SetTextSize(0.045);
   else     Latex.SetTextSize(0.030);
   Latex.SetTextAlign(22);
   Latex.SetTextAngle(90);
   Latex.DrawLatex(PadX0 * 0.20, PadY0 + PadDY, "R_{AA}");

   Latex.SetTextFont(62);
   if(Pres) Latex.SetTextSize(0.045);
   else     Latex.SetTextSize(0.035);
   Latex.SetTextAlign(11);
   Latex.SetTextAngle(0);
   string TypeTag = "";
   if(DoData)               TypeTag = "CMS";
   if(Prelim)               TypeTag += " #scale[0.8]{#font[52]{Preliminary}}";
   if(DoTheory && !DoData)  TypeTag = "Theory";
   Latex.DrawLatex(PadX0, PadY0 + PadDY * 2 + PadY0 * 0.15, TypeTag.c_str());

   Latex.SetTextFont(42);
   if(Pres) Latex.SetTextSize(0.035);
   else     Latex.SetTextSize(0.030);
   Latex.SetTextAlign(31);
   Latex.SetTextAngle(0);
   if(DoData) Latex.DrawLatex(PadX0 + PadDX * 3, PadY0 + PadDY * 2 + PadY0 * 0.15, "#sqrt{s_{NN}} = 5.02 TeV, PbPb 404 #mub^{-1}, pp 27.4 pb^{-1}");
   if(!DoData) Latex.DrawLatex(PadX0 + PadDX * 3, PadY0 + PadDY * 2 + PadY0 * 0.15, "#sqrt{s_{NN}} = 5.02 TeV");

   TGraph GLine;
   GLine.SetPoint(0, XMin, 1);
   GLine.SetPoint(1, XMax, 1);
   GLine.SetLineStyle(kDashed);

   double Bins[] = {200, 250, 300, 400, 500, 1000};
   const int NRadii = 6;
   TH1D HC00RAA[NRadii];
   TH1D HC01RAA[NRadii];
   TH1D HC02RAA[NRadii];
   TH1D HC03RAA[NRadii];
   HC00RAA[0] = TH1D("HR02C00RAA", "", 5, Bins);   GetRAA(HC00RAA[0], File, R02, C0, false);
   HC01RAA[0] = TH1D("HR02C01RAA", "", 5, Bins);   GetRAA(HC01RAA[0], File, R02, C1, false);
   HC02RAA[0] = TH1D("HR02C02RAA", "", 5, Bins);   GetRAA(HC02RAA[0], File, R02, C2, false);
   HC03RAA[0] = TH1D("HR02C03RAA", "", 5, Bins);   GetRAA(HC03RAA[0], File, R02, C3, false);
   HC00RAA[1] = TH1D("HR03C00RAA", "", 5, Bins);   GetRAA(HC00RAA[1], File, R03, C0, false);
   HC01RAA[1] = TH1D("HR03C01RAA", "", 5, Bins);   GetRAA(HC01RAA[1], File, R03, C1, false);
   HC02RAA[1] = TH1D("HR03C02RAA", "", 5, Bins);   GetRAA(HC02RAA[1], File, R03, C2, false);
   HC03RAA[1] = TH1D("HR03C03RAA", "", 5, Bins);   GetRAA(HC03RAA[1], File, R03, C3, false);
   HC00RAA[2] = TH1D("HR04C00RAA", "", 5, Bins);   GetRAA(HC00RAA[2], File, R04, C0, false);
   HC01RAA[2] = TH1D("HR04C01RAA", "", 5, Bins);   GetRAA(HC01RAA[2], File, R04, C1, false);
   HC02RAA[2] = TH1D("HR04C02RAA", "", 5, Bins);   GetRAA(HC02RAA[2], File, R04, C2, false);
   HC03RAA[2] = TH1D("HR04C03RAA", "", 5, Bins);   GetRAA(HC03RAA[2], File, R04, C3, false);
   HC00RAA[3] = TH1D("HR06C00RAA", "", 5, Bins);   GetRAA(HC00RAA[3], File, R06, C0, false);
   HC01RAA[3] = TH1D("HR06C01RAA", "", 5, Bins);   GetRAA(HC01RAA[3], File, R06, C1, false);
   HC02RAA[3] = TH1D("HR06C02RAA", "", 5, Bins);   GetRAA(HC02RAA[3], File, R06, C2, false);
   HC03RAA[3] = TH1D("HR06C03RAA", "", 5, Bins);   GetRAA(HC03RAA[3], File, R06, C3, false);
   HC00RAA[4] = TH1D("HR08C00RAA", "", 5, Bins);   GetRAA(HC00RAA[4], File, R08, C0, false);
   HC01RAA[4] = TH1D("HR08C01RAA", "", 5, Bins);   GetRAA(HC01RAA[4], File, R08, C1, false);
   HC02RAA[4] = TH1D("HR08C02RAA", "", 5, Bins);   GetRAA(HC02RAA[4], File, R08, C2, false);
   HC03RAA[4] = TH1D("HR08C03RAA", "", 5, Bins);   GetRAA(HC03RAA[4], File, R08, C3, false);
   HC00RAA[5] = TH1D("HR10C00RAA", "", 5, Bins);   GetRAA(HC00RAA[5], File, R10, C0, false);
   HC01RAA[5] = TH1D("HR10C01RAA", "", 5, Bins);   GetRAA(HC01RAA[5], File, R10, C1, false);
   HC02RAA[5] = TH1D("HR10C02RAA", "", 5, Bins);   GetRAA(HC02RAA[5], File, R10, C2, false);
   HC03RAA[5] = TH1D("HR10C03RAA", "", 5, Bins);   GetRAA(HC03RAA[5], File, R10, C3, false);
   
   TH1D HC00RAASys[NRadii];
   TH1D HC01RAASys[NRadii];
   TH1D HC02RAASys[NRadii];
   TH1D HC03RAASys[NRadii];
   HC00RAASys[0] = TH1D("HR02C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[0], File, R02, C0, true);
   HC01RAASys[0] = TH1D("HR02C01RAASys", "", 5, Bins);   GetRAA(HC01RAASys[0], File, R02, C1, true);
   HC02RAASys[0] = TH1D("HR02C02RAASys", "", 5, Bins);   GetRAA(HC02RAASys[0], File, R02, C2, true);
   HC03RAASys[0] = TH1D("HR02C03RAASys", "", 5, Bins);   GetRAA(HC03RAASys[0], File, R02, C3, true);
   HC00RAASys[1] = TH1D("HR03C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[1], File, R03, C0, true);
   HC01RAASys[1] = TH1D("HR03C01RAASys", "", 5, Bins);   GetRAA(HC01RAASys[1], File, R03, C1, true);
   HC02RAASys[1] = TH1D("HR03C02RAASys", "", 5, Bins);   GetRAA(HC02RAASys[1], File, R03, C2, true);
   HC03RAASys[1] = TH1D("HR03C03RAASys", "", 5, Bins);   GetRAA(HC03RAASys[1], File, R03, C3, true);
   HC00RAASys[2] = TH1D("HR04C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[2], File, R04, C0, true);
   HC01RAASys[2] = TH1D("HR04C01RAASys", "", 5, Bins);   GetRAA(HC01RAASys[2], File, R04, C1, true);
   HC02RAASys[2] = TH1D("HR04C02RAASys", "", 5, Bins);   GetRAA(HC02RAASys[2], File, R04, C2, true);
   HC03RAASys[2] = TH1D("HR04C03RAASys", "", 5, Bins);   GetRAA(HC03RAASys[2], File, R04, C3, true);
   HC00RAASys[3] = TH1D("HR06C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[3], File, R06, C0, true);
   HC01RAASys[3] = TH1D("HR06C01RAASys", "", 5, Bins);   GetRAA(HC01RAASys[3], File, R06, C1, true);
   HC02RAASys[3] = TH1D("HR06C02RAASys", "", 5, Bins);   GetRAA(HC02RAASys[3], File, R06, C2, true);
   HC03RAASys[3] = TH1D("HR06C03RAASys", "", 5, Bins);   GetRAA(HC03RAASys[3], File, R06, C3, true);
   HC00RAASys[4] = TH1D("HR08C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[4], File, R08, C0, true);
   HC01RAASys[4] = TH1D("HR08C01RAASys", "", 5, Bins);   GetRAA(HC01RAASys[4], File, R08, C1, true);
   HC02RAASys[4] = TH1D("HR08C02RAASys", "", 5, Bins);   GetRAA(HC02RAASys[4], File, R08, C2, true);
   HC03RAASys[4] = TH1D("HR08C03RAASys", "", 5, Bins);   GetRAA(HC03RAASys[4], File, R08, C3, true);
   HC00RAASys[5] = TH1D("HR10C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[5], File, R10, C0, true);
   HC01RAASys[5] = TH1D("HR10C01RAASys", "", 5, Bins);   GetRAA(HC01RAASys[5], File, R10, C1, true);
   HC02RAASys[5] = TH1D("HR10C02RAASys", "", 5, Bins);   GetRAA(HC02RAASys[5], File, R10, C2, true);
   HC03RAASys[5] = TH1D("HR10C03RAASys", "", 5, Bins);   GetRAA(HC03RAASys[5], File, R10, C3, true);

   vector<TGraphAsymmErrors> Felix = GetTheoryGraphs("Theory/Felix/FelixRAA.txt", kBlack);
   vector<TGraphAsymmErrors> DanielWake = GetTheoryGraphs("Theory/Daniel/ForCMS/RAA_vs_R/Daniel_wake.txt", kOrange + 10);
   vector<TGraphAsymmErrors> DanielOnlyPos = GetTheoryGraphs("Theory/Daniel/ForCMS/RAA_vs_R/Daniel_onlypos.txt", kOrange);
   vector<TGraphAsymmErrors> DanielNoWake = GetTheoryGraphs("Theory/Daniel/ForCMS/RAA_vs_R/Daniel_nowake.txt", kOrange + 4);
   vector<TGraphAsymmErrors> KorinnaC00 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPb_0to10_RAA.txt", kPink + 1);
   vector<TGraphAsymmErrors> KorinnaC01 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPb_10to30_RAA.txt", kPink + 1);
   vector<TGraphAsymmErrors> KorinnaC02 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPb_30to50_RAA.txt", kPink + 1);
   vector<TGraphAsymmErrors> KorinnaC03 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPb_50to90_RAA.txt", kPink + 1);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC00 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPbNoRecoil_0to10_RAA.txt", kMagenta);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC01 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPbNoRecoil_10to30_RAA.txt", kMagenta);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC02 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPbNoRecoil_30to50_RAA.txt", kMagenta);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC03 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPbNoRecoil_50to90_RAA.txt", kMagenta);
   vector<TGraphAsymmErrors> PyquenC00 = GetTheoryGraphs("Theory/Pyquen/PyquenTest_PbPb350_0to10_RAA.txt", kCyan + 3);
   vector<TGraphAsymmErrors> PyquenWideRadC00 = GetTheoryGraphs("Theory/Pyquen/PyquenTest_PbPbWide350_0to10_RAA.txt", kCyan - 6);
   vector<TGraphAsymmErrors> IvanC00 = GetTheoryGraphs("Theory/Ivan/RAA_Centrality-0-10.txt", kBlue); // Only R = 02, 04, 08
   vector<TGraphAsymmErrors> IvanC01 = GetTheoryGraphs("Theory/Ivan/RAA_Centrality-10-30.txt", kBlue); // Only R = 02, 04, 08
   vector<TGraphAsymmErrors> IvanC02 = GetTheoryGraphs("Theory/Ivan/RAA_Centrality-30-50.txt", kBlue); // Only R = 02, 04, 08
   vector<TGraphAsymmErrors> SCET = GetTheoryGraphs("Theory/SCET/SCET_RAA.txt", kAzure + 7); // Only R = 02, 04, 06, 08, 10
   vector<TGraphAsymmErrors> MartiniC00 = GetTheoryGraphs("Theory/Martini/MARTINI_RAA_0to10.txt", kViolet - 3);
   vector<TGraphAsymmErrors> MartiniC01 = GetTheoryGraphs("Theory/Martini/MARTINI_RAA_10to30.txt", kViolet - 3);
   vector<TGraphAsymmErrors> MartiniC02 = GetTheoryGraphs("Theory/Martini/MARTINI_RAA_30to50.txt", kViolet - 3);
   vector<TGraphAsymmErrors> LBTNoResponse = GetTheoryGraphs("Theory/LBT/RAA_showers_only.txt", kSpring - 6); // Only R = 02, 04, 06, 08, 10
   vector<TGraphAsymmErrors> LBTResponse = GetTheoryGraphs("Theory/LBT/RAA_with_recoil_and_back_reaction.txt", kSpring + 5); // Only R = 02, 04, 06, 08, 10
   vector<TGraphAsymmErrors> Yacine = GetTheoryGraphs("Theory/Yacine/RAA.txt", kOrange + 1); // Only R = 02, 04, 06, 08, 10
   vector<TGraphAsymmErrors> CCNUHydro = GetTheoryGraphs("Theory/CCNU/RAA_hydro.txt", kBlue - 7);
   vector<TGraphAsymmErrors> CCNUNoHydro = GetTheoryGraphs("Theory/CCNU/RAA_no_hydro.txt", kViolet + 2);

   // For data-only plot.
   TLegend LegendDataOnly1(0.1, 0.08, 0.5, 0.28);
   LegendDataOnly1.SetTextFont(42);
   LegendDataOnly1.SetTextSize(0.07);
   LegendDataOnly1.SetFillStyle(0);
   LegendDataOnly1.SetBorderSize(0);
   LegendDataOnly1.AddEntry(&HC00RAASys[0], "0-10%", "plf");
   LegendDataOnly1.AddEntry(&HC01RAASys[0], "10-30%", "plf");

   TLegend LegendDataOnly2(0.45, 0.08, 0.85, 0.28);
   LegendDataOnly2.SetTextFont(42);
   LegendDataOnly2.SetTextSize(0.08);
   LegendDataOnly2.SetFillStyle(0);
   LegendDataOnly2.SetBorderSize(0);
   LegendDataOnly2.AddEntry(&HC00RAA[0], "T_{AA}", "f");
   LegendDataOnly2.AddEntry(&HC01RAA[0], "", "f");
   
   TLegend LegendDataOnly3(0.1, 0.08, 0.5, 0.28);
   LegendDataOnly3.SetTextFont(42);
   LegendDataOnly3.SetTextSize(0.07);
   LegendDataOnly3.SetFillStyle(0);
   LegendDataOnly3.SetBorderSize(0);
   LegendDataOnly3.AddEntry(&HC02RAASys[0], "30-50%", "plf");
   LegendDataOnly3.AddEntry(&HC03RAASys[0], "50-90%", "plf");

   TLegend LegendDataOnly4(0.45, 0.08, 0.85, 0.28);
   LegendDataOnly4.SetTextFont(42);
   LegendDataOnly4.SetTextSize(0.07);
   LegendDataOnly4.SetFillStyle(0);
   LegendDataOnly4.SetBorderSize(0);
   LegendDataOnly4.AddEntry(&HC02RAA[0], "T_{AA}", "f");
   LegendDataOnly4.AddEntry(&HC03RAA[0], "", "f");
   
   TH1D HLumi("HLumi", "", 1, 0, 1);
   HLumi.SetFillColor(Color[4]);
   
   TLegend LegendDataOnly5(0.1, 0.08, 0.5, 0.28);
   LegendDataOnly5.SetTextFont(42);
   LegendDataOnly5.SetTextSize(0.07);
   LegendDataOnly5.SetFillStyle(0);
   LegendDataOnly5.SetBorderSize(0);
   LegendDataOnly5.AddEntry(&HLumi, "Lumi", "f");
   LegendDataOnly5.AddEntry("", "", "");

   // For all theory-data plots.
   double LegendStart = Pres ? 0.7 : 0.55;
   TLegend LegendTheoryData(LegendStart, 0.03, LegendStart+0.4, 0.23);
   LegendTheoryData.SetTextFont(42);
   LegendTheoryData.SetTextSize(0.07);
   LegendTheoryData.SetFillStyle(0);
   LegendTheoryData.SetBorderSize(0);
   if(Do0to10)  LegendTheoryData.AddEntry(&HC00RAA[0], "T_{AA}", "f");
   if(Do10to30) LegendTheoryData.AddEntry(&HC01RAA[0], "T_{AA}", "f");
   if(Do30to50) LegendTheoryData.AddEntry(&HC02RAA[0], "T_{AA}", "f");
   if(Do50to90) LegendTheoryData.AddEntry(&HC03RAA[0], "T_{AA}", "f");
   LegendTheoryData.AddEntry(&HLumi, "Lumi", "f");

   // For all theory plots except the grand compilation.
   TLegend LegendTheory1(0.07, 0.03, 0.48, 0.25);
   LegendTheory1.SetTextFont(42);
   LegendTheory1.SetTextSize(0.07);
   LegendTheory1.SetFillStyle(0);
   LegendTheory1.SetBorderSize(0);
   if(Do0to10Calc && DoData)     LegendTheory1.AddEntry("", "", "");
   if(Do0to10Calc && DoData)     LegendTheory1.AddEntry(&HC00RAASys[0], "CMS 0-10%", "plf");
   if(Do0to10Calc && !DoData)    LegendTheory1.AddEntry(&Felix[0], "Factorization", "f");
   if(Do0to10Calc && !DoData)    LegendTheory1.AddEntry(&SCET[0], "SCET w/o coll. E-loss", "f");
   if(Do0to10Calc)               LegendTheory1.AddEntry(&IvanC00[0], "Li and Vitev", "f");
   if(Do0to10MC)                 LegendTheory1.AddEntry("", "", "");
   if(Do0to10MC && !DoData)      LegendTheory1.AddEntry("", "", "");
   if(Do0to10MC && DoData)       LegendTheory1.AddEntry(&HC00RAASys[0], "CMS 0-10%", "plf");
   if(Do0to10MC && DoData)       LegendTheory1.AddEntry(&MartiniC00[0], "MARTINI", "f");
   if(Do0to10Gen && DoData)      LegendTheory1.AddEntry(&HC00RAASys[0], "CMS 0-10%", "plf");
   if(Do0to10Gen)                LegendTheory1.AddEntry(&KorinnaC00[0], "JEWEL", "f");
   if(Do0to10Gen)                LegendTheory1.AddEntry(&KorinnaNoRecoilC00[0], "JEWEL w/o recoil", "f");
   if(Do10to30 && DoData)        LegendTheory1.AddEntry(&HC01RAASys[0], "CMS 10-30%", "plf");
   if(Do10to30 && !DoData)       LegendTheory1.AddEntry("", "", "");
   if(Do10to30)                  LegendTheory1.AddEntry(&KorinnaC01[0], "JEWEL", "f");
   if(Do10to30)                  LegendTheory1.AddEntry(&KorinnaNoRecoilC01[0], "JEWEL w/o recoil", "f");
   if(Do30to50 && DoData)        LegendTheory1.AddEntry(&HC02RAASys[0], "CMS 30-50%", "plf");
   if(Do30to50 && !DoData)       LegendTheory1.AddEntry("", "", "");
   if(Do30to50)                  LegendTheory1.AddEntry(&KorinnaC02[0], "JEWEL", "f");
   if(Do30to50)                  LegendTheory1.AddEntry(&KorinnaNoRecoilC02[0], "JEWEL w/o recoil", "f");
   if(Do50to90 && DoData)        LegendTheory1.AddEntry(&HC03RAASys[0], "CMS 50-90%", "plf");
   if(Do50to90 && !DoData)       LegendTheory1.AddEntry("", "", "");
   if(Do50to90)                  LegendTheory1.AddEntry(&KorinnaC03[0], "JEWEL", "f");
   if(Do50to90)                  LegendTheory1.AddEntry(&KorinnaNoRecoilC03[0], "JEWEL w/o recoil", "f");
   
   TLegend LegendTheory2(0.07, 0.03, 0.48, 0.25);
   LegendTheory2.SetTextFont(42);
   LegendTheory2.SetTextSize(0.07);
   LegendTheory2.SetFillStyle(0);
   LegendTheory2.SetBorderSize(0);
   if(Do0to10Calc)               LegendTheory2.AddEntry(&Yacine[0], "Coherent antenna BDMPS", "lf");
   if(Do0to10Calc && DoData)     LegendTheory2.AddEntry(&Felix[0], "Factorization", "f");
   if(Do0to10Calc && DoData)     LegendTheory2.AddEntry(&SCET[0], "SCET w/o coll. E-loss", "f");
   if(Do0to10Calc && !DoData)    LegendTheory2.AddEntry("", "", "");
   if(Do0to10Calc && !DoData)    LegendTheory2.AddEntry("", "", "");
   if(Do0to10MC)       LegendTheory2.AddEntry(&DanielWake[0], "Hybrid w/ wake", "lf");
   if(Do0to10MC)       LegendTheory2.AddEntry(&DanielNoWake[0], "Hybrid w/o wake", "lf");
   if(Do0to10MC)       LegendTheory2.AddEntry(&DanielOnlyPos[0], "Hybrid w/ pos wake", "lf");
   if(Do0to10Gen)      LegendTheory2.AddEntry("", "", "");
   if(Do0to10Gen)      LegendTheory2.AddEntry(&PyquenC00[0], "PYQUEN", "f");
   if(Do0to10Gen)      LegendTheory2.AddEntry(&PyquenWideRadC00[0], "PYQUEN w/ wide rad.", "f");
   if(Do10to30)        LegendTheory2.AddEntry("", "", "");
   if(Do10to30)        LegendTheory2.AddEntry(&IvanC01[0], "Li and Vitev", "f");
   if(Do10to30)        LegendTheory2.AddEntry(&MartiniC01[0], "MARTINI", "f");
   if(Do30to50)        LegendTheory2.AddEntry("", "", "");
   if(Do30to50)        LegendTheory2.AddEntry(&IvanC02[0], "Li and Vitev", "f");
   if(Do30to50)        LegendTheory2.AddEntry(&MartiniC02[0], "MARTINI", "f");
   
   
   TLegend LegendTheory3(0.07, 0.03, 0.48, 0.25);
   LegendTheory3.SetTextFont(42);
   LegendTheory3.SetTextSize(0.07);
   LegendTheory3.SetFillStyle(0);
   LegendTheory3.SetBorderSize(0);
   LegendTheory3.AddEntry("", "", "");
   LegendTheory3.AddEntry(&LBTNoResponse[0], "LBT w/ showers only", "lf");
   LegendTheory3.AddEntry(&LBTResponse[0], "LBT w/ med. response", "lf");

   TLegend LegendTheory4(0.07, 0.03, 0.48, 0.25);
   LegendTheory4.SetTextFont(42);
   LegendTheory4.SetTextSize(0.06);
   LegendTheory4.SetFillStyle(0);
   LegendTheory4.SetBorderSize(0);
   LegendTheory4.AddEntry("", "", "");
   LegendTheory4.AddEntry(&CCNUHydro[0], "CCNU coupled jet fluid w/ hydro", "pl");
   LegendTheory4.AddEntry(&CCNUNoHydro[0], "CCNU coupled jet fluid w/o hydro", "pl");

   // For the theory grand compilation.
   TLegend LegendTheoryAll1(0.07, 0.03, 0.47, 0.23);
   LegendTheoryAll1.SetTextFont(42);
   LegendTheoryAll1.SetTextSize(0.07);
   LegendTheoryAll1.SetFillStyle(0);
   LegendTheoryAll1.SetBorderSize(0);
   LegendTheoryAll1.AddEntry(&HC00RAA[0], "T_{AA}", "f");
   LegendTheoryAll1.AddEntry(&HLumi, "Lumi", "f");
   
   
   LegendStart = DoData ? 0.35 : 0.07;
   TLegend LegendTheoryAll2(LegendStart, 0.03, LegendStart + 0.41, 0.3);
   LegendTheoryAll2.SetTextFont(42);
   LegendTheoryAll2.SetTextSize(0.06);
   LegendTheoryAll2.SetFillStyle(0);
   LegendTheoryAll2.SetBorderSize(0);
   LegendTheoryAll2.AddEntry(&SCET[0], "SCET w/o coll. E-loss", "f");
   LegendTheoryAll2.AddEntry(&DanielWake[0], "Hybrid w/ wake", "lf");
   LegendTheoryAll2.AddEntry(&DanielNoWake[0], "Hybrid w/o wake", "lf");
   LegendTheoryAll2.AddEntry(&DanielOnlyPos[0], "Hybrid w/ pos wake", "lf");
   
   TLegend LegendTheoryAll3(0.03, 0.03, 0.44, 0.25);
   LegendTheoryAll3.SetTextFont(42);
   LegendTheoryAll3.SetTextSize(0.06);
   LegendTheoryAll3.SetFillStyle(0);
   LegendTheoryAll3.SetBorderSize(0);
   if(DoData)  LegendTheoryAll3.AddEntry(&HC00RAASys[0], "CMS 0-10%", "plf");
   else        LegendTheoryAll3.AddEntry("", "", "");
   LegendTheoryAll3.AddEntry(&KorinnaC00[0], "JEWEL", "f");
   LegendTheoryAll3.AddEntry(&KorinnaNoRecoilC00[0], "JEWEL w/o recoil", "f");
   
   TLegend LegendTheoryAll4(0.55, 0.03, 0.96, 0.25);
   LegendTheoryAll4.SetTextFont(42);
   LegendTheoryAll4.SetTextSize(0.06);
   LegendTheoryAll4.SetFillStyle(0);
   LegendTheoryAll4.SetBorderSize(0);
   LegendTheoryAll4.AddEntry(&MartiniC00[0], "MARTINI", "f");
   LegendTheoryAll4.AddEntry(&IvanC00[0], "Li and Vitev", "f");
   LegendTheoryAll4.AddEntry(&Felix[0], "Factorization", "f");
   
   TLegend LegendTheoryAll5(0.07, 0.03, 0.48, 0.25);
   LegendTheoryAll5.SetTextFont(42);
   LegendTheoryAll5.SetTextSize(0.06);
   LegendTheoryAll5.SetFillStyle(0);
   LegendTheoryAll5.SetBorderSize(0);
   LegendTheoryAll5.AddEntry(&Yacine[0], "Coherent antenna BDMPS", "lf");
   LegendTheoryAll5.AddEntry(&LBTNoResponse[0], "LBT w/ showers only", "lf");
   LegendTheoryAll5.AddEntry(&LBTResponse[0], "LBT w/ med. response", "lf");
   
   TLegend LegendTheoryAll6(0.07, 0.03, 0.48, 0.25);
   LegendTheoryAll6.SetTextFont(42);
   LegendTheoryAll6.SetTextSize(0.06);
   LegendTheoryAll6.SetFillStyle(0);
   LegendTheoryAll6.SetBorderSize(0);
   LegendTheoryAll6.AddEntry("", "", "");
   LegendTheoryAll6.AddEntry(&PyquenC00[0], "PYQUEN", "f");
   LegendTheoryAll6.AddEntry(&PyquenWideRadC00[0], "PYQUEN w/ wide rad.", "f");
   
   TLegend LegendTheoryAll7(0.07, 0.03, 0.48, 0.25);
   LegendTheoryAll7.SetTextFont(42);
   LegendTheoryAll7.SetTextSize(0.06);
   LegendTheoryAll7.SetFillStyle(0);
   LegendTheoryAll7.SetBorderSize(0);
   LegendTheoryAll7.AddEntry("", "", "");
   LegendTheoryAll7.AddEntry("", "", "");
   LegendTheoryAll7.AddEntry(&CCNUNoHydro[0], "CCNU coupled jet fluid w/o hydro", "pl");
   
   TLegend LegendTheoryAll8(0.07, 0.03, 0.48, 0.25);
   LegendTheoryAll8.SetTextFont(42);
   LegendTheoryAll8.SetTextSize(0.06);
   LegendTheoryAll8.SetFillStyle(0);
   LegendTheoryAll8.SetBorderSize(0);
   LegendTheoryAll8.AddEntry("", "", "");
   LegendTheoryAll8.AddEntry("", "", "");
   LegendTheoryAll8.AddEntry(&CCNUHydro[0], "CCNU coupled jet fluid w/ hydro", "pl");
   
   Latex.SetTextFont(42);
   if(Pres) Latex.SetTextSize(0.09);
   else     Latex.SetTextSize(0.07);
   Latex.SetTextAlign(31);
   Latex.SetTextAngle(0);
   
   for(int i = 0; i < NPad; i++)
   {
      LBTNoResponse[i].SetLineWidth(2.0);
      LBTResponse[i].SetLineWidth(2.0);
      Yacine[i].SetLineWidth(2.0);
      DanielWake[i].SetLineWidth(2.0);
      DanielNoWake[i].SetLineWidth(2.0);
      DanielOnlyPos[i].SetLineWidth(2.0);
      CCNUHydro[i].SetLineWidth(2.0);
      CCNUNoHydro[i].SetLineWidth(2.0);
      
      CCNUHydro[i].SetMarkerStyle(21);
      CCNUHydro[i].SetMarkerColor(kBlue - 7);
      CCNUHydro[i].SetMarkerSize(2.2);
      CCNUNoHydro[i].SetMarkerStyle(21);
      CCNUNoHydro[i].SetMarkerColor(kViolet + 2);
      CCNUNoHydro[i].SetMarkerSize(2.2);
      
      P[i]->cd();
      HWorld.Draw("axis");
      if(Do0to10MC || Do0to10All)       DanielWake[i].Draw("3");
      if(Do0to10MC || Do0to10All)       DanielWake[i].Draw("lX");
      if(Do0to10MC || Do0to10All)       DanielNoWake[i].Draw("3");
      if(Do0to10MC || Do0to10All)       DanielNoWake[i].Draw("lX");
      if(Do0to10MC || Do0to10All)       DanielOnlyPos[i].Draw("3");
      if(Do0to10MC || Do0to10All)       DanielOnlyPos[i].Draw("lX");
      if(Do0to10MC || Do0to10All)       CCNUHydro[i].Draw("p");
      if(Do0to10MC || Do0to10All)       CCNUNoHydro[i].Draw("p");
      if(Do0to10MC || Do0to10All)       MartiniC00[i].Draw("5");
      if(Do0to10Calc || Do0to10All)     Felix[i].Draw("5");
      if(Do0to10Gen || Do0to10All)      KorinnaC00[i].Draw("5");
      if(Do0to10Gen || Do0to10All)      KorinnaNoRecoilC00[i].Draw("5");
      if(Do0to10Gen || Do0to10All)      PyquenC00[i].Draw("5");
      if(Do0to10Gen || Do0to10All)      PyquenWideRadC00[i].Draw("5");
      if(Do10to30)   KorinnaC01[i].Draw("5");
      if(Do10to30)   KorinnaNoRecoilC01[i].Draw("5");
      if(Do10to30)   MartiniC01[i].Draw("5");
      if(Do30to50)   KorinnaC02[i].Draw("5");
      if(Do30to50)   KorinnaNoRecoilC02[i].Draw("5");
      if(Do30to50)   MartiniC02[i].Draw("5");
      if(Do50to90)   KorinnaC03[i].Draw("5");
      if(Do50to90)   KorinnaNoRecoilC03[i].Draw("5");
      
      if(i == 0 || i == 2 || i == 4) // Ivan only has R = 2, 4, 8
      {
         if(Do0to10Calc || Do0to10All)     IvanC00[i].Draw("3");
         if(Do10to30)   IvanC01[i].Draw("3");
         if(Do30to50)   IvanC02[i].Draw("3");
      }
      
      if(i != 1) // SCET and LBT do not have R = 3
      {
         if(Do0to10Calc || Do0to10All)     SCET[i].Draw("3");
         if(Do0to10Calc || Do0to10All)     Yacine[i].Draw("lX");
         if(Do0to10MC || Do0to10All)       LBTNoResponse[i].Draw("3");
         if(Do0to10MC || Do0to10All)       LBTNoResponse[i].Draw("lX");
         if(Do0to10MC || Do0to10All)       LBTResponse[i].Draw("3");
         if(Do0to10MC || Do0to10All)       LBTResponse[i].Draw("lX");
      }
      
      if(OnlyData || (DoData && Do0to10))    HC00RAASys[i].Draw("same e2");
      if(OnlyData || (DoData && Do10to30))   HC01RAASys[i].Draw("same e2");
      if(OnlyData || (DoData && Do30to50))   HC02RAASys[i].Draw("same e2");
      if(OnlyData || (DoData && Do50to90))   HC03RAASys[i].Draw("same e2");
      if(OnlyData || (DoData && Do0to10))    HC00RAA[i].Draw("same");
      if(OnlyData || (DoData && Do10to30))   HC01RAA[i].Draw("same");
      if(OnlyData || (DoData && Do30to50))   HC02RAA[i].Draw("same");
      if(OnlyData || (DoData && Do50to90))   HC03RAA[i].Draw("same");
      if(OnlyData)            DrawLumiTAA(Lumi, TAA, XMin, XMax, "All");
      if(Do0to10 && DoData)   DrawLumiTAA(Lumi, TAA, XMin, XMax, "0to10");
      if(Do10to30 && DoData)  DrawLumiTAA(Lumi, TAA, XMin, XMax, "10to30");
      if(Do30to50 && DoData)  DrawLumiTAA(Lumi, TAA, XMin, XMax, "30to50");
      if(Do50to90 && DoData)  DrawLumiTAA(Lumi, TAA, XMin, XMax, "50to90");
      GLine.Draw("l");
   }
   
   P[0]->cd();
   Latex.SetTextAlign(11);
   if(Do0to10All && Pres)
   {
      Latex.SetTextSize(0.07);
      Latex.DrawLatex(0.08, 0.9, "anti-k_{T}, |#eta_{jet}| < 2");
      Latex.SetTextSize(0.09);
   }
   if(Do0to10All && !Pres)  Latex.DrawLatex(0.08, 0.9, "anti-k_{T}, |#eta_{jet}| < 2");
   if(!Do0to10All)          Latex.DrawLatex(0.08, 0.1, "anti-k_{T}, |#eta_{jet}| < 2");
   Latex.SetTextAlign(31);
   if(Pres) Latex.DrawLatex(0.92, 0.88, "R = 0.2");
   else     Latex.DrawLatex(0.92, 0.9, "R = 0.2");
   Latex.SetTextAlign(11);
   if(DoData && DoTheory && !Do0to10All)     LegendTheoryData.Draw();
   if(DoData && Do0to10All)                  LegendTheoryAll1.Draw();
   if(Do0to10All) LegendTheoryAll2.Draw();
      
   P[1]->cd();
   Latex.SetTextAlign(31);
   if(Pres) Latex.DrawLatex(0.92, 0.88, "R = 0.3");
   else     Latex.DrawLatex(0.92, 0.9, "R = 0.3");
   if(DoTheory && !Do0to10All) LegendTheory1.Draw();
   if(Do0to10All) LegendTheoryAll3.Draw();
   if(Do0to10All) LegendTheoryAll4.Draw();
   
   P[2]->cd();
   Latex.SetTextAlign(31);
   if(Pres) Latex.DrawLatex(0.92, 0.88, "R = 0.4");
   else     Latex.DrawLatex(0.92, 0.9, "R = 0.4");
   if(DoTheory && !Do0to10All) LegendTheory2.Draw();
   if(Do0to10All) LegendTheoryAll5.Draw();
   
   P[3]->cd();
   Latex.SetTextAlign(31);
   if(Pres) Latex.DrawLatex(0.92, 0.88, "R = 0.6");
   else     Latex.DrawLatex(0.92, 0.9, "R = 0.6");
   if(OnlyData) LegendDataOnly1.Draw();
   if(OnlyData) LegendDataOnly2.Draw();
   if(Do0to10All) LegendTheoryAll6.Draw();
   if(Do0to10MC)  LegendTheory3.Draw();
   
   P[4]->cd();
   Latex.SetTextAlign(31);
   if(Pres) Latex.DrawLatex(0.92, 0.88, "R = 0.8");
   else     Latex.DrawLatex(0.92, 0.9, "R = 0.8");
   if(OnlyData) LegendDataOnly3.Draw();
   if(OnlyData) LegendDataOnly4.Draw();
   if(Do0to10All) LegendTheoryAll7.Draw();
   
   Latex.SetTextFont(62);
   Latex.SetTextAlign(21);
   if(Pres) Latex.SetTextSize(0.10);
   else     Latex.SetTextSize(0.08);
   if(Do0to10 && !Do0to10All)   Latex.DrawLatex(0.5, 0.08, "0-10%");
   if(Do10to30)  Latex.DrawLatex(0.5, 0.08, "10-30%");
   if(Do30to50)  Latex.DrawLatex(0.5, 0.08, "30-50%");
   if(Do50to90)  Latex.DrawLatex(0.5, 0.08, "50-90%");
   Latex.SetTextFont(42);
   if(Pres) Latex.SetTextSize(0.09);
   else     Latex.SetTextSize(0.07);
   Latex.SetTextAlign(31);
   
   P[5]->cd();
   Latex.SetTextAlign(31);
   if(Pres) Latex.DrawLatex(0.92, 0.88, "R = 1.0");
   else     Latex.DrawLatex(0.92, 0.9, "R = 1.0");
   if(OnlyData)   LegendDataOnly5.Draw();
   if(Do0to10MC)  LegendTheory4.Draw();
   if(Do0to10All) LegendTheoryAll8.Draw();

   string OutputBase = "";
   if(Pres)          OutputBase += "Pres_";
   else              OutputBase += "Paper_";
   if(Prelim)        OutputBase += "Prelim_";
   if(DoData)        OutputBase += "Data";
   if(Do0to10All)    OutputBase += "Theory0to10";
   if(Do0to10MC)     OutputBase += "MC0to10";
   if(Do0to10Calc)   OutputBase += "Calc0to10";
   if(Do0to10Gen)    OutputBase += "Gen0to10";
   if(Do10to30)      OutputBase += "Theory10to30";
   if(Do30to50)      OutputBase += "Theory30to50";
   if(Do50to90)      OutputBase += "Theory50to90";
   OutputBase += "RAA";

   Canvas.SaveAs((OutputBase + ".pdf").c_str());
//   Canvas.SaveAs((OutputBase + ".png").c_str());
//   Canvas.SaveAs((OutputBase + ".C").c_str());
   
   for(int i = 0; i < NPad; i++)
   {
      delete P[i];
   }

   return 0;
}

void SetPad(TPad &P)
{
   P.SetLogx();
   P.SetLeftMargin(0);
   P.SetTopMargin(0);
   P.SetRightMargin(0);
   P.SetBottomMargin(0);
   P.SetTickx();
   P.SetTicky();
   P.Draw();
}

void SetAxis(TGaxis &A, bool Pres)
{
   A.SetLabelFont(42);
   if(Pres) A.SetLabelSize(0.040);
   else     A.SetLabelSize(0.030);
   A.SetMaxDigits(6);
   A.SetMoreLogLabels();
   A.SetNoExponent();
   A.Draw();
}

void GetRAA(TH1D &H, string FileName, int R, int C, bool Sys)
{
   TFile File(FileName.c_str());

   string Rs[] = {"2", "3", "4", "6", "8", "10"};
   string Cs[] = {"0to10", "10to30", "30to50", "50to90"};

   string HistogramName = "";
   
   HistogramName = HistogramName + "akCs" + Rs[R] + "PU3PFFlowJetAnalyzer";
   HistogramName = HistogramName + "_Cent" + Cs[C];
   if(Sys == false)   HistogramName = HistogramName + "_1";
   else               HistogramName = HistogramName + "_2";

   TH1D *Histogram = (TH1D *)File.Get(HistogramName.c_str());
   Rebin(*Histogram, H);

   int Style[] = {20, 20, 20, 20};

   if(Color[C] == -1)
   {
      Color[0] = TColor::GetFreeColorIndex();
      new TColor(Color[0], 0.9648, 0.2969, 0.2344);   // E74C3C
//      new TColor(Color[0], 1, 0, 0);
      Color[1] = TColor::GetFreeColorIndex();
      new TColor(Color[1], 0.2031, 0.5938, 0.8555);   // 3498DB
      Color[2] = TColor::GetFreeColorIndex();
      new TColor(Color[2], 0.8164, 0.6406, 0.0391);   // #D1A40A
      Color[3] = TColor::GetFreeColorIndex();
      new TColor(Color[3], 0.1172, 0.6719, 0.3789);   // #1EAC61
      Color[4] = TColor::GetFreeColorIndex();
      new TColor(Color[4], 0.5, 0.5, 0.5);
   }

   H.SetMarkerStyle(Style[C]);
   H.SetMarkerColor(Color[C]);
   H.SetLineColor(Color[C]);
   H.SetFillColor(Color[C]);
   if(Sys == true)
      H.SetFillColorAlpha(Color[C], 0.25);
//   H.SetMarkerSize(1.8);
   H.SetMarkerSize(2.2);
   H.SetLineWidth(2.0);

   File.Close();
}

void Rebin(TH1D &H1, TH1D &H2)
{
   H2.Reset();

   for(int i = 1; i <= H1.GetNbinsX(); i++)
   {
      double V = H1.GetBinContent(i);
      double E = H1.GetBinError(i);

      int B = H2.FindBin(H1.GetBinCenter(i));

      V = V + H2.GetBinContent(B);
      E = sqrt(E * E + H2.GetBinError(B) * H2.GetBinError(B));

      H2.SetBinContent(B, V);
      H2.SetBinError(B, E);
   }
}

void DrawLumiTAA(double Lumi, double TAA[4], double XMin, double XMax, string type)
{
   double Offset = 0.06;
   double Size = 0.08;
   double Grid = 0.10;

   TBox Box;

   double X1, X2;
   double x1, x2;

   Box.SetFillColor(Color[4]);
   x1 = Offset - Size * 0.5 + Grid * 0;
   x2 = Offset + Size * 0.5 + Grid * 0;
   X1 = exp(x1 * (log(XMax) - log(XMin)) + log(XMin));
   X2 = exp(x2 * (log(XMax) - log(XMin)) + log(XMin));
   Box.DrawBox(X1, 1 - Lumi, X2, 1 + Lumi);

   if(type == "All") for(int i = 0; i < 4; i++)
   {
      if(TAA[i] < 0)
         continue;

      Box.SetFillColor(Color[i]);
      x1 = Offset - Size * 0.5 + Grid * (i + 1);
      x2 = Offset + Size * 0.5 + Grid * (i + 1);
      X1 = exp(x1 * (log(XMax) - log(XMin)) + log(XMin));
      X2 = exp(x2 * (log(XMax) - log(XMin)) + log(XMin));
      Box.DrawBox(X1, 1 - TAA[i], X2, 1 + TAA[i]);
   }
   else
   {
      int index;
      
      if(type == "0to10")  index = 0;
      if(type == "10to30") index = 1;
      if(type == "30to50") index = 2;
      if(type == "50to90") index = 3;
      
      Box.SetFillColor(Color[index]);
      x1 = Offset - Size * 0.5 + Grid;
      x2 = Offset + Size * 0.5 + Grid;
      X1 = exp(x1 * (log(XMax) - log(XMin)) + log(XMin));
      X2 = exp(x2 * (log(XMax) - log(XMin)) + log(XMin));
      Box.DrawBox(X1, 1 - TAA[index], X2, 1 + TAA[index]);
   }
}

vector<TGraphAsymmErrors> GetTheoryGraphs(string TheoryFileName, int BaseColor)
{
   vector<TGraphAsymmErrors> Result(6);   // 0.2, 0.3, 0.4, 0.6, 0.8, 1.0

   for(int i = 0; i < 6; i++)
   {
      Result[i].SetLineColor(BaseColor);
      Result[i].SetFillColorAlpha(BaseColor, 0.25);
      Result[i].SetMarkerStyle(20);
      Result[i].SetMarkerColor(BaseColor);
   }

   ifstream in(TheoryFileName);

   int Count = 0;

   while(in)
   {
      char ch[10239];
      ch[0] = '\0';
      in.getline(ch, 10239, '\n');
      if(ch[0] == '\0')
         continue;

      stringstream str(ch);

      double PTMin = 0, PTMax = 0;
      str >> PTMin >> PTMax;

      if(PTMin < 1 || PTMax < 1)
         continue;

      double X = (PTMin + PTMax) / 2;
      double DX = fabs(PTMax - PTMin) / 2;

      for(int i = 0; i < 6; i++)
      {
         double R, RMin, RMax;
         str >> R >> RMax >> RMin;
         Result[i].SetPoint(Count, X, R);
         Result[i].SetPointError(Count, DX, DX, R - RMin, RMax - R);
      }

      Count = Count + 1;
   }

   in.close();

   return Result;
}
