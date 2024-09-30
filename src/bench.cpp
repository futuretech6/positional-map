#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <map>
#include <thread>
#include <utility>

#include "SkipList.h"
#include "avl_order_statistic_tree.h"
#include "bench.h"

int main() {
    using namespace std;

    for (unsigned const size : {1e4, 1e6}) {
        cout << "[SIZE: " << size << "]" << endl;

        init_input(size);

        auto random_key = random() % size;

        pair<string, vector<int>> inputs[] = {
            {"Random", random_input},
            {"Ordered", ordered_input},
            {"Reverse Ordered", reverse_ordered_input},
        };

        for (auto const &[name, input] : inputs) {
            cout << "[Input: " << name << "]" << endl;

            {
                std::map<int, int> std_map;
                cout << "std::map:" << endl;
                time_insert(std_map, input);
                assert(std_map[random_key] == random_key);
                time_find_by_key(std_map);
                time_erase(std_map);
                cout << endl;
            }

            {
                SkipList<int, int> skip_list_map;
                cout << "SkipList:" << endl;
                time_insert(skip_list_map, input);
                time_find_by_key(skip_list_map);
                time_find_by_pos(skip_list_map);
                time_erase(skip_list_map);
                cout << endl;
            }

            {
                AvlOrderStatisticTree<int, int> avl_tree;
                cout << "AvlOrderStatisticTree:" << endl;
                time_insert(avl_tree, input);
                time_find_by_key(avl_tree);
                time_find_by_pos(avl_tree);
                time_erase(avl_tree);
                cout << endl;
            }

            // {
            //     RbOrderStatisticTree<int, int> rb_tree;
            //     cout << "RbOrderStatisticTree:" << endl;
            //     time_insert(rb_tree, input);
            //     time_find_by_key(rb_tree);
            //     time_find_by_pos(rb_tree);
            //     time_erase(rb_tree);
            //     cout << endl;
            // }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    return 0;
}
