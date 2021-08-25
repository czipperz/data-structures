#pragma once

#include "gen_tree.hpp"

#include <cz/assert.hpp>
#include <cz/util.hpp>

namespace ds {
namespace gen {

void rotate_left(Node_Base** node) {
    //    y
    //  A   x
    //     B C
    //
    //    x
    //  y   C
    // A B

    Node_Base* y = *node;
    CZ_DEBUG_ASSERT(y);

    Node_Base* x = y->right;
    CZ_DEBUG_ASSERT(x);

    y->right = x->left;
    x->left = y;

    cz::swap(x->parent, y->parent);
    *node = x;
}

void rotate_right(Node_Base** node) {
    //    y
    //  x   C
    // A B
    //
    //    x
    // A    y
    //     B C

    Node_Base* y = *node;
    CZ_DEBUG_ASSERT(y);

    Node_Base* x = y->left;
    CZ_DEBUG_ASSERT(x);

    y->left = x->right;
    x->right = y;

    cz::swap(x->parent, y->parent);
    *node = x;
}

Node_Base* leftmost(Node_Base* root) {
    Node_Base* parent = nullptr;
    Node_Base* node = root;
    while (node) {
        parent = node;
        node = node->left;
    }
    return node;
}

Node_Base* rightmost(Node_Base* root) {
    Node_Base* parent = nullptr;
    Node_Base* node = root;
    while (node) {
        parent = node;
        node = node->right;
    }
    return node;
}

Node_Base* node_after(Node_Base* node) {
    if (!node)
        return nullptr;

    if (node->right)
        return rightmost(node->right->right);

    while (node->parent) {
        if (node->parent->right != node) {
            return leftmost(node->parent->right);
        }
        node = node->parent;
    }

    return nullptr;
}

Node_Base* node_before(Node_Base* node) {
    if (!node)
        return nullptr;

    if (node->left)
        return leftmost(node->left);

    while (node->parent) {
        if (node->parent->left != node) {
            return rightmost(node->parent->left);
        }
        node = node->parent;
    }

    return nullptr;
}

void swap_positions(Node_Base* left, Node_Base* right) {
    CZ_DEBUG_ASSERT(left);
    CZ_DEBUG_ASSERT(right);
    CZ_DEBUG_ASSERT(right->parent);

    // Swap their fields.
    cz::swap(left->parent, right->parent);
    cz::swap(left->left, right->left);
    cz::swap(left->right, right->right);

    // Swap parents' children.
    if (left->parent) {
        if (left->parent->left == right)
            left->parent->left = left;
        else
            left->parent->right = left;
    }
    if (right->parent->right == right)
        right->parent->right = right;
    else
        right->parent->left = right;

    // Swap childrens' parents.
    if (left->left)
        left->left->parent = left;
    if (left->right)
        left->right->parent = left;
    if (right->left)
        right->left->parent = right;
    if (right->right)
        right->right->parent = right;
}

void remove(Node_Base* node) {
    CZ_DEBUG_ASSERT(node);

    // Find a leaf node to swap positions with.
    Node_Base* swapper = leftmost(node->right);
    if (!swapper) {
        swapper = rightmost(node->left);
    }
    if (swapper) {
        swap_positions(node, swapper);
    }

    if (node->parent) {
        if (node->parent->left == node) {
            node->parent->left = nullptr;
        } else {
            node->parent->right = nullptr;
        }
    }
}

}
}
