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
#include "TTree.h"
#include "TRandom3.h"

#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

static float dr2(float eta1, float eta2, float phi1, float phi2) {
    auto deta = eta1 - eta2;
    auto dphi = revert_radian(convert_radian(phi1) - convert_radian(phi2));

    return deta * deta + dphi * dphi;
}

float res(float c, float s, float n, float pt) {
    return std::sqrt(c*c + s*s / pt + n*n / (pt * pt));
}

template <typename T>
static int sgn(T val) { return (T(0) < val) - (val < T(0)); }

int fabulate(char const* config, char const* output) {
    auto conf = new configurer(config);

    auto input = conf->get<std::string>("input");
    auto tag = conf->get<std::string>("tag");

    auto smear_input_aa = conf->get<std::string>("smear_input_aa");
    auto smear_input_pp = conf->get<std::string>("smear_input_pp");
    auto smear_tag = conf->get<std::string>("smear_tag");
    auto cent = conf->get<int64_t>("cent");

    auto heavyion = conf->get<bool>("heavyion");
    auto apply_jec = conf->get<bool>("apply_jec");

    auto rho = conf->get<std::string>("rho");
    auto rho_label = conf->get<std::string>("rho_label");

    auto pt_min = conf->get<float>("pt_min");
    auto eta_max = conf->get<float>("eta_max");

    auto rddr = conf->get<std::vector<float>>("ddr_range");

    auto dpt = conf->get<std::vector<float>>("pt_diff");
    auto ddr = conf->get<std::vector<float>>("dr_diff");
    auto dhf = conf->get<std::vector<float>>("hf_diff");

    /* load centrality weighting for MC */
    TFile* frho;
    history<TH1F>* rho_weighting = nullptr;
    
    if (!rho.empty()) {
        frho = new TFile(rho.data(), "read");
        rho_weighting = new history<TH1F>(frho, rho_label);
    }

    /* load the smearing information */
    TFile* fsmear_aa = new TFile(smear_input_aa.data(), "read");
    TFile* fsmear_pp = new TFile(smear_input_pp.data(), "read");;
    history<TH1F>* smear_fits_aa = new history<TH1F>(fsmear_aa, "aa_" + smear_tag);
    history<TH1F>* smear_fits_pp = new history<TH1F>(fsmear_pp, "pp_" + smear_tag);

    auto rng = new TRandom3(144);

    /* prepare histograms */
    auto mptdrhf = new multival(dpt, ddr, dhf);
    auto ipt = new interval(dpt);
    auto idr = new interval(ddr);
    auto ihf = new interval(dhf);

    auto iddr = new interval("#deltar^{2}"s, rddr[0], rddr[1], rddr[2]);
    auto fdr = std::bind(&interval::book<TH1F>, iddr, _1, _2, _3);
    auto angle = new memory<TH1F>("angle"s, "counts", fdr, mptdrhf);

    /* manage memory manually */
    TH1::AddDirectory(false);
    TH1::SetDefaultSumw2();

    /* load input */
    TFile* f = new TFile(input.data(), "read");
    TTree* t = (TTree*)f->Get("pj");
    auto p = new pjtree(true, false, heavyion, t, { 1, 1, 1, 0, 1, 0, heavyion });

    /* fill histograms */
    auto nentries = static_cast<int64_t>(t->GetEntries());
    for (int64_t i = 0; i < nentries; ++i) {
        if (i % 100000 == 0) { printf("%li/%li\n", i, nentries); }

        t->GetEntry(i);

        std::vector<int64_t> exclusion;
        for (int64_t j = 0; j < p->nMC; ++j) {
            auto pid = (*p->mcPID)[j];
            auto mpid = (*p->mcMomPID)[j];
            if (pid != 22 || (std::abs(mpid) > 22 && mpid != -999)) { continue; }

            /* gen isolation requirement */
            float isolation = (*p->mcCalIsoDR04)[j];
            if (isolation > 5.) { continue; }

            exclusion.push_back(j);
        }

        std::unordered_map<float, int64_t> genid;
            for (int64_t j = 0; j < p->ngen; ++j)
                genid[(*p->genpt)[j]] = j;

        for (int64_t j = 0; j < p->nref; ++j) {
            auto reco_pt = apply_jec ? (*p->jtptCor)[j] : (*p->jtpt)[j];
            auto reco_eta = (*p->jteta)[j];
            auto reco_phi = (*p->jtphi)[j];
            auto gen_pt = (*p->refpt)[j];

            if (reco_pt < pt_min) { continue; }
            if (gen_pt < pt_min) { continue; }

            if (std::abs(reco_eta) >= eta_max) { continue; }

            auto gen_eta = (*p->refeta)[j];
            auto gen_phi = (*p->refphi)[j];

            bool match = false;
            for (auto const& index : exclusion) {
                if (dr2((*p->mcEta)[index], gen_eta,
                        (*p->mcPhi)[index], gen_phi) < 0.01) {
                    match = true; break; }
            }

            if (match == true) { continue; }

            if (heavyion && in_jet_failure_region(p, j))
                continue;

            auto id = genid[gen_pt];
            auto gdr = std::sqrt(dr2(gen_eta, (*p->WTAgeneta)[id], gen_phi, (*p->WTAgenphi)[id]));
            auto rdr = std::sqrt(dr2(reco_eta, (*p->WTAeta)[j], reco_phi, (*p->WTAphi)[j]));
            
            if (rdr > rddr.back()) { continue; }

            auto pt_x = ipt->index_for(reco_pt);
            auto dr_x = idr->index_for(rdr);

            /* do the smearing */
            auto aa_c = (*smear_fits_aa)[x{dr_x, cent}]->GetBinContent(1);
            auto aa_s = (*smear_fits_aa)[x{dr_x, cent}]->GetBinContent(2);
            auto aa_n = (*smear_fits_aa)[x{dr_x, cent}]->GetBinContent(3);

            auto pp_c = (*smear_fits_pp)[x{dr_x, 0}]->GetBinContent(1);
            auto pp_s = (*smear_fits_pp)[x{dr_x, 0}]->GetBinContent(2);
            auto pp_n = (*smear_fits_pp)[x{dr_x, 0}]->GetBinContent(3);

            auto res_diff = res(aa_c, aa_s, aa_n, reco_pt) - res(pp_c, pp_s, pp_n, reco_pt);

            if (res_diff > 0) {
                auto change = rng->Exp(res(aa_c, aa_s, aa_n, reco_pt)) - rng->Exp(res(pp_c, pp_s, pp_n, reco_pt));
                // auto sign = (rng->Integer(2) == 0) ? -1 : 1;                
                // auto adj = rdr + change * sign;
                auto adj = rdr + change;

                // std::cout << dr_x << " " << reco_pt << " " << res_diff << " " << change << std::endl;

                if (ddr.front()< adj && adj < ddr.back()) rdr = adj;
            }

            auto weight = p->w;
            std::vector<float> weights(ihf->size(), weight);
            
            /* fill event weight */
            if (heavyion) {
                auto avg_rho = get_avg_rho(p, -1.442, 1.442);

                for (int64_t j = 0; j < ihf->size(); ++j) {
                    auto bin = (*rho_weighting)[j]->FindBin(avg_rho);
                    auto corr = (*rho_weighting)[j]->GetBinContent(bin);
                    weights[j] *= corr;
                }
            }

            /* fill histograms */
            for (int64_t j = 0; j < ihf->size(); ++j) {
                auto index = mptdrhf->index_for(x{pt_x, dr_x, j});

                (*angle)[index]->Fill(gdr-rdr, weights[j]);
            }
        }
    }

    /* save output */
    in(output, [&]() {
        angle->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return fabulate(argv[1], argv[2]);

    return 0;
}
