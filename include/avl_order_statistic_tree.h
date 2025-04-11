#pragma once

#include <iostream>
#include <iterator>
#include <queue>
#include <utility>

template <typename K, typename V>
class AvlOrderStatisticTree {
  public:
    static constexpr int BASE_INDEX = 1;  // the base index of `findbypos`

  public:
    // Define for some STL usage
    using key_type        = K;
    using value_type      = V;
    using pair_type       = std::pair<key_type, value_type>;
    using size_type       = size_t;
    using balance_type    = ptrdiff_t;
    using pointer         = value_type *;
    using const_pointer   = const value_type *;
    using reference       = value_type &;
    using const_reference = const value_type &;

  private:
    class Node {
      public:
        // key_type key;
        // value_type value;
        pair_type data;
        size_type height, size;
        Node *left, *right, *parent;

      public:
        Node(key_type k, value_type v)
            : data(std::make_pair(k, v)), height(1), size(1), left(nullptr), right(nullptr), parent(nullptr) {}
        ~Node() {}

        Node &operator=(Node const &other) {
            if (this == &other)
                return *this;

            this->data   = std::make_pair(other.data.first, other.data.second);
            this->height = other.height;
            this->size   = other.size;
            this->left   = other.left;
            this->right  = other.right;
            this->parent = other.parent;

            return *this;
        }

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

        Node *const next() const {
            auto *current = this;
            if (current->right) {
                return current->right->min_value_node();
            } else {
                while (current->parent && current->parent->right == current) {
                    current = current->parent;
                }
                return current->parent;
            }
        }

        Node *const prev() const {
            auto *current = this;
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
        using value_type        = pair_type;
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

        reference operator*() { return m_ptr->data; }
        const_reference operator*() const { return m_ptr->data; }
        pointer operator->() { return &m_ptr->data; }

      protected:
        Node *m_ptr = nullptr;
    };

    class const_iterator {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = pair_type;
        using size_type         = size_t;
        using difference_type   = ptrdiff_t;
        using pointer           = const value_type *;
        using const_pointer     = const value_type *;
        using reference         = const value_type &;
        using const_reference   = const value_type &;

      public:
        const_iterator(const Node *ptr = nullptr) : m_ptr(ptr) {}
        const_iterator(const const_iterator &other) = default;

        // 允许从 iterator 构造 const_iterator
        const_iterator(const iterator &other) : m_ptr(other.operator->()) {}

        ~const_iterator() { m_ptr = nullptr; }

        const_iterator &operator=(const const_iterator &other) = default;

        operator bool() const { return m_ptr != nullptr; }

        bool operator==(const const_iterator &other) const { return m_ptr == other.m_ptr; }
        bool operator!=(const const_iterator &other) const { return m_ptr != other.m_ptr; }

        const_iterator &operator++() {
            m_ptr = m_ptr->next();
            return (*this);
        }
        const_iterator &operator--() {
            m_ptr = m_ptr->prev();
            return (*this);
        }
        const_iterator operator++(int) {
            auto temp(*this);
            m_ptr = m_ptr->next();
            return temp;
        }
        const_iterator operator--(int) {
            auto temp(*this);
            m_ptr = m_ptr->prev();
            return temp;
        }

        const_reference operator*() const { return m_ptr->data; }
        const_pointer operator->() const { return &m_ptr->data; }

      protected:
        const Node *m_ptr = nullptr;
    };

  private:
    Node *root = nullptr;
    // [[deprecated("low performance, use function pointer instead")]] std::function<bool(key_type, key_type)> cmp_func
    // = std::less<key_type>();
    bool (*cmp)(key_type, key_type) = nullptr;

    /* AVL low-level operations */

    static size_type height(Node const *node) { return node ? node->height : 0; }

    static size_type size(Node const *node) { return node ? node->size : 0; }

    static void update(Node *node) {
        if (node) {
            node->height = 1 + std::max(height(node->left), height(node->right));
            node->size   = 1 + size(node->left) + size(node->right);
        }
    }

