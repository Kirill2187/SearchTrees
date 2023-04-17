#pragma once

#include "trees.h"

template <typename Node>
class rb_tree : public binary_tree<Node> {
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
    void inline rb_insert_fixup(Node* node);
    void inline rotate_left(Node* pivot);
    void inline rotate_right(Node* pivot);

    static Node* _merge(Node* left, Node* mid, Node* right);
    static std::pair<Node*, Node*> _merge_no_fix(Node* left, Node* mid, Node* right);
    static std::tuple<Node*, Node*, Node*> _split_k(Node* node, size_t k);
    static inline void clear_vertex(Node* node);

};

template <typename Node>
void rb_tree<Node>::clear_vertex(Node *node) {
    if (node == nullptr) return;
    node->push();
    if (node->left) {
        node->left->parent = nullptr;
        node->left->set_black(true);
    }
    if (node->right) {
        node->right->parent = nullptr;
        node->right->set_black(true);
    }

    node->left = nullptr;
    node->right = nullptr;
    node->set_black(true);
    if (node->parent != nullptr) {
        if (node->parent->left == node) {
            node->parent->left = nullptr;
        } else {
            node->parent->right = nullptr;
        }
        node->parent->update();
        node->parent = nullptr;
    }
    node->update();
}

template <typename Node>
void rb_tree<Node>::insert(Node *node) {
    if (this->root == nullptr) {
        this->root = node;
        this->root->set_black(true);
        return;
    }
    Node* cur = this->root;
    Node* parent = nullptr;
    while (cur != nullptr) {
        parent = cur;
        if (node->key < cur->key) {
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    }
    node->parent = parent;

    if (node->key < parent->key) {
        parent->left = node;
    } else {
        parent->right = node;
    }

    for (Node* n = node; n != nullptr; n = n->parent) {
        n->update();
    }

    node->set_black(false);
    rb_insert_fixup(node);
}

template <typename Node>
void flip_colors(Node* node) {
    node->left->flip_color();
    node->right->flip_color();
    node->flip_color();
}

template <typename Node>
void rb_tree<Node>::rb_insert_fixup(Node *node) {
    while (node != this->root && !node->parent->black) {
        if (node->parent == node->parent->parent->left) {
            Node* uncle = node->parent->parent->right;
            if (uncle != nullptr && !uncle->black) {
                flip_colors(node->parent->parent);
                node = node->parent->parent;
            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    rotate_left(node);
                }
                node->parent->set_black(true);
                node->parent->parent->set_black(false);
                rotate_right(node->parent->parent);
            }
        } else {
            Node* uncle = node->parent->parent->left;
            if (uncle != nullptr && !uncle->black) {
                flip_colors(node->parent->parent);
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    rotate_right(node);
                }
                node->parent->set_black(true);
                node->parent->parent->set_black(false);
                rotate_left(node->parent->parent);
            }
        }
    }
    this->root->set_black(true);
}

template <typename Node>
void rb_tree<Node>::rotate_left(Node *pivot) {
    Node* new_pivot = pivot->right;
    if (this->root == pivot) this->root = new_pivot;

    Node* pivot_parent = pivot->parent;
    bool pivot_is_left = pivot_parent && pivot_parent->left == pivot;

    if (pivot->parent && pivot->parent->parent) pivot->parent->parent->push();
    if (pivot->parent) pivot->parent->push();
    pivot->push();
    new_pivot->push();

    pivot->right = new_pivot->left;
    if (new_pivot->left) new_pivot->left->parent = pivot;
    new_pivot->left = pivot;
    new_pivot->parent = pivot->parent;
    pivot->parent = new_pivot;

    pivot->update();
    new_pivot->update();

    if (pivot_parent) {
        if (pivot_is_left) {
            pivot_parent->left = new_pivot;
        } else {
            pivot_parent->right = new_pivot;
        }
        pivot_parent->update();
    }
}

