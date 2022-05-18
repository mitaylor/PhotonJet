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

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string DataFileName            = CL.Get("Data");
   string DataHistogram           = CL.Get("DataHistogram");
   string ResponseFileName        = CL.Get("Response");
   string ResponseHistogram       = CL.Get("ResponseHistogram");
   string ResponseTruth           = CL.Get("ResponseTruth");
   string ResponseReco            = CL.Get("ResponseReco");
   string OutputFileName          = CL.Get("Output");
   vector<double> PrimaryRecoBins = CL.GetDoubleVector("PrimaryRecoBins");
   vector<double> BinningRecoBins = CL.GetDoubleVector("BinningRecoBins");
   vector<double> PrimaryGenBins  = CL.GetDoubleVector("PrimaryGenBins");
   vector<double> BinningGenBins  = CL.GetDoubleVector("BinningGenBins");

   TFile DataFile(DataFileName.c_str());
   TFile ResponseFile(ResponseFileName.c_str());

   TFile OutputFile(OutputFileName.c_str(), "RECREATE");

   TH1F *HInputData          = (TH1F *)DataFile.Get(DataHistogram.c_str());
   TH2F *HInputResponse      = (TH2F *)ResponseFile.Get(ResponseHistogram.c_str());
   TH1F *HInputResponseTruth = (TH1F *)ResponseFile.Get(ResponseTruth.c_str());
   TH1F *HInputResponseReco  = (TH1F *)ResponseFile.Get(ResponseReco.c_str());

   Assert(HInputData != nullptr,          "Input data distribution not found");
   Assert(HInputResponse != nullptr,      "Input response matrix not found");
   Assert(HInputResponseTruth != nullptr, "Input response truth not found");
   Assert(HInputResponseReco != nullptr,  "Input response Reco not found");

   int NReco = HInputResponse->GetNbinsX();
   int NGen  = HInputResponse->GetNbinsY();
   
   int NReco1 = PrimaryRecoBins.size() - 1;
   int NReco2 = BinningRecoBins.size() - 1;
   int NGen1  = PrimaryGenBins.size() - 1;
   int NGen2  = BinningGenBins.size() - 1;

   cout << NReco1 << " " << NReco2 << " " << NReco1 * NReco2 << " " << NReco << endl;
   cout << NGen1 << " " << NGen2 << " " << NGen1 * NGen2 << " " << NGen << endl;

   // Copy over data
   TH1D HDataReco("HDataReco", ";;", NReco, 0, NReco);
   for(int i = 1; i <= NReco; i++)
   {
      HDataReco.SetBinContent(i, HInputData->GetBinContent(i));
      HDataReco.SetBinError(i, HInputData->GetBinError(i));
   }
   HDataReco.Write();
   
   // Copy over response
   TH2D HResponse("HResponse", ";;", NReco, 0, NReco, NGen, 0, NGen);
   for(int iX = 1; iX <= NReco; iX++)
   {
      for(int iY = 1; iY <= NGen; iY++)
      {
         HResponse.SetBinContent(iX, iY, HInputResponse->GetBinContent(iX, iY));
         HResponse.SetBinError(iX, iY, HInputResponse->GetBinError(iX, iY));
      }
   }
   HResponse.Write();

   // Copy over MC truth
   TH1D HMCGen("HMCGen", ";;", NGen, 0, NGen);
   for(int i = 1; i <= NGen; i++)
   {
      HMCGen.SetBinContent(i, HInputResponseTruth->GetBinContent(i));
      HMCGen.SetBinError(i, HInputResponseTruth->GetBinError(i));
   }
   HMCGen.Write();
   
   // Copy over MC measured
   TH1D HMCReco("HMCReco", ";;", NReco, 0, NReco);
   for(int i = 1; i <= NReco; i++)
   {
      HMCReco.SetBinContent(i, HInputResponseReco->GetBinContent(i));
      HMCReco.SetBinError(i, HInputResponseReco->GetBinError(i));
   }
   HMCReco.Write();

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



