#ifndef DS_SPLAY_TREE_CPP
#define DS_SPLAY_TREE_CPP

#include "splay_tree.hpp"
#include "splay.hpp"

#include <Tracy.hpp>
#include <cz/compare.hpp>

namespace ds {
namespace splay {

template <class T>
void Tree<T>::drop(cz::Allocator allocator) {
    gen::recursive_dealloc(allocator, root);
}

template <class T, class Comparator>
static Iterator<T> find_gen(Tree<T>* tree, int64_t* last_comparison, Comparator&& comparator) {
    Node<T>* parent = gen::find_comparator(tree->root, last_comparison, comparator);

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
    Node<T>* guess = gen::find(root, &last_comparison, element);

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
        using cz::compare;
        CZ_DEBUG_ASSERT(compare(node->element, ((Node<T>*)node->left)->element) > 0);
    } else {
        node->left = nullptr;
        node->right = guess;
        using cz::compare;
        CZ_DEBUG_ASSERT(compare(node->element, ((Node<T>*)node->right)->element) < 0);
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
    return detail::find_equal_comparator(this, gen::element_comparator(element));
}
template <class T>
Iterator<T> Tree<T>::find_less(const T& element) {
    return detail::find_less_comparator(this, gen::element_comparator(element));
}
template <class T>
Iterator<T> Tree<T>::find_greater(const T& element) {
    return detail::find_greater_comparator(this, gen::element_comparator(element));
}
template <class T>
Iterator<T> Tree<T>::find_less_equal(const T& element) {
    return detail::find_less_equal_comparator(this, gen::element_comparator(element));
}
template <class T>
Iterator<T> Tree<T>::find_greater_equal(const T& element) {
    return detail::find_greater_equal_comparator(this, gen::element_comparator(element));
}

namespace detail {

template <class T, class Comparator>
Iterator<T> find_equal_comparator(Tree<T>* tree, Comparator&& comparator) {
    int64_t last_comparison;
    Iterator<T> iterator = find_gen(tree, &last_comparison, comparator);
    if (last_comparison == 0) {
        return iterator;
    } else {
        return tree->end();
    }
}
template <class T, class Comparator>
Iterator<T> find_less_comparator(Tree<T>* tree, Comparator&& comparator) {
    int64_t last_comparison;
    Iterator<T> iterator = find_gen(tree, &last_comparison, comparator);
    if (last_comparison > 0) {
        return iterator;
        // Not necessary because --tree->start() == tree->end().
        // } else if (iterator == tree->start()) {
        //     return tree->end();
    } else {
        --iterator;
        CZ_DEBUG_ASSERT(iterator == tree->end() || comparator(*iterator) > 0);
        return iterator;
    }
}
template <class T, class Comparator>
Iterator<T> find_greater_comparator(Tree<T>* tree, Comparator&& comparator) {
    int64_t last_comparison;
    Iterator<T> iterator = find_gen(tree, &last_comparison, comparator);
    if (last_comparison < 0) {
        return iterator;
    } else if (iterator == tree->end()) {
        return tree->end();
    } else {
        ++iterator;
        CZ_DEBUG_ASSERT(iterator == tree->end() || comparator(*iterator) < 0);
        return iterator;
    }
}
template <class T, class Comparator>
Iterator<T> find_less_equal_comparator(Tree<T>* tree, Comparator&& comparator) {
    int64_t last_comparison;
    Iterator<T> iterator = find_gen(tree, &last_comparison, comparator);
    if (last_comparison >= 0) {
        return iterator;
        // Not necessary because --tree->start() == tree->end().
        // } else if (iterator == start()) {
        //     return tree->end();
    } else {
        --iterator;
        CZ_DEBUG_ASSERT(iterator == tree->end() || comparator(*iterator) > 0);
        return iterator;
    }
}
template <class T, class Comparator>
Iterator<T> find_greater_equal_comparator(Tree<T>* tree, Comparator&& comparator) {
    int64_t last_comparison;
    Iterator<T> iterator = find_gen(tree, &last_comparison, comparator);
    if (last_comparison <= 0) {
        return iterator;
    } else if (iterator == tree->end()) {
        return tree->end();
    } else {
        ++iterator;
        CZ_DEBUG_ASSERT(iterator == tree->end() || comparator(*iterator) < 0);
        return iterator;
    }
}

}

}
}

#endif
