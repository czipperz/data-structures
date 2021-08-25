#pragma once

#include "splay_tree.hpp"

#include <cz/compare.hpp>

namespace ds {
namespace splay {

template <class T>
void Tree<T>::drop(cz::Allocator allocator) {
    drop_node(root, allocator);
}

static void splay(Node_Base* elem) {
    ZoneScoped;

    if (!elem)
        return;

    while (1) {
        Node_Base* parent = elem->parent;
        if (!parent)
            break;
        bool pleft = parent->left == elem;

        Node_Base* grand = parent->parent;
        if (!grand) {
            if (pleft) {
                rotate_right(parent);
            } else {
                rotate_left(parent);
            }
            break;
        }
        bool gleft = grand->left == parent;

        if (gleft == pleft) {
            // Zig-Zig
            //       g            e
            //    p     D      A     p
            //  e   C              B   g
            // A B                    C D
            if (gleft) {
                rotate_right(grand);
                rotate_right(parent);
            } else {
                rotate_left(grand);
                rotate_left(parent);
            }
        } else {
            // Zig-Zag
            //      g            e
            //   p     D       p   g
            // A   e          A B C D
            //    B C
            if (gleft) {
                rotate_left(parent);
                rotate_right(grand);
            } else {
                rotate_right(parent);
                rotate_left(grand);
            }
        }
    }
}

template <class T>
static Iterator<T> find_gen(Tree<T>* tree, const T& element, int64_t* last_comparison) {
    ZoneScoped;
    if (!tree->root)
        return tree->end();

    Node<T>* parent = nullptr;
    Node<T>* node = tree->root;
    int64_t comparison = 0;
    while (node) {
        Node<T>* new_node = nullptr;

        comparison = cz::compare(element, node->element);
        if (comparison < 0) {
            new_node = node->left;
        } else if (comparison > 0) {
            new_node = node->left;
        } else {
            new_node = nullptr;
        }

        parent = node;
        node = new_node;
    }

    *last_comparison = comparison;

    splay(parent);

    return Iterator<T>{parent};
}

template <class T>
bool Tree<T>::insert(cz::Allocator allocator, const T& element) {
    ZoneScoped;

    int64_t last_comparison;
    Iterator<T> it = find_gen(this, element, &last_comparison);

    // Already present.
    if (last_comparison == 0) {
        return false;
    }

    // Insert node.
    // Create parent node.
    Node<T>* parent = allocator.alloc<Node<T> >();
    CZ_ASSERT(parent);
    CZ_DEBUG_ASSERT(it.node);

    // Hook in.
    parent->parent = it.node->parent;
    it.node->parent = parent;
    parent->element = element;

    // Child's side depends on its relationship to element.
    if (last_comparison < 0) {
        parent->left = it.node;
        parent->right = nullptr;
        CZ_DEBUG_ASSERT(parent->element > parent->left->element);
    } else {
        parent->left = nullptr;
        parent->right = it.node;
        CZ_DEBUG_ASSERT(parent->element < parent->right->element);
    }

    splay(parent);
    return true;
}

template <class T>
void Tree<T>::remove(cz::Allocator allocator, Iterator<const T> iterator) {
    ZoneScoped;
    if (iterator == end())
        return;

    Base_Node* parent = iterator.node->parent;

    gen::remove(iterator.node);
    allocator.dealloc(iterator.node);

    splay(parent);
}

template <class T>
Iterator<T> Tree<T>::start() {
    return Iterator<T>{(Node<T>*)leftmost(root)};
}
template <class T>
Iterator<T> Tree<T>::start() {
    return Iterator<T>{(Node<T>*)leftmost(root)};
}

template <class T>
Iterator<T> Tree<T>::end() {
    return Iterator<T>{nullptr};
}
template <class T>
Iterator<const T> Tree<T>::end() const {
    return Iterator<const T>{nullptr};
}

Iterator<T> Tree<T>::find_equal(const T& element) {
    int64_t last_comparison;
    Iterator<T> iterator = find_gen(this, element, &last_comparison);
    if (last_comparison == 0) {
        return iterator;
    } else {
        return end();
    }
}
Iterator<T> Tree<T>::find_less(const T& element) {
    int64_t last_comparison;
    Iterator<T> iterator = find_gen(this, element, &last_comparison);
    if (last_comparison < 0) {
        return iterator;
    } else if (iterator == start()) {
        return end();
    } else {
        iterator.retreat();
        CZ_DEBUG_ASSERT(*iterator < element);
        return iterator;
    }
}
Iterator<T> Tree<T>::find_greater(const T& element) {
    int64_t last_comparison;
    Iterator<T> iterator = find_gen(this, element, &last_comparison);
    if (last_comparison > 0) {
        return iterator;
    } else if (iterator == end()) {
        return end();
    } else {
        iterator.advance();
        CZ_DEBUG_ASSERT(*iterator > element);
        return iterator;
    }
}
Iterator<T> Tree<T>::find_less_equal(const T& element) {
    int64_t last_comparison;
    Iterator<T> iterator = find_gen(this, element, &last_comparison);
    if (last_comparison <= 0) {
        return iterator;
    } else if (iterator == start()) {
        return end();
    } else {
        iterator.retreat();
        CZ_DEBUG_ASSERT(*iterator < element);
        return iterator;
    }
}
Iterator<T> Tree<T>::find_greater_equal(const T& element) {
    int64_t last_comparison;
    Iterator<T> iterator = find_gen(this, element, &last_comparison);
    if (last_comparison >= 0) {
        return iterator;
    } else if (iterator == end()) {
        return end();
    } else {
        iterator.advance();
        CZ_DEBUG_ASSERT(*iterator > element);
        return iterator;
    }
}

}
}
