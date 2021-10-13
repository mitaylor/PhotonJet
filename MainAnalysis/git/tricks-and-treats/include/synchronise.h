#ifndef SYNCHRONISE_H
#define SYNCHRONISE_H

#include <algorithm>
#include <iterator>
#include <vector>

template <typename T, typename U>
void sort_by(std::vector<T> const& order, std::vector<U>& values) {
    std::vector<U> result;
    for (auto const& o : order)
        result.emplace_back(std::move(values[o.first]));
    values.swap(result);
}

template <typename T, typename... U>
void sort_by(std::vector<T> const& order, std::vector<U>&... values) {
    (void)(int [sizeof...(U)]) { (sort_by(order, values), 0)... };
}

template <typename T>
using vector_t_const_iterator = typename std::vector<T>::const_iterator;

template <typename T>
using order_type = std::pair<std::size_t, vector_t_const_iterator<T>>;

template <typename T, typename... U>
void synchronise(std::vector<T>& ref, std::vector<U>&... values) {
    std::vector<order_type<T>> order;
    for (auto it = std::begin(ref); it != std::end(ref); ++it)
        order.emplace_back(std::distance(std::begin(ref), it), it);
    std::sort(std::begin(order), std::end(order),
              [](auto const& a, auto const& b) {
        return *a.second < *b.second; });

    sort_by(order, ref, values...);
}

#endif /* SYNCHRONISE_H */
