#pragma once

#include <cstddef>
#include <optional>
#include <vector>

struct null_type {
    null_type() = default;
    null_type(const null_type&) = default;
    null_type(null_type&&) = default;
    null_type& operator=(const null_type&) = default;
    null_type& operator=(null_type&&) = default;
    ~null_type() = default;
    std::partial_ordering operator<=>(const null_type&) const {
        return std::partial_ordering::equivalent;
    };
    bool operator==(const null_type&) const {
        return true;
    };
};

template <typename Node>
class tree {
public:
    tree(Node* root = nullptr) : root(root) {}

    using key_t = typename Node::key_t;
    using value_t = typename Node::value_t;

    Node* root;

    virtual Node* search(const key_t& key) = 0;
    virtual void insert(const key_t& key, const value_t& value) = 0;
    virtual bool exists(const key_t& key) = 0;
    virtual void remove(const key_t& key) = 0;
    virtual Node* get_min() = 0;
    virtual size_t size() = 0;

    virtual Node* get_kth(size_t k) = 0;
    virtual void remove_kth(size_t k) = 0;
    virtual void insert_kth(size_t k, const value_t& value) = 0;

    virtual Node* next(const key_t& key) = 0;
    virtual Node* prev(const key_t& key) = 0;

    virtual std::vector<Node*> get_traversal() = 0;
    virtual ~tree() = default;
};

template <typename Node>
class binary_tree : public tree<Node> {
public:
    using key_t = typename tree<Node>::key_t;
    using value_t = typename tree<Node>::value_t;
    
    std::vector<Node*> get_traversal() override {
        std::vector<Node*> result;
        traversal(tree<Node>::root, result);
        return result;
    } 

    Node* search(const key_t& key) override {
        Node* node = tree<Node>::root;
        while (node != nullptr) {
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

    Node* get_min() override {
        Node* node = tree<Node>::root;
        while (node != nullptr && node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    Node* get_kth(size_t k) override {
        Node* node = tree<Node>::root;
        while (node != nullptr) {
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

    Node* next(const key_t& key) override {
        Node* node = tree<Node>::root;
        Node* result = nullptr;
        while (node != nullptr) {
            if (node->key > key) {
                result = node;
                node = node->left;
            } else {
                node = node->right;
            }
        }
        return result;
    }

    Node* prev(const key_t& key) override {
        Node* node = tree<Node>::root;
        Node* result = nullptr;
        while (node != nullptr) {
            if (node->key < key) {
                result = node;
                node = node->right;
            } else {
                node = node->left;
            }
        }
        return result;
    }

    bool exists(const key_t& key) override {
        return search(key) != nullptr;
    }

    size_t size() override {
        return get_size(tree<Node>::root);
    }



    void insert(const key_t& key, const value_t& value) override = 0;
    void remove(const key_t& key) override = 0;
    void remove_kth(size_t k) override = 0;
    void insert_kth(size_t k, const value_t& value) override = 0;

private:
    void traversal(Node* node, std::vector<Node*>& result) {
        if (node == nullptr) return;
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

template <typename Node, typename Value>
Value get_value(Node* node) {
    if (node == nullptr) return Value();
    return node->value;
}




