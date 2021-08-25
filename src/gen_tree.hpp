#pragma once

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
void rotate_left(Node_Base** node);
void rotate_right(Node_Base** node);

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
struct Iterator {
    bool operator==(Iterator other) { return node == other.node; }
    bool operator!=(Iterator other) { return !(*this == other); }

    bool operator<(Iterator other) {
        if (node == nullptr)
            return false;
        else if (other.node == nullptr)
            return true;
        else
            return node->element < other.node->element;
    }
    bool operator>(Iterator other) { return other < *this; }
    bool operator<=(Iterator other) { return !(other < *this); }
    bool operator>=(Iterator other) { return !(*this < other); }

    void advance() { node = node_after(node); }
    void retreat() {
        node = node_before(node);
        // TODO: deal with retreating from start?
        // TODO: deal with retreating from end doing nothing!
    }

    operator Iterator<const T>() const { return {(Node<const T>*)node}; }

    T& operator*() const { return node->element; }
    T* operator->() const { return &node->element; }

    Node<T>* node;
};

}
}

#include "gen_tree.cpp"
