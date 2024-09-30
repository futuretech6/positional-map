#pragma once
#include <cstring>
#include <functional>
#include <iostream>
#include <utility>

#define MAX_LEVEL (15)
template <typename K, typename V>
struct Node {
    K first;
    V second;
    Node **next;
    int *span;
    Node(const K &k, const V &v, int level) {
        first  = k;
        second = v;
        next   = (Node<K, V> **)malloc(sizeof(Node<K, V> *) * level);
        memset(next, 0, sizeof(Node<K, V> *) * level);

        span = new int[level];
        memset(span, 0, sizeof(int) * level);
    }
    ~Node() {
        free(next);
        delete[] span;
    }
};

template <typename K, typename V>
class SkipList {
  public:
    class iterator {
      public:
        iterator(Node<K, V> *ptr) : m_ptr(ptr) {}
        bool operator==(const iterator &it) const { return m_ptr == it.m_ptr; }
        bool operator!=(const iterator &it) const { return m_ptr != it.m_ptr; }
        Node<K, V> *operator->() const { return m_ptr; }
        Node<K, V> *operator*() const { return m_ptr; }
        // 前缀自加
        iterator &operator++() {
            if (m_ptr && m_ptr->next)
                m_ptr = m_ptr->next[0];
            else
                m_ptr = nullptr;
            return *this;
        }

        // 后缀自加
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

      private:
        Node<K, V> *m_ptr;
    };

    SkipList(bool ascend = true);
    ~SkipList();
    void Init();
    void Depose();
    bool insert(std::pair<const K, const V> const &p) { return insert(p.first, p.second); }
    bool insert(const K &key, const V &v);
    iterator find(const K &key) const;
    V &operator[](const K &key);
    iterator find_by_pos(int pos) const;
    bool erase(const K &key);
    int get_random_level();
    iterator begin() const { return iterator(m_head->next[0]); }
    iterator last() const { return iterator(m_last); }
    iterator end() const { return iterator(nullptr); }
    int size() const { return m_elem_count; }
    bool ascend_func(const K &k1, const K &k2) { return k1 < k2; }
    bool descend_func(const K &k1, const K &k2) { return k1 > k2; }
    void display_list() {

        std::cout << "\n*****Skip List*****"
                  << "\n";
        for (int i = 0; i < m_curr_level; i++) {
            Node<K, V> *node = this->m_head->next[i];
            std::cout << "Level " << i << ": ";
            while (node != NULL) {
                std::cout << "(" << node->first << ":" << node->second << ";" << node->span[i] << ")  ";
                node = node->next[i];
            }
            std::cout << std::endl;
        }
    }
    SkipList &operator=(SkipList const &sl);

  private:
    std::function<bool(const K &k1, const K &k2)> m_func_cmp;
    int m_maxlevel;
    int m_curr_level;
    int m_elem_count;
    Node<K, V> *m_head;
    Node<K, V> *m_last         = nullptr;
    bool m_ascend              = true;
    SkipList(const SkipList &) = delete;
};

template <typename K, typename V>
SkipList<K, V>::SkipList(bool ascend) : m_ascend(ascend) {
    m_maxlevel   = MAX_LEVEL;
    m_curr_level = 0;
    m_elem_count = 0;

    Init();
}
template <typename K, typename V>
void SkipList<K, V>::Init() {
    K k;
    V v;
    m_head = new Node<K, V>(k, v, m_maxlevel);

    if (m_ascend)
        m_func_cmp = std::bind(&SkipList::ascend_func, this, std::placeholders::_1, std::placeholders::_2);
    else
        m_func_cmp = std::bind(&SkipList::descend_func, this, std::placeholders::_1, std::placeholders::_2);
}

