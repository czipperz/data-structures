#include <czt/test_base.hpp>

#include <cz/defer.hpp>
#include <cz/heap.hpp>
#include <random>
#include "splay_tree.hpp"

using namespace cz;
using namespace ds::splay;
using namespace ds::gen;

template <class T>
static void val_tree(Tree<T> tree) {
    val_node<T>(tree.root, nullptr);
}

TEST_CASE("Splay_Tree insertion") {
    Tree<int> tree = {};
    CZ_DEFER(tree.drop(cz::heap_allocator()));
    tree.insert(cz::heap_allocator(), 1);
    val_tree(tree);
    CHECK(tree.root->element == 1);
    tree.insert(cz::heap_allocator(), 2);
    val_tree(tree);
    CHECK(tree.root->element == 2);
    tree.insert(cz::heap_allocator(), 3);
    val_tree(tree);
    CHECK(tree.root->element == 3);

    Iterator<int> it = tree.start();
    REQUIRE(it != tree.end());
    CHECK(*it == 1);
    ++it;
    REQUIRE(it != tree.end());
    CHECK(*it == 2);
    ++it;
    REQUIRE(it != tree.end());
    CHECK(*it == 3);
    ++it;
    CHECK(it == tree.end());
}

TEST_CASE("Splay_Tree start") {
    Tree<int> tree = {};
    CZ_DEFER(tree.drop(cz::heap_allocator()));
    CHECK(tree.start() == tree.end());
    tree.insert(cz::heap_allocator(), 1);
    CHECK(tree.start() != tree.end());
    CHECK(*tree.start() == 1);
    tree.insert(cz::heap_allocator(), 2);
    CHECK(tree.start() != tree.end());
    CHECK(*tree.start() == 1);
    tree.insert(cz::heap_allocator(), 3);
    CHECK(tree.start() != tree.end());
    CHECK(*tree.start() == 1);
    val_tree(tree);
}

TEST_CASE("Splay_Tree find") {
    Tree<int> tree = {};
    CZ_DEFER(tree.drop(cz::heap_allocator()));
    tree.insert(cz::heap_allocator(), 1);
    tree.insert(cz::heap_allocator(), 2);
    tree.insert(cz::heap_allocator(), 3);
    val_tree(tree);

    Iterator<int> it;
    it = tree.find(0);
    val_tree(tree);
    CHECK(tree.root->element == 1);
    CHECK(it == tree.end());
    it = tree.find(1);
    REQUIRE(it != tree.end());
    CHECK(*it == 1);
    CHECK(it.node == tree.root);
    it = tree.find(2);
    REQUIRE(it != tree.end());
    CHECK(*it == 2);
    it = tree.find(3);
    REQUIRE(it != tree.end());
    CHECK(*it == 3);
    it = tree.find(4);
    CHECK(it == tree.end());
}

TEST_CASE("Splay_Tree find_less") {
    Tree<int> tree = {};
    CZ_DEFER(tree.drop(cz::heap_allocator()));
    tree.insert(cz::heap_allocator(), 1);
    tree.insert(cz::heap_allocator(), 2);
    tree.insert(cz::heap_allocator(), 3);
    val_tree(tree);

    Iterator<int> it;
    it = tree.find_less(0);
    CHECK(it == tree.end());
    it = tree.find_less(1);
    CHECK(it == tree.end());
    it = tree.find_less(2);
    REQUIRE(it != tree.end());
    CHECK(*it == 1);
    CHECK(tree.root->element == 2);
    it = tree.find_less(3);
    REQUIRE(it != tree.end());
    CHECK(*it == 2);
    it = tree.find_less(4);
    REQUIRE(it != tree.end());
    CHECK(*it == 3);
}

TEST_CASE("Splay_Tree find_greater") {
    Tree<int> tree = {};
    CZ_DEFER(tree.drop(cz::heap_allocator()));
    tree.insert(cz::heap_allocator(), 1);
    tree.insert(cz::heap_allocator(), 2);
    tree.insert(cz::heap_allocator(), 3);
    val_tree(tree);

    Iterator<int> it;
    it = tree.find_greater(0);
    REQUIRE(it != tree.end());
    CHECK(*it == 1);
    it = tree.find_greater(1);
    CHECK(it != tree.end());
    CHECK(*it == 2);
    it = tree.find_greater(2);
    REQUIRE(it != tree.end());
    CHECK(*it == 3);
    CHECK(tree.root->element == 2);
    it = tree.find_greater(3);
    CHECK(it == tree.end());
    it = tree.find_greater(4);
    CHECK(it == tree.end());
}

