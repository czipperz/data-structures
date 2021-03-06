#pragma once

#include <cz/allocator.hpp>

namespace ds {
namespace gen {

struct Node_Base {
    Node_Base* parent;
    Node_Base* left;
    Node_Base* right;
};

template <class T>
struct Node : Node_Base {
    T element;
};

/// Requires non-null input and non-null child being rotated.
void rotate_left(Node_Base* node);
void rotate_right(Node_Base* node);

/// Allow null inputs.
Node_Base* leftmost(Node_Base* root);
Node_Base* rightmost(Node_Base* root);

/// Allow null inputs.
Node_Base* node_after(Node_Base*);
Node_Base* node_before(Node_Base*);

/// Requires non-null input.
Node_Base* remove(Node_Base*);
void remove_leaf(Node_Base*);

/// Allow null inputs.
size_t count(Node_Base*);

template <class T>
void recursive_dealloc(cz::Allocator allocator, Node<T>* node) {
    while (1) {
        if (!node)
            return;

        recursive_dealloc(allocator, (Node<T>*)node->left);
        Node<T>* right = (Node<T>*)node->right;
        allocator.dealloc(node);
        node = right;
    }
}

template <class T>
struct Iterator {
    bool operator==(Iterator other) const { return node == other.node; }
    bool operator!=(Iterator other) const { return !(*this == other); }

    bool operator<(Iterator other) const {
        if (node == nullptr)
            return false;
        else if (other.node == nullptr)
            return true;
        else
            return node->element < other.node->element;
    }
    bool operator>(Iterator other) const { return other < *this; }
    bool operator<=(Iterator other) const { return !(other < *this); }
    bool operator>=(Iterator other) const { return !(*this < other); }

    Iterator& operator++() {
        node = (Node<T>*)node_after(node);
        return *this;
    }
    Iterator& operator--() {
        // TODO: deal with retreating from start?
        // TODO: deal with retreating from end doing nothing!
        node = (Node<T>*)node_before(node);
        return *this;
    }

    operator Iterator<const T>() const { return {(Node<const T>*)node}; }

    T& operator*() const { return node->element; }
    T* operator->() const { return &node->element; }

    Node<T>* node;
};

template <class T>
void val_node(Node<T>* node, Node<T>* parent) {
    if (!node)
        return;

    val_node((Node<T>*)node->left, node);
    val_node((Node<T>*)node->right, node);

    CZ_ASSERT(node->parent == parent);

    using cz::compare;
    if (node->left) {
        CZ_ASSERT(((Node<T>*)node->left)->element < node->element);
    }
    if (node->right) {
        CZ_ASSERT(((Node<T>*)node->right)->element > node->element);
    }
}

template <class T>
struct Element_Comparator {
    const T* element;
    int64_t operator()(const T& other) const {
        using cz::compare;
        return compare(*element, other);
    }
};
/// Convenience constructor.
template <class T>
Element_Comparator<T> element_comparator(const T& element) {
    return {&element};
}

template <class T, class Comparator>
Node<T>* find_comparator(Node<T>* root, int64_t* last_comparison, Comparator&& comparator) {
    Node<T>* parent = nullptr;
    Node<T>* node = root;
    int64_t comparison = 0;
    while (node) {
        gen::Node_Base* new_node = nullptr;

        comparison = comparator(node->element);
        if (comparison < 0) {
            new_node = node->left;
        } else if (comparison > 0) {
            new_node = node->right;
        } else {
            new_node = nullptr;
        }

        parent = node;
        node = (Node<T>*)new_node;
    }

    *last_comparison = comparison;
    return parent;
}

template <class T>
Node<T>* find(Node<T>* root, int64_t* last_comparison, const T& element) {
    return find_comparator(root, last_comparison, element_comparator(element));
}

}
}

#include <cz/format.hpp>

namespace cz {

template <class T>
void append(cz::Allocator allocator, cz::String* string, ds::gen::Iterator<T> iterator) {
    if (iterator.node) {
        cz::append(allocator, string, iterator.node->element);
    } else {
        cz::append(allocator, string, "<end>");
    }
}

}