template <typename Node>
void rb_tree<Node>::rotate_right(Node *pivot) {
    Node* new_pivot = pivot->left;
    if (this->root == pivot) this->root = new_pivot;

    Node* pivot_parent = pivot->parent;
    bool pivot_is_left = pivot_parent && pivot_parent->left == pivot;

    if (pivot->parent && pivot->parent->parent) pivot->parent->parent->push();
    if (pivot->parent) pivot->parent->push();
    pivot->push();
    new_pivot->push();

    pivot->left = new_pivot->right;
    if (new_pivot->right) new_pivot->right->parent = pivot;

    new_pivot->right = pivot;
    new_pivot->parent = pivot->parent;
    pivot->parent = new_pivot;

    pivot->update();
    new_pivot->update();

    if (pivot_parent) {
        if (pivot_is_left) {
            pivot_parent->left = new_pivot;
        } else {
            pivot_parent->right = new_pivot;
        }
        pivot_parent->update();
    }
}

template <typename Node>
unsigned char get_black_height(Node* node) {
    return node == nullptr ? 1 : node->black_height;
}
template <typename Node>
bool is_black(Node* node) {
    return node == nullptr || node->black;
}

template <typename Node>
std::pair<Node*, Node*> rb_tree<Node>::_merge_no_fix(Node *left, Node *mid, Node *right) {
    if (get_black_height(left) == get_black_height(right)) {
        mid->left = left;
        mid->right = right;
        mid->set_black(false);
        mid->parent = nullptr;
        mid->update();
        mid->push();
        if (left) left->parent = mid;
        if (right) right->parent = mid;

        return {mid, mid};
    }

    if (left) left->push();
    if (right) right->push();

    bool left_is_higher = get_black_height(left) > get_black_height(right);
    Node* higher = left_is_higher ? left : right;
    Node* lower = left_is_higher ? right : left;

    Node* to_fix;
    if (left_is_higher) {
        auto [new_higher, fix] = _merge_no_fix(higher->right, mid, lower);
        higher->right = new_higher;
        if (new_higher) new_higher->parent = higher;
        to_fix = fix;
    } else {
        auto [new_higher, fix] = _merge_no_fix(lower, mid, higher->left);
        higher->left = new_higher;
        if (new_higher) new_higher->parent = higher;
        to_fix = fix;
    }
    higher->update();
    return {higher, to_fix};
}

template <typename Node>
Node* rb_tree<Node>::_merge(Node *left, Node *mid, Node *right) {
    if (!mid) {
        if (!left) return right;
        if (!right) return left;
        throw std::runtime_error("mid is null");
    }

    auto [new_root, to_fix] = _merge_no_fix(left, mid, right);
    new_root->set_black(true);
    new_root->update();
    rb_tree<Node> new_tree = {new_root};

    if (to_fix != nullptr && !is_black(to_fix->parent)) new_tree.rb_insert_fixup(to_fix);
    return new_tree.root;
}

template <typename Node>
Node* rb_tree<Node>::merge(Node* left, Node* right) {
    if (!left) return right;
    if (!right) return left;

    auto [left_part, mid] = split_k(left, get_size(left) - 1);
    return _merge(left_part, mid, right);
}

template <typename Node>
std::tuple<Node*, Node*, Node*> rb_tree<Node>::_split_k(Node* node, size_t k) {
    if (!node) return std::make_tuple(nullptr, nullptr, nullptr);
    node->push();
    Node* node_left = node->left;
    Node* node_right = node->right;
    clear_vertex(node);

    if (get_size(node_left) + 1 == k) {
        auto res = std::make_tuple(node_left, node, node_right);
        return res;
    }
    if (get_size(node_left) >= k) {
        auto [left, mid, right] = _split_k(node_left, k);

        return std::make_tuple(left, mid, _merge(right, node, node_right));
    } else {
        auto [left, mid, right] = _split_k(node_right, k - get_size(node_left) - 1);

        return std::make_tuple(_merge(node_left, node, left), mid, right);
    }
}

