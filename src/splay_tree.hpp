#pragma once

#include <cz/allocator.hpp>
#include "gen_tree.hpp"

namespace ds {
namespace splay {

using gen::Iterator;
using gen::Node;

template <class T>
struct Tree {
    void drop(cz::Allocator allocator);

    /// Insert the element into the tree.  If the element already
    /// is present then does nothing and returns `false`.
    bool insert(cz::Allocator allocator, const T& element);

    /// Remove the element at the iterator.
    /// If the iterator is `end` then nothing is done.
    void remove(cz::Allocator allocator, Iterator<const T> iterator);

    /// Get iterators allowing you to iterate through the entire tree.
    Iterator<T> start();
    Iterator<T> end();
    Iterator<const T> start() const;
    Iterator<const T> end() const;

    /// Get iterators based on the position of the query.
    /// If there are no matches then `end` is returned.
    /// These methods `splay` so are not const.
    Iterator<T> find(const T& query) { return find_equal(query); }
    Iterator<T> find_equal(const T& query);
    Iterator<T> find_less(const T& query);
    Iterator<T> find_greater(const T& query);
    Iterator<T> find_less_equal(const T& query);
    Iterator<T> find_greater_equal(const T& query);

    bool contains(const T& element) { return find(element) != end(); }

    size_t count() const { return gen::count(root); }

    Node<T>* root;
};

namespace detail {

/// Same as `Tree` methods above except uses `comparator(node->element)`
/// instead of `compare(query, node->element)` to compare.
template <class T, class Comparator>
Iterator<T> find_equal_comparator(Tree<T>* tree, Comparator&& comparator);
template <class T, class Comparator>
Iterator<T> find_less_comparator(Tree<T>* tree, Comparator&& comparator);
template <class T, class Comparator>
Iterator<T> find_greater_comparator(Tree<T>* tree, Comparator&& comparator);
template <class T, class Comparator>
Iterator<T> find_less_equal_comparator(Tree<T>* tree, Comparator&& comparator);
template <class T, class Comparator>
Iterator<T> find_greater_equal_comparator(Tree<T>* tree, Comparator&& comparator);

}

}
}

#include "splay_tree.tpp"
