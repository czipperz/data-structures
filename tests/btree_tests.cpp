#include <czt/test_base.hpp>

#include "btree.hpp"

using namespace cz;
using namespace ds::btree;

TEST_CASE("BTree insert all in root") {
    Tree<int, 4> btree = {};
    CZ_DEFER(btree.drop(cz::heap_allocator()));

    btree.insert(cz::heap_allocator(), 10);
    btree.insert(cz::heap_allocator(), 7);
    btree.insert(cz::heap_allocator(), 13);

    Iterator<int, 4> it = btree.start();
    REQUIRE(it != btree.end());
    CHECK(*it == 7);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 10);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 13);
    ++it;

    CHECK(it == btree.end());
}

TEST_CASE("BTree insert split root") {
    Tree<int, 4> btree = {};
    CZ_DEFER(btree.drop(cz::heap_allocator()));

    btree.insert(cz::heap_allocator(), 10);
    btree.insert(cz::heap_allocator(), 7);
    btree.insert(cz::heap_allocator(), 13);
    btree.insert(cz::heap_allocator(), 61);
    btree.insert(cz::heap_allocator(), -1);

    Iterator<int, 4> it = btree.start();
    REQUIRE(it != btree.end());
    CHECK(*it == -1);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 7);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 10);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 13);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 61);
    ++it;

    CHECK(it == btree.end());
}

TEST_CASE("BTree insert into children of root") {
    Tree<int, 4> btree = {};
    CZ_DEFER(btree.drop(cz::heap_allocator()));

    btree.insert(cz::heap_allocator(), 10);
    btree.insert(cz::heap_allocator(), 7);
    btree.insert(cz::heap_allocator(), 13);
    btree.insert(cz::heap_allocator(), 61);
    btree.insert(cz::heap_allocator(), -1);
    btree.insert(cz::heap_allocator(), 2);
    btree.insert(cz::heap_allocator(), 31);

    Iterator<int, 4> it = btree.start();
    REQUIRE(it != btree.end());
    CHECK(*it == -1);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 2);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 7);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 10);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 13);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 31);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 61);
    ++it;

    CHECK(it == btree.end());
}

TEST_CASE("BTree split children of root height=1") {
    Tree<int, 4> btree = {};
    CZ_DEFER(btree.drop(cz::heap_allocator()));

    btree.insert(cz::heap_allocator(), 10);
    btree.insert(cz::heap_allocator(), 7);
    btree.insert(cz::heap_allocator(), 13);
    btree.insert(cz::heap_allocator(), 61);

    btree.insert(cz::heap_allocator(), -1);

    btree.insert(cz::heap_allocator(), 2);
    btree.insert(cz::heap_allocator(), 31);
    btree.insert(cz::heap_allocator(), 32);
    btree.insert(cz::heap_allocator(), 33);

    btree.insert(cz::heap_allocator(), 34);
    btree.insert(cz::heap_allocator(), 35);

    Iterator<int, 4> it = btree.start();
    REQUIRE(it != btree.end());
    CHECK(*it == -1);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 2);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 7);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 10);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 13);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 31);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 32);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 33);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 34);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 35);
    ++it;

    REQUIRE(it != btree.end());
    CHECK(*it == 61);
    ++it;

    CHECK(it == btree.end());
}

TEST_CASE("BTree insert 100 elements") {
    Tree<int, 4> btree = {};
    CZ_DEFER(btree.drop(cz::heap_allocator()));

    for (int i = 0; i < 100; ++i) {
        INFO("i = " << i);
        btree.insert(cz::heap_allocator(), i);
        CHECK(btree.count == i + 1);

        Iterator<int, 4> it = btree.start();
        for (int j = 0; j <= i; ++j) {
            INFO("j = " << j);
            REQUIRE(it != btree.end());
            CHECK(*it == j);
            ++it;
        }
        REQUIRE(it == btree.end());
    }
}

TEST_CASE("BTree insert 100 elements reverse") {
    Tree<int, 4> btree = {};
    CZ_DEFER(btree.drop(cz::heap_allocator()));

    for (int i = 100; i-- > 0;) {
        INFO("i = " << i);
        btree.insert(cz::heap_allocator(), i);
        CHECK(btree.count == 100 - i);

        Iterator<int, 4> it = btree.start();
        for (int j = i; j < 100; ++j) {
            INFO("j = " << j);
            REQUIRE(it != btree.end());
            CHECK(*it == j);
            ++it;
        }
        REQUIRE(it == btree.end());
    }
}