template <typename Node>
std::pair<Node*, Node*> rb_tree<Node>::split_k(Node* node, size_t k) {
    auto [left, mid, right] = _split_k(node, k);
    left = _merge(left, mid, nullptr);
    return {left, right};
}

template <typename Node>
std::pair<Node*, Node*> rb_tree<Node>::split(Node* node, const key_t& key) {
    size_t k = binary_tree<Node>::order_of_key(node, key);
    return split_k(node, k);
}

template <typename Node>
void rb_tree<Node>::insert_kth(size_t k, Node *node) {
    auto [left, right] = split_k(this->root, k);
    this->root = _merge(left, node, right);
}

template <typename Node>
template <typename... Args>
void rb_tree<Node>::insert_kth(size_t k, Args&&... args) {
    insert_kth(k, new Node(std::forward<Args>(args)...));
}

template <typename Node>
Node* rb_tree<Node>::cut_subsegment(size_t l, size_t r) {
    auto [left, join, right] = _split_k(this->root, l);
    auto [mid, right2] = split_k(right, r - l + 1);
    this->root = _merge(left, join, right2);
    return mid;
}

template <typename Node>
void rb_tree<Node>::insert_subsegment(size_t i, Node* t) {
    auto [left, join, right] = _split_k(this->root, i);
    this->root = merge(_merge(left, join, t), right);
}

template <typename Node>
void rb_tree<Node>::erase_kth(size_t k) {
    auto [left, mid, right] = _split_k(this->root, k + 1);
    delete mid;
    this->root = merge(left, right);
}

template <typename Node>
void rb_tree<Node>::erase(const key_t &key) {
    auto [left, mid, right] = _split_k(this->root, binary_tree<Node>::order_of_key(this->root, key) + 1);
    delete mid;
    this->root = merge(left, right);
}


template <typename Node>
template <typename... Args>
void rb_tree<Node>::insert(const key_t& key, Args&&... args) {
    insert(new Node(key, std::forward<Args>(args)...));
}

template <typename Node>
template <typename... Args>
void rb_tree<Node>::push_back(Args&&... args) {
    if (!this->root) {
        this->root = new Node(std::forward<Args>(args)...);
        return;
    }
    Node* cur = this->root;
    Node* prev = nullptr;
    while (cur) {
        cur->push();
        prev = cur;
        cur = cur->right;
    }
    prev->right = new Node(std::forward<Args>(args)...);
    prev->right->parent = prev;
    prev->right->set_black(false);

    for (cur = prev->right; cur != nullptr; cur = cur->parent) {
        cur->update();
    }

    if (!prev->black) rb_insert_fixup(prev->right);
}


template <typename Key, typename Node>
struct rb_node_template {
    using key_t = Key;

    bool black = true;
    key_t key;
    unsigned char black_height = 1;
    Node* left;
    Node* right;
    Node* parent = nullptr;
    size_t size;

    rb_node_template() : left(nullptr), right(nullptr), size(1) {
        update();
    }
    rb_node_template(const key_t& key)
            : key(key), left(nullptr), right(nullptr), size(1) {
        update();
    }

    void flip_color() {
        black = !black;
        update_black_height();
    }

    void set_black(bool flag) {
        black = flag;
        update_black_height();
    }

    void update_black_height() {
        black_height = std::max(get_black_height(left), get_black_height(right)) + black;
    }

    void update() {
        update_black_height();
        size = 1 + get_size(left) + get_size(right);
    }

    void push() {}
};

template <typename Key, typename Value>
using rb_node = common_node<rb_node_template, Key, Value>;

template <typename Value>
using rb_implicit_node = implicit_node<rb_node_template, Value>;

template <typename Key>
using rb_key_node = key_node<rb_node_template, Key>;

template <typename Value>
using rb_implicit_reverse_node = implicit_reverse_node<rb_node_template, Value>;

