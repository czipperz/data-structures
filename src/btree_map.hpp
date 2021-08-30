#pragma once

#include "btree.hpp"
#include "gen_map.hpp"

namespace ds {
namespace btree {

template <class Key, class Value>
using Pair = gen::Map_Pair<Key, Value>;
template <class Key, class Value>
using Map_Iterator = Iterator<Pair<Key, Value> >;

template <class Key, class Value>
struct Map {
    using Pair = gen::Map_Pair<Key, Value>;

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
    void remove(cz::Allocator allocator, Iterator<const Pair> iterator) {
        return tree.remove(allocator, iterator, cz::compare<Pair>);
    }

    /// Get iterators allowing you to iterate through the entire tree.
    Iterator<Pair> start() { return tree.start(); }
    Iterator<Pair> end() { return tree.end(); }
    Iterator<const Pair> start() const { return tree.start(); }
    Iterator<const Pair> end() const { return tree.end(); }

    /// Convenience methods for loops.
    /// Example:
    /// ```
    /// Map<int, String> map;
    /// for (Map_Iterator<int, String> it = map.start_iter(1), end = map.end_iter(5);
    ///      it < end; ++it) {
    ///     CZ_ASSERT(*it >= 1 && *it < 5);
    /// }
    /// ```
    Iterator<Pair> start_iter(const Key& first) { return find_ge(first); }
    Iterator<Pair> end_iter(const Key& last) { return find_ge(last); }
    Iterator<const Pair> start_iter(const Key& first) const { return find_ge(first); }
    Iterator<const Pair> end_iter(const Key& last) const { return find_ge(last); }

    /// Get iterators based on the position of the element.
    /// If there are no matches then `end` is returned.
    /// These methods `splay` so are not const.
    Iterator<Pair> find(const Key& key) { return find_eq(key); }
    Iterator<Pair> find_eq(const Key& key);
    Iterator<Pair> find_lt(const Key& key);
    Iterator<Pair> find_gt(const Key& key);
    Iterator<Pair> find_le(const Key& key);
    Iterator<Pair> find_ge(const Key& key);
    Iterator<const Pair> find(const Key& key) const { return find_eq(key); }
    Iterator<const Pair> find_eq(const Key& key) const;
    Iterator<const Pair> find_lt(const Key& key) const;
    Iterator<const Pair> find_gt(const Key& key) const;
    Iterator<const Pair> find_le(const Key& key) const;
    Iterator<const Pair> find_ge(const Key& key) const;

    Tree_Comparator<Pair> tree;
};

}
}

#include "btree_map.cpp"
