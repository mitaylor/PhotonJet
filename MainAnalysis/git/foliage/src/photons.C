#include "../include/photons.h"

void photons::read(TTree* t) {
    B_VAL_PHO_RECO(SETZERO)
    B_VEC_PHO_RECO(SETZERO)

    B_VAL_PHO_RECO(SETVALADDR, t)
    B_VEC_PHO_RECO(SETVALADDR, t)
}
