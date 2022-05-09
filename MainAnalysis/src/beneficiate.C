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
#include "TH1.h"

#include <memory>
#include <string>
#include <vector>


using namespace std::literals::string_literals;
using namespace std::placeholders;
// using x = std::initializer_list<int64_t> const;

bool in_pho_failure_region(float phoEta, float phoPhi) {
    auto ex_1 = phoEta < -1.3 && 
                phoPhi < -0.7 && phoPhi > -1.6;
    auto ex_2 = phoEta < 1.5 && phoEta > 0.1 &&
                phoPhi < 1.0 && phoPhi > 0.2;

    return ex_1 || ex_2;
}

bool in_jet_failure_region(float jetEta, float jetPhi) {
    auto ex_1 = jetEta < -1.3 && 
                jetPhi < -0.8 && jetPhi > -1.7;
    auto ex_2 = jetEta < 1.5 && jetEta > -0.5 &&
                jetPhi < 1.0 && jetPhi > 0;
    auto ex_3 = jetEta < 1.6 && jetEta > 1.5 &&
                jetPhi < -1.0 && jetPhi > -1.5;

    return ex_1 || ex_2 || ex_3;
}

int beneficiate(char const* output) {
    std::string tag = "aa";

    int64_t dim_size = 100;

    /* define history based on multival of jet eta, photon eta, and dphi */
    double jet_eta_min = -1.6;
    double jet_eta_max = 1.6;
    double pho_eta_min = -1.442;
    double pho_eta_max = 1.442;

    int64_t phi_min = -3.1416;
    int64_t phi_max = 3.1416;

    auto ijeta = new interval("jetEta"s, dim_size, jet_eta_min, jet_eta_max);
    auto ipeta = new interval("phoEta"s, dim_size, pho_eta_min, pho_eta_max);
    auto idphi = new interval("dphi"s, dim_size, 0, phi_max);

    auto mjpdphi = new multival(*ijeta, *ipeta, *idphi);

    auto incl = new interval(""s, 1, 0.f, 9999.f);
    auto fincl = std::bind(&interval::book<TH1F>, incl, _1, _2, _3);

    auto nevt = new memory<TH1F>("nevt"s, "", fincl, mjpdphi);
    auto nacc = new memory<TH1F>("nacc"s, "", fincl, mjpdphi);

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

                    auto dphi = revert_radian(convert_radian(phoPhi) - convert_radian(jetPhi));

                    auto dphi_x = idphi->index_for(dphi);
                    auto jpdphi_x = mjpdphi->index_for(x{i, j, dphi_x});

                    (*nevt)[jpdphi_x]->Fill(1);

                    if (in_pho_failure_region(phoEta, phoPhi) || in_jet_failure_region(jetEta, jetPhi)) { continue; }

                    (*nacc)[jpdphi_x]->Fill(1);
                    std::cout << i << " " << j << " " << k << " " << l << std::endl;
                }
            }
        }
    }


    /* normalise by number of entries */
    nacc->divide(*nevt);

    /* save histograms */
    in(output, [&]() {
        nacc->save(tag);
        nevt->save(tag);
    });

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 2)
        return beneficiate(argv[2]);

    printf("usage: %s [config] [output]\n", argv[0]);
    return 1;
}
