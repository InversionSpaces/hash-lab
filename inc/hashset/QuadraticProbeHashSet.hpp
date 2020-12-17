#pragma once

#include "OpenKeyHashSet.hpp"

namespace hashset {

template<typename T, class Hash>
struct QuadraticRun {
    const size_t hash;
    const size_t size;

    QuadraticRun(const T& val, const size_t size) :
        hash(Hash{}(val)), size(size) {}

    size_t operator()(size_t i) const {
        size_t id = hash + (i + i*i) >> 1;
        return id % size;
    }
};

// size of array should be always power of 2
template<typename T, class Hash=std::hash<T>>
using QuadraticProbeHashSet = OpenKeyHashSet<T, QuadraticRun<T, Hash>, 16, 2>;
} // namespace hashset