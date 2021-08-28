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

namespace detail {
template <class T, size_t Maximum_Elements>
void insert_inplace(Node<T, Maximum_Elements>* node, const T& element, size_t index) {
    for (size_t i = node->num_elements + 1; i-- > index;) {
        node->children[i + 1] = node->children[i];
    }
    for (size_t i = node->num_elements; i-- > index;) {
        node->elements[i + 1] = node->elements[i];
    }

    node->elements[index] = element;
    ++node->num_elements;
}

template <class T, size_t Maximum_Elements>
void split_node_insert(Node<T, Maximum_Elements>* left,
                       Node<T, Maximum_Elements>* right,
                       const T& element,
                       size_t element_index,
                       T* middle) {
    CZ_DEBUG_ASSERT(left->num_elements == Maximum_Elements);

    right->children[0] = nullptr;

    const size_t split = (Maximum_Elements + 1) / 2;
    left->num_elements = split;

    if (element_index >= split) {
        size_t i = split;
        for (; i < element_index; ++i) {
            right->elements[i - split] = left->elements[i];
            right->children[i - split + 1] = left->children[i + 1];
        }

        right->elements[i - split] = element;
        right->children[i - split + 1] = nullptr;
        ++i;

        for (; i < Maximum_Elements + 1; ++i) {
            right->elements[i - split] = left->elements[i];
            right->children[i - split + 1] = left->children[i + 1];
        }
    } else {
        for (size_t i = split; i < Maximum_Elements; ++i) {
            right->elements[i - split] = left->elements[i];
            right->children[i - split + 1] = left->children[i + 1];
        }

        insert_inplace(left, element, element_index);
    }

    right->num_elements = Maximum_Elements - split;

    --left->num_elements;
    *middle = left->elements[left->num_elements];
    CZ_DEBUG_ASSERT(left->children[left->num_elements + 1] == nullptr);
}
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

    while (1) {
        size_t index;
        if (detail::binary_search({node->elements, node->num_elements}, element, &index)) {
            return false;
        }

        // If not a leaf node then descend one level.
        if (node->children[index]) {
            node = node->children[index];
            continue;
        }

        // Simply insert into this node.
        if (node->num_elements < Maximum_Elements) {
            detail::insert_inplace(node, element, index);
            return true;
        }

        // Split leaf into 2 leafs.
        T middle;
        Node* right = allocator.alloc<Node>();
        CZ_ASSERT(right);
        right->parent = nullptr;
        right->parent_index = 0;
        right->num_elements = 0;

        detail::split_node_insert(node, right, element, index, &middle);

        // Make new root node.
        Node* new_root = allocator.alloc<Node>();
        CZ_ASSERT(new_root);
        new_root->parent = nullptr;
        new_root->parent_index = 0;
        new_root->num_elements = 1;
        new_root->children[0] = node;
        new_root->children[1] = right;
        new_root->elements[0] = middle;
        root = new_root;

        node->parent = new_root;
        node->parent_index = 0;
        right->parent = new_root;
        right->parent_index = 1;

        return true;
    }
}

namespace detail {
template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> const_start(BTree<T, Maximum_Elements>* btree) {
    if (!btree->root)
        return {};

    Node<T, Maximum_Elements>* node = btree->root;
    while (node->children[0])
        node = node->children[0];

    return {node, 0};
}
template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> const_end(BTree<T, Maximum_Elements>* btree) {
    if (!btree->root)
        return {};

    Node<T, Maximum_Elements>* node = btree->root;
    // while (node->children[node->num_elements])
    //     node = node->children[node->num_elements];

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
