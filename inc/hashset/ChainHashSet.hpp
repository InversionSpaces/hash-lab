#pragma once

#include <functional>
#include <vector>
#include <list>
#include <algorithm>

#include "IHashSet.hpp"

namespace hashset {

using std::vector;
using std::list;

template<typename T, class Hash=std::hash<T>, size_t scale=2>
class ChainHashSet : public IHashSet<T> {
    Hash hash;
    vector<list<T>> array;
    
    const double factor;
    size_t size;

    inline list<T>& get_chain(const T& val) {
        size_t h = hash(val);
        return array.at(h % array.size());
    }

    inline const list<T>& get_chain(const T& val) const {
        size_t h = hash(val);
        return array.at(h % array.size());
    }

    inline void rehash() {
        if (size < array.size() * factor) return;

        vector<list<T>> elems = std::move(array);
        array.clear(); array.resize(size * scale);
        size = 0;
        
        for (auto& chain: elems)
            for (auto& elem: chain)
                emplace(std::move(elem));
        
        elems.clear();
    }
public:
    ChainHashSet(double factor=0.75) : 
    factor(factor), size(0), array(16) {}

    virtual bool insert(const T& val) override {
        rehash();

        list<T>& chain = get_chain(val);
        auto it = std::find(chain.begin(), chain.end(), val);
        
        if (it != chain.end()) return false;
        
        chain.push_front(val);
        ++size;

        return true;
    }

    bool emplace(T&& val) {
        rehash();

        list<T>& chain = get_chain(val);
        auto it = std::find(chain.begin(), chain.end(), val);
        
        if (it != chain.end()) return false;
        
        chain.emplace_front(std::move(val));
        ++size;

        return true;
    }

    virtual bool find(const T& val) const override {
        const list<T>& chain = get_chain(val);
        auto it = std::find(chain.begin(), chain.end(), val);
        
        return it != chain.end();
    }

    virtual bool remove(const T& val) override {
        list<T>& chain = get_chain(val);
        auto it = std::find(chain.begin(), chain.end(), val);

        if (it == chain.end()) return false;

        chain.erase(it);
        --size;

        return true;
    }
};

} // namespace hashset