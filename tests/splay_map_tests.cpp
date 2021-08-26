#include <czt/test_base.hpp>

#include <cz/defer.hpp>
#include <cz/heap.hpp>
#include <cz/str.hpp>
#include <random>
#include "splay_map.hpp"

using namespace cz;
using namespace ds::splay;
using namespace ds::gen;

template <class K, class V>
static void val_map(Map<K, V> map) {
    val_node<Pair<K, V> >(map.tree.root, nullptr);
}

TEST_CASE("Splay_Map insertion") {
    Map<int, const char*> map = {};
    CZ_DEFER(map.drop(cz::heap_allocator()));
    map.insert(cz::heap_allocator(), 1, "hello");
    map.insert(cz::heap_allocator(), 2, "world");

    Map_Iterator<int, const char*> it;
    it = map.find(1);
    CHECK(it != map.end());
    CHECK(it->key == 1);
    CHECK(cz::Str(it->value) == "hello");
}
