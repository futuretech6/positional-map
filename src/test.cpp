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

    auto const target_index_find      = n / 2;
    auto const target_pos_find_by_pos = n / 2;
    auto const target_val_erase       = input[n / 2];

    AvlOrderStatisticTree<int, int> tree;
    for (auto x : input) {
        tree.insert(x, x);
    }

    assert(tree.size() == input.size());

    assert(tree.begin()->value == sorted.front());
    assert(tree.last()->value == sorted.back());

    assert(tree.find(input[target_index_find])->value == input[target_index_find]);
    assert(tree[input[target_index_find]] == input[target_index_find]);
    assert(tree.at(input[target_index_find]) == input[target_index_find]);

    assert(tree.find_by_pos(target_pos_find_by_pos + 1)->value == sorted[target_pos_find_by_pos]);

    print_tree(tree);

    tree.erase(target_val_erase);
    assert(tree.size() == input.size() - 1);
    assert(tree.find(target_val_erase) == tree.end());

    print_tree(tree);

    cout << "all tests passed" << endl;
    return 0;
}
