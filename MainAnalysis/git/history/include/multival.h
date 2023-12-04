#ifndef MULTIVAL_H
#define MULTIVAL_H

#include <iterator>
#include <numeric>
#include <type_traits>
#include <vector>

#include "interval.h"

class multival {
  public:
    template <typename... T>
    multival(T const&... intervals)
            : _dims(sizeof...(T)) {
        extract(intervals...);
        _size = std::accumulate(std::begin(_shape), std::end(_shape),
                                1, std::multiplies<int64_t>());
    }

    multival(multival const& other) = default;
    multival& operator=(multival const& other) = default;
    ~multival() = default;

    std::vector<int64_t> indices_for(int64_t index) const;

    template <template <typename...> class T>
    std::vector<int64_t> indices_for(T<double> const& values) const {
        std::vector<int64_t> indices;
        auto append = [&](interval const& axis, double val) -> int32_t {
            indices.push_back(axis.index_for(val)); return 0; };

        std::inner_product(std::begin(_intervals), std::end(_intervals),
                           std::begin(values), 0, std::plus<>(), append);

        return indices;
    }

    template <template <typename...> class T, typename U>
    typename std::enable_if<std::is_integral<U>::value, int64_t>::type
    index_for(T<U> const& indices) const {
        int64_t block = 1;
        int64_t index = 0;

        for (size_t i = 0; i < _dims; ++i) {
            index += block * indices[i];
            block *= axis;

            if (indices[i] < 0) return -1;
            if (indices[i] > _intervals[i].size()) return size_ + 1;
        }

        return index;
    }

    template <template <typename...> class T, typename U>
    typename std::enable_if<std::is_floating_point<U>::value, int64_t>::type
    index_for(T<U> const& values) const {
        return index_for(indices_for(values)); }

    template <typename T>
    T* book(int64_t, std::string const&, std::string const&) const;

    template <typename T, int64_t N>
    T* book(int64_t, std::string const&, std::string const&,
            std::array<int64_t, N> const&) const;

    std::vector<int64_t> const& shape() const { return _shape; }
    int64_t dims() const { return _dims; }
    int64_t size() const { return _size; }

    interval const& axis(int64_t i) const { return _intervals[i]; }
    std::vector<interval> const& axes() const { return _intervals; }

  private:
    template <typename... T>
    void extract(T const&... args) {
        (void) (int [sizeof...(T)]) { (_intervals.emplace_back(args), 0)... };
        for (auto const& axis : _intervals) { _shape.push_back(axis.size()); }
    }

    std::vector<int64_t> _shape;
    int64_t _dims;
    int64_t _size;

    std::vector<interval> _intervals;
};

#endif /* MULTIVAL_H */