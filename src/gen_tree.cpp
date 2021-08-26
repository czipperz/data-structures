#include "gen_tree.hpp"

#include <cz/assert.hpp>
#include <cz/util.hpp>

namespace ds {
namespace gen {

void rotate_left(Node_Base* y) {
    //    y
    //  A   x
    //     B C
    //
    //    x
    //  y   C
    // A B

    CZ_DEBUG_ASSERT(y);

    Node_Base* x = y->right;
    CZ_DEBUG_ASSERT(x);

    y->right = x->left;
    x->left = y;

    if (y->right)
        y->right->parent = y;

    x->parent = y->parent;
    y->parent = x;

    if (x->parent) {
        if (x->parent->left == y) {
            x->parent->left = x;
        } else {
            x->parent->right = x;
        }
    }
}

void rotate_right(Node_Base* y) {
    //    y
    //  x   C
    // A B
    //
    //    x
    // A    y
    //     B C

    CZ_DEBUG_ASSERT(y);

    Node_Base* x = y->left;
    CZ_DEBUG_ASSERT(x);

    y->left = x->right;
    x->right = y;

    if (y->left)
        y->left->parent = y;

    x->parent = y->parent;
    y->parent = x;

    if (x->parent) {
        if (x->parent->left == y) {
            x->parent->left = x;
        } else {
            x->parent->right = x;
        }
    }
}

Node_Base* leftmost(Node_Base* root) {
    Node_Base* parent = nullptr;
    Node_Base* node = root;
    while (node) {
        parent = node;
        node = node->left;
    }
    return parent;
}

Node_Base* rightmost(Node_Base* root) {
    Node_Base* parent = nullptr;
    Node_Base* node = root;
    while (node) {
        parent = node;
        node = node->right;
    }
    return parent;
}

Node_Base* node_after(Node_Base* node) {
    if (!node)
        return nullptr;

    if (node->right)
        return leftmost(node->right);

    while (node->parent) {
        if (node->parent->left == node) {
            return node->parent;
        }
        node = node->parent;
    }

    return nullptr;
}

Node_Base* node_before(Node_Base* node) {
    if (!node)
        return nullptr;

    if (node->left)
        return rightmost(node->left);

    while (node->parent) {
        if (node->parent->right == node) {
            return node->parent;
        }
        node = node->parent;
    }

    return nullptr;
}

void remove_leaf(Node_Base* node) {
    CZ_DEBUG_ASSERT(node);

    if (node->parent) {
        if (node->parent->left == node)
            node->parent->left = nullptr;
        else
            node->parent->right = nullptr;
    }
}

Node_Base* remove(Node_Base* node) {
    CZ_DEBUG_ASSERT(node);

    // Find a child element to replace this node.
    Node_Base* replacement = rightmost(node->left);
    if (!replacement) {
        replacement = leftmost(node->right);
    }

    // No children so just unhook this element.
    if (!replacement) {
        remove_leaf(node);
        return replacement;
    }

    remove_leaf(replacement);

    replacement->parent = node->parent;
    if (node->parent) {
        if (node->parent->left == node) {
            node->parent->left = replacement;
        } else {
            node->parent->right = replacement;
        }
    }

    replacement->left = node->left;
    if (replacement->left)
        replacement->left->parent = replacement;

    replacement->right = node->right;
    if (replacement->right)
        replacement->right->parent = replacement;

    return replacement;
}

size_t count(Node_Base* node) {
    size_t total = 0;
    while (node) {
        total += 1;
        total += count(node->left);
        node = node->right;
    }
    return total;
}

}
}
