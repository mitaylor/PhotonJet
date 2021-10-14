#include "../include/eggen.h"

eggen::eggen(bool gen)
    : _gen(gen) { }

void eggen::read(TTree* t) {
    B_VAL_EGM_GEN(SETZERO)
    B_VEC_EGM_GEN(SETZERO)

    if (_gen) {
        B_VAL_EGM_GEN(SETVALADDR, t)
        B_VEC_EGM_GEN(SETVALADDR, t)
    }
}
