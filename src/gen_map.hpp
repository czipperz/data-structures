#pragma once

#include <cz/compare.hpp>
#include <cz/format.hpp>

namespace ds {
namespace gen {

template <class Key, class Value>
struct Map_Pair {
    Key key;
    Value value;

    bool operator==(const Map_Pair& other) const { return key == other.key; }
    bool operator!=(const Map_Pair& other) const { return !(*this == other); }
    bool operator<(const Map_Pair& other) const { return key < other.key; }
    bool operator>(const Map_Pair& other) const { return other < *this; }
    bool operator<=(const Map_Pair& other) const { return !(other < *this); }
    bool operator>=(const Map_Pair& other) const { return !(*this < other); }
};

}
}

namespace cz {

template <class Key, class Value>
int64_t compare(const ds::gen::Map_Pair<Key, Value>& left,
                const ds::gen::Map_Pair<Key, Value>& right) {
    return compare(left.key, right.key);
}

template <class Key, class Value>
void append(cz::Allocator allocator,
            cz::String* string,
            const ds::gen::Map_Pair<Key, Value>& pair) {
    append(allocator, string, "(", pair.key, " -> ", pair.value, ")");
}

}
