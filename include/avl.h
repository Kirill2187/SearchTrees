#pragma once

#include <trees.h>

template <typename Node>
class AVL: public binary_tree<Node> {
public:
    using key_t = typename binary_tree<Node>::key_t;

    template <typename... Args>
    void insert(const key_t& key, Args&&... args);
    void insert(Node* node);

    void erase(const key_t& key);

//    template <typename... Args>
//    void insert_kth(size_t k, Args&&... args);
//    void insert_kth(size_t k, Node* node);
//    size_t order_of_key(const key_t& key);
//
//    static Node* merge(Node* left, Node* right);
//    static std::pair<Node*, Node*> split(Node* node, const key_t& key);
//    static std::pair<Node*, Node*> split_k(Node* node, size_t k);
//
//    Node* cut_subsegment(size_t l, size_t r);
//    void insert_subsegment(size_t i, Node* t);

private:
    Node* rotate_left(Node* node);
    Node* rotate_right(Node* node);
    Node* balance(Node* node);

    Node* _insert(Node* node, Node* parent);
    Node* _erase(Node* parent, const key_t& key);
    Node* _remove_min(Node* parent);
};

template <typename Node>
unsigned char get_height(Node* node) {
    return node == nullptr ? 0 : node->height;
}

template <typename Node>
char get_balance(Node* node) {
    return get_height(node->right) - get_height(node->left);
}

template <typename Node>
Node* AVL<Node>::rotate_right(Node* pivot) {
    Node* q = pivot->left;
    if (q == nullptr) throw std::runtime_error("rotate_right: q == nullptr");

    pivot->left = q->right;
    q->right = pivot;

    pivot->update();
    q->update();

    return q;
}

template <typename Node>
Node* AVL<Node>::rotate_left(Node* pivot) {
    Node* q = pivot->right;
    if (q == nullptr) throw std::runtime_error("rotate_left: q == nullptr");

    pivot->right = q->left;
    q->left = pivot;

    pivot->update();
    q->update();

    return q;
}

template <typename Node>
Node* AVL<Node>::balance(Node* node) {
    node->update();
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

template <typename Key, typename Node>
struct avl_node_template {
    using key_t = Key;

    key_t key;
    unsigned char height;
    Node* left;
    Node* right;
//    Node* parent;
    size_t size;

    avl_node_template(const key_t& key, Node* parent = nullptr)
        : key(key), height(1), left(nullptr), right(nullptr) {
        update();
    }

    void update() {
        height = 1 + std::max(get_height(left), get_height(right));
        size = 1 + get_size(left) + get_size(right);
    }
};

template <typename Key, typename Value = null_type>
struct avl_node: avl_node_template<Key, avl_node<Key, Value>> {
    using avl_node_template<Key, avl_node<Key, Value>>::avl_node_template;
    using key_t = Key;

    [[no_unique_address]] Value value;

    avl_node(const key_t& key, const Value& value)
        : avl_node_template<Key, avl_node<Key, Value>>(key), value(value) {}
};

template <typename Value>
struct implicit_avl_node: avl_node_template<null_type, implicit_avl_node<Value>> {
    using avl_node_template<null_type, implicit_avl_node<Value>>::avl_node_template;

    Value value;

    implicit_avl_node(const Value& value)
        : avl_node_template<null_type, implicit_avl_node<Value>>(null_type()), value(value) {}
};