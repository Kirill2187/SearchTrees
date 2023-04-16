#pragma once

#include <random>
#include "trees.h"

template <typename Node>
class treap : public binary_tree<Node> {
public:
    using key_t = typename binary_tree<Node>::key_t;

    template <typename... Args>
    void insert(const key_t& key, Args&&... args);
    void insert(Node* node);

    void erase(const key_t& key);
    void erase_kth(size_t k);

    template <typename... Args>
    void insert_kth(size_t k, Args&&... args);
    void insert_kth(size_t k, Node* node);

    static Node* merge(Node* left, Node* right);
    static std::pair<Node*, Node*> split(Node* node, const key_t& key);
    static std::pair<Node*, Node*> split_k(Node* node, size_t k);

    Node* cut_subsegment(size_t l, size_t r);
    void insert_subsegment(size_t i, Node* t);
};

template <typename Node>
std::pair<Node*, Node*> treap<Node>::split(Node* node, const key_t& key) {
    if (node == nullptr) {
        return {nullptr, nullptr};
    }
    node->push();
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
    node->push();

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

    right->push();
    left->push();

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
Node* treap<Node>::cut_subsegment(size_t l, size_t r) {
    auto [left, right] = split_k(this->root, r + 1);
    auto [left2, right2] = split_k(left, l);
    this->root = merge(left2, right);
    return right2;
}

template <typename Node>
void treap<Node>::insert_subsegment(size_t i, Node* t) {
    auto [left, right] = split_k(this->root, i);
    this->root = merge(merge(left, t), right);
}

template <typename Node>
template <typename... Args>
void treap<Node>::insert(const key_t& key, Args&&... args) {
    insert(new Node(key, std::forward<Args>(args)...));
}

template <typename Node>
void treap<Node>::insert(Node* node) {
    auto [left, right] = split(this->root, node->key);
    this->root = merge(merge(left, node), right);
}

template <typename Node>
void treap<Node>::erase(const key_t& key) {
    auto [left, right] = split(this->root, key);
    auto [left2, right2] = split_k(right, 1);
    this->root = merge(left, right2);
}

template <typename Node>
void treap<Node>::erase_kth(size_t k) {
    auto [left, right] = split_k(this->root, k);
    auto [left2, right2] = split_k(right, 1);
    this->root = merge(left, right2);
}

template <typename Node>
template <typename... Args>
void treap<Node>::insert_kth(size_t k, Args&&... args) {
    insert_kth(k, new Node(std::forward<Args>(args)...));
}

template<typename Node>
void treap<Node>::insert_kth(size_t k, Node* node) {
    auto [left, right] = split_k(this->root, k);
    this->root = merge(merge(left, node), right);
}

using rnd_t = std::mt19937;
rnd_t rnd = rnd_t(std::random_device()());

template <typename Key, typename Node>
class treap_node_template {
public:
    using key_t = Key;

    [[no_unique_address]] Key key;
    size_t priority;
    size_t size;

    Node* left;
    Node* right;

    treap_node_template() : priority(rnd()), size(1), left(nullptr), right(nullptr) {}
    treap_node_template(const Key& key) : key(key), priority(rnd()), size(1), left(nullptr), right(nullptr) {}

    void update() {
        size = 1 + get_size(left) + get_size(right);
    }

    void push() {}
};

template <typename Key, typename Value>
using treap_node = common_node<treap_node_template, Key, Value>;

template <typename Value>
using treap_implicit_node = implicit_node<treap_node_template, Value>;

template <typename Key>
using treap_key_node = key_node<treap_node_template, Key>;

template <typename Value>
using treap_implicit_reverse_node = implicit_reverse_node<treap_node_template, Value>;