#pragma once

#include "btree.hpp"
#include "gen_map.hpp"

namespace ds {
namespace btree {

template <class Key, class Value>
using Pair = gen::Map_Pair<Key, Value>;
template <class Key, class Value>
using Map_Iterator = Iterator<Pair<Key, Value> >;

template <class Key,
          class Value,
          size_t Maximum_Elements = Default_Maximum_Elements<Pair<Key, Value> >::value>
struct Map {
    using Pair = gen::Map_Pair<Key, Value>;
    using Iterator = ds::btree::Iterator<Pair, Maximum_Elements>;
    using Const_Iterator = ds::btree::Iterator<const Pair, Maximum_Elements>;
    constexpr static const size_t M = Maximum_Elements;

    void drop(cz::Allocator allocator) { return tree.drop(allocator); }

    /// Insert the element into the tree.  If the element already
    /// is present then does nothing and returns `false`.
    bool insert(cz::Allocator allocator, const Key& key, const Value& value) {
        return insert(allocator, {key, value});
    }
    bool insert(cz::Allocator allocator, const Pair& pair) {
        return tree.insert(allocator, pair, cz::compare<Pair>);
    }

    /// Remove the element at the iterator.
    /// If the iterator is `end` then nothing is done.
    void remove(cz::Allocator allocator, Const_Iterator iterator) {
        return tree.remove(allocator, iterator, cz::compare<Pair>);
    }

    /// Get iterators allowing you to iterate through the entire tree.
    Iterator start() { return tree.start(); }
    Iterator end() { return tree.end(); }
    Const_Iterator start() const { return tree.start(); }
    Const_Iterator end() const { return tree.end(); }

    /// Convenience methods for loops.
    /// Example:
    /// ```
    /// Map<int, String> map;
    /// for (Map_Iterator<int, String> it = map.start_iter(1), end = map.end_iter(5);
    ///      it < end; ++it) {
    ///     CZ_ASSERT(*it >= 1 && *it < 5);
    /// }
    /// ```
    Iterator start_iter(const Key& first) { return find_ge(first); }
    Iterator end_iter(const Key& last) { return find_ge(last); }
    Const_Iterator start_iter(const Key& first) const { return find_ge(first); }
    Const_Iterator end_iter(const Key& last) const { return find_ge(last); }

    /// Get iterators based on the position of the element.
    /// If there are no matches then `end` is returned.
    /// These methods `splay` so are not const.
    Iterator find(const Key& key) { return find_eq(key); }
    Iterator find_eq(const Key& key);
    Iterator find_lt(const Key& key);
    Iterator find_gt(const Key& key);
    Iterator find_le(const Key& key);
    Iterator find_ge(const Key& key);
    Const_Iterator find(const Key& key) const { return find_eq(key); }
    Const_Iterator find_eq(const Key& key) const;
    Const_Iterator find_lt(const Key& key) const;
    Const_Iterator find_gt(const Key& key) const;
    Const_Iterator find_le(const Key& key) const;
    Const_Iterator find_ge(const Key& key) const;

    Tree_Comparator<Pair, Maximum_Elements> tree;
};

}
}

#include "btree_map.cpp"
