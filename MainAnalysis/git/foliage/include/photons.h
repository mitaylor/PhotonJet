#ifndef PHOTONS_H
#define PHOTONS_H

#include "tree.h"
#include "foliage.h"

#include "TTree.h"

#include <vector>

#define B_VAL_PHO_RECO(ACTION, ...)                                         \
    ACTION(int32_t,         nPho,                       ## __VA_ARGS__)     \

#define B_VEC_PHO_RECO(ACTION, ...)                                         \
    ACTION(sv<float>,       phoE,                       ## __VA_ARGS__)     \
    ACTION(sv<float>,       phoEt,                      ## __VA_ARGS__)     \
    ACTION(sv<float>,       phoEta,                     ## __VA_ARGS__)     \
    ACTION(sv<float>,       phoPhi,                     ## __VA_ARGS__)     \
                                                                            \
    ACTION(sv<float>,       phoSCE,                     ## __VA_ARGS__)     \
    ACTION(sv<float>,       phoSCEta,                   ## __VA_ARGS__)     \
    ACTION(sv<float>,       phoSCPhi,                   ## __VA_ARGS__)     \
                                                                            \
    ACTION(sv<float>,       phoHoverE,                  ## __VA_ARGS__)     \
    ACTION(sv<float>,       phoR9_2012,                 ## __VA_ARGS__)     \
    ACTION(sv<float>,       phoSigmaIEtaIEta_2012,      ## __VA_ARGS__)     \
                                                                            \
    ACTION(sv<float>,       pho_ecalClusterIsoR3,       ## __VA_ARGS__)     \
    ACTION(sv<float>,       pho_hcalRechitIsoR3,        ## __VA_ARGS__)     \
    ACTION(sv<float>,       pho_trackIsoR3PtCut20,      ## __VA_ARGS__)     \
                                                                            \
    ACTION(sv<float>,       pho_swissCrx,               ## __VA_ARGS__)     \
    ACTION(sv<float>,       pho_seedTime,               ## __VA_ARGS__)     \

class photons : tree {
  public:
    photons() = default;
    photons(photons const&) = delete;
    photons& operator=(photons const&) = delete;
    ~photons() = default;

    void read(TTree* t);

    B_VAL_PHO_RECO(DECLVAL)
    B_VEC_PHO_RECO(DECLPTR)
};

#endif  /* PHOTONS_H */
