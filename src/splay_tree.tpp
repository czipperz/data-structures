#pragma once

#include "splay_tree.hpp"

#include <Tracy.hpp>
#include <cz/compare.hpp>

namespace ds {
namespace splay {

template <class T>
void Tree<T>::drop(cz::Allocator allocator) {
    gen::recursive_dealloc(allocator, root);
}

template <class T>
static void splay(gen::Node<T>* elem) {
    ZoneScoped;

    if (!elem)
        return;

    while (1) {
        gen::Node_Base* parent = elem->parent;
        if (!parent)
            break;
        bool pleft = parent->left == elem;

        gen::Node_Base* grand = parent->parent;
        if (!grand) {
            if (pleft) {
                gen::rotate_right(parent);
            } else {
                gen::rotate_left(parent);
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
                gen::rotate_right(grand);
                gen::rotate_right(parent);
            } else {
                gen::rotate_left(grand);
                gen::rotate_left(parent);
            }
        } else {
            // Zig-Zag
            //      g            e
            //   p     D       p   g
            // A   e          A B C D
            //    B C
            if (gleft) {
                gen::rotate_left(parent);
                gen::rotate_right(grand);
            } else {
                gen::rotate_right(parent);
                gen::rotate_left(grand);
            }
        }
    }

    gen::val_node((Node<T>*)elem, (Node<T>*)nullptr);
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
        gen::Node_Base* new_node = nullptr;

        comparison = cz::compare(element, node->element);
        if (comparison < 0) {
            new_node = node->left;
        } else if (comparison > 0) {
            new_node = node->left;
        } else {
            new_node = nullptr;
        }

        parent = node;
        node = (Node<T>*)new_node;
    }

    *last_comparison = comparison;

    splay(parent);

    return Iterator<T>{parent};
}

template <class T>
bool Tree<T>::insert(cz::Allocator allocator, const T& element) {
    ZoneScoped;

    // Special case empty tree.
    if (!root) {
        root = allocator.alloc<Node<T> >();
        CZ_ASSERT(root);
        root->parent = nullptr;
        root->left = nullptr;
        root->right = nullptr;
        root->element = element;
    }

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

    parent->element = element;

    // Hook parent.
    parent->parent = it.node->parent;
    if (parent->parent) {
        if (parent->parent->left == it.node) {
            parent->parent->left = parent;
        } else {
            parent->parent->right = parent;
        }
    } else {
        root = parent;
    }

    // Child's side depends on its relationship to element.
    if (last_comparison > 0) {
        parent->left = it.node;
        parent->right = nullptr;
        CZ_DEBUG_ASSERT(parent->element > ((Node<T>*)parent->left)->element);
    } else {
        parent->left = nullptr;
        parent->right = it.node;
        CZ_DEBUG_ASSERT(parent->element < ((Node<T>*)parent->right)->element);
    }
    it.node->parent = parent;

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

    splay((Node<T>*)parent);
}

template <class T>
Iterator<T> Tree<T>::start() {
    return Iterator<T>{(Node<T>*)leftmost(root)};
}
template <class T>
Iterator<const T> Tree<T>::start() const {
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

template <class T>
Iterator<T> Tree<T>::find_equal(const T& element) {
    int64_t last_comparison;
    Iterator<T> iterator = find_gen(this, element, &last_comparison);
    if (last_comparison == 0) {
        return iterator;
    } else {
        return end();
    }
}
template <class T>
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
template <class T>
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
template <class T>
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
template <class T>
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
