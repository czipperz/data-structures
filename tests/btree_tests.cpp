#include <czt/test_base.hpp>

#include "btree.hpp"

using namespace cz;
using namespace ds::btree;

TEST_CASE("BTree insert all in root") {
    BTree<int, 4> btree = {};
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
    BTree<int, 4> btree = {};
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
    BTree<int, 4> btree = {};
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
