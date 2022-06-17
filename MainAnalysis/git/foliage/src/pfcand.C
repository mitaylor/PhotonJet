#include "../include/pfcand.h"

void pfcand::read(TTree* t) {
    B_VEC_PF_RECO(SETZERO)
    B_VEC_PF_RECO(SETVALADDR, t)
}