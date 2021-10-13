#ifndef EGGEN_H
#define EGGEN_H

#include "tree.h"
#include "foliage.h"

#include "TTree.h"

#include <vector>

#define B_VAL_EGM_RECO(ACTION, ...)                                         \
    ACTION(float,           rho,                        ## __VA_ARGS__)     \

#define B_VAL_EGM_GEN(ACTION, ...)                                          \
    ACTION(int32_t,         nMC,                        ## __VA_ARGS__)     \

#define B_VEC_EGM_GEN(ACTION, ...)                                          \
    ACTION(sv<int>,         mcPID,                      ## __VA_ARGS__)     \
    ACTION(sv<int>,         mcStatus,                   ## __VA_ARGS__)     \
    ACTION(sv<float>,       mcEt,                       ## __VA_ARGS__)     \
    ACTION(sv<float>,       mcPt,                       ## __VA_ARGS__)     \
    ACTION(sv<float>,       mcEta,                      ## __VA_ARGS__)     \
    ACTION(sv<float>,       mcPhi,                      ## __VA_ARGS__)     \
                                                                            \
    ACTION(sv<int>,         mcMomPID,                   ## __VA_ARGS__)     \
    ACTION(sv<float>,       mcMomPt,                    ## __VA_ARGS__)     \
    ACTION(sv<float>,       mcMomEta,                   ## __VA_ARGS__)     \
    ACTION(sv<float>,       mcMomPhi,                   ## __VA_ARGS__)     \
    ACTION(sv<float>,       mcMomMass,                  ## __VA_ARGS__)     \
                                                                            \
    ACTION(sv<float>,       mcCalIsoDR04,               ## __VA_ARGS__)     \
    ACTION(sv<float>,       mcTrkIsoDR04,               ## __VA_ARGS__)     \
                                                                            \
    ACTION(sv<int>,         pho_genMatchedIndex,        ## __VA_ARGS__)     \

class eggen : tree {
  public:
    eggen(bool gen);
    eggen(eggen const&) = delete;
    eggen& operator=(eggen const&) = delete;
    ~eggen() = default;

    void read(TTree* t);

    B_VAL_EGM_RECO(DECLVAL)
    B_VAL_EGM_GEN(DECLVAL)
    B_VEC_EGM_GEN(DECLPTR)

  private:
    bool _gen;
};

#endif  /* EGGEN_H */
