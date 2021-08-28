#include <czt/test_base.hpp>

#include "btree.hpp"

using namespace cz;
using namespace ds::btree;

TEST_CASE("BTree insertion") {
    BTree<int> btree = {};
    CZ_DEFER(btree.drop(cz::heap_allocator()));

    btree.insert(cz::heap_allocator(), 10);
    btree.insert(cz::heap_allocator(), 7);
    btree.insert(cz::heap_allocator(), 13);

    Iterator<int> it = btree.start();
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
