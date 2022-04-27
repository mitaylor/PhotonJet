#include "../include/lambdas.h"
#include "../include/pjtree.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/history.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TH1.h"
#include "TLatex.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std::literals::string_literals;
using namespace std::placeholders;

double get_avg_rho(pjtree* pjt, double eta_min, double eta_max) {
    double result = 0;
    int count = 0;

    if(pjt->etaMin == nullptr)
        return -1;

    int NBin = pjt->etaMin->size();
    if(NBin == 0)
        return -1;

    for(int i = 0; i < NBin; i++) {
        if ((*pjt->etaMax)[i] > eta_max)
            continue;
        if ((*pjt->etaMin)[i] < eta_min)
            continue;

        count++;
        result += (*pjt->evtRho)[i];
    }

    if (count > 0) return result/count;
    else return -1;
}

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

    auto rho_data = new history<TH1F>("rho_data"s, "", frho, eta_min.size(), dhf.size()-1);
    auto rho_mc = new history<TH1F>("rho_mc"s, "", frho, eta_min.size());

    TFile* f = new TFile(data.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto pjt = new pjtree(false, false, true, t, { 1, 0, 0, 0, 0, 0, 1 });

    int64_t nentries = static_cast<int64_t>(t->GetEntries());

    for (int64_t i = 0; i < nentries; ++i) {
        t->GetEntry(i);
        auto hf = ihf->index_for(pjt->hiHF);

        for (size_t j = 0; j < eta_min.size(); ++j) 
            (*rho_data)[rho_data->index_for(j,hf)]->Fill(get_avg_rho(pjt, eta_min[j], eta_max[j]))
    }

    f->Close();
    delete f;
    delete pjt;

    for (auto const& file : files) {
        f = new TFile(file.data(), "read");
        t = (TTree*)f->Get("pj");
        pjt = new pjtree(false, false, true, t, { 1, 0, 0, 0, 0, 0, 1 });

        int64_t nentries = static_cast<int64_t>(t->GetEntries());

        for (int64_t i = 0; i < nentries; ++i) {
            t->GetEntry(i);

            for (size_t j = 0; j < eta_min.size(); ++j) 
                (*rho_mc)[j]->Fill(get_avg_rho(pjt, eta_min[j], eta_max[j]))
        }

        f->Close();
        delete f;
        delete pjt;
    }

    for (size_t i = 0; i < eta_min.size(); ++i) {
        for (size_t j = 0; j < dhf.size()-1; ++j) 
            (*rho_data)[rho_data->index_for(i,j)]->Scale(1. / (*rho_data)[rho_data->index_for(i,j)]->Integral());
    
        (*rho_mc)[i]->Scale(1. / (*rho_mc)[i]->Integral());
    }

    /* draw rho distributions */
    auto hf_info = [&](int64_t index) {
        info_text(index, 0.75, "%i - %i%%", dcent, true); };

    auto hb = new pencil();
    hb->category("type", "Data", "MC");

    for (size_t i = 0; i < eta_min.size(); ++i) {
        auto c1 = new paper(tag + "_rho_distribution_" + bound_string[i], hb);
        apply_style(c1, system + " #sqrt{s} = 5.02 TeV"s);
        c1->accessory(hf_info);
        c1->divide(ihf->size(), -1);
        c1->set(paper::flags::logy);

        for (int64_t j = 0; j < dhf->size(); ++j) {
            c1->add((*rho_data)[rho_data->index_for(i,j)], "Data");
            c1->stack((*rho_mc)[i], "MC");
        }

        hb->sketch();
        c1->draw("pdf");
    }

    /* save output */
    in(output, [&]() {
        rho_data->save(tag);
        rho_mc->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return narrate(argv[1], argv[2]);

    return 0;
}
