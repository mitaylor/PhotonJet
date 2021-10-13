#include "../include/l1objs.h"

void l1objs::read(TTree* t) {
    B_VAL_L1O(SETZERO)
    B_VEC_L1O(SETZERO)

    B_VAL_L1O(SETVALADDR, t)
    B_VEC_L1O(SETVALADDR, t)
}
