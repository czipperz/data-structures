#pragma once

#include "splay_map.hpp"

namespace ds {
namespace splay {

template <class Key>
struct Key_Comparator {
    const Key* key;

    template <class Value>
    int64_t operator()(const Pair<Key, Value>& pair) const {
        using cz::compare;
        return compare(*key, pair.key);
    }
};

template <class Key>
Key_Comparator<Key> key_comparator(const Key& key) {
    return {&key};
}

template <class Key, class Value>
Iterator<Pair<Key, Value> > Map<Key, Value>::find_equal(const Key& key) {
    return detail::find_equal_comparator(&tree, key_comparator(key));
}

template <class Key, class Value>
Iterator<Pair<Key, Value> > Map<Key, Value>::find_less(const Key& key) {
    return detail::find_less_comparator(&tree, key_comparator(key));
}

template <class Key, class Value>
Iterator<Pair<Key, Value> > Map<Key, Value>::find_greater(const Key& key) {
    return detail::find_greater_comparator(&tree, key_comparator(key));
}

template <class Key, class Value>
Iterator<Pair<Key, Value> > Map<Key, Value>::find_less_equal(const Key& key) {
    return detail::find_less_equal_comparator(&tree, key_comparator(key));
}

template <class Key, class Value>
Iterator<Pair<Key, Value> > Map<Key, Value>::find_greater_equal(const Key& key) {
    return detail::find_greater_equal_comparator(&tree, key_comparator(key));
}

}
}
