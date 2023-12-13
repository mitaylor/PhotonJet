#ifndef OVERFLOW_ANGLES_H
#define OVERFLOW_ANGLES_H

#include <limits>

constexpr int64_t convert_pi(long double angle) {
    return static_cast<int64_t>(angle
        * std::numeric_limits<int64_t>::max());
}

constexpr int64_t convert_radian(long double angle) {
    return static_cast<int64_t>((angle / M_PI)
        * std::numeric_limits<int64_t>::max());
}

constexpr int64_t convert_degree(long double angle) {
    return static_cast<int64_t>((angle / 180.)
        * std::numeric_limits<int64_t>::max());
}

constexpr int64_t operator ""_pi(long double angle) {
    return convert_pi(angle);
}

constexpr int64_t operator ""_rad(long double angle) {
    return convert_radian(angle);
}

constexpr int64_t operator ""_deg(long double angle) {
    return convert_degree(angle);
}

constexpr double revert_pi(int64_t angle) {
    return static_cast<double>(angle)
        / std::numeric_limits<int64_t>::max();
}

constexpr double revert_radian(int64_t angle) {
    return static_cast<double>(angle) * M_PI
        / std::numeric_limits<int64_t>::max();
}

constexpr double revert_degree(int64_t angle) {
    return static_cast<double>(angle) * 180.
        / std::numeric_limits<int64_t>::max();
}

template <template <typename...> class T, typename U>
void convert_in_place_pi(T<U>& container) {
    for (auto& element : container)
        element = convert_pi(element);
}

template <template <typename...> class T, typename U>
void convert_in_place_radian(T<U>& container) {
    for (auto& element : container)
        element = convert_radian(element);
}

template <template <typename...> class T, typename U>
void convert_in_place_degree(T<U>& container) {
    for (auto& element : container)
        element = convert_degree(element);
}

#endif /* OVERFLOW_ANGLES_H */
