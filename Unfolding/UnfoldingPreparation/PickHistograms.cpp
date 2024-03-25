#include <vector>
#include <iostream>
using namespace std;

#include "TH1F.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"

#include "CommandLine.h"
#include "CustomAssert.h"

int main(int argc, char *argv[]);
TH1D *ForwardFold(TH1 *HGen, TH2D *HResponse);

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string DataFileName            = CL.Get("Data");
   string DataHistogram           = CL.Get("DataHistogram");
   string ResponseFileName        = CL.Get("Response");
   string ResponseHistogram       = CL.Get("ResponseHistogram");
   string ResponseTruth           = CL.Get("ResponseTruth");
   string ResponseReco            = CL.Get("ResponseReco");
   string ResponseTruthEfficiency = CL.Get("ResponseTruthEfficiency");
   string ResponseRecoEfficiency  = CL.Get("ResponseRecoEfficiency");
   string OutputFileName          = CL.Get("Output");
   vector<double> PrimaryRecoBins = CL.GetDoubleVector("PrimaryRecoBins");
   vector<double> BinningRecoBins = CL.GetDoubleVector("BinningRecoBins");
   vector<double> PrimaryGenBins  = CL.GetDoubleVector("PrimaryGenBins");
   vector<double> BinningGenBins  = CL.GetDoubleVector("BinningGenBins");

   TFile DataFile(DataFileName.c_str());
   TFile ResponseFile(ResponseFileName.c_str());

   TFile OutputFile(OutputFileName.c_str(), "RECREATE");

   TH1F *HInputData                        = (TH1F *)DataFile.Get(DataHistogram.c_str());
   TH2F *HInputResponse                    = (TH2F *)ResponseFile.Get(ResponseHistogram.c_str());
   TH1F *HInputResponseTruth               = (TH1F *)ResponseFile.Get(ResponseTruth.c_str());
   TH1F *HInputResponseReco                = (TH1F *)ResponseFile.Get(ResponseReco.c_str());
   TH1F *HInputResponseTruthEfficiency     = (TH1F *)ResponseFile.Get(ResponseTruthEfficiency.c_str());
   TH1F *HInputResponseRecoEfficiency      = (TH1F *)ResponseFile.Get(ResponseRecoEfficiency.c_str());

   Assert(HInputData != nullptr,                      "Input data distribution not found");
   Assert(HInputResponse != nullptr,                  "Input response matrix not found");
   Assert(HInputResponseTruth != nullptr,             "Input response truth not found");
   Assert(HInputResponseReco != nullptr,              "Input response reco not found");
   Assert(HInputResponseTruthEfficiency != nullptr,   "Input response truth efficiency not found");
   Assert(HInputResponseRecoEfficiency != nullptr,    "Input response reco efficiency not found");

   int NReco = HInputResponse->GetNbinsX();
   int NGen  = HInputResponse->GetNbinsY();
   
   int NReco1 = PrimaryRecoBins.size() - 1;
   int NReco2 = BinningRecoBins.size() - 1;
   int NGen1  = PrimaryGenBins.size() - 1;
   int NGen2  = BinningGenBins.size() - 1;

   cout << NReco1 << " " << NReco2 << " " << NReco1 * NReco2 << " " << NReco << endl;
   cout << NGen1 << " " << NGen2 << " " << NGen1 * NGen2 << " " << NGen << endl;
   
   // Copy over response
   TH2D HResponse("HResponse", ";;", NReco, 0, NReco, NGen, 0, NGen);
   for(int iX = 0; iX <= NReco + 1; iX++)
   {
      for(int iY = 0; iY <= NGen + 1; iY++)
      {
         HResponse.SetBinContent(iX, iY, HInputResponse->GetBinContent(iX, iY));
         HResponse.SetBinError(iX, iY, HInputResponse->GetBinError(iX, iY));
      }
   }
   HResponse.Write();

   // Copy over MC truth
   TH1D HMCGen("HMCGen", ";;", NGen, 0, NGen);
   for(int i = 0; i <= NGen + 1; i++)
   {
      HMCGen.SetBinContent(i, HInputResponseTruth->GetBinContent(i));
      HMCGen.SetBinError(i, HInputResponseTruth->GetBinError(i));
   }
   HMCGen.Write();
   
   // Copy over MC measured
   TH1D HMCReco("HMCReco", ";;", NReco, 0, NReco);
   for(int i = 0; i <= NReco + 1; i++)
   {
      HMCReco.SetBinContent(i, HInputResponseReco->GetBinContent(i));
      HMCReco.SetBinError(i, HInputResponseReco->GetBinError(i));
   }
   HMCReco.Write();

   // Copy over MC truth efficiency
   TH1D HMCGenEfficiency("HMCGenEfficiency", ";;", NGen, 0, NGen);
   for(int i = 0; i <= NGen + 1; i++)
   {
      HMCGenEfficiency.SetBinContent(i, HInputResponseTruthEfficiency->GetBinContent(i));
      HMCGenEfficiency.SetBinError(i, 0);
   }
   HMCGenEfficiency.Write();
   
   // Copy over MC measured efficiency
   TH1D HMCRecoEfficiency("HMCRecoEfficiency", ";;", NReco, 0, NReco);
   for(int i = 0; i <= NReco + 1; i++)
   {
      HMCRecoEfficiency.SetBinContent(i, HInputResponseRecoEfficiency->GetBinContent(i));
      HMCRecoEfficiency.SetBinError(i, 0);
   }
   HMCRecoEfficiency.Write();

   // Copy over gen input
   TH1D HData("HData", ";;", NReco, 0, NReco);
   for(int i = 0; i <= NReco + 1; i++)
   {
      HData.SetBinContent(i, HInputData->GetBinContent(i));
      HData.SetBinError(i, HInputData->GetBinError(i));
   }
   HData.Write();
   
   // Binning histograms
   TH1D HGenPrimaryBinMin("HGenPrimaryBinMin", ";;", NGen, 0, NGen);
   TH1D HGenPrimaryBinMax("HGenPrimaryBinMax", ";;", NGen, 0, NGen);
   TH1D HGenBinningBinMin("HGenBinningBinMin", ";;", NGen, 0, NGen);
   TH1D HGenBinningBinMax("HGenBinningBinMax", ";;", NGen, 0, NGen);
   for(int i = 0; i < NGen1; i++)
   {
      for(int j = 0; j < NGen2; j++)
      {
         HGenPrimaryBinMin.SetBinContent(j * NGen1 + i + 1, PrimaryGenBins[i]);
         HGenPrimaryBinMax.SetBinContent(j * NGen1 + i + 1, PrimaryGenBins[i+1]);
         HGenBinningBinMin.SetBinContent(j * NGen1 + i + 1, BinningGenBins[j]);
         HGenBinningBinMax.SetBinContent(j * NGen1 + i + 1, BinningGenBins[j+1]);
      }
   }
   HGenPrimaryBinMin.Write();
   HGenPrimaryBinMax.Write();
   HGenBinningBinMin.Write();
   HGenBinningBinMax.Write();

   TH1D HRecoPrimaryBinMin("HRecoPrimaryBinMin", ";;", NReco, 0, NReco);
   TH1D HRecoPrimaryBinMax("HRecoPrimaryBinMax", ";;", NReco, 0, NReco);
   TH1D HRecoBinningBinMin("HRecoBinningBinMin", ";;", NReco, 0, NReco);
   TH1D HRecoBinningBinMax("HRecoBinningBinMax", ";;", NReco, 0, NReco);
   TH1D HMatchedPrimaryBinMin("HMatchedPrimaryBinMin", ";;", NReco, 0, NReco);
   TH1D HMatchedPrimaryBinMax("HMatchedPrimaryBinMax", ";;", NReco, 0, NReco);
   TH1D HMatchedBinningBinMin("HMatchedBinningBinMin", ";;", NReco, 0, NReco);
   TH1D HMatchedBinningBinMax("HMatchedBinningBinMax", ";;", NReco, 0, NReco);
   for(int i = 0; i < NReco1; i++)
   {
      for(int j = 0; j < NReco2; j++)
      {
         HRecoPrimaryBinMin.SetBinContent(j * NReco1 + i + 1, PrimaryRecoBins[i]);
         HRecoPrimaryBinMax.SetBinContent(j * NReco1 + i + 1, PrimaryRecoBins[i+1]);
         HRecoBinningBinMin.SetBinContent(j * NReco1 + i + 1, BinningRecoBins[j]);
         HRecoBinningBinMax.SetBinContent(j * NReco1 + i + 1, BinningRecoBins[j+1]);
         HMatchedPrimaryBinMin.SetBinContent(j * NReco1 + i + 1, PrimaryRecoBins[i]);
         HMatchedPrimaryBinMax.SetBinContent(j * NReco1 + i + 1, PrimaryRecoBins[i+1]);
         HMatchedBinningBinMin.SetBinContent(j * NReco1 + i + 1, BinningRecoBins[j]);
         HMatchedBinningBinMax.SetBinContent(j * NReco1 + i + 1, BinningRecoBins[j+1]);
      }
   }
   HRecoPrimaryBinMin.Write();
   HRecoPrimaryBinMax.Write();
   HRecoBinningBinMin.Write();
   HRecoBinningBinMax.Write();
   HMatchedPrimaryBinMin.Write();
   HMatchedPrimaryBinMax.Write();
   HMatchedBinningBinMin.Write();
   HMatchedBinningBinMax.Write();

   OutputFile.Close();

   ResponseFile.Close();
   DataFile.Close();

   return 0;
}

