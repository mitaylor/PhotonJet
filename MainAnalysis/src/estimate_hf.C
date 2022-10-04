#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"
#include "../git/tricks-and-treats/include/zip.h"

#include "../git/paper-and-pencil/include/paper.h"
#include "../git/paper-and-pencil/include/pencil.h"

#include "TFile.h"
#include "TTree.h"
#include "TLatex.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"
#include "TProfile.h"

#include <memory>
#include <string>
#include <vector>
#include <ctime>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int estimate_hf(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    
    auto entries = conf->get<int64_t>("entries");
    auto frequency = conf->get<int64_t>("frequency");
    auto tag = conf->get<std::string>("tag");
    auto type = conf->get<std::string>("type");

    /* options */
    auto heavyion = conf->get<bool>("heavyion");
    auto ele_rej = conf->get<bool>("electron_rejection");
    auto apply_er = conf->get<bool>("apply_er");

    /* selections */
    auto const photon_pt_min = conf->get<float>("photon_pt_min");
    auto const photon_eta_abs = conf->get<float>("photon_eta_abs");
    auto const hovere_max = conf->get<float>("hovere_max");
    auto const iso_max = conf->get<float>("iso_max");

    auto dpt = conf->get<std::vector<float>>("pt_diff");

    /* create histograms */
    auto ipt = new interval(dpt);
    auto ihf = new interval("Estimated HF"s, 50, 0, 250);
    auto fhf = std::bind(&interval::book<TH1F>, ihf, _1, _2, _3);
    auto fnvtx = [&](int64_t, std::string const& name, std::string const& label) {
        return new TProfile(name.data(), (";nVtx;HF Energy;"s + label).data(), 18, 0, 18, 0, 7000, "LE"); };

    auto hf = new history<TH1F>("hf"s, "", fhf, ipt->size());
    auto nvtx = new history<TProfile>("nvtx"s, "", fnvtx, 1);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    TFile* f = new TFile(input.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto pjt = new pjtree(false, false, heavyion, t, { 1, 1, 1, 1, 1, 0, heavyion, 1, 1 });

    printf("iterate..\n");

    int64_t nentries = static_cast<int64_t>(t->GetEntries());
    int64_t mod = 1;
    if (entries) { mod = nentries / entries; }
    if (mod != 1) { std::cout << "mod: " << mod << std::endl; }

    for (int64_t i = 0; i < nentries; ++i) {
        if (i % frequency == 0) { printf("entry: %li/%li\n", i, nentries); }
        if (i % mod != 0) { continue; }

        t->GetEntry(i);

        if (std::abs(pjt->vz) > 15) { continue; }

        int64_t leading = -1;
        float leading_pt = 0;
        for (int64_t j = 0; j < pjt->nPho; ++j) {
            if ((*pjt->phoEt)[j] <= 30) { continue; }
            if (std::abs((*pjt->phoSCEta)[j]) >= photon_eta_abs) { continue; }
            if ((*pjt->phoHoverE)[j] > hovere_max) { continue; }

            auto pho_et = (*pjt->phoEt)[j];
            if (heavyion && apply_er) pho_et = (*pjt->phoEtErNew)[j];
            if (!heavyion && apply_er) pho_et = (*pjt->phoEtEr)[j];

            if (pho_et < photon_pt_min) { continue; }

            if (pho_et > leading_pt) {
                leading = j;
                leading_pt = pho_et;
            }
        }

        /* require leading photon */
        if (leading < 0) { continue; }

        if ((*pjt->phoSigmaIEtaIEta_2012)[leading] > 0.02)
            continue;

        /* isolation requirement */
        float isolation = (*pjt->pho_ecalClusterIsoR3)[leading]
            + (*pjt->pho_hcalRechitIsoR3)[leading]
            + (*pjt->pho_trackIsoR3PtCut20)[leading];
        if (isolation > iso_max) { continue; }

        /* leading photon axis */
        auto photon_eta = (*pjt->phoEta)[leading];
        auto photon_phi = convert_radian((*pjt->phoPhi)[leading]);

        /* electron rejection */
        if (ele_rej) {
            bool electron = false;
            for (int64_t j = 0; j < pjt->nEle; ++j) {
                if (std::abs((*pjt->eleSCEta)[j]) > 1.4442) { continue; }

                auto deta = photon_eta - (*pjt->eleEta)[j];
                if (deta > 0.1) { continue; }

                auto ele_phi = convert_radian((*pjt->elePhi)[j]);
                auto dphi = revert_radian(photon_phi - ele_phi);
                auto dr2 = deta * deta + dphi * dphi;

                if (dr2 < 0.01 && passes_electron_id<
                            det::barrel, wp::loose, pjtree
                        >(pjt, j, heavyion)) {
                    electron = true; break; }
            }

            if (electron) { continue; }
        }

        if (leading_pt > 200) { continue; }
        auto pt_x = ipt->index_for(leading_pt);

        float pf_sum = 0;

        for (size_t j = 0; j < pjt->pfPt->size(); ++j) {
            // if (std::abs((*pjt->pfEta)[j]) > 3 && std::abs((*pjt->pfEta)[j]) < 5) {
            if ((*pjt->pfId)[j] >= 6) {
                pf_sum += (*pjt->pfPt)[j];
            }
        }

        if (pjt->nVtx == 1) { 
            (*hf)[pt_x]->Fill(pf_sum, pjt->w);
        }

        (*nvtx)[0]->Fill(pjt->nVtx, pf_sum, pjt->w);

    }

    /* save histograms */
    in(output, [&]() {
        hf->save(tag);
        nvtx->save(tag);
    });

    auto pt_info = [&](int64_t index) {
        info_text(index, 0.75, "%.0f < p_{T}^{#gamma} < %.0f", dpt, false); };

    auto mean_info = [&](int64_t index) {
        char buffer[128] = { '\0' };
        sprintf(buffer, "mean: %.3f",
            (*hf)[index - 1]->GetMean(1));

        TLatex* text = new TLatex();
        text->SetTextFont(43);
        text->SetTextSize(12);
        text->DrawLatexNDC(0.54, 0.75, buffer);
    };

    auto hb = new pencil();
    hb->category("type", "Data", "MC");
    
    auto c1 = new paper(tag + "_estimated_hf", hb);
    apply_style(c1, "", "pp #sqrt{s} = 5.02 TeV"s);

    c1->accessory(pt_info);
    c1->accessory(mean_info);

    c1->divide(ipt->size(), -1);
    // c1->set(paper::flags::logy);

    for (int64_t j = 0; j < ipt->size(); ++j) {
        c1->add((*hf)[j], type);
    }

    hb->sketch();
    c1->draw("pdf");

    auto c2 = new paper(tag + "_nvtx_hf", hb);
    apply_style(c2, "", "pp #sqrt{s} = 5.02 TeV"s);

    c2->add((*nvtx)[0], type);

    hb->sketch();
    c2->draw("pdf");

    printf("destroying objects..\n");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return estimate_hf(argv[1], argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
