#ifndef EVENT_H
#define EVENT_H

#include "tree.h"
#include "foliage.h"

#include "TTree.h"

#include <vector>

#define B_VAL_EVT_RECO(ACTION, ...)                                         \
    ACTION(float,           vz,                         ## __VA_ARGS__)     \
    ACTION(int32_t,         hiBin,                      ## __VA_ARGS__)     \
    ACTION(float,           hiHF,                       ## __VA_ARGS__)     \

#define B_VAL_EVT_GEN(ACTION, ...)                                          \
    ACTION(float,           pthat,                      ## __VA_ARGS__)     \
    ACTION(float,           Ncoll,                      ## __VA_ARGS__)     \

class event : tree {
  public:
    event(bool gen);
    event(event const&) = delete;
    event& operator=(event const&) = delete;
    ~event() = default;

    void read(TTree* t);

    B_VAL_EVT_RECO(DECLVAL)
    B_VAL_EVT_GEN(DECLVAL)

  private:
    bool _gen;
};

#endif  /* EVENT_H */
