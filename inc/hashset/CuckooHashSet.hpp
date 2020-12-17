#pragma once

#include <array>
#include <vector>
#include <optional>
#include <utility>
#include <iostream>

namespace hashset {

using std::array;
using std::vector;
using std::pair;
using std::optional;
using std::nullopt;

template<typename T, class Hash1, class Hash2>
class CuckooHashSet : public IHashSet<T> {
    using elem_t = optional<T>;

    array<vector<elem_t>, 2> table;
    size_t table_size;
    size_t populated;

    Hash1 hash1;
    Hash2 hash2;

    elem_t& get_elem(const T& val, size_t half) {
        half &= 1;
        const size_t hash = half ? hash2(val) : hash1(val);
        return table.at(half).at(hash % table_size);
    }

    const elem_t& get_elem(const T& val, size_t half) const {
        half &= 1;
        const size_t hash = half ? hash2(val) : hash1(val);
        return table.at(half).at(hash % table_size);
    }

    pair<elem_t&, elem_t&> get_elems(const T& val) {
        return {
            get_elem(val, 0),
            get_elem(val, 1)
        };
    }

    pair<const elem_t&, const elem_t&> get_elems(const T& val) const {
        return {
            get_elem(val, 0),
            get_elem(val, 1)
        };
    }

    void rehash() {
        array<vector<elem_t>, 2> old = std::move(table);
        
        table_size *= 2;
        for (auto& half: table) {
            half.clear();
            half.assign(table_size, nullopt);
        }

        for (const auto& half: old)
            for (const auto& elem: half)
                if (elem)
                    insert(elem.value());
        
        for (auto& half: old)
            half.clear();
    }

    bool try_cuckoo(T&& drop, size_t half) {
        for (size_t i = 0; i < populated; ++i, ++half) {
            elem_t& elem = get_elem(drop, half);

            if (!elem) {
                elem = std::move(drop);
                ++populated;

                return true;
            }

            std::swap(drop, elem.value());
        }

        rehash();

        return insert(drop);
    }

public:
    // dont want size to be zero
    CuckooHashSet(size_t size=16) : 
        table_size(size == 0 ? 16 : size),
        populated(0) {
        for (auto& half: table)
            half.assign(table_size, nullopt);
    }

    virtual bool insert(const T& val) {
        auto [elem1, elem2] = get_elems(val);

        if (elem1 == val ||
            elem2 == val)
            return false;
        
        if (!elem1) {
            elem1 = val;
            ++populated;

            return true;
        }

        if (!elem2) {
            elem2 = val;
            ++populated;

            return true;
        }

        T drop = std::move(elem1.value());
        elem1 = val;

        return try_cuckoo(std::move(drop), 1);
    }

    virtual bool find(const T& val) const {
        const auto& [elem1, elem2] = get_elems(val);

        if (elem1 == val || elem2 == val)
            return true;
        
        return false;
    }

    virtual bool remove(const T& val) {
        auto [elem1, elem2] = get_elems(val);

        if (elem1 == val) {
            elem1 = nullopt;
            --populated;

            return true;
        }
            
        
        if (elem2 == val){
            elem2 = nullopt;
            --populated;

            return true;
        }

        return false;
    }

    void print(std::ostream& out) {
        out << populated << ": " << std::endl;
        for (const auto& half: table) {
            for (const auto& elem: half) {
                if (elem) out << elem.value();
                else out << "_";
                out << "\t";
            }
            out << std::endl;
        }
    }
};
} // namspace hashset