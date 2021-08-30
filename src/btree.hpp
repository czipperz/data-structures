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
        if (node->children[index + 1]) {
            node = node->children[index + 1];
            index = 0;
            while (node->children[0])
                node = node->children[0];
        } else {
            ++index;
            while (index == node->num_elements) {
                if (!node->parent) {
                    return *this;
                }
                index = node->parent_index;
                node = node->parent;
            }
        }
        return *this;
    }
    Iterator& operator--() {
        if (node->children[index]) {
            node = node->children[index];
            index = node->num_elements;
            while (node->children[index]) {
                node = node->children[index];
                index = node->num_elements;
            }
            --index;
        } else {
            while (index == 0) {
                if (!node->parent) {
                    --index;
                    return *this;
                }
                index = node->parent_index;
                node = node->parent;
            }
            --index;
        }
        return *this;
    }

    bool operator==(const Iterator& other) const {
        return node == other.node && index == other.index;
    }
    bool operator!=(const Iterator& other) const { return !(*this == other); }
};

template <class T, size_t Maximum_Elements>
struct Tree_Base {
    static_assert(Maximum_Elements >= 1, "0 elements doesn't allow insertion");
    using Node = Node<T, Maximum_Elements>;
    using Iterator = ds::btree::Iterator<T, Maximum_Elements>;
    using Const_Iterator = ds::btree::Iterator<const T, Maximum_Elements>;
    constexpr static const size_t M = Maximum_Elements;

    void drop(cz::Allocator allocator);

    Iterator start();
    Iterator end();
    Const_Iterator start() const;
    Const_Iterator end() const;

    void remove(cz::Allocator allocator, Const_Iterator iterator);

    Node* root;
};

template <class T, size_t Maximum_Elements = Default_Maximum_Elements<T>::value>
struct Tree : Tree_Base<T, Maximum_Elements> {
    bool insert(cz::Allocator allocator, const T& element);

    Iterator find(const T& element) { return find_eq(element); }
    Iterator find_eq(const T& element);
    Iterator find_lt(const T& element);
    Iterator find_gt(const T& element);
    Iterator find_le(const T& element);
    Iterator find_ge(const T& element);
};

template <class T, size_t Maximum_Elements = Default_Maximum_Elements<T>::value>
struct Tree_Comparator : Tree_Base<T, Maximum_Elements> {
    template <class Comparator>
    bool insert(cz::Allocator allocator, const T& element, Comparator&& comparator);

    template <class Comparator>
    Iterator find(const T& element, Comparator&& comparator) {
        return find_eq(element, comparator);
    }
    template <class Comparator>
    Iterator find_eq(const T& element, Comparator&& comparator);
    template <class Comparator>
    Iterator find_lt(const T& element, Comparator&& comparator);
    template <class Comparator>
    Iterator find_gt(const T& element, Comparator&& comparator);
    template <class Comparator>
    Iterator find_le(const T& element, Comparator&& comparator);
    template <class Comparator>
    Iterator find_ge(const T& element, Comparator&& comparator);
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
