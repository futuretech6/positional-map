#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <utility>

template <typename K, typename V>
class AvlOrderStatisticTree {
  public:
    static constexpr int BASE_INDEX = 1;  // the base index of `find_by_pos`

  public:
    // Define for some STL usage
    using key_type        = K;
    using value_type      = V;
    using size_type       = size_t;
    using balance_type    = ptrdiff_t;
    using pointer         = value_type *;
    using const_pointer   = const value_type *;
    using reference       = value_type &;
    using const_reference = const value_type &;

  private:
    class Node {
      public:
        key_type key;
        value_type value;
        size_type height, size;
        Node *left, *right, *parent;

      public:
        Node(key_type k, value_type v)
            : key(k), value(v), height(1), size(1), left(nullptr), right(nullptr), parent(nullptr) {}

        ~Node() {}

        Node *min_value_node() {
            Node *current = this;
            while (current->left != nullptr) {
                current = current->left;
            }
            return current;
        }

        Node *max_value_node() {
            Node *current = this;
            while (current->right != nullptr) {
                current = current->right;
            }
            return current;
        }

        Node *next() {
            Node *current = this;
            if (current->right) {
                return current->right->min_value_node();
            } else {
                while (current->parent && current->parent->right == current) {
                    current = current->parent;
                }
                return current->parent;
            }
        }

        Node *prev() {
            Node *current = this;
            if (current->left) {
                current->left->max_value_node();
            } else {
                while (current->parent && current->parent->left == current) {
                    current = current->parent;
                }
                return current->parent;
            }
        }
    };

    class iterator {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = Node;
        using size_type         = size_t;
        using difference_type   = ptrdiff_t;
        using pointer           = value_type *;
        using const_pointer     = const value_type *;
        using reference         = value_type &;
        using const_reference   = const value_type &;

      public:
        iterator(Node *ptr = nullptr) : m_ptr(ptr) {}
        iterator(const iterator &other) = default;
        ~iterator() { m_ptr = nullptr; }

        iterator &operator=(iterator const &other) = default;

        operator bool() const { return m_ptr != nullptr ? true : false; }

        bool operator==(const iterator &other) const {  // ==
            return m_ptr == other.m_ptr;
        }
        bool operator!=(const iterator &other) const {  // !=
            return m_ptr != other.m_ptr;
        }

        iterator &operator++() {  // ++itor
            m_ptr = m_ptr->next();
            return (*this);
        }
        iterator &operator--() {  // --itor
            m_ptr = m_ptr->prev();
            return (*this);
        }
        iterator operator++(int) {  // itor++
            auto temp(*this);
            m_ptr = m_ptr->next();
            return temp;
        }
        iterator operator--(int) {  // itor--
            auto temp(*this);
            m_ptr = m_ptr->prev();
            return temp;
        }

        reference operator*() { return *m_ptr; }              // obj
        const_reference operator*() const { return *m_ptr; }  // const obj
        pointer operator->() { return m_ptr; }                // ptr

      protected:
        Node *m_ptr = nullptr;
    };

  private:
    Node *root;

    /* AVL low-level operations */

    size_type height(Node const *node) const { return node ? node->height : 0; }

    size_type size(Node const *node) const { return node ? node->size : 0; }

    void update(Node *node) {
        if (node) {
            node->height = 1 + std::max(height(node->left), height(node->right));
            node->size   = 1 + size(node->left) + size(node->right);
        }
    }

    Node *right_rotate(Node *node) {
        Node *const left       = node->left;
        Node *const left_right = left->right;

        // rotate
        left->right = node;
        node->left  = left_right;

        // update parent
        left->parent = node->parent;
        node->parent = left;
        if (left_right) {
            left_right->parent = node;
        }

        // update height and size
        update(node);
        update(left);

        return left;
    }

    Node *left_rotate(Node *node) {
        Node *const right      = node->right;
        Node *const right_left = right->left;

        // rotate
        right->left = node;
        node->right = right_left;

        // update parent
        right->parent = node->parent;
        node->parent  = right;
        if (right_left) {
            right_left->parent = node;
        }

        // update height and size
        update(node);
        update(right);

        return right;
    }

    balance_type get_balance(Node *node) { return node ? height(node->left) - height(node->right) : 0; }

    /* AVL high-level operations */

