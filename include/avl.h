#pragma once

#include <tuple>
#include "trees.h"

template <typename Node>
class AVL: public binary_tree<Node> {
public:
    using key_t = typename binary_tree<Node>::key_t;

    template <typename... Args>
    void insert(const key_t& key, Args&&... args);
    void insert(Node* node);

    void erase(const key_t& key);

    template <typename... Args>
    void insert_kth(size_t k, Args&&... args);
    void insert_kth(size_t k, Node* node);
    void erase_kth(size_t k);

    static Node* merge(Node* left, Node* right);
    static std::pair<Node*, Node*> split(Node* node, const key_t& key);
    static std::pair<Node*, Node*> split_k(Node* node, size_t k);

    Node* cut_subsegment(size_t l, size_t r);
    void insert_subsegment(size_t i, Node* t);

    template<typename... Args>
    void push_back(Args&&... args);

private:
    static inline Node* rotate_left(Node* pivot);
    static inline Node* rotate_right(Node* pivot);
    static inline Node* balance(Node* node);

    static Node* _insert(Node* node, Node* parent);
    Node* _erase(Node* parent, const key_t& key);
    static Node* _remove_min(Node* parent);

    static Node* _merge(Node* left, Node* mid, Node* right);
    static std::tuple<Node*, Node*, Node*> _split_k(Node* node, size_t k);
};

template <typename Node>
unsigned char get_height(Node* node) {
    return node == nullptr ? 0 : node->height;
}

template <typename Node>
void clear_vertex(Node* node) {
    if (node == nullptr) return;
    node->left = nullptr;
    node->right = nullptr;
    node->update();
}

template <typename Node>
char get_balance(Node* node) {
    return get_height(node->right) - get_height(node->left);
}

template <typename Node>
Node* AVL<Node>::rotate_right(Node* pivot) {
    if (pivot) pivot->push();
    Node* q = pivot->left;
    if (q) q->push();

    pivot->left = q->right;
    q->right = pivot;

    pivot->update();
    q->update();

    return q;
}

template <typename Node>
Node* AVL<Node>::rotate_left(Node* pivot) {
    if (pivot) pivot->push();
    Node* q = pivot->right;
    if (q) q->push();

    pivot->right = q->left;
    q->left = pivot;

    pivot->update();
    q->update();

    return q;
}

template <typename Node>
Node* AVL<Node>::balance(Node* node) {
    node->update();
    node->push();
    if (get_balance(node) == 2) {
        if (get_balance(node->right) < 0) {
            node->right = rotate_right(node->right);
        }
        return rotate_left(node);
    }
    if (get_balance(node) == -2) {
        if (get_balance(node->left) > 0) {
            node->left = rotate_left(node->left);
        }
        return rotate_right(node);
    }
    return node;
}

template <typename Node>
Node* AVL<Node>::_insert(Node* node, Node* parent) {
    if (!parent) return node;
    parent->push();
    if (node->key == parent->key) {
        delete node;
        return parent;
    }
    if (node->key < parent->key) {
        parent->left = _insert(node, parent->left);
    } else {
        parent->right = _insert(node, parent->right);
    }
    return balance(parent);
}

template <typename Node>
template <typename... Args>
void AVL<Node>::insert(const key_t& key, Args&&... args) {
    insert(new Node(key, std::forward<Args>(args)...));
}

template <typename Node>
void AVL<Node>::insert(Node* node) {
    if (!node) return;
    this->root = _insert(node, this->root);
}

template <typename Node>
Node* AVL<Node>::_remove_min(Node *parent) {
    if (!parent) return nullptr;
    if (!parent->left) return parent->right;
    parent->left = _remove_min(parent->left);
    return balance(parent);
}

template <typename Node>
Node* AVL<Node>::_erase(Node* parent, const key_t& key) {
    if (!parent) return nullptr;
    parent->push();
    if (key < parent->key) {
        parent->left = _erase(parent->left, key);
    } else if (key > parent->key) {
        parent->right = _erase(parent->right, key);
    } else {
        Node* left = parent->left;
        Node* right = parent->right;
        delete parent;
        if (!right) return left;
        Node* min = binary_tree<Node>::min_in_subtree(right);
        min->right = _remove_min(right);
        min->left = left;
        return balance(min);
    }
    return balance(parent);
}

template <typename Node>
void AVL<Node>::erase(const key_t& key) {
    this->root = _erase(this->root, key);
}

