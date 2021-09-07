#ifndef DS_PAGE_TABLE_CPP
#define DS_PAGE_TABLE_CPP

#include "page_table.hpp"

namespace ds {
namespace pt {

namespace detail {
static inline constexpr uint8_t comptime_log2(uint64_t in) {
#define CASE(X) in >= ((uint64_t)1 << (63 - X)) ? (63 - X):
    return CASE(0) CASE(1) CASE(2) CASE(3) CASE(4) CASE(5) CASE(6) CASE(7) CASE(8) CASE(9) CASE(10)
        CASE(11) CASE(12) CASE(13) CASE(14) CASE(15) CASE(16) CASE(17) CASE(18) CASE(19) CASE(20)
            CASE(21) CASE(22) CASE(23) CASE(24) CASE(25) CASE(26) CASE(27) CASE(28) CASE(29)
                CASE(30) CASE(31) CASE(32) CASE(33) CASE(34) CASE(35) CASE(36) CASE(37) CASE(38)
                    CASE(39) CASE(40) CASE(41) CASE(42) CASE(43) CASE(44) CASE(45) CASE(46) CASE(47)
                        CASE(48) CASE(49) CASE(50) CASE(51) CASE(52) CASE(53) CASE(54) CASE(55)
                            CASE(56) CASE(57) CASE(58) CASE(59) CASE(60) CASE(61) CASE(62)
                                CASE(63) 0;
}

static inline constexpr uint64_t comptime_next_power_of_two_32(uint64_t value) {
    return value | (value >> 32);
}
static inline constexpr uint64_t comptime_next_power_of_two_16(uint64_t value) {
    return comptime_next_power_of_two_32(value | (value >> 16));
}
static inline constexpr uint64_t comptime_next_power_of_two_8(uint64_t value) {
    return comptime_next_power_of_two_16(value | (value >> 8));
}
static inline constexpr uint64_t comptime_next_power_of_two_4(uint64_t value) {
    return comptime_next_power_of_two_8(value | (value >> 4));
}
static inline constexpr uint64_t comptime_next_power_of_two_2(uint64_t value) {
    return comptime_next_power_of_two_4(value | (value >> 2));
}
static inline constexpr uint64_t comptime_next_power_of_two(uint64_t value) {
    return comptime_next_power_of_two_2(value | (value >> 1)) + 1;
}
}

template <class T>
struct Leaf_Elements {
    static constexpr const uint64_t size = detail::comptime_next_power_of_two(sizeof(T));
    static constexpr const uint64_t per_page = 4096 / size;
    static constexpr const uint64_t value = per_page >= 1 ? per_page : 1;
};

struct Node_Branch {
    void* children[512];
};

namespace detail {
template <class T>
void drop(void* node, uint8_t depth, cz::Allocator allocator) {
    if (depth <= 1) {
        allocator.dealloc((T*)node, Leaf_Elements<T>::value);
    } else {
        Node_Branch* branch = (Node_Branch*)node;
        for (size_t i = 512; i-- > 0;) {
            drop<T>(branch->children[i], depth - 1, allocator);
        }
        allocator.dealloc(branch);
    }
}

template <class T>
uint64_t add(Page_Table<T>* page_table, cz::Allocator allocator, const T& element) {
    uint64_t id = page_table->next_id++;

    uint8_t depth = page_table->depth;

    const uint8_t each = detail::comptime_log2(sizeof(Node_Branch) / sizeof(void*));
    const uint8_t base = detail::comptime_log2(Leaf_Elements<T>::value);
    const uint64_t each_mask = (1 << each) - 1;
    const uint64_t base_mask = (1 << base) - 1;
    static_assert(sizeof(Node_Branch) == sizeof(void*) * (1 << each),
                  "each must be log2(DIM(Node_Branch::children))");

    if (depth == 0) {
        T* leaf = allocator.alloc<T>(Leaf_Elements<T>::value);
        CZ_ASSERT(leaf);
        page_table->root = leaf;
        page_table->depth = 1;
        leaf[0] = element;
        return id;
    }

    // Add a new level on top.
    uint64_t total_shift = (depth - 1) * each + base;
    if (id == ((uint64_t)1 << total_shift)) {
        Node_Branch* branch = allocator.alloc_zeroed<Node_Branch>();
        branch->children[0] = page_table->root;
        page_table->root = branch;
        ++page_table->depth;
        ++depth;
    }

    void** node = &page_table->root;

    for (uint8_t i = depth; i-- > 1;) {
        uint8_t shift = (i - 1) * each + base;
        uint64_t index = (id >> shift) & each_mask;

        Node_Branch* branch = (Node_Branch*)*node;
        node = &branch->children[index];
        if (!*node) {
            if (i > 1) {
                *node = allocator.alloc_zeroed<Node_Branch>();
            } else {
                *node = allocator.alloc<T>(Leaf_Elements<T>::value);
            }
            CZ_ASSERT(*node);
        }
    }

    T* leaf = (T*)*node;
    uint64_t index = id & base_mask;
    leaf[index] = element;

    return id;
}

template <class T>
const T* lookup(const Page_Table<T>* page_table, uint64_t id) {
    if (id >= page_table->next_id)
        return nullptr;

    uint8_t depth = page_table->depth;
    if (depth == 0)
        return nullptr;

    const uint8_t each = detail::comptime_log2(sizeof(Node_Branch) / sizeof(void*));
    const uint8_t base = detail::comptime_log2(Leaf_Elements<T>::value);
    const uint64_t each_mask = (1 << each) - 1;
    const uint64_t base_mask = (1 << base) - 1;
    static_assert(sizeof(Node_Branch) == sizeof(void*) * (1 << each),
                  "each must be log2(DIM(Node_Branch::children))");

    void* node = page_table->root;

    for (uint8_t i = depth; i-- > 1;) {
        uint8_t shift = (i - 1) * each + base;
        uint64_t index = (id >> shift) & each_mask;

        Node_Branch* branch = (Node_Branch*)node;
        node = branch->children[index];
        CZ_DEBUG_ASSERT(node);
    }

    T* leaf = (T*)node;
    uint64_t index = id & base_mask;
    T* element = &leaf[index];
    return element;
}
}

template <class T>
void Page_Table<T>::drop(cz::Allocator allocator) {
    if (!root)
        return;

    detail::drop<T>(root, depth, allocator);
}

template <class T>
uint64_t Page_Table<T>::add(cz::Allocator allocator, const T& element) {
    return detail::add(this, allocator, element);
}

template <class T>
T* Page_Table<T>::lookup(uint64_t id) {
    return (T*)detail::lookup(this, id);
}

template <class T>
const T* Page_Table<T>::lookup(uint64_t id) const {
    return detail::lookup(this, id);
}

}
}

#endif
