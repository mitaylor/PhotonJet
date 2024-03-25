#include "../include/lambdas.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TMatrixT.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

template <typename T>
T* null(int64_t, std::string const&, std::string const&) {
    return nullptr;
}

void DoProjection(TH2F *HResponse, TH1F **HGen, TH1F **HReco)
{
   if(HResponse == nullptr)
      return;
   if((*HGen) != nullptr || (*HReco) != nullptr)
      return;

   static int Count = 0;
   Count = Count + 1;

   int NX = HResponse->GetNbinsX();
   int NY = HResponse->GetNbinsY();

   *HGen = new TH1F(Form("HGen%d", Count), "", NY, 0, NY);
   *HReco = new TH1F(Form("HReco%d", Count), "", NX, 0, NX);

   for(int iX = 1; iX <= NX; iX++)
   {
      for(int iY = 1; iY <= NY; iY++)
      {
         double V = HResponse->GetBinContent(iX, iY);
         (*HGen)->AddBinContent(iY, V);
         (*HReco)->AddBinContent(iX, V);
      }
   }
}

int quantitate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto tag = conf->get<std::string>("tag");
    auto unfolding = conf->get<std::string>("unfolding");

    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");
    auto base = sel->get<std::string>("base");

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    auto funfolding = new TFile((base + unfolding).data(), "read");

    /* prepare output */
    TFile* fout = new TFile(output, "recreate");

    /* prepare data */
    auto input_mc_gen = new history<TH1F>(funfolding, tag + "_g");
    auto input_mc_gen_reco = new history<TH1F>(funfolding, tag + "_g_r");
    auto input_mc_reco = new history<TH1F>(funfolding, tag + "_r");
    auto input_mc_response = new history<TH2F>(funfolding, tag + "_c");
    auto input_mc_n = new history<TH1F>(funfolding, tag + "_c_n");

    auto input_mc_proj_gen = new history<TH1F>("input_mc_proj_gen", "", null<TH1F>, input_mc_gen->size());
    auto input_mc_proj_reco = new history<TH1F>("input_mc_proj_reco", "", null<TH1F>, input_mc_gen->size());

    auto input_mc_eff_gen = new history<TH1F>("input_mc_eff_gen", "", null<TH1F>, input_mc_gen->size());
    auto input_mc_eff_reco = new history<TH1F>("input_mc_eff_reco", "", null<TH1F>, input_mc_gen->size());

    /* extract chosen histograms */
    for (int64_t j = 0; j < input_mc_gen->size(); ++j) {
        DoProjection((*input_mc_response)[j], &(*input_mc_proj_gen)[j], &(*input_mc_proj_reco)[j]);
        DoProjection((*input_mc_response)[j], &(*input_mc_eff_gen)[j], &(*input_mc_eff_reco)[j]);

        (*input_mc_proj_gen)[j]->Scale(1/(*input_mc_n)[j]->GetBinContent(1));
        (*input_mc_proj_reco)[j]->Scale(1/(*input_mc_n)[j]->GetBinContent(1));

        (*input_mc_eff_gen)[j]->Scale(1/(*input_mc_n)[j]->GetBinContent(1));
        (*input_mc_eff_reco)[j]->Scale(1/(*input_mc_n)[j]->GetBinContent(1));

        (*input_mc_eff_gen)[j]->Divide((*input_mc_gen)[j]);
        (*input_mc_eff_reco)[j]->Divide((*input_mc_reco)[j]);
    }

    /* rename histograms */
    input_mc_proj_gen->rename(tag + "_proj_g");
    input_mc_eff_reco->rename(tag + "_proj_r");
    input_mc_eff_gen->rename(tag + "_g_eff");
    input_mc_eff_reco->rename(tag + "_r_eff");

    /* save histograms */
    input_mc_gen->save();
    input_mc_gen_reco->save();
    input_mc_reco->save();
    input_mc_response->save();
    input_mc_n->save();
    input_mc_proj_gen->save();
    input_mc_eff_reco->save();
    input_mc_eff_gen->save();
    input_mc_eff_reco->save();

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return quantitate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
