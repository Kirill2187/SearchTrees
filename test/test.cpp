#include "gtest/gtest.h"
#include "trees.h"
#include "treap.h"
#include <map>

TEST(IncludeTest, IncludeTest) {}

template <typename Tree>
class SearchTreeTest: public ::testing::Test {};

template <typename Tree>
class ImplicitTreeTest: public ::testing::Test {};

typedef ::testing::Types< treap<treap_node<int, int>> > SearchTreeTypes;
typedef ::testing::Types< treap<implicit_treap_node<int>> > ImplicitSearchTreeTypes;

TYPED_TEST_SUITE(SearchTreeTest, SearchTreeTypes);
TYPED_TEST_SUITE(ImplicitTreeTest, ImplicitSearchTreeTypes);

TYPED_TEST(SearchTreeTest, SimpleTest) {
    TypeParam tree;

    std::vector<int> keys = {6, -1, 0, 1, 3, 10, 15};
    for (int key : keys) {
        tree.insert(key, 0);
    }
    ASSERT_EQ(tree.get_min()->key, -1);

    tree.remove(-1);
    ASSERT_EQ(tree.get_min()->key, 0);

    ASSERT_EQ(tree.get_kth(3)->key, 6);
    tree.remove_kth(3);

    ASSERT_EQ(tree.get_kth(3)->key, 10);
}

TYPED_TEST(SearchTreeTest, BigTest) {
    TypeParam tree;

    std::map<int, int> map;
    srand(0);

    for (int i = 0; i < 100000; ++i) {
        int key = rand();
        if (map.count(key)) {
            ASSERT_TRUE(tree.exists(key));
            tree.remove(key);
            map.erase(key);
        } else {
            tree.insert(key, i);
            map[key] = i;
        }
    }

    for (auto [key, value] : map) {
        ASSERT_TRUE(tree.exists(key));
        ASSERT_EQ(tree.search(key)->value, value);
    }

    auto v = tree.get_traversal();
    for (int i = 0; i < v.size() - 1; ++i) {
        ASSERT_TRUE(v[i]->key <= v[i + 1]->key);
        ASSERT_TRUE(map.count(v[i]->key));
    }
}

TYPED_TEST(ImplicitTreeTest, SimpleTest) {
    TypeParam tree;

    std::vector<int> values = {6, -1, 0, -1, 0, 10, 15};
    for (int val : values) {
        tree.insert_kth(tree.size(), val);
    }
    ASSERT_EQ(tree.get_kth(0)->value, 6);
    ASSERT_EQ(tree.get_kth(1)->value, -1);

    tree.remove_kth(0);
    ASSERT_EQ(tree.get_kth(0)->value, -1);

    ASSERT_EQ(tree.get_kth(3)->value, 0);

    tree.insert_kth(3, 100);
    ASSERT_EQ(tree.get_kth(3)->value, 100);
    ASSERT_EQ(tree.get_kth(4)->value, 0);
}

TYPED_TEST(ImplicitTreeTest, CutTest) {
    TypeParam tree;

    std::vector<int> values = {6, -1, 0, -1, 0, 10, 15};
    for (int val : values) {
        tree.insert_kth(tree.size(), val);
    }

    TypeParam t = {tree.cut_subsegment(2, 5)};
    ASSERT_EQ(tree.get_kth(2)->value, 15);
    ASSERT_EQ(t.get_kth(0)->value, 0);

    tree.insert_subsegment(2, t.root);
    ASSERT_EQ(tree.get_kth(2)->value, 0);
    ASSERT_EQ(tree.get_kth(3)->value, -1);
}
