#include "../include/rho.h"

void photons::read(TTree* t) {
    B_VEC_RHO(SETZERO)

    t->SetBranchStatus("etaMin", 1);                                         \
    t->SetBranchAddress("etaMin", &etaMin);
    t->SetBranchStatus("etaMax", 1);                                         \
    t->SetBranchAddress("etaMax", &etaMax);
    t->SetBranchStatus("rho", 1);                                         \
    t->SetBranchAddress("rho", &evtRho);
}
