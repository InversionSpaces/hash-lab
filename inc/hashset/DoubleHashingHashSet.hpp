#pragma once

#include "OpenKeyHashSet.hpp"

namespace hashset {

template<typename T, class Hash1, class Hash2>
struct DoubleHashingRun {
    const size_t hash1;
    const size_t hash2;
    const size_t size;

    DoubleHashingRun(const T& val, const size_t size) :
        hash1(Hash1{}(val)), hash2(Hash2{}(val)), size(size) {}

    size_t operator()(size_t i) const {
        // here second hash is always odd
        // so it is mutually simple with size
        // also it is always positive
        const size_t id = hash1 + (hash2 | 1) * i;
        return id % size;
    }
};

// size of array should be always power of 2
template<typename T, class Hash1=std::hash<T>, class Hash2=std::hash<T>>
using DoubleHashingHashSet = OpenKeyHashSet<T, DoubleHashingRun<T, Hash1, Hash2>, 16, 2>;
} // namespace hashset