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

int beneficiate(char const* selections, char const* output) {
    auto sel = new configurer(selections);
    auto dphi_range = sel->get<std::vector<float>>("dphi_range");

    std::string tag = "aa";

    int64_t dim_size = 100;

    /* define history based on multival of jet eta, photon eta, and dphi */
    double jet_eta_min = -2;
    double jet_eta_max = 2;
    double pho_eta_min = -2;
    double pho_eta_max = 2;

    double phi_min = -TMath::Pi();
    double phi_max = TMath::Pi();

    auto ijeta = new interval("jetEta"s, dim_size, jet_eta_min, jet_eta_max);
    auto ipeta = new interval("phoEta"s, dim_size, pho_eta_min, pho_eta_max);
    auto idphi = new interval("dphi"s, dphi_range);

    auto mpjeta = new multival(*ijeta, *ipeta);
    auto mdphi = new multival (*idphi);

    auto fincl = std::bind(&multival::book<TH2F>, mpjeta, _1, _2, _3);

    auto nevt = new memory<TH2F>("nevt"s, "none", fincl, mdphi);
    auto nacc = new memory<TH2F>("nacc"s, "none", fincl, mdphi);

    std::vector<float> evt(mdphi->size()(), 0);
    std::vector<float> acc(mdphi->size()(), 0);

    /* create vectors for photon and jet phi */
    auto ijphi = new interval("jetPhi"s, dim_size, phi_min, phi_max);
    auto ipphi = new interval("phoPhi"s, dim_size, phi_min, phi_max);

    /* fill the histograms */
    for (int64_t i = 0; i < dim_size; ++i) {
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

                    auto dphi = revert_pi(convert_radian(phoPhi) - convert_radian(jetPhi));

                    auto dphi_x = idphi->index_for(dphi);

                    (*nevt)[dphi_x]->Fill(jetEta, phoEta);
                    evt[dphi_x] += 1;

                    if (in_pho_failure_region(phoEta, phoPhi) || in_jet_failure_region(jetEta, jetPhi)) { continue; }

                    (*nacc)[dphi_x]->Fill(jetEta, phoEta);
                    acc[dphi_x] += 1;
                }
            }
        }
    }

    for (size_t i = 0; i < mdphi->size()(); ++i) {
        std::cout << acc[i]/evt[i] << std::endl;
    }

    for (size_t i = 0; i < mdphi->size()(); ++i) {
        std::cout << acc[i] << std::endl;
    }

    for (size_t i = 0; i < mdphi->size()(); ++i) {
        std::cout << evt[i] << std::endl;
    }

    /* normalise by number of entries */
    // nacc->divide(*nevt);

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