TH1D *ForwardFold(TH1 *HGen, TH2D *HResponse)
{
   if(HGen == nullptr || HResponse == nullptr)
      return nullptr;

   int NGen = HResponse->GetNbinsY();
   int NReco = HResponse->GetNbinsX();

   TH1D *HResult = new TH1D("HDataReco", "", NReco, 0, NReco);

   HResult->Sumw2();

   for(int iG = 1; iG <= NGen; iG++)
   {
      double N = 0;
      for(int iR = 1; iR <= NReco; iR++)
         N = N + HResponse->GetBinContent(iR, iG);

      if(N == 0)
         continue;

      for(int iR = 1; iR <= NReco; iR++)
      {
         double T = HResponse->GetBinContent(iR, iG) / N;
         double G = HGen->GetBinContent(iG);
         double ET = HResponse->GetBinError(iR, iG) / HResponse->GetBinContent(iR, iG);
         double EG = HGen->GetBinError(iG) / HGen->GetBinContent(iG);

         if(HResponse->GetBinContent(iR, iG) == 0)
            ET = 0;
         if(HGen->GetBinContent(iG) == 0)
            EG = 0;

         double V = T * G;
         double E = sqrt(ET * ET + EG * EG) * V;

         double Current = HResult->GetBinContent(iR);
         HResult->SetBinContent(iR, Current + V);

         double Error = HResult->GetBinError(iR);
         Error = sqrt(Error * Error + E * E);
         HResult->SetBinError(iR, Error);
      }
   }

   return HResult;
}