#ifndef FOLIAGE_H
#define FOLIAGE_H

#define DECLVAL(type, var) type var;
#define DECLPTR(type, var) type* var;

#define SETZERO(type, var) var = 0;
#define SETMONE(type, var) var = -1;

#define ALLOCARR(type, var, size) var = new type[size];
#define ALLOCOBJ(type, var) var = new type();

#define SETVALADDR(type, var, tree)                                         \
    tree->SetBranchStatus(#var, 1);                                         \
    tree->SetBranchAddress(#var, &var);
#define SETPTRADDR(type, var, tree)                                         \
    tree->SetBranchStatus(#var, 1);                                         \
    tree->SetBranchAddress(#var, var);

#define BRANCHVAL(type, var, tree) tree->Branch(#var, &var);
#define BRANCHPTR(type, var, tree) tree->Branch(#var, var);

#define COPYVAL(type, var, tree)                                            \
    var = tree->var;

#define COPYOBJ(type, var, tree)                                            \
    if (tree->var != nullptr)                                               \
        std::copy(std::begin((*tree->var)), std::end((*tree->var)),         \
                  std::back_inserter(*var));

#define COPYPTR(type, var, tree, size)                                      \
    if (tree->var != nullptr)                                               \
        std::copy(tree->var, tree->var + size, std::back_inserter(*var));

#define CLEAROBJ(type, var) var->clear();

#include <iterator>
#include <vector>

template <typename T>
using sv = std::vector<T>;

#endif /* FOLIAGE_H */