TEST_CASE("BTree insert 100 elements random") {
    Tree<int, 4> btree = {};
    CZ_DEFER(btree.drop(cz::heap_allocator()));

    {
        int nums[100];
        for (int i = 0; i < 100; ++i) {
            nums[i] = i;
        }

        std::mt19937 g{std::random_device{}()};
        std::shuffle(nums, nums + 100, g);

        for (int i = 0; i < 100; ++i) {
            CHECK(btree.count == i);
            btree.insert(cz::heap_allocator(), nums[i]);
        }
        CHECK(btree.count == 100);
    }

    Iterator<int, 4> it = btree.start();
    for (int j = 0; j < 100; ++j) {
        INFO("j = " << j);
        REQUIRE(it != btree.end());
        CHECK(*it == j);
        ++it;
    }
    REQUIRE(it == btree.end());
}

TEST_CASE("BTree find") {
    Tree<int, 4> btree = {};
    CZ_DEFER(btree.drop(cz::heap_allocator()));

    btree.insert(cz::heap_allocator(), 1);
    btree.insert(cz::heap_allocator(), 3);
    btree.insert(cz::heap_allocator(), 5);
    btree.insert(cz::heap_allocator(), 7);

    Iterator<int, 4> it;

    it = btree.find(0);
    CHECK(it == btree.end());

    it = btree.find(1);
    REQUIRE(it != btree.end());
    CHECK(*it == 1);

    it = btree.find(2);
    CHECK(it == btree.end());

    it = btree.find(3);
    REQUIRE(it != btree.end());
    CHECK(*it == 3);

    it = btree.find(4);
    CHECK(it == btree.end());
}

TEST_CASE("BTree find_lt") {
    Tree<int, 4> btree = {};
    CZ_DEFER(btree.drop(cz::heap_allocator()));

    btree.insert(cz::heap_allocator(), 1);
    btree.insert(cz::heap_allocator(), 3);

    Iterator<int, 4> it;

    it = btree.find_lt(0);
    CHECK(it == btree.end());

    it = btree.find_lt(1);
    CHECK(it == btree.end());

    it = btree.find_lt(2);
    REQUIRE(it != btree.end());
    CHECK(*it == 1);

    it = btree.find_lt(3);
    REQUIRE(it != btree.end());
    CHECK(*it == 1);

    it = btree.find_lt(4);
    REQUIRE(it != btree.end());
    CHECK(*it == 3);
}

TEST_CASE("BTree find_gt") {
    Tree<int, 4> btree = {};
    CZ_DEFER(btree.drop(cz::heap_allocator()));

    btree.insert(cz::heap_allocator(), 1);
    btree.insert(cz::heap_allocator(), 3);

    Iterator<int, 4> it;

    it = btree.find_gt(0);
    REQUIRE(it != btree.end());
    CHECK(*it == 1);

    it = btree.find_gt(1);
    REQUIRE(it != btree.end());
    CHECK(*it == 3);

    it = btree.find_gt(2);
    REQUIRE(it != btree.end());
    CHECK(*it == 3);

    it = btree.find_gt(3);
    CHECK(it == btree.end());

    it = btree.find_gt(4);
    CHECK(it == btree.end());
}

TEST_CASE("BTree find_le") {
    Tree<int, 4> btree = {};
    CZ_DEFER(btree.drop(cz::heap_allocator()));

    btree.insert(cz::heap_allocator(), 1);
    btree.insert(cz::heap_allocator(), 3);

    Iterator<int, 4> it;

    it = btree.find_le(0);
    CHECK(it == btree.end());

    it = btree.find_le(1);
    REQUIRE(it != btree.end());
    CHECK(*it == 1);

    it = btree.find_le(2);
    REQUIRE(it != btree.end());
    CHECK(*it == 1);

    it = btree.find_le(3);
    REQUIRE(it != btree.end());
    CHECK(*it == 3);

    it = btree.find_le(4);
    REQUIRE(it != btree.end());
    CHECK(*it == 3);
}

TEST_CASE("BTree find_ge") {
    Tree<int, 4> btree = {};
    CZ_DEFER(btree.drop(cz::heap_allocator()));

    btree.insert(cz::heap_allocator(), 1);
    btree.insert(cz::heap_allocator(), 3);

    Iterator<int, 4> it;

    it = btree.find_ge(0);
    REQUIRE(it != btree.end());
    CHECK(*it == 1);

    it = btree.find_ge(1);
    REQUIRE(it != btree.end());
    CHECK(*it == 1);

    it = btree.find_ge(2);
    REQUIRE(it != btree.end());
    CHECK(*it == 3);

    it = btree.find_ge(3);
    REQUIRE(it != btree.end());
    CHECK(*it == 3);

    it = btree.find_ge(4);
    CHECK(it == btree.end());
}
