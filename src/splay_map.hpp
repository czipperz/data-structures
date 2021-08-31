#pragma once

#include <cz/allocator.hpp>
#include "gen_map.hpp"
#include "splay_tree.hpp"

namespace ds {
namespace splay {

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
        return tree.insert(allocator, {key, value});
    }
    bool insert(cz::Allocator allocator, const Pair& pair) { return tree.insert(allocator, pair); }

    /// Remove the element at the iterator.
    /// If the iterator is `end` then nothing is done.
    void remove(cz::Allocator allocator, Iterator<const Pair> iterator) {
        return tree.remove(allocator, iterator);
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
    Iterator<Pair> start_iter(const Key& first) { return find_greater_equal(first); }
    Iterator<Pair> end_iter(const Key& last) { return find_greater_equal(last); }

    /// Get iterators based on the position of the element.
    /// If there are no matches then `end` is returned.
    /// These methods `splay` so are not const.
    Iterator<Pair> find(const Key& key) { return find_equal(key); }
    Iterator<Pair> find_equal(const Key& key);
    Iterator<Pair> find_less(const Key& key);
    Iterator<Pair> find_greater(const Key& key);
    Iterator<Pair> find_less_equal(const Key& key);
    Iterator<Pair> find_greater_equal(const Key& key);

    bool contains(const Key& key) { return find(key) != end(); }

    size_t count() const { return tree.count(); }

    Tree<Pair> tree;
};

}
}

#include "splay_map.cpp"
