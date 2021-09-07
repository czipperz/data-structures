#pragma once

#include <stdint.h>
#include <cz/allocator.hpp>

namespace ds {
namespace pt {

template <class T>
struct Page_Table {
    void* root;
    uint8_t depth;
    uint64_t next_id;

    void drop(cz::Allocator allocator);

    /// Add an element and return its id.
    uint64_t add(cz::Allocator allocator, const T& element);

    /// Lookup an element by its id.  Returns `nullptr` if no match.
    T* lookup(uint64_t id);
    const T* lookup(uint64_t id) const;
};

}
}

#include "page_table.cpp"
