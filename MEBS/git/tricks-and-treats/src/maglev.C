#include "../include/maglev.h"

#include <algorithm>

template <>
struct turnout_t<coords::p3m> {
    static std::array<double, 2> ml_invariant_masses(
            double px1, double py1, double pz1, double m1,
            double px2, double py2, double pz2, double m2) {
        double sqm1 = m1 * m1;
        double magp1 = px1 * px1 + py1 * py1 + pz1 * pz1;

        double sqm2 = m2 * m2;
        double magp2 = px2 * px2 + py2 * py2 + pz2 * pz2;

        double sumsqsum = (px1 + px2) * (px1 + px2)
            + (py1 + py2) * (py1 + py2)
            + (pz1 + pz2) * (pz1 + pz2);

        double sqec11 = sqm1 + magp1;
        double sqec22 = sqm2 + magp2;
        double sqec12 = sqm1 + magp2;
        double sqec21 = sqm2 + magp1;

        double mcand = sqec11 + sqec22 + 2. * sqrt(sqec11 * sqec22) - sumsqsum;
        double mswap = sqec12 + sqec21 + 2. * sqrt(sqec12 * sqec21) - sumsqsum;

        return { mcand, mswap };
    }

    static double ml_invariant_mass(
            double px1, double py1, double pz1, double m1,
            double px2, double py2, double pz2, double m2) {
        double sqm1 = m1 * m1;
        double magp1 = px1 * px1 + py1 * py1 + pz1 * pz1;

        double sqm2 = m2 * m2;
        double magp2 = px2 * px2 + py2 * py2 + pz2 * pz2;

        double sumsqsum = (px1 + px2) * (px1 + px2)
            + (py1 + py2) * (py1 + py2)
            + (pz1 + pz2) * (pz1 + pz2);

        double sqec11 = sqm1 + magp1;
        double sqec22 = sqm2 + magp2;

        return sqec11 + sqec22 + 2. * sqrt(sqec11 * sqec22) - sumsqsum;
    }
};

template <>
struct turnout_t<coords::collider> {
    static std::array<double, 2> ml_invariant_masses(
            double pt1, double eta1, double phi1, double m1,
            double pt2, double eta2, double phi2, double m2) {
        double pz1 = pt1 * std::sinh(eta1);
        double pz2 = pt2 * std::sinh(eta2);

        double sqm1 = m1 * m1;
        double magp1 = pt1 * pt1 + pz1 * pz1;

        double sqm2 = m2 * m2;
        double magp2 = pt2 * pt2 + pz2 * pz2;

        double sumsqsum = (pt1 * pt1) + (pt2 * pt2)
            + 2. * pt1 * pt2 * std::cos(phi1 - phi2)
            + (pz1 + pz2) * (pz1 + pz2);

        double sqec11 = sqm1 + magp1;
        double sqec22 = sqm2 + magp2;
        double sqec12 = sqm1 + magp2;
        double sqec21 = sqm2 + magp1;

        double mcand = sqec11 + sqec22 + 2. * sqrt(sqec11 * sqec22) - sumsqsum;
        double mswap = sqec12 + sqec21 + 2. * sqrt(sqec12 * sqec21) - sumsqsum;

        return { mcand, mswap };
    }

    static double ml_invariant_mass(
            double pt1, double eta1, double phi1, double m1,
            double pt2, double eta2, double phi2, double m2) {
        double pz1 = pt1 * std::sinh(eta1);
        double pz2 = pt2 * std::sinh(eta2);

        double sqm1 = m1 * m1;
        double magp1 = pt1 * pt1 + pz1 * pz1;

        double sqm2 = m2 * m2;
        double magp2 = pt2 * pt2 + pz2 * pz2;

        double sumsqsum = (pt1 * pt1) + (pt2 * pt2)
            + 2. * pt1 * pt2 * std::cos(phi1 - phi2)
            + (pz1 + pz2) * (pz1 + pz2);

        double sqec11 = sqm1 + magp1;
        double sqec22 = sqm2 + magp2;

        return sqec11 + sqec22 + 2. * sqrt(sqec11 * sqec22) - sumsqsum;
    }
};

template <coords T>
std::array<double, 2> ml_invariant_masses(
        double r1, double s1, double t1, double u1,
        double r2, double s2, double t2, double u2) {
    return turnout_t<T>::ml_invariant_masses(r1, s1, t1, u1, r2, s2, t2, u2);
}

template <coords T>
double ml_invariant_mass(double r1, double s1, double t1, double u1,
                         double r2, double s2, double t2, double u2) {
    return turnout_t<T>::ml_invariant_mass(r1, s1, t1, u1, r2, s2, t2, u2);
}

/* explicit template instantiation */
template std::array<double, 2> ml_invariant_masses<coords::p3m>(
    double, double, double, double, double, double, double, double);
template std::array<double, 2> ml_invariant_masses<coords::collider>(
    double, double, double, double, double, double, double, double);
template double ml_invariant_mass<coords::p3m>(
    double, double, double, double, double, double, double, double);
template double ml_invariant_mass<coords::collider>(
    double, double, double, double, double, double, double, double);
