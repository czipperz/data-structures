#include "splay.hpp"

#include <Tracy.hpp>

namespace ds {
namespace splay {

void splay(gen::Node_Base* elem) {
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
}

}
}
