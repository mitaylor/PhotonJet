#ifndef EVTRHO_H
#define EVTRH0_H

#include "tree.h"
#include "foliage.h"

#include "TTree.h"

#include <vector>

#define B_VEC_RHO(ACTION, ...)                                              \
    ACTION(sv<float>,       etaMin,                     ## __VA_ARGS__)     \
    ACTION(sv<float>,       etaMax,                     ## __VA_ARGS__)     \
    ACTION(sv<float>,       evtRho,                     ## __VA_ARGS__)     \

class evtrho : tree {
  public:
    evtrho() = default;
    evtrho(evtrho const&) = delete;
    evtrho& operator=(evtrho const&) = delete;
    ~evtrho() = default;

    void read(TTree* t);

    B_VEC_RHO(DECLPTR)
};

#endif  /* EVTRHO_H */
