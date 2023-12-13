#include "../include/electrons.h"

void electrons::read(TTree* t) {
    B_VAL_ELE_RECO(SETZERO)
    B_VEC_ELE_RECO(SETZERO)

    B_VAL_ELE_RECO(SETVALADDR, t)
    B_VEC_ELE_RECO(SETVALADDR, t)
}
