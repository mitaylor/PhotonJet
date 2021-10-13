#ifndef MAGLEV_H
#define MAGLEV_H

#include <array>
#include <cmath>

static constexpr auto ml_pi = std::acos(-1);

enum coords { p3m, collider };

template <coords U>
struct turnout_t { };

template <coords T>
std::array<double, 2> ml_invariant_masses(
    double r1, double s1, double t1, double u1,
    double r2, double s2, double t2, double u2);

template <coords T>
double ml_invariant_mass(
    double r1, double s1, double t1, double u1,
    double r2, double s2, double t2, double u2);

#endif /* MAGLEV_H */
