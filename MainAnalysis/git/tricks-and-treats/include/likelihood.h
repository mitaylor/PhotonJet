#ifndef LIKELIHOOD_H
#define LIKELIHOOD_H

#include <array>
#include <functional>

namespace likelihood { enum op { add, sub, mul, div, nop }; }

template <likelihood::op T>
struct operation_t { };

template <likelihood::op T>
void likelihood_combination(std::array<double, 3> const& a,
                            std::array<double, 3> const& b,
                            std::function<double(std::array<double, 2>)> f,
                            double start);

#endif /* LIKELIHOOD_H */
