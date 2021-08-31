#ifndef DS_BTREE_MAP_CPP
#define DS_BTREE_MAP_CPP

#include "btree_map.hpp"

namespace ds {
namespace btree {

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

template <class Key, class Value, size_t Maximum_Elements>
Iterator<Pair<Key, Value>, Maximum_Elements> Map<Key, Value, Maximum_Elements>::find_eq(
    const Key& key) {
    return tree.find_eq(key_comparator(key));
}

template <class Key, class Value, size_t Maximum_Elements>
Iterator<Pair<Key, Value>, Maximum_Elements> Map<Key, Value, Maximum_Elements>::find_lt(
    const Key& key) {
    return tree.find_lt(key_comparator(key));
}

template <class Key, class Value, size_t Maximum_Elements>
Iterator<Pair<Key, Value>, Maximum_Elements> Map<Key, Value, Maximum_Elements>::find_gt(
    const Key& key) {
    return tree.find_gt(key_comparator(key));
}

template <class Key, class Value, size_t Maximum_Elements>
Iterator<Pair<Key, Value>, Maximum_Elements> Map<Key, Value, Maximum_Elements>::find_le(
    const Key& key) {
    return tree.find_le(key_comparator(key));
}

template <class Key, class Value, size_t Maximum_Elements>
Iterator<Pair<Key, Value>, Maximum_Elements> Map<Key, Value, Maximum_Elements>::find_ge(
    const Key& key) {
    return tree.find_ge(key_comparator(key));
}

template <class Key, class Value, size_t Maximum_Elements>
Iterator<const Pair<Key, Value>, Maximum_Elements> Map<Key, Value, Maximum_Elements>::find_eq(
    const Key& key) const {
    return tree.find_eq(key_comparator(key));
}

template <class Key, class Value, size_t Maximum_Elements>
Iterator<const Pair<Key, Value>, Maximum_Elements> Map<Key, Value, Maximum_Elements>::find_lt(
    const Key& key) const {
    return tree.find_lt(key_comparator(key));
}

template <class Key, class Value, size_t Maximum_Elements>
Iterator<const Pair<Key, Value>, Maximum_Elements> Map<Key, Value, Maximum_Elements>::find_gt(
    const Key& key) const {
    return tree.find_gt(key_comparator(key));
}

template <class Key, class Value, size_t Maximum_Elements>
Iterator<const Pair<Key, Value>, Maximum_Elements> Map<Key, Value, Maximum_Elements>::find_le(
    const Key& key) const {
    return tree.find_le(key_comparator(key));
}

template <class Key, class Value, size_t Maximum_Elements>
Iterator<const Pair<Key, Value>, Maximum_Elements> Map<Key, Value, Maximum_Elements>::find_ge(
    const Key& key) const {
    return tree.find_ge(key_comparator(key));
}

}
}

#endif
