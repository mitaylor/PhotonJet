#ifndef ZIP_H
#define ZIP_H

#include <iterator>

template <typename... T>
void _advance(T&... it) {
    (void)(int [sizeof...(T)]) { (++it, 0)... };
}

template <typename T, typename U, typename... V>
void _zip(T lambda, U it, U end, V... other) {
    for (; it != end; _advance(it, other...))
        lambda(*it, *other...);
}

template <typename T, typename U, typename... V>
void zip(T lambda, U& first, V&... other) {
    _zip(lambda, std::begin(first), std::end(first), std::begin(other)...);
}

#endif /* ZIP_H */
