#ifndef EVTRHO_H
#define EVTRHO_H

#include "tree.h"
#include "foliage.h"

#include "TTree.h"

#include <vector>

#define B_VEC_RHO(ACTION, ...)                                              \
    ACTION(sv<double>,       etaMin,                     ## __VA_ARGS__)     \
    ACTION(sv<double>,       etaMax,                     ## __VA_ARGS__)     \
    ACTION(sv<double>,       evtRho,                     ## __VA_ARGS__)     \

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
