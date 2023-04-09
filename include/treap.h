#pragma once

#include <random>
#include "trees.h"

template <typename Node>
class treap : public binary_tree<Node> {
public:
    using key_t = typename binary_tree<Node>::key_t;
    using value_t = typename binary_tree<Node>::value_t;

    void insert(const key_t& key, const value_t& value) override;
    void remove(const key_t& key) override;
    void remove_kth(size_t k) override;
    void insert_kth(size_t k, const value_t& value) override;

    static Node* merge(Node* left, Node* right);
    static std::pair<Node*, Node*> split(Node* root, const key_t& key);
    static std::pair<Node*, Node*> split_k(Node* root, size_t k);
};

template <typename Node>
std::pair<Node*, Node*> treap<Node>::split(Node* node, const key_t& key) {
    if (node == nullptr) {
        return {nullptr, nullptr};
    }
    if (node->key < key) {
        auto [left, right] = split(node->right, key);
        node->right = left;
        node->update();
        return {node, right};
    } else {
        auto [left, right] = split(node->left, key);
        node->left = right;
        node->update();
        return {left, node};
    }
}

template <typename Node>
std::pair<Node*, Node*> treap<Node>::split_k(Node* node, size_t k) {
    if (node == nullptr) return {nullptr, nullptr};
    if (k == 0) return {nullptr, node};
    if (k == get_size(node)) return {node, nullptr};

    size_t left_size = get_size(node->left);
    if (left_size >= k) {
        auto [left, right] = split_k(node->left, k);
        node->left = right;
        node->update();
        return {left, node};
    } else {
        auto [left, right] = split_k(node->right, k - left_size - 1);
        node->right = left;
        node->update();
        return {node, right};
    }
}

template <typename Node>
Node* treap<Node>::merge(Node* left, Node* right) {
    if (left == nullptr) return right;
    if (right == nullptr) return left;
    if (left->priority > right->priority) {
        left->right = merge(left->right, right);
        left->update();
        return left;
    } else {
        right->left = merge(left, right->left);
        right->update();
        return right;
    }
}

template <typename Node>
void treap<Node>::insert(const key_t& key, const value_t& value) {
    auto [left, right] = split(this->root, key);
    this->root = merge(merge(left, new Node(key, value)), right);
}

template <typename Node>
void treap<Node>::remove(const key_t& key) {
    auto [left, right] = split(this->root, key);
    auto [left2, right2] = split_k(right, 1);
    this->root = merge(left, right2);
    delete left2;
}

template <typename Node>
void treap<Node>::remove_kth(size_t k) {
    auto [left, right] = split_k(this->root, k);
    auto [left2, right2] = split_k(right, 1);
    this->root = merge(left, right2);
    delete left2;
}

template<typename Node>
void treap<Node>::insert_kth(size_t k, const value_t &value) {
    auto [left, right] = split_k(this->root, k);
    this->root = merge(merge(left, new Node(value)), right);
}

using rnd_t = std::mt19937;
rnd_t rnd = rnd_t(std::random_device()());

template <typename Key, typename Value, typename Node>
class treap_node_template {
public:
    using key_t = Key;
    using value_t = Value;

    Key key;
    Value value;
    size_t priority;
    size_t size;

    Node* left;
    Node* right;

    treap_node_template(const Key& key, const Value& value) : key(key), value(value), priority(rnd()), size(1), left(nullptr), right(nullptr) {}
    treap_node_template(const Value& value) : value(value), priority(rnd()), size(1), left(nullptr), right(nullptr) {}

    void update() {
        size = 1 + get_size(left) + get_size(right);
    }
};

template <typename Key, typename Value>
struct treap_node : public treap_node_template<Key, Value, treap_node<Key, Value>> {
    using treap_node_template<Key, Value, treap_node<Key, Value>>::treap_node_template;
};
