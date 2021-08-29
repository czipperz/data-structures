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
void insert_inplace(Node<T, Maximum_Elements>* node,
                    const T& element,
                    Node<T, Maximum_Elements>* child,
                    size_t index) {
    for (size_t i = node->num_elements + 1; i-- > index;) {
        node->children[i + 1] = node->children[i];
    }
    for (size_t i = index + 1; i < node->num_elements + 1; ++i) {
        if (!node->children[i + 1])
            break;
        ++node->children[i + 1]->parent_index;
    }
    for (size_t i = node->num_elements; i-- > index;) {
        node->elements[i + 1] = node->elements[i];
    }

    node->elements[index] = element;
    node->children[index + 1] = child;
    ++node->num_elements;
}

template <class T, size_t Maximum_Elements>
void split_node_insert(Node<T, Maximum_Elements>* left,
                       Node<T, Maximum_Elements>* right,
                       const T& element,
                       Node<T, Maximum_Elements>* element_child,
                       size_t element_index,
                       const T** middle) {
    CZ_DEBUG_ASSERT(left->num_elements == Maximum_Elements);

    size_t split = Maximum_Elements / 2 + 1;

    if (element_index >= split) {
        size_t i = split;
        for (; i < element_index; ++i) {
            right->elements[i - split] = left->elements[i];
            right->children[i - split + 1] = left->children[i + 1];
        }

        right->elements[i - split] = element;
        right->children[i - split + 1] = element_child;

        for (; i < Maximum_Elements; ++i) {
            right->elements[i - split + 1] = left->elements[i];
            right->children[i - split + 2] = left->children[i + 1];
        }

        left->num_elements = split;
        right->num_elements = Maximum_Elements + 1 - split;
    } else {
        --split;

        for (size_t i = split; i < Maximum_Elements; ++i) {
            right->elements[i - split] = left->elements[i];
            right->children[i - split + 1] = left->children[i + 1];
        }

        left->num_elements = split;
        insert_inplace(left, element, element_child, element_index);
        right->num_elements = Maximum_Elements - split;
    }

    --left->num_elements;
    *middle = &left->elements[left->num_elements];
    right->children[0] = left->children[left->num_elements + 1];
    CZ_DEBUG_ASSERT(left->num_elements + right->num_elements == Maximum_Elements);

    for (size_t i = 0; i < right->num_elements + 1; ++i) {
        Node<T, Maximum_Elements>* child = right->children[i];
        if (!child)
            break;
        child->parent = right;
        child->parent_index = i;
    }
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

    // Find a leaf node to insert into.
    size_t index;
    while (1) {
        if (detail::binary_search({node->elements, node->num_elements}, element, &index)) {
            return false;
        }

        if (!node->children[index]) {
            break;
        }

        node = node->children[index];
    }

    // Split then insert, stepping up one level each time.
    const T* pelement = &element;
    Node* child = nullptr;
    while (1) {
        // Simply insert into this node.
        if (node->num_elements < Maximum_Elements) {
            detail::insert_inplace(node, *pelement, child, index);
            return true;
        }

        // Split node into two.  `node` becomes the left side.
        Node* right = allocator.alloc<Node>();
        CZ_ASSERT(right);
        right->parent = nullptr;
        right->parent_index = 0;
        right->num_elements = 0;

        detail::split_node_insert(node, right, *pelement, child, index, &pelement);

        child = right;

        if (!node->parent) {
            // Make new root node.
            Node* new_root = allocator.alloc<Node>();
            CZ_ASSERT(new_root);
            new_root->parent = nullptr;
            new_root->parent_index = 0;
            new_root->num_elements = 1;
            new_root->children[0] = node;
            new_root->children[1] = right;
            new_root->elements[0] = *pelement;
            root = new_root;

            node->parent = new_root;
            node->parent_index = 0;
            right->parent = new_root;
            right->parent_index = 1;
            return true;
        }

        // Recurse into parent.
        right->parent = node->parent;
        right->parent_index = node->parent_index + 1;
        index = node->parent_index;
        node = right->parent;
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
