#pragma once

#include "OpenKeyHashSet.hpp"

namespace hashset {

template<typename T, class Hash>
struct SimpleRun {
    const size_t hash;
    const size_t size;

    SimpleRun(const T& val, const size_t size) :
        hash(Hash{}(val)), size(size) {}

    size_t operator()(size_t i) const {
        return (hash + i) % size;
    }
};

template<typename T, class Hash=std::hash<T>, size_t size=16, size_t scale=2>
using LinearProbeHashSet = OpenKeyHashSet<T, SimpleRun<T, Hash>, size, scale>;
} // namespace hashset