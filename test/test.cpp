#include "gtest/gtest.h"
#include "trees.h"
#include "treap.h"
#include "rb_tree.h"
#include "avl.h"
#include <map>

TEST(IncludeTest, IncludeTest) {}

template <typename Tree>
class SearchTreeTest: public ::testing::Test {};

template <typename Tree>
class ImplicitTreeTest: public ::testing::Test {};

typedef ::testing::Types< treap<treap_node<int, int>>, AVL<avl_node<int, int>>, rb_tree<rb_node<int, int>> > SearchTreeTypes;
typedef ::testing::Types< treap<implicit_treap_node<int>>, AVL<implicit_avl_node<int>>, rb_tree<implicit_rb_node<int>> > ImplicitSearchTreeTypes;

TYPED_TEST_SUITE(SearchTreeTest, SearchTreeTypes);
TYPED_TEST_SUITE(ImplicitTreeTest, ImplicitSearchTreeTypes);

TYPED_TEST(SearchTreeTest, SimpleTest) {
    TypeParam tree;

    std::vector<int> keys = {6, -1, 0, 1, 3, 10, 15};
    for (int key : keys) {
        tree.insert(key, 0);
    }
    ASSERT_EQ(tree.get_min()->key, -1);

    tree.erase(-1);
    ASSERT_EQ(tree.get_min()->key, 0);
}

TYPED_TEST(SearchTreeTest, BigTest) {
    TypeParam tree;

    std::map<int, int> map;
    srand(0);

    for (int i = 0; i < 200000; ++i) {
        int key = rand() % 1000;
        if (map.count(key)) {
            ASSERT_TRUE(tree.exists(key));
            tree.erase(key);
            map.erase(key);
        } else {
            tree.insert(key, i);
            map[key] = i;
        }
    }

    std::vector<int> keys;
    for (auto [key, value] : map) {
        keys.push_back(key);
    }

    for (auto key : keys) {
        if (rand() % 10 < 3) {
            ASSERT_TRUE(tree.exists(key));
            map.erase(key);
            tree.erase(key);
        }
    }

    for (auto [key, value] : map) {
        ASSERT_TRUE(tree.exists(key));
        ASSERT_EQ(tree.find(key)->value, value);
    }

    auto v = tree.get_traversal();
    for (int i = 0; i < v.size() - 1; ++i) {
        ASSERT_TRUE(v[i]->key <= v[i + 1]->key);
        ASSERT_TRUE(map.count(v[i]->key));
    }
}

TYPED_TEST(SearchTreeTest, PerfomanceTest) {
    TypeParam tree;
    srand(0);

    for (int i = 0; i < 500000; ++i) {
        int key = rand();
        if (tree.exists(key)) {
            tree.erase(key);
        } else {
            tree.insert(key, i);
        }
    }
    while (tree.size() > 0) {
        tree.erase(tree.get_kth(rand() % tree.size())->key);
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

    tree.erase_kth(0);
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

TYPED_TEST(ImplicitTreeTest, BigTest) {
    TypeParam tree;
    std::vector<int> values;
    srand(0);

    for (int i = 0; i < 200000; ++i) {
        int type = rand() % 3;
        if (type <= 1) {
            int val = rand();
            int pos = rand() % (values.size() + 1);

            tree.insert_kth(pos, val);
            values.insert(values.begin() + pos, val);
        } else {
            if (values.size() == 0) continue;
            int pos = rand() % values.size();
            tree.erase_kth(pos);
            values.erase(values.begin() + pos);
        }
    }

    for (int i = 0; i < values.size(); ++i) {
        ASSERT_EQ(tree.get_kth(i)->value, values[i]);
    }
}

TYPED_TEST(ImplicitTreeTest, PerfomanceTest) {
    TypeParam tree;
    srand(0);

    for (int i = 0; i < 500000; ++i) {
        int type = rand() % 3;
        if (type <= 1) {
            int val = rand();
            int pos = rand() % (tree.size() + 1);
            tree.insert_kth(pos, val);
        } else {
            int pos = rand() % tree.size();
            tree.erase_kth(pos);
        }
    }
}
