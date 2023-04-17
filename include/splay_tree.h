#pragma once

#include "trees.h"

template <typename Node>
class splay_tree : public binary_tree<Node> {

public:
    using key_t = typename binary_tree<Node>::key_t;

    void splay_kth(size_t k);
    Node* find(const key_t& key);
    bool exists(const key_t& key);
    Node* get_kth(size_t k);
    Node* get_min();
    size_t order_of_key(const key_t& key);
//    Node* next(const key_t& key);
//    Node* prev(const key_t& key);

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

private:
    static inline Node* rotate_right(Node* pivot, std::vector<Node*>& update_stack);
    static inline Node* rotate_left(Node* pivot, std::vector<Node*>& update_stack);
    static inline Node* break_left(Node* v, Node* &l_root, Node* &l, std::vector<Node*>& update_stack);
    static inline Node* break_right(Node* v, Node* &r_root, Node* &r, std::vector<Node*>& update_stack);
    static inline void assemble(Node* cur, Node* &l_root, Node* &r_root, Node* &l, Node* &r, std::vector<Node*>& update_stack);
};

template <typename Node>
void splay_tree<Node>::splay_kth(size_t k) {
    if (this->root == nullptr) return;
    Node* cur = this->root;
    if (cur) cur->push();
    size_t cur_index = get_size(cur->left);
    Node* l_root = nullptr, *r_root = nullptr;
    Node* l = nullptr, *r = nullptr;

    std::vector<Node*> update_stack;

    while (cur_index != k) {
        if (cur) cur->push();
        if (k < cur_index) {
            if (cur->left) cur->left->push();
            cur_index -= get_size(cur->left->right) + 1;
            if (k < cur_index) {
                if (cur->left->left) cur->left->left->push();
                cur_index -= get_size(cur->left->left->right) + 1;
                cur = rotate_right(cur, update_stack);
                cur = break_right(cur, r_root, r, update_stack);
            } else if (k > cur_index) {
                if (cur->left->right) cur->left->right->push();
                cur_index += get_size(cur->left->right->left) + 1;
                cur = break_right(cur, r_root, r, update_stack);
                cur = break_left(cur, l_root, l, update_stack);
            } else {
                cur = break_right(cur, r_root, r, update_stack);
            }
        } else {
            if (cur->right) cur->right->push();
            cur_index += get_size(cur->right->left) + 1;
            if (k < cur_index) {
                if (cur->right->left) cur->right->left->push();
                cur_index -= get_size(cur->right->left->right) + 1;
                cur = break_left(cur, l_root, l, update_stack);
                cur = break_right(cur, r_root, r, update_stack);
            } else if (cur_index < k) {
                if (cur->right->right) cur->right->right->push();
                cur_index += get_size(cur->right->right->left) + 1;
                cur = rotate_left(cur, update_stack);
                cur = break_left(cur, l_root, l, update_stack);
            } else {
                cur = break_left(cur, l_root, l, update_stack);
            }
        }
    }
    this->root = cur;
    assemble(cur, l_root, r_root, l, r, update_stack);

    for (auto it = update_stack.rbegin(); it != update_stack.rend(); ++it) {
        if (*it) (*it)->update();
    }
    this->root->update();
}

template <typename Node>
Node* splay_tree<Node>::rotate_left(Node *pivot, std::vector<Node*>& update_stack) {
    if (pivot) pivot->push();
    Node* new_pivot = pivot->right;
    if (new_pivot) new_pivot->push();

    pivot->right = new_pivot->left;
    new_pivot->left = pivot;

    update_stack.push_back(new_pivot);
    update_stack.push_back(pivot);

    return new_pivot;
}

template <typename Node>
Node* splay_tree<Node>::rotate_right(Node *pivot, std::vector<Node*>& update_stack) {
    if (pivot) pivot->push();
    Node* new_pivot = pivot->left;
    if (new_pivot) new_pivot->push();

    pivot->left = new_pivot->right;
    new_pivot->right = pivot;

    update_stack.push_back(new_pivot);
    update_stack.push_back(pivot);

    return new_pivot;
}

template <typename Node>
Node* splay_tree<Node>::break_left(Node* v, Node* &l_root, Node* &l, std::vector<Node*>& update_stack) {
    v->push();
    Node* tmp = v->right;
    if (tmp) tmp->push();

    v->right = nullptr;

    if (!l) l_root = (l = v);
    else l = (l->right = v);

    update_stack.push_back(v);

    return tmp;
}

template <typename Node>
Node* splay_tree<Node>::break_right(Node* v, Node* &r_root, Node* &r, std::vector<Node*>& update_stack) {
    v->push();
    Node* tmp = v->left;
    if (tmp) tmp->push();
    v->left = nullptr;

    if (!r) r_root = (r = v);
    else r = (r->left = v);

    update_stack.push_back(v);

    return tmp;
}

template <typename Node>
void splay_tree<Node>::assemble(Node* cur, Node* &l_root, Node* &r_root, Node* &l, Node* &r, std::vector<Node*>& update_stack) {
    cur->push();
    if (cur->left) cur->left->push();
    if (cur->right) cur->right->push();

    if (!l_root) l_root = (l = cur->left);
    else l->right = cur->left;

    if (!r_root) r_root = (r = cur->right);
    else r->left = cur->right;

    cur->left = l_root;
    cur->right = r_root;

    update_stack.push_back(l);
    update_stack.push_back(r);
    update_stack.push_back(cur);
}