template <typename Node>
Node* AVL<Node>::_merge(Node *left, Node *mid, Node *right) {
    if (mid) mid->push();
    if (left) left->push();
    if (right) right->push();

    if (!mid) {
        if (!right) return left;
        if (!left) return right;
    }

    bool left_is_higher = get_height(left) > get_height(right);
    Node* higher = left_is_higher ? left : right;
    Node* lower = left_is_higher ? right : left;

    if (get_height(higher) - get_height(lower) <= 1) {
        mid->left = left;
        mid->right = right;
        mid->update();
        return mid;
    }

    higher->push();
    if (left_is_higher) {
        higher->right = _merge(higher->right, mid, lower);
    } else {
        higher->left = _merge(lower, mid, higher->left);
    }
    return balance(higher);
}

template <typename Node>
Node* AVL<Node>::merge(Node* left, Node* right) {
    if (!left) return right;
    if (!right) return left;

    auto [left_part, mid] = split_k(left, get_size(left) - 1);
    return _merge(left_part, mid, right);
}

template <typename Node>
std::tuple<Node*, Node*, Node*> AVL<Node>::_split_k(Node* node, size_t k) {
    if (!node) return std::make_tuple(nullptr, nullptr, nullptr);
    node->push();

    if (get_size(node->left) + 1 == k) {
        auto res = std::make_tuple(node->left, node, node->right);
        clear_vertex(node);
        return res;
    }
    if (get_size(node->left) >= k) {
        auto [left, mid, right] = _split_k(node->left, k);
        clear_vertex(mid);

        return std::make_tuple(left, mid, _merge(right, node, node->right));
    } else {
        auto [left, mid, right] = _split_k(node->right, k - get_size(node->left) - 1);
        clear_vertex(mid);

        return std::make_tuple(_merge(node->left, node, left), mid, right);
    }
}

template <typename Node>
std::pair<Node*, Node*> AVL<Node>::split_k(Node* node, size_t k) {
    auto [left, mid, right] = _split_k(node, k);
    left = _merge(left, mid, nullptr);
    return {left, right};
}

template <typename Node>
std::pair<Node*, Node*> AVL<Node>::split(Node* node, const key_t& key) {
    size_t k = binary_tree<Node>::order_of_key(node, key);
    return split_k(node, k);
}

template <typename Node>
void AVL<Node>::insert_kth(size_t k, Node *node) {
    auto [left, right] = split_k(this->root, k);
    this->root = _merge(left, node, right);
}

template <typename Node>
template <typename... Args>
void AVL<Node>::insert_kth(size_t k, Args&&... args) {
    insert_kth(k, new Node(std::forward<Args>(args)...));
}

template <typename Node>
Node* AVL<Node>::cut_subsegment(size_t l, size_t r) {
    auto [left, join, right] = _split_k(this->root, l);
    auto [mid, right2] = split_k(right, r - l + 1);
    this->root = _merge(left, join, right2);
    return mid;
}

template <typename Node>
void AVL<Node>::insert_subsegment(size_t i, Node* t) {
    auto [left, join, right] = _split_k(this->root, i);
    this->root = merge(_merge(left, join, t), right);
}

template <typename Node>
void AVL<Node>::erase_kth(size_t k) {
    auto [left, mid, right] = _split_k(this->root, k + 1);
    delete mid;
    this->root = merge(left, right);
}

template <typename Node>
template <typename... Args>
void AVL<Node>::push_back(Args&&... args) {
    this->root = _merge(this->root, new Node(std::forward<Args>(args)...), nullptr);
}

template <typename Key, typename Node>
struct avl_node_template {
    using key_t = Key;

    [[no_unique_address]] key_t key;
    unsigned char height;
    Node* left;
    Node* right;
    size_t size;

    avl_node_template() : height(1), left(nullptr), right(nullptr) {
        update();
    }
    avl_node_template(const key_t& key)
        : key(key), height(1), left(nullptr), right(nullptr) {
        update();
    }

    void update() {
        height = 1 + std::max(get_height(left), get_height(right));
        size = 1 + get_size(left) + get_size(right);
    }

    void push() {}
};

template <typename Key, typename Value>
using avl_node = common_node<avl_node_template, Key, Value>;

template <typename Value>
using avl_implicit_node = implicit_node<avl_node_template, Value>;

template <typename Key>
using avl_key_node = key_node<avl_node_template, Key>;

template <typename Value>
using avl_implicit_reverse_node = implicit_reverse_node<avl_node_template, Value>;