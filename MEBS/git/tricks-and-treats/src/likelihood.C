#include "../include/likelihood.h"

#include "TMinuit.h"

#include <array>
#include <cstdlib>
#include <functional>

template <>
struct operation_t<likelihood::op::add> {
    static double rewrite(double c, double a) { return c - a; }
    static double derivative(double, double) { return -1.; }
};

template <>
struct operation_t<likelihood::op::sub> {
    static double rewrite(double c, double a) { return a - c; }
    static double derivative(double, double) { return 1.; }
};

template <>
struct operation_t<likelihood::op::mul> {
    static double rewrite(double c, double a) { return c / a; }
    static double derivative(double c, double a) { return - c / (a * a); }
};

template <>
struct operation_t<likelihood::op::div> {
    static double rewrite(double c, double a) { return a / c; }
    static double derivative(double c, double) { return 1. / c; }
};

template <likelihood::op T>
double rewrite(double c, double a) {
    return operation_t<T>::rewrite(c, a);
}

template <likelihood::op T>
double derivative(double c, double a) {
    return operation_t<T>::derivative(c, a);
}

template <likelihood::op T>
void dnlldn(int&, double*, double& value, double* p, int) {
    auto a1 = p[1] - p[2];
    auto a2 = rewrite<T>(p[0], p[1]) - p[5];
    auto s1 = p[3] - p[4];
    auto s2 = p[6] - p[7];
    auto d1 = p[3] * p[4] + s1 * a1;
    auto d2 = p[6] * p[7] + s2 * a2;

    value = std::abs(a1 * (2 * d1 - a1 * s1) / (d1 * d1)
        + derivative<T>(p[0], p[1]) * a2 * (2 * d2 - a2 * s2) / (d2 * d2));
}

template <likelihood::op T>
double nll(std::array<double, 3> const& a, std::array<double, 3> const& b,
           double c, double n) {
    auto a1 = n - a[0];
    auto a2 = rewrite<T>(c, n) - b[0];
    auto s1 = a[1] - a[2];
    auto s2 = b[1] - b[2];
    auto d1 = a[1] * a[2] + s1 * a1;
    auto d2 = b[1] * b[2] + s2 * a2;

    return a1 * a1 / d1 + a2 * a2 / d2;
}

template <likelihood::op T>
void likelihood_combination(std::array<double, 3> const& a,
                            std::array<double, 3> const& b,
                            std::function<double(std::array<double, 2>)> f,
                            double start) {
    TMinuit* min = new TMinuit(8);
    min->SetFCN(dnlldn<T>);

    /* https://root.cern.ch/root/html/src/TMinuit.cxx.html#h_OGX */
    int flag;
    double args[2];

    /* silence output */
    args[0] = -1;
    min->mnexcm("SET PRI", args, 1, flag);
    min->mnexcm("SET NOW", args, 0, flag);

    /* set and fix pseudo-parameters */
    min->mnparm(0, "C", 0, 0, 0, 0, flag);
    min->mnparm(1, "N", a[0], 0.01, 0, 0, flag);
    min->mnparm(2, "A", a[0], 0, 0, 0, flag);
    min->mnparm(3, "AP", a[1], 0, 0, 0, flag);
    min->mnparm(4, "AM", a[2], 0, 0, 0, flag);
    min->mnparm(5, "B", b[0], 0, 0, 0, flag);
    min->mnparm(6, "BP", b[1], 0, 0, 0, flag);
    min->mnparm(7, "BM", b[2], 0, 0, 0, flag);

    /* minimise log likelihood w.r.t. nuisance parameter a */
    auto eval = [&](double x) {
        args[0] = 1;
        args[1] = x;
        min->mnexcm("SET PAR", args, 2, flag);

        args[0] = 500;
        args[1] = 0.1;
        min->mnexcm("MIGRAD", args, 2, flag);

        TString name;
        double val;
        double err;
        double low;
        double high;

        min->mnpout(1, name, val, err, low, high, flag);

        return std::array<double, 2>({x, nll<T>(a, b, x, val)});
    };

    for (auto x = start; !std::isnan(x); x = f(eval(x)));
}

/* explicit template instantiation */
template void likelihood_combination<likelihood::op::add>(
    std::array<double, 3> const&, std::array<double, 3> const&,
    std::function<double(std::array<double, 2>)>, double);
template void likelihood_combination<likelihood::op::sub>(
    std::array<double, 3> const&, std::array<double, 3> const&,
    std::function<double(std::array<double, 2>)>, double);
template void likelihood_combination<likelihood::op::mul>(
    std::array<double, 3> const&, std::array<double, 3> const&,
    std::function<double(std::array<double, 2>)>, double);
template void likelihood_combination<likelihood::op::div>(
    std::array<double, 3> const&, std::array<double, 3> const&,
    std::function<double(std::array<double, 2>)>, double);
