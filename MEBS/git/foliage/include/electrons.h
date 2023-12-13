#ifndef ELECTRONS_H
#define ELECTRONS_H

#include "tree.h"
#include "foliage.h"

#include "TTree.h"

#include <vector>

#define B_VAL_ELE_RECO(ACTION, ...)                                         \
    ACTION(int32_t,         nEle,                       ## __VA_ARGS__)     \

#define B_VEC_ELE_RECO(ACTION, ...)                                         \
    ACTION(sv<float>,       elePt,                      ## __VA_ARGS__)     \
    ACTION(sv<float>,       eleEta,                     ## __VA_ARGS__)     \
    ACTION(sv<float>,       elePhi,                     ## __VA_ARGS__)     \
    ACTION(sv<int>,         eleCharge,                  ## __VA_ARGS__)     \
    ACTION(sv<float>,       eleIP3D,                    ## __VA_ARGS__)     \
    ACTION(sv<float>,       eleIP3DErr,                 ## __VA_ARGS__)     \
                                                                            \
    ACTION(sv<float>,       eleSCEn,                    ## __VA_ARGS__)     \
    ACTION(sv<float>,       eleESEn,                    ## __VA_ARGS__)     \
    ACTION(sv<float>,       eleSCEta,                   ## __VA_ARGS__)     \
    ACTION(sv<float>,       eleSCPhi,                   ## __VA_ARGS__)     \
    ACTION(sv<float>,       eleHoverE,                  ## __VA_ARGS__)     \
    ACTION(sv<float>,       eleHoverEBc,                ## __VA_ARGS__)     \
    ACTION(sv<float>,       eleEoverPInv,               ## __VA_ARGS__)     \
    ACTION(sv<float>,       eleSigmaIEtaIEta_2012,      ## __VA_ARGS__)     \
                                                                            \
    ACTION(sv<int>,         eleMissHits,                ## __VA_ARGS__)     \
    ACTION(sv<int>,         eleConvVeto,                ## __VA_ARGS__)     \
    ACTION(sv<float>,       eledEtaAtVtx,               ## __VA_ARGS__)     \
    ACTION(sv<float>,       eledPhiAtVtx,               ## __VA_ARGS__)     \
    ACTION(sv<float>,       eledEtaSeedAtVtx,           ## __VA_ARGS__)     \
                                                                            \
    ACTION(sv<float>,       elePFChIso,                 ## __VA_ARGS__)     \
    ACTION(sv<float>,       elePFPhoIso,                ## __VA_ARGS__)     \
    ACTION(sv<float>,       elePFNeuIso,                ## __VA_ARGS__)     \

class electrons : tree {
  public:
    electrons() = default;
    electrons(electrons const&) = delete;
    electrons& operator=(electrons const&) = delete;
    ~electrons() = default;

    void read(TTree* t);

    B_VAL_ELE_RECO(DECLVAL)
    B_VEC_ELE_RECO(DECLPTR)
};

#endif  /* ELECTRONS_H */
