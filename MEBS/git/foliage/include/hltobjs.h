#ifndef HLTOBJS_H
#define HLTOBJS_H

#include "tree.h"
#include "foliage.h"

#include "TTree.h"

#define B_ALL_HLT(ACTION, ...)                                              \
    B_VEC_HLT(ACTION,                                   ## __VA_ARGS__)     \

#define B_VEC_HLT(ACTION, ...)                                              \
    ACTION(sv<double>,      pt,                         ## __VA_ARGS__)     \
    ACTION(sv<double>,      eta,                        ## __VA_ARGS__)     \
    ACTION(sv<double>,      phi,                        ## __VA_ARGS__)     \
    ACTION(sv<double>,      mass,                       ## __VA_ARGS__)     \

class hltobjs : tree {
  public:
    hltobjs() = default;
    hltobjs(hltobjs const&) = delete;
    hltobjs& operator=(hltobjs const&) = delete;
    ~hltobjs() = default;

    void read(TTree* t);

    B_VEC_HLT(DECLPTR)
};

#endif  /* HLTOBJS_H */
