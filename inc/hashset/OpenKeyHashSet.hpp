#pragma once

#include <cstddef>
#include <functional>
#include <vector>
#include <algorithm>
#include <variant>
#include <stdexcept>

#include "IHashSet.hpp"

#include <iostream>

namespace hashset {

using std::vector;
using std::variant;
using std::function;

// here size and scale are templates cause
// implementations need to strictly control it
template<typename T, class Run, size_t size, size_t scale>
class OpenKeyHashSet : public IHashSet<T> {
    enum EmptyState {
        EMPTY,
        TOMBSTONE
    };
    
    using elem_t = variant<T, EmptyState>;
    
    vector<elem_t> array;
    
    const double factor;
    size_t populated;

    static inline bool is_tombs(const elem_t& elem) {
        return  std::holds_alternative<EmptyState>(elem) &&
                std::get<EmptyState>(elem) == TOMBSTONE;
    }

    static inline bool is_empty(const elem_t& elem) {
        return  std::holds_alternative<EmptyState>(elem) &&
                std::get<EmptyState>(elem) == EMPTY;
    }

    static inline const T& get_val(const elem_t& elem) {
        return std::get<T>(elem);
    }

    static inline T& get_val(elem_t& elem) {
        return std::get<T>(elem);
    }

    inline void rehash() {
        if (populated < array.size() * factor) return;

        vector<elem_t> elems = std::move(array);
        
        array.clear();
        populated = 0;
        array.assign(elems.size() * scale, EMPTY);
        
        for (elem_t& elem: elems) {
            if (is_empty(elem) || is_tombs(elem))
                continue;
            
            insert(get_val(elem));
        }
        
        elems.clear();
    }

public:
    OpenKeyHashSet(double factor=0.75) : 
        factor(factor), populated(0), array(size, EMPTY) {}

    virtual bool insert(const T& val) override {
        rehash();

        if (find(val))
            return false;

        Run run(val, array.size());
        for (size_t i = 0; i < array.size(); ++i) {
            const size_t& id = run(i);
            auto& elem = array.at(id);

            if (is_tombs(elem) || is_empty(elem)) {
                if (is_empty(elem)) 
                    ++populated;
                elem = val;

                return true;
            }
        }
        
        // we choosed a bad run function
        throw std::runtime_error("Run function didn't cover all array");
    }

    virtual bool find(const T& val) const override {
        Run run(val, array.size());
        for (size_t i = 0; i < array.size(); ++i) {
            const size_t& id = run(i);
            const auto& elem = array.at(id);

            if (is_tombs(elem))
                continue;

            if (is_empty(elem))
                return false;

            if (get_val(elem) == val)
                return true;
        }
        
        return false;
    }

    virtual bool remove(const T& val) override {
        Run run(val, array.size());
        for (size_t i = 0; i < array.size(); ++i) {
            const size_t& id = run(i);
            auto& elem = array.at(id);

            if (is_tombs(elem))
                continue;
            
            if (is_empty(elem))
                return false;
            
            if (get_val(elem) == val) {
                elem = TOMBSTONE;

                return true; 
            }
        }
        
        return false;
    }

    void print(std::ostream& out) {
        out << populated << ": ";
        for (const auto& elem: array) {
            if (is_empty(elem))
                out << "E ";
            else if (is_tombs(elem))
                out << "T ";
            else out << get_val(elem) << " ";
        }

        out << std::endl;
    }
};

} // namespace hashset