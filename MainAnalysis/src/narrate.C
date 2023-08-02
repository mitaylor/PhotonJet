#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"
#include "../include/text.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TH1F.h"
#include "TLatex.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int narrate(char const* config, char const* selections, char const* output) {
    auto conf = new configurer(config);

    auto data = conf->get<std::vector<std::string>>("data");
    auto files = conf->get<std::vector<std::string>>("files");

    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto eta_min = conf->get<std::vector<double>>("eta_min");
    auto eta_max = conf->get<std::vector<double>>("eta_max");
    auto bound_string = conf->get<std::vector<std::string>>("bound_string");
    
    auto rho_range = conf->get<std::vector<double>>("rho_range");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    /* selections */
    auto sel = new configurer(selections);

    auto set = sel->get<std::string>("set");

    auto const photon_pt_min = sel->get<float>("photon_pt_min");
    auto const photon_pt_max = sel->get<float>("photon_pt_max");
    auto const hovere_max = sel->get<float>("hovere_max");
    auto const see_min = sel->get<float>("see_min");
    auto const see_max = sel->get<float>("see_max");
    auto const iso_max = sel->get<float>("iso_max");

    TH1::SetDefaultSumw2();
    
    auto ihf = new interval(dhf);
    auto irho = new interval("#rho"s, rho_range[0], rho_range[1], rho_range[2]);
    auto frho = std::bind(&interval::book<TH1F>, irho, _1, _2, _3);

    auto dim_1_size = static_cast<int64_t>(eta_min.size());
    auto dim_2_size = static_cast<int64_t>(dhf.size()-1);

    auto rho_data = new history<TH1F>("rho_data"s, "", frho, dim_1_size, dim_2_size);
    auto rho_mc = new history<TH1F>("rho_mc"s, "", frho, dim_1_size, dim_2_size);
    auto rho_ratio = new history<TH1F>("rho_ratio"s, "", frho, dim_1_size, dim_2_size);

    auto rho_data_merge = new history<TH1F>("rho_data_merge"s, "", frho, dim_1_size, 1);
    auto rho_mc_merge = new history<TH1F>("rho_mc_merge"s, "", frho, dim_1_size, 1);
    auto rho_ratio_merge = new history<TH1F>("rho_ratio_merge"s, "", frho, dim_1_size, 1);

    for (auto const& file : data) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(false, false, true, t, { 1, 0, 1, 1, 0, 0, 1, 0, 0 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());
        // int64_t nentries = 10000;

        for (int64_t i = 0; i < nentries-1; ++i) {
            if (i % 100000 == 0)
                printf("entry: %li/%li\n", i, nentries);

            t->GetEntry(i);
            auto hf_x = ihf->index_for(pjt->hiHF);

            if (hf_x < 0) continue;
            if (hf_x == ihf->size()) continue;

            if (std::abs(pjt->vz) > 15) { continue; }

            int64_t leading = -1;
            float leading_pt = 0;
            for (int64_t j = 0; j < pjt->nPho; ++j) {
                if ((*pjt->phoEt)[j] <= 30) { continue; }
                if (std::abs((*pjt->phoSCEta)[j]) >= eta_max[0]) { continue; }
                if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }

                auto pho_et = (*pjt->phoEtErNew)[j];

                if (pho_et < photon_pt_min || pho_et > photon_pt_max) { continue; }

                if (pho_et > leading_pt) {
                    leading = j;
                    leading_pt = pho_et;
                }
            }

            if (leading < 0) { continue; }

            if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > see_max
                    || (*pjt->phoSigmaIEtaIEta_2012)[leading] < see_min)
                continue;

            if (in_pho_failure_region(pjt, leading)) { continue; }

            float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
                    + (*pjt->pho_hcalRechitIsoR3)[leading]
                    + (*pjt->pho_trackIsoR3PtCut20)[leading];
            if (isolation > iso_max) { continue; }

            for (size_t j = 0; j < eta_min.size(); ++j) {
                auto eta_x = static_cast<int64_t>(j);
                auto avg_rho = get_avg_rho(pjt, eta_min[j], eta_max[j]);

                (*rho_data)[rho_data->index_for(x{eta_x,hf_x})]->Fill(avg_rho);
                (*rho_data_merge)[eta_x]->Fill(avg_rho);
            }
        }
    }

    for (auto const& file : files) {
        std::cout << file << std::endl;

        TFile* f = new TFile(file.data(), "read");
        TTree* t = (TTree*)f->Get("pj");
        auto pjt = new pjtree(false, false, true, t, { 1, 0, 1, 1, 0, 0, 1, 0, 0 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());
        // nentries = 10000;

        for (int64_t i = 0; i < nentries-1; ++i) {
            if (i % 100000 == 0)
                printf("entry: %li/%li\n", i, nentries);

            t->GetEntry(i);

            if (std::abs(pjt->vz) > 15) { continue; }

            int64_t leading = -1;
            float leading_pt = 0;
            for (int64_t j = 0; j < pjt->nPho; ++j) {
                if ((*pjt->phoEt)[j] <= 30) { continue; }
                if (std::abs((*pjt->phoSCEta)[j]) >= eta_max[0]) { continue; }
                if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }

                auto pho_et = (*pjt->phoEtErNew)[j];

                if (pho_et < photon_pt_min || pho_et > photon_pt_max) { continue; }

                if (pho_et > leading_pt) {
                    leading = j;
                    leading_pt = pho_et;
                }
            }

            if (leading < 0) { continue; }

            if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > see_max
                    || (*pjt->phoSigmaIEtaIEta_2012)[leading] < see_min)
                continue;

            if (in_pho_failure_region(pjt, leading)) { continue; }

            float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
                    + (*pjt->pho_hcalRechitIsoR3)[leading]
                    + (*pjt->pho_trackIsoR3PtCut20)[leading];
            if (isolation > iso_max) { continue; }

            for (size_t j = 0; j < eta_min.size(); ++j) {
                for (size_t k = 0; k < dhf.size()-1; ++k) {
                    auto eta_x = static_cast<int64_t>(j);
                    auto hf_x = static_cast<int64_t>(k);
                    auto index = rho_data->index_for(x{eta_x,hf_x});
                    auto avg_rho = get_avg_rho(pjt, eta_min[j], eta_max[j]);

                    (*rho_mc)[index]->Fill(avg_rho, pjt->w);
                    (*rho_mc_merge)[eta_x]->Fill(avg_rho, pjt->w);
                }
            }
        }
    }

    for (size_t i = 0; i < eta_min.size(); ++i) {
        for (size_t j = 0; j < dhf.size()-1; ++j) {
            auto eta_x = static_cast<int64_t>(i);
            auto hf_x = static_cast<int64_t>(j);
            auto index = rho_data->index_for(x{eta_x,hf_x});

            (*rho_data)[index]->Scale(1. / (*rho_data)[index]->Integral());
            (*rho_mc)[index]->Scale(1. / (*rho_mc)[index]->Integral());

            (*rho_data)[index]->SetMaximum(10);
            (*rho_data)[index]->SetMinimum(1E-7);
            (*rho_mc)[index]->SetMaximum(10);
            (*rho_mc)[index]->SetMinimum(1E-7);

            (*rho_ratio)[index]->Divide((*rho_data)[index], (*rho_mc)[index]);
            (*rho_ratio)[index]->SetMaximum(100);
            (*rho_ratio)[index]->SetMinimum(1E-3);
        }

        auto eta_x = static_cast<int64_t>(i);

        (*rho_data_merge)[eta_x]->Scale(1. / (*rho_data_merge)[eta_x]->Integral());
        (*rho_mc_merge)[eta_x]->Scale(1. / (*rho_mc_merge)[eta_x]->Integral());

        (*rho_data_merge)[eta_x]->SetMaximum(10);
        (*rho_data_merge)[eta_x]->SetMinimum(1E-7);
        (*rho_mc_merge)[eta_x]->SetMaximum(10);
        (*rho_mc_merge)[eta_x]->SetMinimum(1E-7);

        (*rho_ratio_merge)[eta_x]->Divide((*rho_data_merge)[eta_x], (*rho_mc_merge)[eta_x]);
        (*rho_ratio_merge)[eta_x]->SetMaximum(100);
        (*rho_ratio_merge)[eta_x]->SetMinimum(1E-3);
    }

    /* draw rho distributions */
    auto system_tag = system + "  #sqrt{s_{NN}} = 5.02 TeV"s;
    auto cms = "#bf{#scale[1.4]{CMS}} #it{#scale[1.2]{Preliminary}}"s;
    cms += "         p_{T}^{#gamma} > "s + to_text(photon_pt_min) + " GeV"s;

    auto hf_info = [&](int64_t index) {
        info_text(index, 0.75, "Cent. %i - %i%%", dcent, true); };

    for (size_t i = 0; i < eta_min.size(); ++i) {
        auto hb = new pencil();
        hb->category("type", "Data", "MC");
        
        auto c1 = new paper(set + "_" + tag + "_rho_distribution_" + bound_string[i], hb);
        apply_style(c1, cms, system_tag);
        c1->accessory(hf_info);
        c1->divide(ihf->size()/2, -1);
        c1->set(paper::flags::logy);

        for (size_t j = 0; j < dhf.size()-1; ++j) {
            auto eta_x = static_cast<int64_t>(i);
            auto hf_x = static_cast<int64_t>(j);
            auto index = rho_data->index_for(x{eta_x,hf_x});
            
            c1->add((*rho_mc)[index], "MC");
            c1->stack((*rho_data)[index], "Data");
        }

        auto c2 = new paper(set + "_" + tag + "_rho_distribution_merge_" + bound_string[i], hb);
        apply_style(c2, cms, system_tag);
        // c2->accessory(hf_info);
        c2->set(paper::flags::logy);

        c2->add((*rho_mc_merge)[static_cast<int64_t>(i)], "MC");
        c2->stack((*rho_data_merge)[static_cast<int64_t>(i)], "Data");

        hb->sketch();
        c1->draw("pdf");
        c2->draw("pdf");
    }

    for (size_t i = 0; i < eta_min.size(); ++i) {
        auto hb = new pencil();
        hb->category("type", "Data/MC");
        
        auto c1 = new paper(set + "_" + tag + "_rho_weight_" + bound_string[i], hb);
        apply_style(c1, cms, system_tag);
        c1->accessory(hf_info);
        c1->divide(ihf->size()/2, -1);
        c1->set(paper::flags::logy);

        for (size_t j = 0; j < dhf.size()-1; ++j) {
            auto eta_x = static_cast<int64_t>(i);
            auto hf_x = static_cast<int64_t>(j);
            auto index = rho_ratio->index_for(x{eta_x,hf_x});
            
            c1->add((*rho_ratio)[index], "Data/MC");
        }

        auto c2 = new paper(set + "_" + tag + "_rho_weight_merge_" + bound_string[i], hb);
        apply_style(c2, cms, system_tag);
        // c2->accessory(hf_info);
        c2->set(paper::flags::logy);

        c2->add((*rho_ratio_merge)[static_cast<int64_t>(i)], "Data/MC");

        hb->sketch();
        c1->draw("pdf");
        c2->draw("pdf");
    }

    /* save output */
    in(output, [&]() {
        rho_data->save(tag);
        rho_mc->save(tag);
        rho_ratio->save(tag);

        rho_data_merge->save(tag);
        rho_mc_merge->save(tag);
        rho_ratio_merge->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 4)
        return narrate(argv[1], argv[2], argv[3]);

    printf("usage: %s [config] [selections] [output]\n", argv[0]);
    return 1;
}
