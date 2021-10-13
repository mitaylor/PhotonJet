#ifndef TREE_H
#define TREE_H

#include "TTree.h"

class tree {
  public:
    tree() = default;
    tree(tree const&) = delete;
    tree& operator=(tree const&) = delete;
    ~tree() = default;

  private:
    virtual void read(TTree* t) = 0;
};

template <typename T, typename... U>
T* harvest(TTree* t, U... args) {
    if (!t) { return nullptr; }

    auto ptr = new T(args...);
    ptr->read(t);
    return ptr;
}

#endif /* TREE_H */
