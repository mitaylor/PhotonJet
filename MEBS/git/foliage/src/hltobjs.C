#include "../include/hltobjs.h"

void hltobjs::read(TTree* t) {
    B_VEC_HLT(SETZERO)

    B_VEC_HLT(SETVALADDR, t)
}
