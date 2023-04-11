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
        while (node != nullptr && node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    Node* get_min() {
        return min_in_subtree(tree<Node>::root);
    }

    Node* get_kth(size_t k) {
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

    Node* next(const key_t& key) {
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

    Node* prev(const key_t& key) {
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





