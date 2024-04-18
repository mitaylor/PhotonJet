#include "../include/pjtree.h"

#include "../git/config/include/configurer.h"

#include "../git/foliage/include/eggen.h"
#include "../git/foliage/include/electrons.h"
#include "../git/foliage/include/event.h"
#include "../git/foliage/include/jets.h"
#include "../git/foliage/include/photons.h"
#include "../git/foliage/include/triggers.h"
#include "../git/foliage/include/evtrho.h"
#include "../git/foliage/include/pfcand.h"
#include "../git/foliage/include/tracks.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/history.h"

#include "../git/tricks-and-treats/include/train.h"

#include "../include/JetCorrector.h"
#include "../include/JetUncertainty.h"
#include "../include/phoERegression.h"
#include "../include/phoERegressionNew.h"

#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TRandom3.h"
#include "TTree.h"

#include <algorithm>
#include <array>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>

using namespace std::literals::string_literals;

float weight_for(std::vector<int32_t> const& divisions,
                 std::vector<float> const& weights, float value) {
    int64_t index = -1;
    for (auto edge : divisions)
        if (value > edge)
            ++index;

    return weights[index];
}

float smear(std::vector<float> const& csn, float pt) {
    auto result = (csn[0] - csn[3]) + (csn[1] - csn[4]) / pt + (csn[2] - csn[5]) / (pt * pt);
    return result > 0 ? std::sqrt(result) : 0;
}

float get_UE(pjtree* tree_pj, float eta) {
    float result = 0;
    float R = 0.3;

    if(tree_pj->etaMin == nullptr)
        return -1;

    int NBin = tree_pj->etaMin->size();
    if(NBin == 0)
        return -1;

    for(int i = 0; i < NBin; i++)
    {
        float etaMin = static_cast<float>((*tree_pj->etaMin)[i]);
        float etaMax = static_cast<float>((*tree_pj->etaMax)[i]);
        float evtRho = static_cast<float>((*tree_pj->evtRho)[i]);

        if(etaMax < eta - R)
            continue;
        if(etaMin > eta + R)
            continue;

        float XMin = (std::max(etaMin, eta - R) - eta) / R;
        float XMax = (std::min(etaMax, eta + R) - eta) / R;

        if(XMin <= -1)
            XMin = -0.99999;
        if(XMax >= +1)
            XMax = +0.99999;

        float high = XMax * std::sqrt(1 - XMax * XMax) + std::asin(XMax);
        float low = XMin * std::sqrt(1 - XMin * XMin) + std::asin(XMin);

        result = result + R * R * (high - low) * evtRho;
    }

    return result;
}

