#include "../include/lambdas.h"
#include "../include/pjtree.h"
#include "../include/specifics.h"

#include "../git/config/include/configurer.h"

#include "../git/history/include/interval.h"
#include "../git/history/include/multival.h"
#include "../git/history/include/memory.h"

#include "../git/tricks-and-treats/include/overflow_angles.h"
#include "../git/tricks-and-treats/include/trunk.h"

#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TMath.h"

#include <memory>
#include <string>
#include <vector>

using namespace std::literals::string_literals;
using namespace std::placeholders;

static float dr2(float eta1, float eta2, float phi1, float phi2) {
    auto deta = eta1 - eta2;
    float dphi = std::abs(phi1 - phi2);
    if (dphi > TMath::Pi()) dphi = std::abs(dphi - 2*TMath::Pi());

    return deta * deta + dphi * dphi;
}

int beneficiate(char const* selections, char const* output) {
    auto sel = new configurer(selections);

    auto dphi_range = sel->get<std::vector<float>>("dphi_range");

    auto photon_eta_abs = sel->get<float>("photon_eta_abs");
    auto jet_eta_abs = sel->get<float>("jet_eta_abs");

    std::string tag = "aa";

    int64_t dim_size = 200;

    /* define history based on multival of jet eta, photon eta, and dphi */
    double jet_eta_min = -jet_eta_abs;
    double jet_eta_max = jet_eta_abs;
    double pho_eta_min = -photon_eta_abs;
    double pho_eta_max = photon_eta_abs;

    double phi_min = -TMath::Pi();
    double phi_max = TMath::Pi();

    auto ijeta = new interval("jetEta"s, dim_size, jet_eta_min, jet_eta_max);
    auto ipeta = new interval("phoEta"s, dim_size, pho_eta_min, pho_eta_max);
    auto idphi = new interval("dphi"s, dphi_range);

    auto mpjeta = new multival(*ijeta, *ipeta);
    auto mdphi = new multival (*idphi);

    auto fincl = std::bind(&multival::book<TH2D>, mpjeta, _1, _2, _3);

    auto nevt = new memory<TH2D>("nevt"s, "none", fincl, mdphi);
    auto nacc = new memory<TH2D>("nacc"s, "none", fincl, mdphi);

    /* create vectors for photon and jet phi */
    auto ijphi = new interval("jetPhi"s, dim_size, phi_min, phi_max);
    auto ipphi = new interval("phoPhi"s, dim_size, phi_min, phi_max);

    /* fill the histograms */
    for (int64_t i = 0; i < dim_size; ++i) {
        std::cout << i << std::endl;
        for (int64_t j = 0; j < dim_size; ++j) {
            for (int64_t k = 0; k < dim_size; ++k) {
                for (int64_t l = 0; l < dim_size; ++l) {
                    auto jeta_edges = ijeta->edges(i);
                    auto peta_edges = ipeta->edges(j);
                    auto jphi_edges = ijphi->edges(k);
                    auto pphi_edges = ipphi->edges(l);

                    auto jetEta = (jeta_edges[1] + jeta_edges[0])/2;
                    auto phoEta = (peta_edges[1] + peta_edges[0])/2;
                    auto jetPhi = (jphi_edges[1] + jphi_edges[0])/2;
                    auto phoPhi = (pphi_edges[1] + pphi_edges[0])/2;

                    auto dphi = std::sqrt(dr2(0, 0, jetPhi, phoPhi)) / TMath::Pi();

                    auto dphi_x = idphi->index_for(dphi);

                    (*nevt)[dphi_x]->Fill(jetEta, phoEta);

                    if (in_pho_failure_region(phoEta, phoPhi) || in_jet_failure_region(jetEta, jetPhi)) { continue; }

                    (*nacc)[dphi_x]->Fill(jetEta, phoEta);
                }
            }
        }
    }

    /* save histograms */
    in(output, [&]() {
        nacc->save(tag);
        nevt->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3)
        return beneficiate(argv[1], argv[2]);

    printf("usage: %s [output]\n", argv[0]);
    return 1;
}
