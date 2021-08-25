#pragma once

#include <cz/allocator.hpp>
#include "gen_tree.hpp"

namespace ds {
namespace splay {

using gen::Node;
using gen::Iterator;

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

    /// Get iterators based on the position of the element.
    /// If there are no matches then `end` is returned.
    /// These methods `splay` so are not const.
    Iterator<T> find(const T& element) { return find_equal(element); }
    Iterator<T> find_equal(const T& element);
    Iterator<T> find_less(const T& element);
    Iterator<T> find_greater(const T& element);
    Iterator<T> find_less_equal(const T& element);
    Iterator<T> find_greater_equal(const T& element);

    Node<T>* root;
};

}
}

#include "splay_tree.cpp"