int regulate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto files = conf->get<std::vector<std::string>>("files");
    auto max_entries = conf->get<int64_t>("max_entries");
    auto array_size = conf->get<int64_t>("array_size");
    auto tag = conf->get<std::string>("tag");

    auto heavyion = conf->get<bool>("heavyion");
    auto mc_branches = conf->get<bool>("mc_branches");
    auto hlt_branches = conf->get<bool>("hlt_branches");
    auto apply_weights = conf->get<bool>("apply_weights");
    auto active = conf->get<std::vector<bool>>("active");

    auto jet_dir = conf->get<std::string>("jet_dir");
    auto photon_dir = conf->get<std::string>("photon_dir");
    auto rho_dir = conf->get<std::string>("rho_dir");
    auto pf_path = conf->get<std::string>("pf_path");
    auto selections = conf->get<std::vector<std::string>>("selections");
    auto paths = conf->get<std::vector<std::string>>("paths");
    auto paths_only = conf->get<bool>("paths_only");

    auto xmls = conf->get<std::vector<std::string>>("xmls");
    auto xmls_new = conf->get<std::vector<std::string>>("xmls_new");

    auto jecs = conf->get<std::vector<std::string>>("jecs");
    auto jecs_scale = conf->get<std::vector<std::string>>("jecs_scale");

    auto csn = conf->get<std::vector<float>>("csn");
    auto pthat = conf->get<std::vector<int32_t>>("pthat");
    auto pthatw = conf->get<std::vector<float>>("pthatw");
    auto vzw = conf->get<std::vector<float>>("vzw");

    auto use_half = conf->get<bool>("use_half");
    auto half = conf->get<bool>("half");

    for (auto& v : csn) { v = v * v; }

    /* load forest */
    auto forest = new train(files);
    auto chain_evt = forest->attach("hiEvtAnalyzer/HiTree", true);
    auto chain_sel = forest->attach("skimanalysis/HltTree", true);
    auto chain_eg = forest->attach((photon_dir + "/EventTree").data(), !photon_dir.empty());
    auto chain_jet = forest->attach((jet_dir + "/t").data(), !jet_dir.empty());
    auto chain_hlt = forest->attach("hltanalysis/HltTree", hlt_branches);
    auto chain_rho = forest->attach(rho_dir + "/t", heavyion);
    auto chain_pf = forest->attach(pf_path, true);
    auto chain_trk = forest->attach("ppTrack/trackTree", !heavyion);

    (*forest)();

    auto tevt = harvest<event>(chain_evt, mc_branches);
    auto tsel = harvest<triggers>(chain_sel, selections);
    auto tegg = harvest<eggen>(chain_eg, mc_branches);
    auto tele = harvest<electrons>(chain_eg);
    auto tpho = harvest<photons>(chain_eg);
    auto tjet = harvest<jets>(chain_jet, mc_branches, array_size);
    auto thlt = harvest<triggers>(chain_hlt, paths);
    auto trho = harvest<evtrho>(chain_rho);
    auto tpf = harvest<pfcand>(chain_pf);
    auto ttrk = harvest<tracks>(chain_trk);

    std::array<bool, tt::ntt> flags = { tevt != nullptr, tegg != nullptr, tpho != nullptr, tele != nullptr, tjet != nullptr, thlt != nullptr, trho != nullptr, tpf != nullptr, ttrk != nullptr };
    std::transform(flags.begin(), flags.end(), active.begin(), flags.begin(),
                   [](bool a, bool b) -> bool { return a && b; });

    /* setup output tree */
    TTree::SetMaxTreeSize(1000000000000LL);

    TFile* fout = new TFile(output, "recreate");
    TTree* tout = new TTree("pj", "photon-jet");
    auto tree_pj = new pjtree(tout, mc_branches, hlt_branches, heavyion, flags);

    /* weights, corrections */
    auto JEC = new JetCorrector(jecs);
    auto JEC_scale = new JetCorrector(jecs_scale);

    TF1* fweight = new TF1("fweight", "(gaus(0))/(gaus(3))");
    if (mc_branches && apply_weights) {
        fweight->SetParameters(vzw[0], vzw[1], vzw[2], vzw[3], vzw[4], vzw[5]); 
    }

    auto regr = new phoERegression();
    if (!xmls.empty()) {
        regr->initialiseReaderEB(xmls[0]);
        regr->initialiseReaderEE(xmls[1]);
    }

    auto regr_new = new phoERegressionNew();
    if (!xmls_new.empty()) {
        regr_new->initialiseReaderEB(xmls_new[0]);
        regr_new->initialiseReaderEE(xmls_new[1]);
    }

    std::vector<float> regr_variables(17, 0);
    auto fill_regr_variables = [&](int64_t index) {
        regr_variables[0] = (*tree_pj->phoSCRawE)[index];
        regr_variables[1] = (*tree_pj->phoSCEta)[index];
        regr_variables[2] = (*tree_pj->phoSCPhi)[index];
        regr_variables[3] = (*tree_pj->phoSCEtaWidth)[index];
        regr_variables[4] = (*tree_pj->phoSCPhiWidth)[index];
        regr_variables[5] = (*tree_pj->phoE3x3_2012)[index];
        regr_variables[6] = (*tree_pj->phoMaxEnergyXtal_2012)[index];
        regr_variables[7] = (*tree_pj->phoE2nd_2012)[index];
        regr_variables[8] = (*tree_pj->phoELeft_2012)[index];
        regr_variables[9] = (*tree_pj->phoERight_2012)[index];
        regr_variables[10] = (*tree_pj->phoETop_2012)[index];
        regr_variables[11] = (*tree_pj->phoEBottom_2012)[index];
        regr_variables[12] = (*tree_pj->phoSigmaIEtaIEta_2012)[index];
        regr_variables[13] = (*tree_pj->phoSigmaIEtaIPhi_2012)[index];
        regr_variables[14] = (*tree_pj->phoSigmaIPhiIPhi_2012)[index];
        regr_variables[15] = tree_pj->rho;
        regr_variables[16] = (*tree_pj->phoESEn)[index];
    };

    auto rng = new TRandom3(144);

    /* boom! */
    int64_t nentries = forest->count();
    if (max_entries) nentries = std::min(nentries, max_entries);

    int64_t modulo = use_half ? 2 : 1;

    for (int64_t i = 0; i < nentries; ++i) {
        tree_pj->clear();

        if (i % 10000 == 0)
            printf("entry: %li/%li\n", i, nentries);

        if ((i + half) % modulo != 0) { continue; }

        forest->get(i);

        if (!selections.empty()) {
            bool pass_selection = true;
            for (auto const& selection : selections)
                pass_selection &= (tsel->accept(selection) == 1);

            if (!pass_selection) { continue; }
        }

        if (!paths.empty()) {
            if (tpho->nPho < 1) { continue; }

            bool pass_skim = false;
            for (auto const& path : paths)
                if (thlt->accept(path) == 1)
                    pass_skim = true;

            if (!pass_skim && paths_only) { continue; }
        }

        tree_pj->copy(tevt, tegg, tpho, tele, tjet, thlt, trho, tpf, ttrk);

        if (!heavyion) {
            tree_pj->hiBin = 0;
            tree_pj->hiHF = 0;
            tree_pj->Ncoll = 1000;
        }

        if (mc_branches) {
            if (apply_weights)
                tree_pj->w = fweight->Eval(tree_pj->vz) * weight_for(pthat, pthatw, tree_pj->pthat) * tree_pj->weight;
            else
                tree_pj->w = tree_pj->weight;
        }
        else
            tree_pj->w = 1.f;
        
        /* apply jet energy corrections and evaluate uncertainties */
        for (int64_t j = 0; j < tree_pj->nref; ++j) {
            float jet_area = 0.3 * 0.3 * 3.14159265359;
            auto avg_rho = get_UE(tree_pj, (*tree_pj->jteta)[j]) / jet_area;

            JEC->SetJetPT((*tree_pj->jtpt)[j]);
            JEC->SetJetEta((*tree_pj->jteta)[j]);
            JEC->SetJetPhi((*tree_pj->jtphi)[j]);
            JEC->SetJetArea(jet_area);
            JEC->SetRho(avg_rho);

            JEC_scale->SetJetPT((*tree_pj->jtpt)[j]);
            JEC_scale->SetJetEta((*tree_pj->jteta)[j]);
            JEC_scale->SetJetPhi((*tree_pj->jtphi)[j]);
            JEC_scale->SetJetArea(jet_area);
            JEC_scale->SetRho(avg_rho);

            if (!jecs.empty()) tree_pj->jtptCor->push_back(JEC->GetCorrectedPT());
            if (!jecs_scale.empty()) tree_pj->jtptCorScale->push_back(JEC_scale->GetCorrectedPT());

            if (!csn.empty()) { 
                (*tree_pj->jtpt)[j] *= smear(csn, (*tree_pj->jtpt)[j]) > 0 ? rng->Gaus(1., smear(csn, (*tree_pj->jtpt)[j])) : 1;
                if (!jecs.empty()) (*tree_pj->jtptCor)[j] *= smear(csn, (*tree_pj->jtptCor)[j]) > 0 ? rng->Gaus(1., smear(csn, (*tree_pj->jtptCor)[j])) : 1;
                if (!jecs_scale.empty()) (*tree_pj->jtptCorScale)[j] *= smear(csn, (*tree_pj->jtptCorScale)[j]) > 0 ? rng->Gaus(1., smear(csn, (*tree_pj->jtptCorScale)[j])) : 1;
            }
        }

        /* apply photon energy corrections */
        if (!xmls.empty() || !xmls_new.empty()) {
            for (int64_t j = 0; j < tree_pj->nPho; ++j) {
                fill_regr_variables(j);
                auto phoEt = (*tree_pj->phoEt)[j];
                auto phoEta = (*tree_pj->phoEta)[j];
                auto phoSCEta = (*tree_pj->phoSCEta)[j];
                if (!xmls.empty())
                    tree_pj->phoEtEr->push_back(regr->getCorrectedPt(regr_variables, phoEt, phoEta, phoSCEta));
                if (!xmls_new.empty())
                    tree_pj->phoEtErNew->push_back(regr_new->getCorrectedPt(regr_variables, phoEt, phoEta, phoSCEta));
            }
        }

        tout->Fill();
    }

    fout->Write("", TObject::kOverwrite);
    fout->Close();
    
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return regulate(argv[1], argv[2]);

    return 0;
}