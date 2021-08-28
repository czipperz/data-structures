#pragma once

#include <cz/allocator.hpp>
#include <cz/format.hpp>

namespace ds {
namespace btree {

template <class T>
struct Default_Maximum_Elements {
    static const size_t items_per_page = (4096 - 4 * sizeof(void*)) / (sizeof(T) + sizeof(void*));
    static const size_t value = items_per_page > 4 ? items_per_page : 4;
};

template <class T, size_t Maximum_Elements>
struct Node {
    Node* parent;
    size_t parent_index;
    size_t num_elements;
    Node* children[Maximum_Elements + 1];
    T elements[Maximum_Elements];
};

template <class T, size_t Maximum_Elements = Default_Maximum_Elements<T>::value>
struct Iterator {
    Node<T, Maximum_Elements>* node;
    size_t index;

    T& operator*() const { return node->elements[index]; }
    T* operator->() const { return &node->elements[index]; }
    operator Iterator<const T>() const { return {node, index}; }

    Iterator& operator++() {
        ++index;
        return *this;
    }
    Iterator& operator--() {
        --index;
        return *this;
    }

    bool operator==(const Iterator& other) const {
        return node == other.node && index == other.index;
    }
    bool operator!=(const Iterator& other) const { return !(*this == other); }
};

template <class T, size_t Maximum_Elements = Default_Maximum_Elements<T>::value>
struct BTree {
    static_assert(Maximum_Elements >= 1, "0 elements doesn't allow insertion");
    using Node = Node<T, Maximum_Elements>;
    using Iterator = ds::btree::Iterator<T, Maximum_Elements>;
    using Const_Iterator = ds::btree::Iterator<const T, Maximum_Elements>;

    void drop(cz::Allocator allocator);

    bool insert(cz::Allocator allocator, const T& element);

    void remove(cz::Allocator allocator, Const_Iterator iterator);

    Iterator start();
    Iterator end();
    Const_Iterator start() const;
    Const_Iterator end() const;

    Node* root;
};

}
}

namespace cz {

template <class T, size_t Maximum_Elements>
void append(Allocator allocator,
            String* string,
            ds::btree::Iterator<T, Maximum_Elements> iterator) {
    if (iterator.node && iterator.index < iterator.node->num_elements) {
        cz::append(allocator, string, iterator.node->elements[iterator.index]);
    } else {
        cz::append(allocator, string, "<end>");
    }
}

}

#include "btree.cpp"
