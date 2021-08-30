#ifndef DS_BTREE_BTREE_CPP
#define DS_BTREE_BTREE_CPP

#include "btree.hpp"

#include <cz/compare.hpp>

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

template <class T, class Comparator>
bool binary_search(cz::Slice<const T> slice,
                   const T& element,
                   size_t* index,
                   Comparator&& comparator) {
    size_t start = 0;
    size_t end = slice.len;
    while (start + 1 < end) {
        size_t mid = (start + end) / 2;
        if (comparator(element, slice[mid]) < 0) {
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
void Tree_Base<T, Maximum_Elements>::drop(cz::Allocator allocator) {
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

namespace detail {
template <class T, size_t Maximum_Elements, class Comparator>
bool insert(Tree_Base<T, Maximum_Elements>* tree,
            cz::Allocator allocator,
            const T& element,
            Comparator&& comparator) {
    using Node = Node<T, Maximum_Elements>;

    if (!tree->root) {
        Node* node = allocator.alloc<Node>();
        CZ_ASSERT(node);
        node->parent = nullptr;
        node->parent_index = 0;
        node->num_elements = 1;
        node->children[0] = nullptr;
        node->children[1] = nullptr;
        node->elements[0] = element;
        tree->root = node;
        return true;
    }

    Node* node = tree->root;

    // Find a leaf node to insert into.
    size_t index;
    while (1) {
        if (detail::binary_search({node->elements, node->num_elements}, element, &index,
                                  comparator)) {
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
            tree->root = new_root;

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
}

template <class T, size_t Maximum_Elements>
bool Tree<T, Maximum_Elements>::insert(cz::Allocator allocator, const T& element) {
    return detail::insert(this, allocator, element, cz::compare<T>);
}

template <class T, size_t Maximum_Elements>
template <class Comparator>
bool Tree_Comparator<T, Maximum_Elements>::insert(cz::Allocator allocator,
                                                  const T& element,
                                                  Comparator&& comparator) {
    return detail::insert(this, allocator, element, comparator);
}

namespace detail {
template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> const_start(const Tree_Base<T, Maximum_Elements>* btree) {
    if (!btree->root)
        return {};

    Node<T, Maximum_Elements>* node = btree->root;
    while (node->children[0])
        node = node->children[0];

    return {node, 0};
}
template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> const_end(const Tree_Base<T, Maximum_Elements>* btree) {
    if (!btree->root)
        return {};

    Node<T, Maximum_Elements>* node = btree->root;
    return {node, node->num_elements};
}
}

template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> Tree_Base<T, Maximum_Elements>::start() {
    return detail::const_start(this);
}
template <class T, size_t Maximum_Elements>
Iterator<const T, Maximum_Elements> Tree_Base<T, Maximum_Elements>::start() const {
    return detail::const_start(this);
}
template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> Tree_Base<T, Maximum_Elements>::end() {
    return detail::const_end(this);
}
template <class T, size_t Maximum_Elements>
Iterator<const T, Maximum_Elements> Tree_Base<T, Maximum_Elements>::end() const {
    return detail::const_end(this);
}

namespace detail {
template <class T, size_t Maximum_Elements, class Comparator>
Iterator<T, Maximum_Elements> gen_find(Tree_Base<T, Maximum_Elements>* tree,
                                       const T& element,
                                       Comparator&& comparator,
                                       int64_t* last_comparison) {
    Node<T, Maximum_Elements>* node = tree->root;

    // Find a leaf node to insert into.
    size_t index;
    while (1) {
        if (detail::binary_search({node->elements, node->num_elements}, element, &index,
                                  comparator)) {
            *last_comparison = 0;
            return {node, index};
        }

        if (!node->children[index]) {
            break;
        }

        node = node->children[index];
    }

    if (index == node->num_elements) {
        --index;
        *last_comparison = 1;
        return {node, index};
    }

    *last_comparison = -1;
    return {node, index};
}

template <class T, size_t Maximum_Elements, class Comparator>
Iterator<T, Maximum_Elements> find_eq(Tree_Base<T, Maximum_Elements>* tree,
                                      const T& element,
                                      Comparator&& comparator) {
    int64_t last_comparison;
    Iterator<T, Maximum_Elements> iterator = gen_find(tree, element, comparator, &last_comparison);
    if (last_comparison == 0) {
        return iterator;
    } else {
        return tree->end();
    }
}
template <class T, size_t Maximum_Elements, class Comparator>
Iterator<T, Maximum_Elements> find_lt(Tree_Base<T, Maximum_Elements>* tree,
                                      const T& element,
                                      Comparator&& comparator) {
    int64_t last_comparison;
    Iterator<T, Maximum_Elements> iterator = gen_find(tree, element, comparator, &last_comparison);
    if (last_comparison > 0) {
        return iterator;
    } else if (iterator == tree->start()) {
        return tree->end();
    } else {
        --iterator;
        return iterator;
    }
}
template <class T, size_t Maximum_Elements, class Comparator>
Iterator<T, Maximum_Elements> find_gt(Tree_Base<T, Maximum_Elements>* tree,
                                      const T& element,
                                      Comparator&& comparator) {
    int64_t last_comparison;
    Iterator<T, Maximum_Elements> iterator = gen_find(tree, element, comparator, &last_comparison);
    if (last_comparison < 0) {
        return iterator;
    } else if (iterator == tree->end()) {
        return tree->end();
    } else {
        ++iterator;
        return iterator;
    }
}
template <class T, size_t Maximum_Elements, class Comparator>
Iterator<T, Maximum_Elements> find_le(Tree_Base<T, Maximum_Elements>* tree,
                                      const T& element,
                                      Comparator&& comparator) {
    int64_t last_comparison;
    Iterator<T, Maximum_Elements> iterator = gen_find(tree, element, comparator, &last_comparison);
    if (last_comparison >= 0) {
        return iterator;
    } else if (iterator == tree->start()) {
        return tree->end();
    } else {
        --iterator;
        return iterator;
    }
}
template <class T, size_t Maximum_Elements, class Comparator>
Iterator<T, Maximum_Elements> find_ge(Tree_Base<T, Maximum_Elements>* tree,
                                      const T& element,
                                      Comparator&& comparator) {
    int64_t last_comparison;
    Iterator<T, Maximum_Elements> iterator = gen_find(tree, element, comparator, &last_comparison);
    if (last_comparison <= 0) {
        return iterator;
    } else if (iterator == tree->end()) {
        return tree->end();
    } else {
        ++iterator;
        return iterator;
    }
}
}

template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> Tree<T, Maximum_Elements>::find_eq(const T& element) {
    return detail::find_eq(this, element, cz::compare<T>);
}
template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> Tree<T, Maximum_Elements>::find_lt(const T& element) {
    return detail::find_lt(this, element, cz::compare<T>);
}
template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> Tree<T, Maximum_Elements>::find_gt(const T& element) {
    return detail::find_gt(this, element, cz::compare<T>);
}
template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> Tree<T, Maximum_Elements>::find_le(const T& element) {
    return detail::find_le(this, element, cz::compare<T>);
}
template <class T, size_t Maximum_Elements>
Iterator<T, Maximum_Elements> Tree<T, Maximum_Elements>::find_ge(const T& element) {
    return detail::find_ge(this, element, cz::compare<T>);
}

template <class T, size_t Maximum_Elements>
template <class Comparator>
Iterator<T, Maximum_Elements> Tree_Comparator<T, Maximum_Elements>::find_eq(
    const T& element,
    Comparator&& comparator) {
    return detail::find_eq(this, element, comparator);
}
template <class T, size_t Maximum_Elements>
template <class Comparator>
Iterator<T, Maximum_Elements> Tree_Comparator<T, Maximum_Elements>::find_lt(
    const T& element,
    Comparator&& comparator) {
    return detail::find_lt(this, element, comparator);
}
template <class T, size_t Maximum_Elements>
template <class Comparator>
Iterator<T, Maximum_Elements> Tree_Comparator<T, Maximum_Elements>::find_gt(
    const T& element,
    Comparator&& comparator) {
    return detail::find_gt(this, element, comparator);
}
template <class T, size_t Maximum_Elements>
template <class Comparator>
Iterator<T, Maximum_Elements> Tree_Comparator<T, Maximum_Elements>::find_le(
    const T& element,
    Comparator&& comparator) {
    return detail::find_le(this, element, comparator);
}
template <class T, size_t Maximum_Elements>
template <class Comparator>
Iterator<T, Maximum_Elements> Tree_Comparator<T, Maximum_Elements>::find_ge(
    const T& element,
    Comparator&& comparator) {
    return detail::find_ge(this, element, comparator);
}

}
}

#endif
