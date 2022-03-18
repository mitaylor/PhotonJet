#include "../include/rho.h"

void photons::read(TTree* t) {
    B_VEC_RHO(SETZERO)

    tree->SetBranchStatus("etaMin", 1);                                         \
    tree->SetBranchAddress("etaMin", &etaMin);
    tree->SetBranchStatus("etaMax", 1);                                         \
    tree->SetBranchAddress("etaMax", &etaMax);
    tree->SetBranchStatus("rho", 1);                                         \
    tree->SetBranchAddress("rho", &evtRho);
}
