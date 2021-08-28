#ifndef DS_BTREE_BTREE_CPP
#define DS_BTREE_BTREE_CPP

#include "btree.hpp"

namespace ds {
namespace btree {

namespace detail {

template <class T, size_t Maximum_Elements>
void drop_node(cz::Allocator allocator, Node<T, Maximum_Elements>* node) {
    if (!node)
        return;

    for (size_t i = 0; i < node->num_elements; ++i) {
        drop_node(allocator, node->children[i]);
    }
    drop_node(allocator, node->children[node->num_elements]);

    allocator.dealloc(node);
}

template <class T>
bool binary_search(cz::Slice<const T> slice, const T& element, size_t* index) {
    size_t start = 0;
    size_t end = slice.len;
    while (start + 1 < end) {
        size_t mid = (start + end) / 2;
        if (element < slice[mid]) {
            end = mid;
        } else {
            start = mid;
        }
    }

    if (slice[start] < element) {
        ++start;
    }

    *index = start;
    return slice[start] == element;
}

}

template <class T, size_t Maximum_Elements>
void BTree<T, Maximum_Elements>::drop(cz::Allocator allocator) {
    detail::drop_node(allocator, root);
}

template <class T, size_t Maximum_Elements>
bool BTree<T, Maximum_Elements>::insert(cz::Allocator allocator, const T& element) {
    if (!root) {
        Node* node = allocator.alloc<Node>();
        CZ_ASSERT(node);
        node->parent = nullptr;
        node->parent_index = 0;
        node->num_elements = 1;
        node->children[0] = nullptr;
        node->children[1] = nullptr;
        node->elements[0] = element;
        root = node;
        return true;
    }

    Node* node = root;

    size_t index;
    if (detail::binary_search({node->elements, node->num_elements}, element, &index)) {
        return false;
    }

    // Simply insert into this node.
    if (!node->children[index] && node->num_elements < Maximum_Elements) {
        for (size_t i = index; i < node->num_elements + 1; ++i) {
            node->children[i + 1] = node->children[i];
        }
        for (size_t i = index; i < node->num_elements; ++i) {
            node->elements[i + 1] = node->elements[i];
        }

        node->elements[index] = element;
        ++node->num_elements;
        return true;
    }

    CZ_PANIC("unimplemented");
}

namespace detail {
template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> const_start(BTree<T, Maximum_Elements>* btree) {
    if (!btree->root)
        return {};

    Node<T, Maximum_Elements>* node = btree->root;
    return {node, 0};
}
template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> const_end(BTree<T, Maximum_Elements>* btree) {
    if (!btree->root)
        return {};

    Node<T, Maximum_Elements>* node = btree->root;
    return {node, node->num_elements};
}
}

template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> BTree<T, Maximum_Elements>::start() {
    return detail::const_start(this);
}
template <class T, size_t Maximum_Elements>
Iterator<const T, Maximum_Elements> BTree<T, Maximum_Elements>::start() const {
    return detail::const_start(this);
}
template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> BTree<T, Maximum_Elements>::end() {
    return detail::const_end(this);
}
template <class T, size_t Maximum_Elements>
Iterator<const T, Maximum_Elements> BTree<T, Maximum_Elements>::end() const {
    return detail::const_end(this);
}

}
}

#endif
