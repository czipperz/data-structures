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
    Node<T>* parent = gen::find(tree->root, element, last_comparison);

    splay(parent);
    tree->root = parent;

    return Iterator<T>{parent};
}

template <class T>
bool Tree<T>::insert(cz::Allocator allocator, const T& element) {
    ZoneScoped;

    // Special case empty tree.
    if (!root) {
        Node<T>* node = allocator.alloc<Node<T> >();
        CZ_ASSERT(node);
        node->parent = nullptr;
        node->left = nullptr;
        node->right = nullptr;
        node->element = element;
        root = node;
        return true;
    }

    int64_t last_comparison;
    Node<T>* guess = gen::find(root, element, &last_comparison);

    // Already present.
    if (last_comparison == 0) {
        return false;
    }

    // Insert node.
    // Create parent node.
    Node<T>* node = allocator.alloc<Node<T> >();
    CZ_ASSERT(node);
    CZ_DEBUG_ASSERT(guess);

    node->element = element;

    // Hook parent.
    node->parent = guess->parent;
    if (node->parent) {
        if (node->parent->left == guess) {
            node->parent->left = node;
        } else {
            node->parent->right = node;
        }
    } else {
        // Done below.
        // root = node;
    }

    // Child's side depends on its relationship to element.
    if (last_comparison > 0) {
        node->left = guess;
        node->right = nullptr;
        CZ_DEBUG_ASSERT(node->element > ((Node<T>*)node->left)->element);
    } else {
        node->left = nullptr;
        node->right = guess;
        CZ_DEBUG_ASSERT(node->element < ((Node<T>*)node->right)->element);
    }
    guess->parent = node;

    splay(node);
    root = node;

    return true;
}

template <class T>
void Tree<T>::remove(cz::Allocator allocator, Iterator<const T> iterator) {
    ZoneScoped;
    if (iterator == end())
        return;

    Node<T>* new_node = (Node<T>*)gen::remove(iterator.node);
    allocator.dealloc(iterator.node);

    splay(new_node);
    root = new_node;
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
    if (last_comparison > 0) {
        return iterator;
    } else if (iterator == start()) {
        return end();
    } else {
        --iterator;
        CZ_DEBUG_ASSERT(iterator == end() || *iterator < element);
        return iterator;
    }
}
template <class T>
Iterator<T> Tree<T>::find_greater(const T& element) {
    int64_t last_comparison;
    Iterator<T> iterator = find_gen(this, element, &last_comparison);
    if (last_comparison < 0) {
        return iterator;
    } else if (iterator == end()) {
        return end();
    } else {
        ++iterator;
        CZ_DEBUG_ASSERT(iterator == end() || *iterator > element);
        return iterator;
    }
}
template <class T>
Iterator<T> Tree<T>::find_less_equal(const T& element) {
    int64_t last_comparison;
    Iterator<T> iterator = find_gen(this, element, &last_comparison);
    if (last_comparison >= 0) {
        return iterator;
    } else if (iterator == start()) {
        return end();
    } else {
        --iterator;
        CZ_DEBUG_ASSERT(iterator == end() || *iterator < element);
        return iterator;
    }
}
template <class T>
Iterator<T> Tree<T>::find_greater_equal(const T& element) {
    int64_t last_comparison;
    Iterator<T> iterator = find_gen(this, element, &last_comparison);
    if (last_comparison <= 0) {
        return iterator;
    } else if (iterator == end()) {
        return end();
    } else {
        ++iterator;
        CZ_DEBUG_ASSERT(iterator == end() || *iterator > element);
        return iterator;
    }
}

}
}