template <typename K, typename V>
SkipList<K, V>::~SkipList() {
    Depose();
}
template <typename K, typename V>
void SkipList<K, V>::Depose() {
    for (auto iter = begin(); iter != end();) {
        auto next = iter;
        iter++;
        delete (*next);
    }
    delete m_head;
    m_head       = nullptr;
    m_curr_level = 0;
    m_elem_count = 0;
}
template <typename K, typename V>
SkipList<K, V> &SkipList<K, V>::operator=(SkipList const &sl) {
    if (&sl == this)
        return *this;
    Depose();

    m_ascend = sl.m_ascend;
    Init();
    // insert every elem
    for (auto iter = sl.begin(); iter != sl.end(); ++iter)
        insert(iter->first, iter->second);

    return *this;
}
template <typename K, typename V>
bool SkipList<K, V>::insert(const K &key, const V &v) {
    // find the max elem that less than key
    Node<K, V> *current = m_head;
    Node<K, V> *update[m_maxlevel];

    int spans[m_maxlevel] = {0};
    for (int i = m_curr_level - 1; i >= 0; i--) {
        int level_span = 0;
        while (current->next[i] && m_func_cmp(current->next[i]->first, key)) {
            level_span += current->span[i];
            current = current->next[i];
        }
        spans[i]  = level_span;
        update[i] = current;
    }
    current = current->next[0];
    if (current && current->first == key) {
        current->second = v;
        return true;
    }

    if (!current || current->first != key) {
        int random_level = get_random_level();
        if (random_level > m_curr_level) {
            for (int i = m_curr_level; i < random_level; i++)
                update[i] = m_head;
            m_curr_level = random_level;
        }
        bool last = current == nullptr;
        current   = new Node<K, V>(key, v, random_level);
        if (last)
            m_last = current;
        int total_span = 0;
        for (int i = 0; i < m_curr_level; ++i) {
            if (i < random_level) {
                total_span += (i > 0 ? spans[i - 1] : 1);

                current->next[i]   = update[i]->next[i];
                current->span[i]   = update[i]->next[i] ? (update[i]->span[i] - total_span + 1) : 0;
                update[i]->next[i] = current;
                update[i]->span[i] = total_span;
            } else if (update[i]->next[i]) {
                update[i]->span[i]++;
            }
        }
        m_elem_count++;
    }
    return true;
}
template <typename K, typename V>
V &SkipList<K, V>::operator[](const K &key) {
    Node<K, V> *current = m_head;
    Node<K, V> *update[m_maxlevel];

    int spans[m_maxlevel] = {0};
    for (int i = m_curr_level - 1; i >= 0; i--) {
        int level_span = 0;
        while (current->next[i] && m_func_cmp(current->next[i]->first, key)) {
            level_span += current->span[i];
            current = current->next[i];
        }
        spans[i]  = level_span;
        update[i] = current;
    }
    current = current->next[0];
    if (current && current->first == key) {
        return current->second;
    }

    if (!current || current->first != key) {
        int random_level = get_random_level();
        if (random_level > m_curr_level) {
            for (int i = m_curr_level; i < random_level; i++)
                update[i] = m_head;
            m_curr_level = random_level;
        }
        bool last = current == nullptr;
        current   = new Node<K, V>(key, V(), random_level);
        if (last)
            m_last = current;
        int total_span = 0;
        for (int i = 0; i < m_curr_level; ++i) {
            if (i < random_level) {
                total_span += (i > 0 ? spans[i - 1] : 1);

                current->next[i]   = update[i]->next[i];
                current->span[i]   = update[i]->next[i] ? (update[i]->span[i] - total_span + 1) : 0;
                update[i]->next[i] = current;
                update[i]->span[i] = total_span;
            } else if (update[i]->next[i]) {
                update[i]->span[i]++;
            }
        }
        m_elem_count++;
    }
    return current->second;
}

template <typename K, typename V>
typename SkipList<K, V>::iterator SkipList<K, V>::find(const K &key) const {
    // find the max elem that less than key
    Node<K, V> *current = m_head;

    for (int i = m_curr_level - 1; i >= 0; i--) {
        while (current->next[i] && m_func_cmp(current->next[i]->first, key))
            current = current->next[i];
    }
    current = current->next[0];
    if (current && current->first == key) {
        return iterator(current);
    }
    return end();
}

template <typename K, typename V>
typename SkipList<K, V>::iterator SkipList<K, V>::find_by_pos(int pos) const {
    // find the max elem that less than key
    Node<K, V> *current = m_head;
    if (pos > 1) {
        int total = 0;
        for (int i = m_curr_level - 1; i >= 0; i--) {
            while (current->next[i] && current->span[i] + total < pos) {
                total += current->span[i];
                current = current->next[i];
            }
        }
    }
    return iterator(current->next[0]);
}
template <typename K, typename V>
bool SkipList<K, V>::erase(const K &key) {
    // find the current elem
    Node<K, V> *current = m_head;
    Node<K, V> *update[m_maxlevel];

    for (int i = m_curr_level - 1; i >= 0; i--) {
        while (current->next[i] && m_func_cmp(current->next[i]->first, key))
            current = current->next[i];
        update[i] = current;
    }
    current = current->next[0];
    if (!current || current->first != key) {
        return false;
    }
    // if remove the last elem
    if (current == m_last) {
        if (update[0] == m_head)
            m_last = nullptr;  // then the list is empty
        else
            m_last = update[0];
    }
    // if find then remove it

    for (int i = 0; i < m_curr_level; ++i) {
        if (update[i]->next[i] == current) {
            update[i]->next[i] = current->next[i];
            int right_val      = current->next[i] ? current->span[i] : 0;
            update[i]->span[i] = update[i]->span[i] + right_val - 1;
        } else if (update[i]->next[i]) {
            update[i]->span[i]--;
        }
    }
    // if remove the highest level then change the curr_level
    while (m_curr_level && m_head->next[m_curr_level - 1] == nullptr)
        m_curr_level--;
    delete current;
    m_elem_count--;
    return true;
}

template <typename K, typename V>
int SkipList<K, V>::get_random_level() {
    int k = 1;
    while (rand() % 2)
        k++;
    return std::min(m_maxlevel, k);
}
