#include <czt/test_base.hpp>

#include "page_table.hpp"

using namespace cz;
using namespace ds::pt;

TEST_CASE("Page_Table uint64_t 100 items") {
    Page_Table<uint64_t> page_table = {};
    CZ_DEFER(page_table.drop(cz::heap_allocator()));

    for (uint64_t i = 0; i < 100; ++i) {
        INFO("i = " << i);
        uint64_t id = page_table.add(cz::heap_allocator(), i);
        REQUIRE(i == id);
    }

    for (uint64_t i = 0; i < 100; ++i) {
        INFO("i = " << i);
        uint64_t* num = page_table.lookup(i);
        REQUIRE(num);
        REQUIRE(i == *num);
    }
}

TEST_CASE("Page_Table uint64_t 10000 items") {
    Page_Table<uint64_t> page_table = {};
    CZ_DEFER(page_table.drop(cz::heap_allocator()));

    for (uint64_t i = 0; i < 10000; ++i) {
        INFO("i = " << i);
        uint64_t id = page_table.add(cz::heap_allocator(), i);
        REQUIRE(i == id);
    }

    for (uint64_t i = 0; i < 10000; ++i) {
        INFO("i = " << i);
        uint64_t* num = page_table.lookup(i);
        REQUIRE(num);
        REQUIRE(i == *num);
    }
}
