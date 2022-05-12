#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

int vacillate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto tag = conf->get<std::string>("tag");

    auto rho = conf->get<std::string>("rho");
    auto rho_label = conf->get<std::string>("rho_label");

    auto acc = conf->get<std::string>("acc");
    auto acc_label_ref = conf->get<std::string>("acc_label_ref");
    auto acc_label_acc = conf->get<std::string>("acc_label_acc");

    auto mod = conf->get<bool>("mod");
    auto parity = conf->get<bool>("parity");

    auto heavyion = conf->get<bool>("heavyion");
    auto apply_er = conf->get<bool>("apply_er");

    auto jet_eta_max = conf->get<float>("jet_eta_max");
    auto photon_pt_min = conf->get<float>("photon_pt_min");
    auto photon_eta_max = conf->get<float>("photon_eta_max");
    auto hovere_max = conf->get<float>("hovere_max");
    auto see_min = conf->get<float>("see_min");
    auto see_max = conf->get<float>("see_max");
    auto iso_max = conf->get<float>("iso_max");

    auto rdphir = conf->get<std::vector<float>>("dphir_range");
    auto rdphig = conf->get<std::vector<float>>("dphig_range");
    auto rptr = conf->get<std::vector<float>>("ptr_range");
    auto rptg = conf->get<std::vector<float>>("ptg_range");

    auto dhf = conf->get<std::vector<float>>("hf_diff");

    /* prepare histograms */
    auto incl = new interval(""s, 1, 0.f, 9999.f);
    auto ihf = new interval(dhf);

    auto mcdphi = new multival(rdphir, rdphig);
    auto mcpt = new multival(rptr, rptg);

    auto mr = new multival(rdphir, rptr);
    auto mg = new multival(rdphig, rptg);

    auto fn = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);
    auto fcdphi = std::bind(&multival::book<TH2F>, mcdphi, _1, _2, _3);
    auto fcpt = std::bind(&multival::book<TH2F>, mcpt, _1, _2, _3);

    auto fr = [&](int64_t, std::string const& name, std::string const& label) {
        return new TH1F(name.data(), (";reco;"s + label).data(),
            mr->size(), 0, mr->size()); };

    auto fg = [&](int64_t, std::string const& name, std::string const& label) {
        return new TH1F(name.data(), (";gen;"s + label).data(),
            mg->size(), 0, mg->size()); };

    auto fc = [&](int64_t, std::string const& name, std::string const& label) {
        return new TH2F(name.data(), (";reco;gen;"s + label).data(),
            mr->size(), 0, mr->size(), mg->size(), 0, mg->size()); };

    auto n = new history<TH1F>("n"s, "events", fn, ihf->size());
    auto r = new history<TH1F>("r"s, "counts", fr, ihf->size());
    auto g = new history<TH1F>("g"s, "counts", fg, ihf->size());
    auto c = new history<TH2F>("c"s, "counts", fc, ihf->size());

    auto cdphi = new history<TH2F>("cdphi"s, "counts", fcdphi, ihf->size());
    auto cpt = new history<TH2F>("cpt"s, "counts", fcpt, ihf->size());

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    TFile* f = new TFile(input.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto p = new pjtree(true, false, heavyion, t, { 1, 1, 1, 0, 1, 0, heavyion});

    /* load centrality weighting for MC */
    TFile* frho;
    history<TH1F>* rho_weighting = nullptr;

    if (!rho.empty()) {
        frho = new TFile(rho.data(), "read");
        rho_weighting = new history<TH1F>(frho, rho_label);
    }

    /* load acceptance weighting for HI */
    TFile* fa;
    history<TH2F>* acceptance = nullptr;
    history<TH2F>* total = nullptr;

    if (!acc.empty()) {
        fa = new TFile(acc.data(), "read");
        acceptance = new history<TH2F>(fa, acc_label_acc);
        total = new history<TH2F>(fa, acc_label_ref);
    }

    int64_t nentries = static_cast<int64_t>(t->GetEntries());

    /* fill histograms */
    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 100000 == 0) { printf("%li/%li\n", i, nentries); }

        if (mod) {
            if ((i + parity) % 2 == 0) { continue; }
        }

        t->GetEntry(i);

        int64_t leading = -1;
        float leading_pt = 0;
        for (int64_t j = 0; j < p->nPho; ++j) {
            if ((*p->phoEt)[j] <= 30) { continue; }
            if (std::abs((*p->phoSCEta)[j]) >= photon_eta_max) { continue; }
            if ((*p->phoHoverE)[j] > hovere_max) { continue; }

            auto pho_et = (*p->phoEt)[j];
            if (heavyion && apply_er) pho_et = (*p->phoEtErNew)[j];
            if (!heavyion && apply_er) pho_et = (*p->phoEtEr)[j];

            if (pho_et < photon_pt_min) { continue; }

            if (pho_et > leading_pt) {
                leading = j;
                leading_pt = pho_et;
            }
        }

        /* require leading photon */
        if (leading < 0) { continue; }

        if ((*p->phoSigmaIEtaIEta_2012)[leading] > see_max
                || (*p->phoSigmaIEtaIEta_2012)[leading] < see_min)
            continue;

        /* hem failure region exclusion */
        if (heavyion && in_pho_failure_region(p, leading)) { continue; }

        /* require match to gen */
        auto gen_index = (*p->pho_genMatchedIndex)[leading];
        if (gen_index == -1) { continue; }

        auto pid = (*p->mcPID)[gen_index];
        auto mpid = (*p->mcMomPID)[gen_index];
        if (pid != 22 || (std::abs(mpid) > 22 && mpid != -999)) { continue; }

        /* isolation requirement */
        if ((*p->mcCalIsoDR04)[gen_index] > 5) { continue; }

        float isolation = (*p->pho_ecalClusterIsoR3)[leading]
            + (*p->pho_hcalRechitIsoR3)[leading]
            + (*p->pho_trackIsoR3PtCut20)[leading];
        if (isolation > iso_max) { continue; }

        /* photon axis */
        auto reco_photon_eta = (*p->phoEta)[leading];
        auto reco_photon_phi = convert_radian((*p->phoPhi)[leading]);
        auto gen_photon_phi = convert_radian((*p->mcPhi)[gen_index]);

        /* electron rejection */
        bool electron = false;
        for (int64_t j = 0; j < p->nEle; ++j) {
            if (std::abs((*p->eleSCEta)[j]) > 1.4442) { continue; }

            auto deta = reco_photon_eta - (*p->eleEta)[j];
            if (deta > 0.1) { continue; }

            auto ele_phi = convert_radian((*p->elePhi)[j]);
            auto dphi = revert_radian(reco_photon_phi - ele_phi);
            auto dr2 = deta * deta + dphi * dphi;

            if (dr2 < 0.01 && passes_electron_id<
                        det::barrel, wp::loose, pjtree
                    >(p, j, heavyion)) {
                electron = true; break; }
        }

        if (electron) { continue; }

        /* fill event weight */
        auto weight = p->w;

        std::vector<float> weights(ihf->size(), weight);
        
        if (heavyion) {
            auto avg_rho = get_avg_rho(pjt,-photon_eta_max,photon_eta_max);

            for (int64_t j = 0; j < ihf->size(); ++j) {
                auto bin = (*rho_weighting)[j]->FindBin(avg_rho);
                auto corr = (*rho_weighting)[j]->GetBinContent(bin);
                weights[j] *= corr;
            }
        }

        for (int64_t j = 0; j < ihf->size(); ++j) {
            (*n)[j]->Fill(1., weights[j]); }

        for (int64_t j = 0; j < p->nref; ++j) {
            auto gen_pt = (*p->refpt)[j];
            auto gen_phi = (*p->refphi)[j];

            if (gen_pt < rptg.front()) { continue; }

            auto reco_pt = (*p->jtpt)[j];
            auto reco_eta = (*p->jteta)[j];
            auto reco_phi = (*p->jtphi)[j];

            if (std::abs(reco_eta) >= jet_eta_max) { continue; }

            auto pj_deta = reco_photon_eta - reco_eta;
            auto pj_dphi = revert_radian(std::abs(reco_photon_phi - convert_radian(reco_phi)));
            auto pj_dr = std::sqrt(pj_deta * pj_deta + pj_dphi * pj_dphi);

            if (pj_dr < 0.4) { continue; }

            if (heavyion && in_jet_failure_region(p, j))
                continue;

            /* do acceptance weighting */
            double corr = 1;
            if (heavyion) {
                auto dphi_x = idphi->index_for(revert_pi(std::abs(reco_photon_phi - convert_radian(reco_phi))));
                auto bin = (*total)[dphi_x]->FindBin(jet_eta, photon_eta);
                corr = (*total)[dphi_x]->GetBinContent(bin) / (*acceptance)[dphi_x]->GetBinContent(bin);
                if (corr < 1) { std::cout << "error" << std::endl; }
            }

            auto gdphi = revert_pi(std::abs(gen_photon_phi - convert_radian(gen_phi)));
            auto g_x = mg->index_for(v{gdphi, gen_pt});

            for (int64_t k = 0; k < ihf->size(); ++k) {
                (*g)[k]->Fill(g_x, weights[k]*corr); }

            if (reco_pt > rptr.front() && reco_pt < rptr.back()) {
                auto rdphi = revert_pi(std::abs(reco_photon_phi - convert_radian(reco_phi)));
                auto r_x = mr->index_for(v{rdphi, reco_pt});

                for (int64_t k = 0; k < ihf->size(); ++k) {
                    (*r)[k]->Fill(r_x, weights[k]*corr);
                    (*cdphi)[k]->Fill(rdphi, gdphi, weights[k]*corr);
                    (*cpt)[k]->Fill(reco_pt, gen_pt, weights[k]*corr);
                    (*c)[k]->Fill(r_x, g_x, weights[k]*corr);
                }
            } else {
                /* missed */
                for (int64_t k = 0; k < ihf->size(); ++k) {
                    (*cpt)[k]->Fill(-1, gen_pt, weights[k]*corr);
                    (*c)[k]->Fill(-1, g_x, weights[k]*corr);
                }
            }
        }
    }

    r->divide(*n);
    g->divide(*n);

    /* save output */
    in(output, [&]() {
        n->save(tag);
        r->save(tag);
        g->save(tag);

        cdphi->save(tag);
        cpt->save(tag);
        c->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return vacillate(argv[1], argv[2]);

    return 0;
}