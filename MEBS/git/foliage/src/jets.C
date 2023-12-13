#include "../include/jets.h"

jets::jets(bool gen, int64_t size)
    : _gen(gen), _size(size) { }

void jets::read(TTree* t) {
    B_VAL_JET_RECO(SETZERO)
    B_ARR_JET_RECO(ALLOCARR, _size)

    if (_gen) {
        B_VAL_JET_GEN(SETZERO)
        B_ARR_JET_GEN(ALLOCARR, _size)
        B_ARR_JET_REF(ALLOCARR, _size)
    }

    B_VAL_JET_RECO(SETVALADDR, t)
    B_ARR_JET_RECO(SETPTRADDR, t)

    if (_gen) {
        B_VAL_JET_GEN(SETVALADDR, t)
        B_ARR_JET_GEN(SETPTRADDR, t)
        B_ARR_JET_REF(SETPTRADDR, t)
    }
}