TEST_CASE("Splay_Tree find_less_equal") {
    Tree<int> tree = {};
    CZ_DEFER(tree.drop(cz::heap_allocator()));
    tree.insert(cz::heap_allocator(), 1);
    tree.insert(cz::heap_allocator(), 2);
    tree.insert(cz::heap_allocator(), 3);
    val_tree(tree);

    Iterator<int> it;
    it = tree.find_less_equal(0);
    CHECK(it == tree.end());
    it = tree.find_less_equal(1);
    REQUIRE(it != tree.end());
    CHECK(*it == 1);
    it = tree.find_less_equal(2);
    REQUIRE(it != tree.end());
    CHECK(*it == 2);
    CHECK(it.node == tree.root);
    it = tree.find_less_equal(3);
    REQUIRE(it != tree.end());
    CHECK(*it == 3);
    it = tree.find_less_equal(4);
    REQUIRE(it != tree.end());
    CHECK(*it == 3);
}

TEST_CASE("Splay_Tree find_greater_equal") {
    Tree<int> tree = {};
    CZ_DEFER(tree.drop(cz::heap_allocator()));
    tree.insert(cz::heap_allocator(), 1);
    tree.insert(cz::heap_allocator(), 2);
    tree.insert(cz::heap_allocator(), 3);
    val_tree(tree);

    Iterator<int> it;
    it = tree.find_greater_equal(0);
    REQUIRE(it != tree.end());
    CHECK(*it == 1);
    it = tree.find_greater_equal(1);
    CHECK(it != tree.end());
    CHECK(*it == 1);
    it = tree.find_greater_equal(2);
    REQUIRE(it != tree.end());
    CHECK(*it == 2);
    CHECK(it.node == tree.root);
    it = tree.find_greater_equal(3);
    REQUIRE(it != tree.end());
    CHECK(*it == 3);
    it = tree.find_greater_equal(4);
    CHECK(it == tree.end());
}

TEST_CASE("Splay_Tree removal") {
    Tree<int> tree = {};
    CZ_DEFER(tree.drop(cz::heap_allocator()));
    tree.insert(cz::heap_allocator(), 1);
    val_tree(tree);
    tree.insert(cz::heap_allocator(), 2);
    val_tree(tree);
    tree.insert(cz::heap_allocator(), 3);
    val_tree(tree);

    REQUIRE(tree.contains(1));
    REQUIRE(tree.contains(2));
    REQUIRE(tree.contains(3));

    tree.remove(cz::heap_allocator(), tree.find(2));
    val_tree(tree);
    CHECK(tree.count() == 2);
    CHECK(tree.contains(1));
    CHECK_FALSE(tree.contains(2));
    CHECK(tree.contains(3));

    tree.remove(cz::heap_allocator(), tree.find(4));
    val_tree(tree);
    CHECK(tree.count() == 2);
    CHECK(tree.contains(1));
    CHECK_FALSE(tree.contains(2));
    CHECK(tree.contains(3));
}

TEST_CASE("Splay_Tree linear insertion") {
    Tree<int> tree = {};
    CZ_DEFER(tree.drop(cz::heap_allocator()));

    for (int i = 0; i < 4096; ++i) {
        tree.insert(cz::heap_allocator(), i);
    }

    Iterator<int> iter = tree.start();
    for (int i = 0; i < 4096; ++i) {
        REQUIRE(iter != tree.end());
        CHECK(*iter == i);
        ++iter;
    }

    for (int i = 0; i < 4096; ++i) {
        tree.remove(cz::heap_allocator(), tree.start());
    }

    CHECK(tree.start() == tree.end());
}

TEST_CASE("Splay_Tree linear insertion reverse") {
    Tree<int> tree = {};
    CZ_DEFER(tree.drop(cz::heap_allocator()));

    for (int i = 4096; i-- > 0;) {
        tree.insert(cz::heap_allocator(), i);
    }

    Iterator<int> iter = tree.start();
    for (int i = 0; i < 4096; ++i) {
        REQUIRE(iter != tree.end());
        CHECK(*iter == i);
        ++iter;
    }

    for (int i = 0; i < 4096; ++i) {
        tree.remove(cz::heap_allocator(), tree.start());
    }

    CHECK(tree.start() == tree.end());
}

TEST_CASE("Splay_Tree random insertion") {
    Tree<int> tree = {};
    CZ_DEFER(tree.drop(cz::heap_allocator()));

    {
        cz::Vector<int> nums = {};
        CZ_DEFER(nums.drop(cz::heap_allocator()));
        nums.reserve_exact(cz::heap_allocator(), 4096);

        while (nums.len < nums.cap) {
            nums.push((int)nums.len);
        }

        std::mt19937 g{std::random_device{}()};
        std::shuffle(nums.begin(), nums.end(), g);

        for (size_t i = 0; i < 4096; ++i) {
            tree.insert(cz::heap_allocator(), nums[i]);
        }
    }

    Iterator<int> iter = tree.start();
    for (int i = 0; i < 4096; ++i) {
        REQUIRE(iter != tree.end());
        CHECK(*iter == i);
        ++iter;
    }

    for (int i = 0; i < 4096; ++i) {
        tree.remove(cz::heap_allocator(), tree.start());
    }

    CHECK(tree.start() == tree.end());
}
