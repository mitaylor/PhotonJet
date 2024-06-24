#ifndef PFCAND_H
#define PFCAND_H

#include "tree.h"
#include "foliage.h"

#include "TTree.h"

#include <vector>

#define B_VEC_PF_RECO(ACTION, ...)                                          \
    ACTION(sv<float>,       pfPt,                        ## __VA_ARGS__)    \
    ACTION(sv<float>,       pfPhi,                       ## __VA_ARGS__)    \
    ACTION(sv<float>,       pfId,                        ## __VA_ARGS__)    \
    ACTION(sv<float>,       pfEta,                       ## __VA_ARGS__)    \
    ACTION(sv<float>,       pfEnergy,                    ## __VA_ARGS__)    \
    ACTION(sv<float>,       pfE,                         ## __VA_ARGS__)    \

class pfcand : tree {
  public:
    pfcand() = default;
    pfcand(pfcand const&) = delete;
    pfcand& operator=(pfcand const&) = delete;
    ~pfcand() = default;

    void read(TTree* t);

    B_VEC_PF_RECO(DECLPTR)
};

#endif  /* PFCAND_H */