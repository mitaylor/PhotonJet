void pho_60_rebin3_final_theory_spectra_pp_whole_log()
{
//=========Macro generated from canvas: canvas/
//=========  (Wed Jun 26 14:09:36 2024) by ROOT version 6.30/02
   TCanvas *canvas = new TCanvas("canvas", "",0,0,720,640);
   canvas->SetHighLightColor(2);
   canvas->Range(0,0,1,1);
   canvas->SetFillColor(0);
   canvas->SetBorderMode(0);
   canvas->SetBorderSize(2);
   canvas->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: P1
   TPad *P1__0 = new TPad("P1", "",0.2361111,0.109375,0.9305556,0.890625);
   P1__0->Draw();
   P1__0->cd();
   P1__0->Range(-2.522879,-2.1,-0.5228787,21);
   P1__0->SetFillColor(0);
   P1__0->SetBorderMode(0);
   P1__0->SetBorderSize(2);
   P1__0->SetLogx();
   P1__0->SetTickx(1);
   P1__0->SetTicky(1);
   P1__0->SetLeftMargin(0);
   P1__0->SetRightMargin(0);
   P1__0->SetTopMargin(0);
   P1__0->SetBottomMargin(0);
   P1__0->SetFrameBorderMode(0);
   P1__0->SetFrameBorderMode(0);
   
   TH2F *world__1 = new TH2F("world__1","",100,0.003,0.3,100,-2.1,21);
   world__1->SetDirectory(nullptr);
   world__1->SetStats(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   world__1->SetLineColor(ci);
   world__1->GetXaxis()->SetLabelFont(42);
   world__1->GetXaxis()->SetTitleOffset(1);
   world__1->GetXaxis()->SetTitleFont(42);
   world__1->GetYaxis()->SetLabelFont(42);
   world__1->GetYaxis()->SetTitleFont(42);
   world__1->GetZaxis()->SetLabelFont(42);
   world__1->GetZaxis()->SetTitleOffset(1);
   world__1->GetZaxis()->SetTitleFont(42);
   world__1->Draw("axis");
   
   Double_t Graph_fx3001[11] = { 0.005, 0.013, 0.02, 0.028, 0.036, 0.045, 0.055, 0.07, 0.09, 0.125, 0.225 };
   Double_t Graph_fy3001[11] = { 6.308031, 12.67764, 13.54843, 12.64567, 10.45005, 7.920866, 6.111764, 3.948399, 2.207411, 0.9603692, 0.04999755 };
   Double_t Graph_felx3001[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fely3001[11] = { 0.3384338, 0.547917, 0.4765262, 0.4706244, 0.4068787, 0.3696475, 0.3634745, 0.2716067, 0.1989742, 0.08603976, 0.01128795 };
   Double_t Graph_fehx3001[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fehy3001[11] = { 0.3384338, 0.547917, 0.4765262, 0.4706244, 0.4068787, 0.3696475, 0.3634745, 0.2716067, 0.1989742, 0.08603976, 0.01128795 };
   TGraphAsymmErrors *grae = new TGraphAsymmErrors(11,Graph_fx3001,Graph_fy3001,Graph_felx3001,Graph_fehx3001,Graph_fely3001,Graph_fehy3001);
   grae->SetName("");
   grae->SetTitle("");

   ci = 1181;
   color = new TColor(ci, 0.6, 0.6, 0.6, " ", 0.6);
   grae->SetFillColor(ci);
   grae->SetFillStyle(1000);
   grae->SetMarkerStyle(20);
   grae->SetMarkerSize(1.5);
   
   TH1F *Graph_Graph3001 = new TH1F("Graph_Graph3001","",100,0.0015,0.3298333);
   Graph_Graph3001->SetMinimum(0.03483864);
   Graph_Graph3001->SetMaximum(15.42358);
   Graph_Graph3001->SetDirectory(nullptr);
   Graph_Graph3001->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_Graph3001->SetLineColor(ci);
   Graph_Graph3001->GetXaxis()->SetLabelFont(42);
   Graph_Graph3001->GetXaxis()->SetTitleOffset(1);
   Graph_Graph3001->GetXaxis()->SetTitleFont(42);
   Graph_Graph3001->GetYaxis()->SetLabelFont(42);
   Graph_Graph3001->GetYaxis()->SetTitleFont(42);
   Graph_Graph3001->GetZaxis()->SetLabelFont(42);
   Graph_Graph3001->GetZaxis()->SetTitleOffset(1);
   Graph_Graph3001->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph3001);
   
   grae->Draw(" 2");
   
   Double_t Graph_fx3002[11] = { 0.005, 0.013, 0.02, 0.028, 0.036, 0.045, 0.055, 0.07, 0.09, 0.125, 0.225 };
   Double_t Graph_fy3002[11] = { 9.34401, 16.24672, 15.40807, 12.4146, 10.02718, 6.758245, 4.855654, 2.971425, 1.67487, 0.5950194, 0.02742476 };
   Double_t Graph_felx3002[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fely3002[11] = { 0.261993, 0.4459953, 0.3761423, 0.3376327, 0.3034364, 0.2228127, 0.188863, 0.1044697, 0.07843295, 0.02956675, 0.003664788 };
   Double_t Graph_fehx3002[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fehy3002[11] = { 0.261993, 0.4459953, 0.3761423, 0.3376327, 0.3034364, 0.2228127, 0.188863, 0.1044697, 0.07843295, 0.02956675, 0.003664788 };
   grae = new TGraphAsymmErrors(11,Graph_fx3002,Graph_fy3002,Graph_felx3002,Graph_fehx3002,Graph_fely3002,Graph_fehy3002);
   grae->SetName("");
   grae->SetTitle("");

   ci = 1193;
   color = new TColor(ci, 0.8980392, 0.3882353, 0.1137255, " ", 0.6);
   grae->SetFillColor(ci);
   grae->SetFillStyle(1000);

   ci = TColor::GetColor("#e5631d");
   grae->SetLineColor(ci);
   grae->SetLineWidth(5);

   ci = TColor::GetColor("#e5631d");
   grae->SetMarkerColor(ci);
   grae->SetMarkerStyle(20);
   grae->SetMarkerSize(0);
   
   TH1F *Graph_Graph3002 = new TH1F("Graph_Graph3002","",100,0.0015,0.3298333);
   Graph_Graph3002->SetMinimum(0.02138398);
   Graph_Graph3002->SetMaximum(18.35961);
   Graph_Graph3002->SetDirectory(nullptr);
   Graph_Graph3002->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_Graph3002->SetLineColor(ci);
   Graph_Graph3002->GetXaxis()->SetLabelFont(42);
   Graph_Graph3002->GetXaxis()->SetTitleOffset(1);
   Graph_Graph3002->GetXaxis()->SetTitleFont(42);
   Graph_Graph3002->GetYaxis()->SetLabelFont(42);
   Graph_Graph3002->GetYaxis()->SetTitleFont(42);
   Graph_Graph3002->GetZaxis()->SetLabelFont(42);
   Graph_Graph3002->GetZaxis()->SetTitleOffset(1);
   Graph_Graph3002->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph3002);
   
   grae->Draw(" 3");
   
   Double_t Graph_fx3003[11] = { 0.005, 0.013, 0.02, 0.028, 0.036, 0.045, 0.055, 0.07, 0.09, 0.125, 0.225 };
   Double_t Graph_fy3003[11] = { 9.34401, 16.24672, 15.40807, 12.4146, 10.02718, 6.758245, 4.855654, 2.971425, 1.67487, 0.5950194, 0.02742476 };
   Double_t Graph_felx3003[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fely3003[11] = { 0.261993, 0.4459953, 0.3761423, 0.3376327, 0.3034364, 0.2228127, 0.188863, 0.1044697, 0.07843295, 0.02956675, 0.003664788 };
   Double_t Graph_fehx3003[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fehy3003[11] = { 0.261993, 0.4459953, 0.3761423, 0.3376327, 0.3034364, 0.2228127, 0.188863, 0.1044697, 0.07843295, 0.02956675, 0.003664788 };
   grae = new TGraphAsymmErrors(11,Graph_fx3003,Graph_fy3003,Graph_felx3003,Graph_fehx3003,Graph_fely3003,Graph_fehy3003);
   grae->SetName("");
   grae->SetTitle("");

   ci = 1193;
   color = new TColor(ci, 0.8980392, 0.3882353, 0.1137255, " ", 0.6);
   grae->SetFillColor(ci);
   grae->SetFillStyle(1000);

   ci = TColor::GetColor("#e5631d");
   grae->SetLineColor(ci);
   grae->SetLineWidth(5);

   ci = TColor::GetColor("#e5631d");
   grae->SetMarkerColor(ci);
   grae->SetMarkerStyle(20);
   grae->SetMarkerSize(0);
   
   TH1F *Graph_Graph3003 = new TH1F("Graph_Graph3003","",100,0.0015,0.3298333);
   Graph_Graph3003->SetMinimum(0.02138398);
   Graph_Graph3003->SetMaximum(18.35961);
   Graph_Graph3003->SetDirectory(nullptr);
   Graph_Graph3003->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_Graph3003->SetLineColor(ci);
   Graph_Graph3003->GetXaxis()->SetLabelFont(42);
   Graph_Graph3003->GetXaxis()->SetTitleOffset(1);
   Graph_Graph3003->GetXaxis()->SetTitleFont(42);
   Graph_Graph3003->GetYaxis()->SetLabelFont(42);
   Graph_Graph3003->GetYaxis()->SetTitleFont(42);
   Graph_Graph3003->GetZaxis()->SetLabelFont(42);
   Graph_Graph3003->GetZaxis()->SetTitleOffset(1);
   Graph_Graph3003->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph3003);
   
   grae->Draw(" lx");
   
   Double_t Graph_fx3004[11] = { 0.005, 0.013, 0.02, 0.028, 0.036, 0.045, 0.055, 0.07, 0.09, 0.125, 0.225 };
   Double_t Graph_fy3004[11] = { 7.800751, 14.67467, 14.50032, 12.13094, 9.45764, 6.911878, 4.956095, 3.023457, 1.602847, 0.5561085, 0.03062701 };
   Double_t Graph_felx3004[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fely3004[11] = { 0.06597802, 0.117554, 0.1032489, 0.09630769, 0.0865866, 0.06664823, 0.05805708, 0.03177147, 0.0228047, 0.008603673, 0.001175645 };
   Double_t Graph_fehx3004[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fehy3004[11] = { 0.06597802, 0.117554, 0.1032489, 0.09630769, 0.0865866, 0.06664823, 0.05805708, 0.03177147, 0.0228047, 0.008603673, 0.001175645 };
   grae = new TGraphAsymmErrors(11,Graph_fx3004,Graph_fy3004,Graph_felx3004,Graph_fehx3004,Graph_fely3004,Graph_fehy3004);
   grae->SetName("");
   grae->SetTitle("");

   ci = 1199;
   color = new TColor(ci, 0.1333333, 0.5294118, 0.172549, " ", 0.6);
   grae->SetFillColor(ci);
   grae->SetFillStyle(1000);

   ci = TColor::GetColor("#22872c");
   grae->SetLineColor(ci);
   grae->SetLineWidth(5);

   ci = TColor::GetColor("#22872c");
   grae->SetMarkerColor(ci);
   grae->SetMarkerStyle(20);
   grae->SetMarkerSize(0);
   
   TH1F *Graph_Graph3004 = new TH1F("Graph_Graph3004","",100,0.0015,0.3298333);
   Graph_Graph3004->SetMinimum(0.02650623);
   Graph_Graph3004->SetMaximum(16.26851);
   Graph_Graph3004->SetDirectory(nullptr);
   Graph_Graph3004->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_Graph3004->SetLineColor(ci);
   Graph_Graph3004->GetXaxis()->SetLabelFont(42);
   Graph_Graph3004->GetXaxis()->SetTitleOffset(1);
   Graph_Graph3004->GetXaxis()->SetTitleFont(42);
   Graph_Graph3004->GetYaxis()->SetLabelFont(42);
   Graph_Graph3004->GetYaxis()->SetTitleFont(42);
   Graph_Graph3004->GetZaxis()->SetLabelFont(42);
   Graph_Graph3004->GetZaxis()->SetTitleOffset(1);
   Graph_Graph3004->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph3004);
   
   grae->Draw(" 3");
   
   Double_t Graph_fx3005[11] = { 0.005, 0.013, 0.02, 0.028, 0.036, 0.045, 0.055, 0.07, 0.09, 0.125, 0.225 };
   Double_t Graph_fy3005[11] = { 7.800751, 14.67467, 14.50032, 12.13094, 9.45764, 6.911878, 4.956095, 3.023457, 1.602847, 0.5561085, 0.03062701 };
   Double_t Graph_felx3005[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fely3005[11] = { 0.06597802, 0.117554, 0.1032489, 0.09630769, 0.0865866, 0.06664823, 0.05805708, 0.03177147, 0.0228047, 0.008603673, 0.001175645 };
   Double_t Graph_fehx3005[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fehy3005[11] = { 0.06597802, 0.117554, 0.1032489, 0.09630769, 0.0865866, 0.06664823, 0.05805708, 0.03177147, 0.0228047, 0.008603673, 0.001175645 };
   grae = new TGraphAsymmErrors(11,Graph_fx3005,Graph_fy3005,Graph_felx3005,Graph_fehx3005,Graph_fely3005,Graph_fehy3005);
   grae->SetName("");
   grae->SetTitle("");

   ci = 1199;
   color = new TColor(ci, 0.1333333, 0.5294118, 0.172549, " ", 0.6);
   grae->SetFillColor(ci);
   grae->SetFillStyle(1000);

   ci = TColor::GetColor("#22872c");
   grae->SetLineColor(ci);
   grae->SetLineWidth(5);

   ci = TColor::GetColor("#22872c");
   grae->SetMarkerColor(ci);
   grae->SetMarkerStyle(20);
   grae->SetMarkerSize(0);
   
   TH1F *Graph_Graph3005 = new TH1F("Graph_Graph3005","",100,0.0015,0.3298333);
   Graph_Graph3005->SetMinimum(0.02650623);
   Graph_Graph3005->SetMaximum(16.26851);
   Graph_Graph3005->SetDirectory(nullptr);
   Graph_Graph3005->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_Graph3005->SetLineColor(ci);
   Graph_Graph3005->GetXaxis()->SetLabelFont(42);
   Graph_Graph3005->GetXaxis()->SetTitleOffset(1);
   Graph_Graph3005->GetXaxis()->SetTitleFont(42);
   Graph_Graph3005->GetYaxis()->SetLabelFont(42);
   Graph_Graph3005->GetYaxis()->SetTitleFont(42);
   Graph_Graph3005->GetZaxis()->SetLabelFont(42);
   Graph_Graph3005->GetZaxis()->SetTitleOffset(1);
   Graph_Graph3005->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph3005);
   
   grae->Draw(" lx");
   
   Double_t Graph_fx3006[11] = { 0.005, 0.013, 0.02, 0.028, 0.036, 0.045, 0.055, 0.07, 0.09, 0.125, 0.225 };
   Double_t Graph_fy3006[11] = { 7.69566, 13.6858, 13.6195, 11.0991, 8.93109, 6.54427, 4.61141, 2.93044, 1.49278, 0.535996, 0.0286431 };
   Double_t Graph_felx3006[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fely3006[11] = { 0.142076, 0.250678, 0.225677, 0.201418, 0.188868, 0.140593, 0.119363, 0.0673829, 0.0486903, 0.0185518, 0.00240898 };
   Double_t Graph_fehx3006[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fehy3006[11] = { 0.142076, 0.250678, 0.225677, 0.201418, 0.188868, 0.140593, 0.119363, 0.0673829, 0.0486903, 0.0185518, 0.00240898 };
   grae = new TGraphAsymmErrors(11,Graph_fx3006,Graph_fy3006,Graph_felx3006,Graph_fehx3006,Graph_fely3006,Graph_fehy3006);
   grae->SetName("");
   grae->SetTitle("");

   ci = 1195;
   color = new TColor(ci, 0.9921569, 0.6588235, 0.1764706, " ", 0.6);
   grae->SetFillColor(ci);
   grae->SetFillStyle(1000);

   ci = TColor::GetColor("#fda82d");
   grae->SetLineColor(ci);
   grae->SetLineWidth(5);

   ci = TColor::GetColor("#fda82d");
   grae->SetMarkerColor(ci);
   grae->SetMarkerStyle(20);
   grae->SetMarkerSize(0);
   
   TH1F *Graph_Graph3006 = new TH1F("Graph_Graph3006","",100,0.0015,0.3298333);
   Graph_Graph3006->SetMinimum(0.02361071);
   Graph_Graph3006->SetMaximum(15.3275);
   Graph_Graph3006->SetDirectory(nullptr);
   Graph_Graph3006->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_Graph3006->SetLineColor(ci);
   Graph_Graph3006->GetXaxis()->SetLabelFont(42);
   Graph_Graph3006->GetXaxis()->SetTitleOffset(1);
   Graph_Graph3006->GetXaxis()->SetTitleFont(42);
   Graph_Graph3006->GetYaxis()->SetLabelFont(42);
   Graph_Graph3006->GetYaxis()->SetTitleFont(42);
   Graph_Graph3006->GetZaxis()->SetLabelFont(42);
   Graph_Graph3006->GetZaxis()->SetTitleOffset(1);
   Graph_Graph3006->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph3006);
   
   grae->Draw(" 3");
   
   Double_t Graph_fx3007[11] = { 0.005, 0.013, 0.02, 0.028, 0.036, 0.045, 0.055, 0.07, 0.09, 0.125, 0.225 };
   Double_t Graph_fy3007[11] = { 7.69566, 13.6858, 13.6195, 11.0991, 8.93109, 6.54427, 4.61141, 2.93044, 1.49278, 0.535996, 0.0286431 };
   Double_t Graph_felx3007[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fely3007[11] = { 0.142076, 0.250678, 0.225677, 0.201418, 0.188868, 0.140593, 0.119363, 0.0673829, 0.0486903, 0.0185518, 0.00240898 };
   Double_t Graph_fehx3007[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fehy3007[11] = { 0.142076, 0.250678, 0.225677, 0.201418, 0.188868, 0.140593, 0.119363, 0.0673829, 0.0486903, 0.0185518, 0.00240898 };
   grae = new TGraphAsymmErrors(11,Graph_fx3007,Graph_fy3007,Graph_felx3007,Graph_fehx3007,Graph_fely3007,Graph_fehy3007);
   grae->SetName("");
   grae->SetTitle("");

   ci = 1195;
   color = new TColor(ci, 0.9921569, 0.6588235, 0.1764706, " ", 0.6);
   grae->SetFillColor(ci);
   grae->SetFillStyle(1000);

   ci = TColor::GetColor("#fda82d");
   grae->SetLineColor(ci);
   grae->SetLineWidth(5);

   ci = TColor::GetColor("#fda82d");
   grae->SetMarkerColor(ci);
   grae->SetMarkerStyle(20);
   grae->SetMarkerSize(0);
   
   TH1F *Graph_Graph3007 = new TH1F("Graph_Graph3007","",100,0.0015,0.3298333);
   Graph_Graph3007->SetMinimum(0.02361071);
   Graph_Graph3007->SetMaximum(15.3275);
   Graph_Graph3007->SetDirectory(nullptr);
   Graph_Graph3007->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_Graph3007->SetLineColor(ci);
   Graph_Graph3007->GetXaxis()->SetLabelFont(42);
   Graph_Graph3007->GetXaxis()->SetTitleOffset(1);
   Graph_Graph3007->GetXaxis()->SetTitleFont(42);
   Graph_Graph3007->GetYaxis()->SetLabelFont(42);
   Graph_Graph3007->GetYaxis()->SetTitleFont(42);
   Graph_Graph3007->GetZaxis()->SetLabelFont(42);
   Graph_Graph3007->GetZaxis()->SetTitleOffset(1);
   Graph_Graph3007->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph3007);
   
   grae->Draw(" lx");
   
   Double_t Graph_fx3008[11] = { 0.005, 0.013, 0.02, 0.028, 0.036, 0.045, 0.055, 0.07, 0.09, 0.125, 0.225 };
   Double_t Graph_fy3008[11] = { 7.141999, 13.62214, 14.45397, 12.69267, 10.53783, 7.792181, 5.807915, 3.632032, 1.905666, 0.7013436, 0.0403583 };
   Double_t Graph_felx3008[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fely3008[11] = { 0.09673027, 0.1637297, 0.1532058, 0.1543197, 0.1510756, 0.1130899, 0.1057624, 0.05444511, 0.03815643, 0.01539151, 0.002195212 };
   Double_t Graph_fehx3008[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fehy3008[11] = { 0.09673027, 0.1637297, 0.1532058, 0.1543197, 0.1510756, 0.1130899, 0.1057624, 0.05444511, 0.03815643, 0.01539151, 0.002195212 };
   grae = new TGraphAsymmErrors(11,Graph_fx3008,Graph_fy3008,Graph_felx3008,Graph_fehx3008,Graph_fely3008,Graph_fehy3008);
   grae->SetName("");
   grae->SetTitle("");

   ci = 1191;
   color = new TColor(ci, 0.2666667, 0.572549, 0.8470588, " ", 0.6);
   grae->SetFillColor(ci);
   grae->SetFillStyle(1000);

   ci = TColor::GetColor("#4492d8");
   grae->SetLineColor(ci);
   grae->SetLineWidth(5);

   ci = TColor::GetColor("#4492d8");
   grae->SetMarkerColor(ci);
   grae->SetMarkerStyle(20);
   grae->SetMarkerSize(0);
   
   TH1F *Graph_Graph3008 = new TH1F("Graph_Graph3008","",100,0.0015,0.3298333);
   Graph_Graph3008->SetMinimum(0.03434678);
   Graph_Graph3008->SetMaximum(16.06408);
   Graph_Graph3008->SetDirectory(nullptr);
   Graph_Graph3008->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_Graph3008->SetLineColor(ci);
   Graph_Graph3008->GetXaxis()->SetLabelFont(42);
   Graph_Graph3008->GetXaxis()->SetTitleOffset(1);
   Graph_Graph3008->GetXaxis()->SetTitleFont(42);
   Graph_Graph3008->GetYaxis()->SetLabelFont(42);
   Graph_Graph3008->GetYaxis()->SetTitleFont(42);
   Graph_Graph3008->GetZaxis()->SetLabelFont(42);
   Graph_Graph3008->GetZaxis()->SetTitleOffset(1);
   Graph_Graph3008->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph3008);
   
   grae->Draw(" 3");
   
   Double_t Graph_fx3009[11] = { 0.005, 0.013, 0.02, 0.028, 0.036, 0.045, 0.055, 0.07, 0.09, 0.125, 0.225 };
   Double_t Graph_fy3009[11] = { 7.141999, 13.62214, 14.45397, 12.69267, 10.53783, 7.792181, 5.807915, 3.632032, 1.905666, 0.7013436, 0.0403583 };
   Double_t Graph_felx3009[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fely3009[11] = { 0.09673027, 0.1637297, 0.1532058, 0.1543197, 0.1510756, 0.1130899, 0.1057624, 0.05444511, 0.03815643, 0.01539151, 0.002195212 };
   Double_t Graph_fehx3009[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fehy3009[11] = { 0.09673027, 0.1637297, 0.1532058, 0.1543197, 0.1510756, 0.1130899, 0.1057624, 0.05444511, 0.03815643, 0.01539151, 0.002195212 };
   grae = new TGraphAsymmErrors(11,Graph_fx3009,Graph_fy3009,Graph_felx3009,Graph_fehx3009,Graph_fely3009,Graph_fehy3009);
   grae->SetName("");
   grae->SetTitle("");

   ci = 1191;
   color = new TColor(ci, 0.2666667, 0.572549, 0.8470588, " ", 0.6);
   grae->SetFillColor(ci);
   grae->SetFillStyle(1000);

   ci = TColor::GetColor("#4492d8");
   grae->SetLineColor(ci);
   grae->SetLineWidth(5);

   ci = TColor::GetColor("#4492d8");
   grae->SetMarkerColor(ci);
   grae->SetMarkerStyle(20);
   grae->SetMarkerSize(0);
   
   TH1F *Graph_Graph3009 = new TH1F("Graph_Graph3009","",100,0.0015,0.3298333);
   Graph_Graph3009->SetMinimum(0.03434678);
   Graph_Graph3009->SetMaximum(16.06408);
   Graph_Graph3009->SetDirectory(nullptr);
   Graph_Graph3009->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_Graph3009->SetLineColor(ci);
   Graph_Graph3009->GetXaxis()->SetLabelFont(42);
   Graph_Graph3009->GetXaxis()->SetTitleOffset(1);
   Graph_Graph3009->GetXaxis()->SetTitleFont(42);
   Graph_Graph3009->GetYaxis()->SetLabelFont(42);
   Graph_Graph3009->GetYaxis()->SetTitleFont(42);
   Graph_Graph3009->GetZaxis()->SetLabelFont(42);
   Graph_Graph3009->GetZaxis()->SetTitleOffset(1);
   Graph_Graph3009->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph3009);
   
   grae->Draw(" lx");
   
   Double_t Graph_fx3010[11] = { 0.005, 0.013, 0.02, 0.028, 0.036, 0.045, 0.055, 0.07, 0.09, 0.125, 0.225 };
   Double_t Graph_fy3010[11] = { 6.308031, 12.67764, 13.54843, 12.64567, 10.45005, 7.920866, 6.111764, 3.948399, 2.207411, 0.9603692, 0.04999755 };
   Double_t Graph_felx3010[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fely3010[11] = { 0.1263575, 0.1589816, 0.165864, 0.1612446, 0.1381829, 0.1192303, 0.1068374, 0.07081904, 0.05966537, 0.02312858, 0.003817768 };
   Double_t Graph_fehx3010[11] = { 0.005, 0.003, 0.004, 0.004000001, 0.003999999, 0.005000001, 0.004999999, 0.01, 0.01, 0.025, 0.075 };
   Double_t Graph_fehy3010[11] = { 0.1263575, 0.1589816, 0.165864, 0.1612446, 0.1381829, 0.1192303, 0.1068374, 0.07081904, 0.05966537, 0.02312858, 0.003817768 };
   grae = new TGraphAsymmErrors(11,Graph_fx3010,Graph_fy3010,Graph_felx3010,Graph_fehx3010,Graph_fely3010,Graph_fehy3010);
   grae->SetName("");
   grae->SetTitle("");

   ci = 1181;
   color = new TColor(ci, 0.6, 0.6, 0.6, " ", 0.6);
   grae->SetFillColor(ci);
   grae->SetFillStyle(1000);
   grae->SetMarkerStyle(20);
   grae->SetMarkerSize(1.5);
   
   TH1F *Graph_Graph3010 = new TH1F("Graph_Graph3010","",100,0.0015,0.3298333);
   Graph_Graph3010->SetMinimum(0.0415618);
   Graph_Graph3010->SetMaximum(15.08111);
   Graph_Graph3010->SetDirectory(nullptr);
   Graph_Graph3010->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_Graph3010->SetLineColor(ci);
   Graph_Graph3010->GetXaxis()->SetLabelFont(42);
   Graph_Graph3010->GetXaxis()->SetTitleOffset(1);
   Graph_Graph3010->GetXaxis()->SetTitleFont(42);
   Graph_Graph3010->GetYaxis()->SetLabelFont(42);
   Graph_Graph3010->GetYaxis()->SetTitleFont(42);
   Graph_Graph3010->GetZaxis()->SetLabelFont(42);
   Graph_Graph3010->GetZaxis()->SetTitleOffset(1);
   Graph_Graph3010->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph3010);
   
   grae->Draw(" pz");
   
   Double_t Graph_fx1[2] = { 0.003, 0.3 };
   Double_t Graph_fy1[2] = { 0, 0 };
   TGraph *graph = new TGraph(2,Graph_fx1,Graph_fy1);
   graph->SetName("");
   graph->SetTitle("");
   graph->SetFillStyle(1000);
   graph->SetLineStyle(2);
   
   TH1F *Graph_Graph1 = new TH1F("Graph_Graph1","",100,0.0027,0.3297);
   Graph_Graph1->SetMinimum(0);
   Graph_Graph1->SetMaximum(1.1);
   Graph_Graph1->SetDirectory(nullptr);
   Graph_Graph1->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_Graph1->SetLineColor(ci);
   Graph_Graph1->GetXaxis()->SetLabelFont(42);
   Graph_Graph1->GetXaxis()->SetTitleOffset(1);
   Graph_Graph1->GetXaxis()->SetTitleFont(42);
   Graph_Graph1->GetYaxis()->SetLabelFont(42);
   Graph_Graph1->GetYaxis()->SetTitleFont(42);
   Graph_Graph1->GetZaxis()->SetLabelFont(42);
   Graph_Graph1->GetZaxis()->SetTitleOffset(1);
   Graph_Graph1->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph1);
   
   graph->Draw("l");
   TArrow *arrow = new TArrow(0.003,6.308031,0.004,6.308031,0.02,"<|");
   arrow->SetFillColor(1);
   arrow->SetFillStyle(1001);
   arrow->SetAngle(40);
   arrow->Draw();
   TLatex *   tex = new TLatex(0.5,0.9,"30 < p_{T}^{jet} < 100 GeV");
   tex->SetNDC();
   tex->SetTextAlign(21);
   tex->SetTextFont(42);
   tex->SetTextSize(0.06);
   tex->SetLineWidth(2);
   tex->Draw();
   
   TLegend *leg = new TLegend(0.05,0.09,0.35,0.3,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetTextSize(0.05);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("","CMS pp","plf");

   ci = 1181;
   color = new TColor(ci, 0.6, 0.6, 0.6, " ", 0.6);
   entry->SetFillColor(ci);
   entry->SetFillStyle(1000);
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(20);
   entry->SetMarkerSize(1.5);
   entry->SetTextFont(42);
   entry=leg->AddEntry("","JEWEL","lf");

   ci = 1191;
   color = new TColor(ci, 0.2666667, 0.572549, 0.8470588, " ", 0.6);
   entry->SetFillColor(ci);
   entry->SetFillStyle(1000);

   ci = TColor::GetColor("#4492d8");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(5);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("","PYQUEN","lf");

   ci = 1193;
   color = new TColor(ci, 0.8980392, 0.3882353, 0.1137255, " ", 0.6);
   entry->SetFillColor(ci);
   entry->SetFillStyle(1000);

   ci = TColor::GetColor("#e5631d");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(5);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.35,0.09,0.65,0.23,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetTextSize(0.05);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("","PYTHIA","lf");

   ci = 1199;
   color = new TColor(ci, 0.1333333, 0.5294118, 0.172549, " ", 0.6);
   entry->SetFillColor(ci);
   entry->SetFillStyle(1000);

   ci = TColor::GetColor("#22872c");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(5);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("","HYBRID","lf");

   ci = 1195;
   color = new TColor(ci, 0.9921569, 0.6588235, 0.1764706, " ", 0.6);
   entry->SetFillColor(ci);
   entry->SetFillStyle(1000);

   ci = TColor::GetColor("#fda82d");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(5);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
      tex = new TLatex(0.97,0.43,"pp 302 pb^{-1}");
   tex->SetNDC();
   tex->SetTextAlign(31);
   tex->SetTextFont(42);
   tex->SetLineWidth(2);
   tex->Draw();
      tex = new TLatex(0.95,0.78,"60 < p_{T}^{#gamma} < 200 GeV");
   tex->SetNDC();
   tex->SetTextAlign(31);
   tex->SetTextFont(42);
   tex->SetLineWidth(2);
   tex->Draw();
      tex = new TLatex(0.95,0.7,"|#eta^{#gamma}| < 1.442");
   tex->SetNDC();
   tex->SetTextAlign(31);
   tex->SetTextFont(42);
   tex->SetLineWidth(2);
   tex->Draw();
      tex = new TLatex(0.95,0.62,"#Delta#phi_{j#gamma} > #frac{2#pi}{3}, |#eta^{jet}| < 1.6");
   tex->SetNDC();
   tex->SetTextAlign(31);
   tex->SetTextFont(42);
   tex->SetLineWidth(2);
   tex->Draw();
      tex = new TLatex(0.95,0.54,"anti-k_{T} R = 0.3");
   tex->SetNDC();
   tex->SetTextAlign(31);
   tex->SetTextFont(42);
   tex->SetLineWidth(2);
   tex->Draw();
   P1__0->Modified();
   canvas->cd();
   TGaxis *gaxis = new TGaxis(0.2361111,0.109375,0.9305556,0.109375,0.003,0.3,510,"SG");
   gaxis->SetLabelOffset(0.005);
   gaxis->SetLabelSize(0.05);
   gaxis->SetTickSize(0);
   gaxis->SetGridLength(0);
   gaxis->SetTitleOffset(1);
   gaxis->SetTitleSize(0.04);
   gaxis->SetTitleColor(1);
   gaxis->SetTitleFont(62);
   gaxis->SetLabelFont(42);
   gaxis->SetNoExponent();
   gaxis->Draw();
      tex = new TLatex(0.5833333,0.0546875,"#Deltaj");
   tex->SetNDC();
   tex->SetTextAlign(22);
   tex->SetTextFont(42);
   tex->SetTextSize(0.07);
   tex->SetLineWidth(2);
   tex->Draw();
   gaxis = new TGaxis(0.2361111,0.109375,0.2361111,0.890625,-2.0979,20.979,510,"S");
   gaxis->SetLabelOffset(0.005);
   gaxis->SetLabelSize(0.05);
   gaxis->SetTickSize(0);
   gaxis->SetGridLength(0);
   gaxis->SetTitleOffset(1);
   gaxis->SetTitleSize(0.04);
   gaxis->SetTitleColor(1);
   gaxis->SetTitleFont(62);
   gaxis->SetLabelFont(42);
   gaxis->SetNoExponent();
   gaxis->Draw();
      tex = new TLatex(0.09444444,0.5,"#frac{1}{N_{#gamma}} #frac{dN_{j#gamma}}{d#Deltaj}");
   tex->SetNDC();
   tex->SetTextAlign(22);
   tex->SetTextFont(42);
   tex->SetTextSize(0.07);
   tex->SetTextAngle(90);
   tex->SetLineWidth(2);
   tex->Draw();
      tex = new TLatex(0.2361111,0.9070312,"CMS #scale[0.8]{#font[52]{Preliminary}}");
   tex->SetNDC();
   tex->SetTextSize(0.07);
   tex->SetLineWidth(2);
   tex->Draw();
      tex = new TLatex(0.9305556,0.9070312,"(5.02 TeV)");
   tex->SetNDC();
   tex->SetTextAlign(31);
   tex->SetTextFont(42);
   tex->SetTextSize(0.045);
   tex->SetLineWidth(2);
   tex->Draw();
   canvas->Modified();
   canvas->SetSelected(canvas);
}

//Make all plots in paper
void plotReproducer()
{
   pho_60_rebin3_final_theory_spectra_pp_whole_log();
   // pho_60_rebin3_final_theory_spectra_pp_subsets_log();
   // pho_60_rebin3_final_theory_spectra_aa_mc_whole_log();
   // pho_60_rebin3_final_theory_spectra_aa_mc_subsets_log();
   // pho_60_rebin3_final_theory_spectra_aa_hybrid_whole_log();
   // pho_60_rebin3_final_theory_spectra_aa_hybrid_subsets_log();
   // pho_60_rebin3_final_theory_ratio_mc_whole_log();
   // pho_60_rebin3_final_theory_ratio_mc_subsets_log();
   // pho_60_rebin3_final_theory_ratio_hybrid_whole_log();
   // pho_60_rebin3_final_theory_ratio_hybrid_subsets_log();
   // pho_60_rebin3_final_spectra_whole_log();
   // pho_60_rebin3_final_spectra_subsets_log();
   // pho_60_rebin3_final_ratio_whole_log();
   // pho_60_rebin3_final_ratio_subsets_log();
   // pho_60_rebin3_final_data_systematics_whole_log();
   // pho_60_rebin3_final_data_systematics_part2_log();
   // pho_60_rebin3_final_data_systematics_part1_log();
}


