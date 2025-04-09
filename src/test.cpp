#include <cassert>
#include <iostream>
#include <vector>

#include "avl_order_statistic_tree.h"

template <typename K, typename V>
inline void print_tree(AvlOrderStatisticTree<K, V> &tree) {
    using namespace std;
    for (auto itor = tree.begin(); itor != tree.end(); ++itor) {
        cout << itor->key << ": " << itor->value << ", ";
    }
    cout << endl;
}

int main() {
    using namespace std;
    vector<int> input  = {3, 5, 2, 9, 8, 7, 1, 4, 6};
    vector<int> sorted = input;
    sort(sorted.begin(), sorted.end());
    auto const &n = input.size();

    auto const target_val_erase = input[n / 2];

    AvlOrderStatisticTree<int, int> tree;
    for (auto val : input) {
        tree.insert(val, val);
    }

    assert(tree.size() == input.size());

    assert(tree.begin()->value == sorted.front());
    assert(tree.last()->value == sorted.back());

    for (auto const &val : input) {
        assert(tree.find(val)->value == val);
        assert(tree[val] == val);
        assert(tree.at(val) == val);
    }
    for (auto pos = tree.BASE_INDEX; pos < n + tree.BASE_INDEX; pos++) {
        assert(tree.find_by_pos(pos)->value == sorted[pos - tree.BASE_INDEX]);
    }

    print_tree(tree);

    tree.erase(target_val_erase);
    assert(tree.size() == input.size() - 1);
    assert(tree.find(target_val_erase) == tree.end());

    print_tree(tree);

    cout << "all tests passed" << endl;
    return 0;
}
