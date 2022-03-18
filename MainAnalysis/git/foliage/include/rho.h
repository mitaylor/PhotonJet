#ifndef RHO_H
#define RH0_H

#include "tree.h"
#include "foliage.h"

#include "TTree.h"

#include <vector>

#define B_VEC_RHO(ACTION, ...)                                              \
    ACTION(sv<float>,       etaMin,                     ## __VA_ARGS__)     \
    ACTION(sv<float>,       etaMax,                     ## __VA_ARGS__)     \
    ACTION(sv<float>,       evtRho,                     ## __VA_ARGS__)     \

class rho : tree {
  public:
    rho() = default;
    rho(rho const&) = delete;
    rho& operator=(rho const&) = delete;
    ~rho() = default;

    void read(TTree* t);

    B_VEC_RHO(DECLPTR)
};

#endif  /* RHO_H */
