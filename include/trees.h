#pragma once

#include <cstddef>
#include <vector>

struct null_type {};

template <typename Node>
class tree {
public:
    tree(Node* root = nullptr) : root(root) {}

    using key_t = typename Node::key_t;

    Node* root;
};

template <typename Node>
class binary_tree : public tree<Node> {
public:
    using key_t = typename tree<Node>::key_t;

    std::vector<Node*> get_traversal() {
        std::vector<Node*> result;
        traversal(tree<Node>::root, result);
        return result;
    } 

    Node* find(const key_t& key) {
        Node* node = tree<Node>::root;
        while (node != nullptr) {
            node->push();
            if (node->key == key) {
                return node;
            } else if (node->key > key) {
                node = node->left;
            } else {
                node = node->right;
            }
        }
        return nullptr;
    }

    Node* min_in_subtree(Node* node) {
        if (node) node->push();
        while (node != nullptr && node->left != nullptr) {
            node = node->left;
            node->push();
        }
        return node;
    }

    Node* get_min() {
        return min_in_subtree(tree<Node>::root);
    }

    Node* get_kth(size_t k) {
        Node* node = tree<Node>::root;
        while (node != nullptr) {
            node->push();
            size_t left_size = get_size(node->left);
            if (left_size == k) {
                return node;
            } else if (left_size > k) {
                node = node->left;
            } else {
                node = node->right;
                k -= left_size + 1;
            }
        }
        return nullptr;
    }

    Node* next(const key_t& key) {
        Node* node = tree<Node>::root;
        Node* result = nullptr;
        while (node != nullptr) {
            node->push();
            if (node->key > key) {
                result = node;
                node = node->left;
            } else {
                node = node->right;
            }
        }
        return result;
    }

    Node* prev(const key_t& key) {
        Node* node = tree<Node>::root;
        Node* result = nullptr;
        while (node != nullptr) {
            node->push();
            if (node->key < key) {
                result = node;
                node = node->right;
            } else {
                node = node->left;
            }
        }
        return result;
    }

    static size_t order_of_key(Node* node, const key_t& key) {
        size_t result = 0;
        while (node != nullptr) {
            node->push();
            if (node->key == key) {
                return result + get_size(node->left);
            } else if (node->key > key) {
                node = node->left;
            } else {
                result += get_size(node->left) + 1;
                node = node->right;
            }
        }
        return result;
    }

    size_t order_of_key(const key_t& key) {
        return order_of_key(tree<Node>::root, key);
    }

    bool exists(const key_t& key) {
        return find(key) != nullptr;
    }

    size_t size()  {
        return get_size(tree<Node>::root);
    }

    void destroy(Node* node) {
        if (node == nullptr) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    void clear() {
        destroy(tree<Node>::root);
        tree<Node>::root = nullptr;
    }

protected:
    void traversal(Node* node, std::vector<Node*>& result) {
        if (node == nullptr) return;
        node->push();
        traversal(node->left, result);
        result.push_back(node);
        traversal(node->right, result);
    }

};

template <typename Node>
size_t get_size(Node* node) {
    if (node == nullptr) return 0;
    return node->size;
}

template <template<typename TKey, typename Node> class Template, typename Key, typename Value=null_type>
struct common_node : public Template<Key, common_node<Template, Key, Value>> {
    using Template<Key, common_node<Template, Key, Value> >::Template;
    Value value;

    common_node(const Key& key, const Value& value) : Template<Key, common_node<Template, Key, Value> >(key), value(value) {}
};

template <template<typename TKey, typename Node> class Template, typename Value>
struct implicit_node : public Template<null_type, implicit_node<Template, Value>> {
    using Template<null_type, implicit_node<Template, Value> >::Template;
    Value value;

    implicit_node(const Value& value) : Template<null_type, implicit_node<Template, Value> >(), value(value) {}
};

template <template<typename TKey, typename Node> class Template, typename Key>
struct key_node : public Template<Key, key_node<Template, Key>> {
    using Template<Key, key_node<Template, Key> >::Template;

    key_node(const Key& key) : Template<Key, key_node<Template, Key> >(key) {}
};

template <template<typename TKey, typename Node> class Template, typename Value>
struct implicit_reverse_node : public Template<null_type, implicit_reverse_node<Template, Value>> {
    using Template<null_type, implicit_reverse_node<Template, Value> >::Template;

    Value value;
    bool reversed;

    implicit_reverse_node(const Value& value) : Template<null_type, implicit_reverse_node<Template, Value> >(),
            value(value), reversed(false) {}

    void update() {
        Template<null_type, implicit_reverse_node<Template, Value> >::update();
        assert(!reversed);
        reversed = false;
    }

    void push() {
        if (reversed) {
            std::swap(this->left, this->right);
            if (this->left != nullptr) this->left->reversed ^= 1;
            if (this->right != nullptr) this->right->reversed ^= 1;
            reversed = false;
        }
    }

    void reverse() {
        reversed ^= 1;
    }
};




