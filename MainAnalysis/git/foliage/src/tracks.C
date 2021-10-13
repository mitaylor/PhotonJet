#include "../include/tracks.h"

tracks::tracks(int64_t size)
    : _size(size) { }

void tracks::read(TTree* t) {
    B_VAL_TRK_RECO(SETZERO)
    B_ARR_TRK_RECO(ALLOCARR, _size)

    B_VAL_TRK_RECO(SETVALADDR, t)
    B_ARR_TRK_RECO(SETPTRADDR, t)
}
