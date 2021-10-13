#ifndef TRACKS_H
#define TRACKS_H

#include "tree.h"
#include "foliage.h"

#include "TTree.h"

#define B_ALL_TRK_RECO(ACTION, ...)                                         \
    B_VAL_TRK_RECO(ACTION,                              ## __VA_ARGS__)     \
    B_ARR_TRK_RECO(ACTION,                              ## __VA_ARGS__)     \

#define B_VAL_TRK_RECO(ACTION, ...)                                         \
    ACTION(int,             nTrk,                       ## __VA_ARGS__)     \

#define B_ARR_TRK_RECO(ACTION, ...)                                         \
    ACTION(float,           trkPt,                      ## __VA_ARGS__)     \
    ACTION(float,           trkPtError,                 ## __VA_ARGS__)     \
    ACTION(uint8_t,         trkNHit,                    ## __VA_ARGS__)     \
    ACTION(uint8_t,         trkNlayer,                  ## __VA_ARGS__)     \
    ACTION(float,           trkEta,                     ## __VA_ARGS__)     \
    ACTION(float,           trkPhi,                     ## __VA_ARGS__)     \
    ACTION(int32_t,         trkCharge,                  ## __VA_ARGS__)     \
    ACTION(bool,            highPurity,                 ## __VA_ARGS__)     \
    ACTION(float,           trkChi2,                    ## __VA_ARGS__)     \
    ACTION(uint8_t,         trkNdof,                    ## __VA_ARGS__)     \
    ACTION(float,           trkDxy1,                    ## __VA_ARGS__)     \
    ACTION(float,           trkDxyError1,               ## __VA_ARGS__)     \
    ACTION(float,           trkDz1,                     ## __VA_ARGS__)     \
    ACTION(float,           trkDzError1,                ## __VA_ARGS__)     \

class tracks : tree {
  public:
    tracks(int64_t size);
    tracks(tracks const&) = delete;
    tracks& operator=(tracks const&) = delete;
    ~tracks() = default;

    void read(TTree* t);

    B_VAL_TRK_RECO(DECLVAL)
    B_ARR_TRK_RECO(DECLPTR)

  private:
    int64_t _size;
};

#endif  /* TRACKS_H */