    static Node *right_rotate(Node *node) {
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

    static Node *left_rotate(Node *node) {
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

    static balance_type get_balance(Node *node) { return node ? height(node->left) - height(node->right) : 0; }

    /* AVL high-level operations */

    Node *insert(Node *node, key_type key, value_type value) {
        if (!node) {  // insert to an empty tree
            return new Node(key, value);
        }

        if (cmp(key, node->data.first)) {
            auto new_node    = insert(node->left, key, value);
            node->left       = new_node;
            new_node->parent = node;
        } else if (cmp(node->data.first, key)) {
            auto new_node    = insert(node->right, key, value);
            node->right      = new_node;
            new_node->parent = node;
        } else {
            // key already exists, update value
            node->data.second = value;
            return node;
        }

        // update current node's height and size
        update(node);

        // rotate
        if (auto balance = get_balance(node); balance > 1) {
            if (cmp(key, node->left->data.first)) {
                // LL
                return right_rotate(node);
            } else {
                // LR
                node->left = left_rotate(node->left);
                return right_rotate(node);
            }
        } else if (balance < -1) {
            if (cmp(node->right->data.first, key)) {
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

        if (cmp(key, node->data.first)) {
            return find(node->left, key);
        }
        if (cmp(node->data.first, key)) {
            return find(node->right, key);
        }
        return node;
    }

    Node *findbypos(Node *const node, size_type const &pos) const {
        auto const left_size = size(node->left);
        auto const root_pos  = left_size + BASE_INDEX;

        if (pos == root_pos) {
            return node;
        }
        if (pos < root_pos) {
            return findbypos(node->left, pos);
        }
        return findbypos(node->right, pos - left_size - 1);
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

        if (cmp(key, node->data.first)) {
            node->left = erase(node->left, key);
        } else if (cmp(node->data.first, key)) {
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
                node->data.first  = right_min_node->data.first;
                node->data.second = right_min_node->data.second;

                // delete min node of right subtree
                node->right = erase(node->right, right_min_node->data.first);
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

    static void free(Node *node) {
        if (node) {
            free(node->left);
            free(node->right);
            delete node;
        }
    }

  public:
    static bool less(key_type a, key_type b) { return a < b; }
    static bool greater(key_type a, key_type b) { return a > b; }

  public:
    AvlOrderStatisticTree(bool (*cmp)(key_type, key_type) = less) : root(nullptr), cmp(cmp) {}

    AvlOrderStatisticTree(AvlOrderStatisticTree const &) = delete;

    ~AvlOrderStatisticTree() { free(root); }

    void Depose() { free(root); }

    iterator begin() { return iterator(root->min_value_node()); }
    iterator end() { return iterator(nullptr); }
    const_iterator begin() const { return const_iterator(root->min_value_node()); }
    const_iterator end() const { return const_iterator(nullptr); }

    const_iterator cbegin() const { return const_iterator(root->min_value_node()); }
    const_iterator cend() const { return const_iterator(nullptr); }

    iterator last() { return iterator(root->max_value_node()); }

    size_type size() { return size(root); }

    void insert(std::pair<key_type, value_type> const &p) { insert(p.first, p.second); }

    void insert(key_type key, value_type value) { root = insert(root, key, value); }

    AvlOrderStatisticTree &operator=(AvlOrderStatisticTree const &that) {
        if (&that == this)
            return *this;

        this->free(this->root);

        this->cmp = that.cmp;
        // for (auto itor = that.cbegin(); itor != that.cend(); ++itor) {
        //     this->insert(itor->data.first, itor->data.second);
        // }
        for (auto &[key, val] : that) {
            this->insert(key, val);
        }
        return *this;
    }

    /**
     * @note this function will not handle error when key not found, use `at` or `find` instead
     */
    reference operator[](key_type const &key) {
        if (auto itor = find(key); itor != end()) {
            return itor->second;
        } else {
            insert(key, value_type());
            return reference(at(key));
        }
    }

    reference at(key_type const &key) {
        if (auto itor = find(key); itor == end()) {
            throw std::out_of_range("key not found");
        } else {
            return itor->second;
        }
    }

    iterator find(key_type const &key) const { return iterator(find(root, key)); }

    /**
     * @brief Find the node by position
     *
     * @param pos position of the node to be found
     * @note base index decided by `BASE_INDEX`
     */
    iterator findbypos(size_type const &pos) const {
        if (pos < BASE_INDEX || pos > size(root)) {
            return iterator(nullptr);
        }
        return iterator(findbypos(root, pos));
    }

    void erase(key_type key) { root = erase(root, key); }

    void print_tree() {
        std::cout << "-- AVL Order Statistic Tree --" << std::endl;
        print_tree(root);
        std::cout << "------------------------------" << std::endl;
    }

    static void print_tree(Node *node) {
        if (!node)
            return;

        int height = node->height;
        int width  = (1 << height) - 1;

        auto res = std::vector(height, std::vector<std::string>(width, " "));

        std::queue<std::pair<Node *, std::pair<int, int>>> q;
        q.push({node, {0, (width - 1) / 2}});

        while (!q.empty()) {
            auto p = q.front();
            q.pop();
            Node *node = p.first;
            int r      = p.second.first;
            int c      = p.second.second;

            res[r][c] = std::to_string(node->data.second);

            if (node->left) {
                q.push({node->left, {r + 1, c - (1 << (height - r - 2))}});
            }
            if (node->right) {
                q.push({node->right, {r + 1, c + (1 << (height - r - 2))}});
            }
        }

        for (const auto &row : res) {
            for (const auto &cell : row) {
                std::cout << cell;
            }
            std::cout << std::endl;
        }
    }
};