template <typename Node>
Node* splay_tree<Node>::find(const key_t& key) {
    if (!this->root) return nullptr;

    Node* cur = this->root;
    size_t cur_index = get_size(cur->left);
    while (cur) {
        cur->push();
        if (key < cur->key) {
            if (cur->left) cur_index -= get_size(cur->left->right) + 1;
            cur = cur->left;
        } else if (cur->key < key) {
            if (cur->right) cur_index += get_size(cur->right->left) + 1;
            cur = cur->right;
        } else {
            break;
        }
    }
    splay_kth(cur_index);

    return cur;
}

template <typename Node>
bool splay_tree<Node>::exists(const key_t& key) {
    return find(key) != nullptr;
}

template <typename Node>
Node* splay_tree<Node>::get_kth(size_t k) {
    splay_kth(k);
    return this->root;
}

template <typename Node>
Node* splay_tree<Node>::get_min() {
    splay_kth(0);
    return this->root;
}

template <typename Node>
size_t splay_tree<Node>::order_of_key(const key_t& key) {
    if (!this->root) return 0;
    find(key);
    return binary_tree<Node>::order_of_key(this->root, key);
}

template <typename Node>
Node* splay_tree<Node>::merge(Node *left, Node *right) {
    if (!left) return right;
    if (!right) return left;

    auto tree = splay_tree<Node> {left};
    tree.splay_kth(get_size(left) - 1);
    left = tree.root;
    left->right = right;
    left->update();

    return left;
}

template <typename Node>
std::pair<Node*, Node*> splay_tree<Node>::split(Node* root, const key_t& key) {
    if (!root) return {nullptr, nullptr};
    splay_tree<Node> tree {root};

    size_t order = binary_tree<Node>::order_of_key(root, key);
    if (order < tree.size()) tree.splay_kth(order);
    else return {root, nullptr};

    Node* left = tree.root->left;
    tree.root->left = nullptr;
    tree.root->update();

    return {left, tree.root};
}

template <typename Node>
std::pair<Node*, Node*> splay_tree<Node>::split_k(Node *root, size_t k) {
    if (k == 0) return {nullptr, root};
    if (k == get_size(root)) return {root, nullptr};

    splay_tree<Node> tree {root};
    tree.splay_kth(k);
    Node* left = tree.root->left;
    tree.root->left = nullptr;
    tree.root->update();

    return {left, tree.root};
}

template <typename Node>
void splay_tree<Node>::insert(Node* node) {
    auto [left, right] = split(this->root, node->key);
    node->left = left;
    node->right = right;
    node->update();
    this->root = node;
}

template<typename Node>
template<typename... Args>
void splay_tree<Node>::insert(const key_t &key, Args &&...args) {
    insert(new Node(key, std::forward<Args>(args)...));
}

template <typename Node>
void splay_tree<Node>::erase(const key_t& key) {
    find(key);
    Node* mid = this->root;
    this->root = merge(this->root->left, this->root->right);
    delete mid;
}

template <typename Node>
void splay_tree<Node>::erase_kth(size_t k) {
    splay_kth(k);
    Node* mid = this->root;
    this->root = merge(this->root->left, this->root->right);
    delete mid;
}

template <typename Node>
void splay_tree<Node>::insert_kth(size_t k, Node *node) {
    auto [left, right] = split_k(this->root, k);
    this->root = merge(merge(left, node), right);
}

template <typename Node>
template<typename... Args>
void splay_tree<Node>::insert_kth(size_t k, Args &&...args) {
    insert_kth(k, new Node(std::forward<Args>(args)...));
}

template <typename Node>
Node* splay_tree<Node>::cut_subsegment(size_t l, size_t r) {
    auto [left, right] = split_k(this->root, r + 1);
    auto [left2, right2] = split_k(left, l);
    this->root = merge(left2, right);
    return right2;
}

template <typename Node>
void splay_tree<Node>::insert_subsegment(size_t i, Node* t) {
    auto [left, right] = split_k(this->root, i);
    this->root = merge(merge(left, t), right);
}

template <typename Key, typename Node>
struct splay_node_template {
    using key_t = Key;

    [[no_unique_address]] key_t key;
    Node* left;
    Node* right;
    size_t size;

    splay_node_template() : left(nullptr), right(nullptr), size(1) {}
    splay_node_template(const key_t& key) : key(key), left(nullptr), right(nullptr), size(1) {}

    void update() {
        size = 1 + get_size(left) + get_size(right);
    }

    void push() {}
};

template <typename Key, typename Value>
using splay_node = common_node<splay_node_template, Key, Value>;

template <typename Value>
using splay_implicit_node = implicit_node<splay_node_template, Value>;

template <typename Key>
using splay_key_node = key_node<splay_node_template, Key>;

template <typename Value>
using splay_implicit_reverse_node = implicit_reverse_node<splay_node_template, Value>;
