#pragma once

namespace hashset {

template<typename T>
class IHashSet {
public:
    virtual bool insert(const T& val) = 0;
    virtual bool find(const T& val) const = 0;
    virtual bool remove(const T& val) = 0;
};

} // namespace