    Node *insert(Node *node, key_type key, value_type value) {
        if (!node) {  // insert to an empty tree
            return new Node(key, value);
        }

        if (key < node->key) {
            auto new_node    = insert(node->left, key, value);
            node->left       = new_node;
            new_node->parent = node;
        } else if (key > node->key) {
            auto new_node    = insert(node->right, key, value);
            node->right      = new_node;
            new_node->parent = node;
        } else {
            // key already exists, update value
            node->value = value;
            return node;
        }

        // update current node's height and size
        update(node);

        // rotate
        if (auto balance = get_balance(node); balance > 1) {
            if (key < node->left->key) {
                // LL
                return right_rotate(node);
            } else {
                // LR
                node->left = left_rotate(node->left);
                return right_rotate(node);
            }
        } else if (balance < -1) {
            if (key > node->right->key) {
                // RR
                return left_rotate(node);
            } else {
                // RL
                node->right = right_rotate(node->right);
                return left_rotate(node);
            }
        } else {
            // no need to rotate
            return node;
        }
    }

    Node *find(Node *const node, key_type const &key) const {
        if (!node) {
            return nullptr;
        }

        if (key < node->key) {
            return find(node->left, key);
        }
        if (key > node->key) {
            return find(node->right, key);
        }
        return node;
    }

    Node *find_by_pos(Node *const node, size_type const &pos) const {
        auto const left_size = size(node->left);
        auto const root_pos  = left_size + BASE_INDEX;

        if (pos == root_pos) {
            return node;
        }
        if (pos < root_pos) {
            return find_by_pos(node->left, pos);
        }
        return find_by_pos(node->right, pos - left_size - 1);
    }

    /**
     * @brief Erase a node from the subtree
     *
     * @param node the root of the subtree
     * @param key the key of the node to be erased
     * @return Node* the new root of the subtree after deletion
     */
    Node *erase(Node *node, key_type key) {
        if (node == nullptr) {  // erase from an empty tree
            return nullptr;
        }

        if (key < node->key) {
            node->left = erase(node->left, key);
        } else if (key > node->key) {
            node->right = erase(node->right, key);
        } else {
            if (node->left == nullptr || node->right == nullptr) {
                // 0 or 1 child case
                Node *child = node->left ? node->left : node->right;

                if (child == nullptr) {
                    // no child case
                    delete node;
                    return nullptr;
                } else {
                    // one child case
                    Node *const old_parent = node->parent;
                    *node                  = *child;
                    node->parent           = old_parent;
                    delete child;
                }
            } else {
                // 2 children case
                // find min node of right subtree
                Node *right_min_node = node->right->min_value_node();

                // replace current node with min node
                node->key   = right_min_node->key;
                node->value = right_min_node->value;

                // delete min node of right subtree
                node->right = erase(node->right, right_min_node->key);
            }
        }

        // update current node
        update(node);

        // rotate
        if (auto balance = get_balance(node); balance > 1) {
            if (get_balance(node->left) >= 0) {
                // LL
                return right_rotate(node);
            } else {
                // LR
                node->left = left_rotate(node->left);
                return right_rotate(node);
            }
        } else if (balance < -1) {
            if (get_balance(node->right) <= 0) {
                // RR
                return left_rotate(node);
            } else {
                // RL
                node->right = right_rotate(node->right);
                return left_rotate(node);
            }
        } else {
            // no need to rotate
            return node;
        }
    }

    void free(Node *node) {
        if (node) {
            free(node->left);
            free(node->right);
            delete node;
        }
    }

  public:
    AvlOrderStatisticTree() : root(nullptr) {}
    AvlOrderStatisticTree(AvlOrderStatisticTree const &) = delete;

    ~AvlOrderStatisticTree() { free(root); }

    void Depose() { free(root); }

    iterator begin() { return iterator(root->min_value_node()); }
    iterator end() { return iterator(nullptr); }
    iterator last() { return iterator(root->max_value_node()); }

    size_type size() { return size(root); }

    void insert(std::pair<key_type, value_type> const &p) { insert(p.first, p.second); }

    void insert(key_type key, value_type value) { root = insert(root, key, value); }

    /**
     * @note this function will not handle error when key not found
     */
    reference operator[](key_type const &key) { return find(key)->value; }

    reference at(key_type const &key) {
        if (auto itor = find(key); itor == end()) {
            throw std::out_of_range("key not found");
        } else {
            return itor->value;
        }
    }

    iterator find(key_type const &key) const { return iterator(find(root, key)); }

    /**
     * @brief Find the node by position
     *
     * @param pos position of the node to be found
     * @note base index decided by `BASE_INDEX`
     */
    iterator find_by_pos(size_type const &pos) const {
        if (pos < BASE_INDEX || pos > size(root)) {
            return iterator(nullptr);
        }
        return iterator(find_by_pos(root, pos));
    }

    void erase(key_type key) { root = erase(root, key); }
};
