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

/// Requires non-null input and non-null `right->parent`.
void swap_positions(Node_Base* left, Node_Base* right);

/// Requires non-null input.
void remove(Node_Base*);

template <class T>
void recursive_dealloc(cz::Allocator allocator, Node<T>* node) {
    if (!node)
        return;

    recursive_dealloc(allocator, (Node<T>*)node->left);
    recursive_dealloc(allocator, (Node<T>*)node->right);
    allocator.dealloc(node);
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
static void val_node(Node<T>* node, Node<T>* parent) {
    if (!node)
        return;

    val_node((Node<T>*)node->left, node);
    val_node((Node<T>*)node->right, node);

    CZ_ASSERT(node->parent == parent);

    if (node->left) {
        CZ_ASSERT(((Node<T>*)node->left)->element < node->element);
    }
    if (node->right) {
        CZ_ASSERT(((Node<T>*)node->right)->element > node->element);
    }
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
