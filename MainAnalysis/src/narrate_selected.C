#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

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

int narrate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto data = conf->get<std::string>("data");
    auto files = conf->get<std::vector<std::string>>("files");

    auto system = conf->get<std::string>("system");
    auto tag = conf->get<std::string>("tag");

    auto eta_min = conf->get<std::vector<double>>("eta_min");
    auto eta_max = conf->get<std::vector<double>>("eta_max");
    auto bound_string = conf->get<std::vector<std::string>>("bound_string");
    
    auto rho_range = conf->get<std::vector<double>>("rho_range");
    auto dhf = conf->get<std::vector<float>>("hf_diff");
    auto dcent = conf->get<std::vector<int32_t>>("cent_diff");

    TH1::SetDefaultSumw2();
    
    auto ihf = new interval(dhf);
    auto irho = new interval("rho"s, rho_range[0], rho_range[1], rho_range[2]);
    auto frho = std::bind(&interval::book<TH1F>, irho, _1, _2, _3);

    auto dim_1_size = static_cast<int64_t>(eta_min.size());
    auto dim_2_size = static_cast<int64_t>(dhf.size()-1);

    auto rho_data = new history<TH1F>("rho_data"s, "", frho, dim_1_size, dim_2_size);
    auto rho_mc = new history<TH1F>("rho_mc"s, "", frho, dim_1_size, dim_2_size);
    auto rho_ratio = new history<TH1F>("rho_ratio"s, "", frho, dim_1_size, dim_2_size);


    TFile* f = new TFile(data.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto pjt = new pjtree(false, false, true, t, { 1, 0, 1, 1, 0, 0, 1, 0 });

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
            if ((*pjt->phoEt)[j] <= 40) { continue; }
            if (std::abs((*pjt->phoSCEta)[j]) >= eta_max[0]) { continue; }
            if ((*pjt->phoHoverE)[j] > 0.119947) { continue; }

            if ((*pjt->phoEt)[j] > leading_pt) {
                leading = j;
                leading_pt = (*pjt->phoEt)[j];
            }
        }

        if (leading < 0) { continue; }
        if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > 0.010392) { continue; }
        if (in_pho_failure_region(pjt, leading)) { continue; }

        float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
            + (*pjt->pho_hcalRechitIsoR3)[leading]
            + (*pjt->pho_trackIsoR3PtCut20)[leading];
        if (isolation > 2.099277) { continue; }

        for (size_t j = 0; j < eta_min.size(); ++j) {
            auto eta_x = static_cast<int64_t>(j);
            auto avg_rho = get_avg_rho(pjt, eta_min[j], eta_max[j]);

            (*rho_data)[rho_data->index_for(x{eta_x,hf_x})]->Fill(avg_rho);
        }
    }

    f->Close();
    delete f;
    delete pjt;

    for (auto const& file : files) {
        f = new TFile(file.data(), "read");
        t = (TTree*)f->Get("pj");
        pjt = new pjtree(false, false, true, t, { 1, 0, 1, 1, 0, 0, 1, 0 });

        nentries = static_cast<int64_t>(t->GetEntries());
        // nentries = 10000;

        for (int64_t i = 0; i < nentries-1; ++i) {
            if (i % 100000 == 0)
                printf("entry: %li/%li\n", i, nentries);

            t->GetEntry(i);

            if (std::abs(pjt->vz) > 15) { continue; }

            int64_t leading = -1;
            float leading_pt = 0;
            for (int64_t j = 0; j < pjt->nPho; ++j) {
                if ((*pjt->phoEt)[j] <= 40) { continue; }
                if (std::abs((*pjt->phoSCEta)[j]) >= eta_max[0]) { continue; }
                if ((*pjt->phoHoverE)[j] > 0.119947) { continue; }

                if ((*pjt->phoEt)[j] > leading_pt) {
                    leading = j;
                    leading_pt = (*pjt->phoEt)[j];
                }
            }

            if (leading < 0) { continue; }
            if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > 0.010392) { continue; }
            if (in_pho_failure_region(pjt, leading)) { continue; }

            float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
                + (*pjt->pho_hcalRechitIsoR3)[leading]
                + (*pjt->pho_trackIsoR3PtCut20)[leading];
            if (isolation > 2.099277) { continue; }

            for (size_t j = 0; j < eta_min.size(); ++j) {
                for (size_t k = 0; k < dhf.size()-1; ++k) {
                    auto eta_x = static_cast<int64_t>(j);
                    auto hf_x = static_cast<int64_t>(k);
                    auto index = rho_data->index_for(x{eta_x,hf_x});
                    auto avg_rho = get_avg_rho(pjt, eta_min[j], eta_max[j]);

                    (*rho_mc)[index]->Fill(avg_rho, pjt->w);
                }
            }
        }

        f->Close();
        delete f;
        delete pjt;
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
    }

    /* draw rho distributions */
    auto hf_info = [&](int64_t index) {
        info_text(index, 0.75, "%i - %i%%", dcent, true); };

    for (size_t i = 0; i < eta_min.size(); ++i) {
        auto hb = new pencil();
        hb->category("type", "Data", "MC");
        
        auto c1 = new paper(tag + "_rho_distribution_" + bound_string[i], hb);
        apply_style(c1, system + " #sqrt{s} = 5.02 TeV"s);
        c1->accessory(hf_info);
        c1->divide(ihf->size(), -1);
        c1->set(paper::flags::logy);

        for (size_t j = 0; j < dhf.size()-1; ++j) {
            auto eta_x = static_cast<int64_t>(i);
            auto hf_x = static_cast<int64_t>(j);
            auto index = rho_data->index_for(x{eta_x,hf_x});
            
            c1->add((*rho_mc)[index], "MC");
            c1->stack((*rho_data)[index], "Data");
        }

        hb->sketch();
        c1->draw("pdf");
    }

    for (size_t i = 0; i < eta_min.size(); ++i) {
        auto hb = new pencil();
        hb->category("type", "Data/MC");
        
        auto c1 = new paper(tag + "_rho_weight_" + bound_string[i], hb);
        apply_style(c1, system + " #sqrt{s} = 5.02 TeV"s);
        c1->accessory(hf_info);
        c1->divide(ihf->size(), -1);
        c1->set(paper::flags::logy);

        for (size_t j = 0; j < dhf.size()-1; ++j) {
            auto eta_x = static_cast<int64_t>(i);
            auto hf_x = static_cast<int64_t>(j);
            auto index = rho_ratio->index_for(x{eta_x,hf_x});
            
            c1->add((*rho_ratio)[index], "Data/MC");
        }

        hb->sketch();
        c1->draw("pdf");
    }

    /* save output */
    in(output, [&]() {
        rho_data->save(tag);
        rho_mc->save(tag);
        rho_ratio->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return narrate(argv[1], argv[2]);

    return 0;
}
