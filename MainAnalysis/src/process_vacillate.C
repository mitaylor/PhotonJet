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

TH2F* variance(TH1* flat, multival const* m) {
    auto cov = new TH2F("cov", "", m->size(), 0, m->size(),
        m->size(), 0, m->size());

    for (int64_t i = 0; i < m->size(); ++i) {
        auto err = flat->GetBinError(i + 1);
        cov->SetBinContent(i + 1, i + 1, err * err);
    }

    return cov;
}

TH1F* fold(TH1* flat, TH2* covariance, multival const* m, int64_t axis,
           std::vector<int64_t>& offsets) {
    auto name = std::string(flat->GetName()) + "_fold" + std::to_string(axis);
    auto hfold = m->axis(axis).book<TH1F, 2>(0, name, "",
        { offsets[axis << 1], offsets[(axis << 1) + 1] });

    auto shape = m->shape();

    auto front = std::vector<int64_t>(m->dims(), 0);
    auto back = std::vector<int64_t>(m->dims(), 0);
    for (int64_t i = 0; i < m->dims(); ++i) {
        front[i] = offsets[i << 1];
        back[i] = shape[i] - offsets[(i << 1) + 1];
    }

    auto size = back[axis] - front[axis];
    auto list = new std::vector<int64_t>[size];

    for (int64_t i = 0; i < m->size(); ++i) {
        auto indices = m->indices_for(i);

        bool flag = false;
        zip([&](int64_t index, int64_t f, int64_t b) {
            flag = flag || index < f || index >= b;
        }, indices, front, back);
        if (flag) { continue; }

        auto index = indices[axis] - front[axis];
        hfold->SetBinContent(index + 1, hfold->GetBinContent(index + 1)
            + flat->GetBinContent(i + 1));

        list[index].push_back(i);
    }

    auto cov = covariance ? (TH2F*)covariance->Clone() : variance(flat, m);

    for (int64_t i = 0; i < size; ++i) {
        auto indices = list[i];
        int64_t count = indices.size();

        auto error = 0.;
        for (int64_t j = 0; j < count; ++j) {
            auto j_x = indices[j] + 1;
            for (int64_t k = 0; k < count; ++k) {
                auto k_x = indices[k] + 1;
                error = error + cov->GetBinContent(j_x, k_x);
            }
        }

        hfold->SetBinError(i + 1, std::sqrt(error));
    }

    delete [] list;
    delete cov;

    hfold->Scale(1., "width");

    return hfold;
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

    auto rdrr = sel->get<std::vector<float>>("drr_range");
    auto rdrg = sel->get<std::vector<float>>("drg_range");
    auto rptr = sel->get<std::vector<float>>("ptr_range");
    auto rptg = sel->get<std::vector<float>>("ptg_range");

    auto osr = sel->get<std::vector<int64_t>>("osr");
    auto osg = sel->get<std::vector<int64_t>>("osg");

    auto osg_part1 = sel->get<std::vector<int64_t>>("osg_part1");
    auto osg_part2 = sel->get<std::vector<int64_t>>("osg_part2");

    auto idrg = new interval("#deltaj"s, rdrg);
    auto iptg = new interval("p_{T}^{j}"s, rptg);
    auto idrr = new interval("#deltaj"s, rdrr);
    auto iptr = new interval("p_{T}^{j}"s, rptr);

    auto mr = new multival(*idrr, *iptr);
    auto mg = new multival(*idrg, *iptg);

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
    auto input_mc_ppt = new history<TH1F>(funfolding, tag + "_ppt");

    auto input_mc_proj_gen = new history<TH1F>("input_mc_proj_gen", "", null<TH1F>, input_mc_gen->size());
    auto input_mc_proj_reco = new history<TH1F>("input_mc_proj_reco", "", null<TH1F>, input_mc_gen->size());

    auto input_mc_eff_gen = new history<TH1F>("input_mc_eff_gen", "", null<TH1F>, input_mc_gen->size());
    auto input_mc_eff_reco = new history<TH1F>("input_mc_eff_reco", "", null<TH1F>, input_mc_gen->size());

    auto input_mc_gen_fold0 = new history<TH1F>("input_mc_gen_fold0", "", null<TH1F>, (int64_t) input_mc_gen->size());
    auto input_mc_gen_fold1 = new history<TH1F>("input_mc_gen_fold1", "", null<TH1F>, (int64_t) input_mc_gen->size());
    auto input_mc_gen_fold0_part1 = new history<TH1F>("input_mc_gen_fold0_part1", "", null<TH1F>, (int64_t) input_mc_gen->size());
    auto input_mc_gen_fold1_part1 = new history<TH1F>("input_mc_gen_fold1_part1", "", null<TH1F>, (int64_t) input_mc_gen->size());
    auto input_mc_gen_fold0_part2 = new history<TH1F>("input_mc_gen_fold0_part2", "", null<TH1F>, (int64_t) input_mc_gen->size());
    auto input_mc_gen_fold1_part2 = new history<TH1F>("input_mc_gen_fold1_part2", "", null<TH1F>, (int64_t) input_mc_gen->size());
    auto input_mc_reco_fold0 = new history<TH1F>("input_mc_reco_fold0", "", null<TH1F>, (int64_t) input_mc_gen->size());
    auto input_mc_reco_fold1 = new history<TH1F>("input_mc_reco_fold1", "", null<TH1F>, (int64_t) input_mc_gen->size());

    /* extract chosen histograms */
    for (int64_t j = 0; j < input_mc_gen->size(); ++j) {
        /* project histograms */
        (*input_mc_reco_fold0)[j] = fold((*input_mc_reco)[j], nullptr, mr, 0, osr);
        (*input_mc_reco_fold1)[j] = fold((*input_mc_reco)[j], nullptr, mr, 1, osr);
        (*input_mc_gen_fold0)[j] = fold((*input_mc_gen)[j], nullptr, mg, 0, osg);
        (*input_mc_gen_fold1)[j] = fold((*input_mc_gen)[j], nullptr, mg, 1, osg);
        (*input_mc_gen_fold0_part1)[j] = fold((*input_mc_gen)[j], nullptr, mg, 0, osg_part1);
        (*input_mc_gen_fold1_part1)[j] = fold((*input_mc_gen)[j], nullptr, mg, 1, osg_part1);
        (*input_mc_gen_fold0_part2)[j] = fold((*input_mc_gen)[j], nullptr, mg, 0, osg_part2);
        (*input_mc_gen_fold1_part2)[j] = fold((*input_mc_gen)[j], nullptr, mg, 1, osg_part2);

        /* calculate efficiencies */
        DoProjection((*input_mc_response)[j], &(*input_mc_proj_gen)[j], &(*input_mc_proj_reco)[j]);
        DoProjection((*input_mc_response)[j], &(*input_mc_eff_gen)[j], &(*input_mc_eff_reco)[j]);

        (*input_mc_proj_gen)[j]->Scale(1/(*input_mc_n)[j]->GetBinContent(1));
        (*input_mc_proj_reco)[j]->Scale(1/(*input_mc_n)[j]->GetBinContent(1));

        (*input_mc_eff_gen)[j]->Scale(1/(*input_mc_n)[j]->GetBinContent(1));
        (*input_mc_eff_reco)[j]->Scale(1/(*input_mc_n)[j]->GetBinContent(1));

        for (int i = 0; i < (*input_mc_proj_gen)[j]->GetNbinsX(); ++i) {
            if ((*input_mc_proj_gen)[j]->GetBinContent(i + 1) > (*input_mc_gen)[j]->GetBinContent(i + 1)) {
                (*input_mc_proj_gen)[j]->SetBinContent(i + 1, (*input_mc_gen)[j]->GetBinContent(i + 1));
                (*input_mc_eff_gen)[j]->SetBinContent(i + 1, (*input_mc_gen)[j]->GetBinContent(i + 1));
            }
        }

        for (int i = 0; i < (*input_mc_proj_reco)[j]->GetNbinsX(); ++i) {
            if ((*input_mc_proj_reco)[j]->GetBinContent(i + 1) > (*input_mc_reco)[j]->GetBinContent(i + 1)) {
                (*input_mc_proj_reco)[j]->SetBinContent(i + 1, (*input_mc_reco)[j]->GetBinContent(i + 1));
                (*input_mc_eff_reco)[j]->SetBinContent(i + 1, (*input_mc_reco)[j]->GetBinContent(i + 1));
            }
        }

        (*input_mc_eff_gen)[j]->Divide((*input_mc_gen)[j]);
        (*input_mc_eff_reco)[j]->Divide((*input_mc_reco)[j]);
    }

    /* rename histograms */
    input_mc_proj_gen->rename(tag + "_proj_g");
    input_mc_proj_reco->rename(tag + "_proj_r");
    input_mc_eff_gen->rename(tag + "_g_eff");
    input_mc_eff_reco->rename(tag + "_r_eff");
    input_mc_reco_fold0->rename(tag + "_r_fold0");
    input_mc_reco_fold1->rename(tag + "_r_fold1");
    input_mc_gen_fold0->rename(tag + "_g_fold0");
    input_mc_gen_fold1->rename(tag + "_g_fold1");
    input_mc_gen_fold0_part1->rename(tag + "_g_fold0_part1");
    input_mc_gen_fold1_part1->rename(tag + "_g_fold1_part1");
    input_mc_gen_fold0_part2->rename(tag + "_g_fold0_part2");
    input_mc_gen_fold1_part2->rename(tag + "_g_fold1_part2");

    /* save histograms */
    input_mc_gen->save();
    input_mc_gen_reco->save();
    input_mc_reco->save();
    input_mc_response->save();
    input_mc_n->save();
    input_mc_proj_gen->save();
    input_mc_proj_reco->save();
    input_mc_eff_gen->save();
    input_mc_eff_reco->save();
    input_mc_ppt->save();
    input_mc_reco_fold0->save();
    input_mc_reco_fold1->save();
    input_mc_gen_fold0->save();
    input_mc_gen_fold1->save();
    input_mc_gen_fold0_part1->save();
    input_mc_gen_fold1_part1->save();
    input_mc_gen_fold0_part2->save();
    input_mc_gen_fold1_part2->save();

    fout->Close();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return quantitate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
