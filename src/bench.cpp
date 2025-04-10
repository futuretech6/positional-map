#include <cassert>
#include <chrono>
#include <iostream>
#include <map>
#include <thread>
#include <utility>

#include "SkipList.h"
#include "avl_order_statistic_tree.h"
#include "bench.h"

constexpr auto SLEEP_TIME = std::chrono::milliseconds(1);

int main() {
    using namespace std;

    std::pair<unsigned, unsigned> constexpr test_config[] = {
        {1e3, 500},
        {1e4, 100},
        {1e5, 10},
    };

    for (auto const &[size, iteration_time] : test_config) {
        cout << "[SIZE: " << size << "]" << endl;

        init_input(size);

        pair<string, vector<int>> inputs[] = {
            {"Random", random_input},
            {"Ordered", ordered_input},
            {"Reverse Ordered", reverse_ordered_input},
        };

        for (auto const &[name, input] : inputs) {
            cout << "[Input: " << name << "]" << endl;

            {
                cout << "std::map:" << endl;
                Duration duration_insert = Duration(0), duration_find = Duration(0), duration_erase = Duration(0);
                for (auto i = iteration_time; i; i--) {
                    std::map<int, int> std_map;
                    duration_insert += measure_insert(std_map, input);
                    auto random_key = random() % size;
                    assert(std_map[random_key] == random_key);
                    duration_find += measure_find(std_map);
                    duration_erase += measure_erase(std_map);
                    std::this_thread::sleep_for(SLEEP_TIME);
                }
                print_time(duration_insert, duration_find, INVALID_DURATION, duration_erase, iteration_time);
                cout << endl;
            }

            {
                cout << "SkipList:" << endl;
                Duration duration_insert = Duration(0), duration_find = Duration(0), duration_find_by_pos = Duration(0),
                         duration_erase = Duration(0);
                for (auto i = iteration_time; i; i--) {
                    SkipList<int, int> skip_list_map;
                    duration_insert += measure_insert(skip_list_map, input);
                    auto random_key = random() % size;
                    assert(skip_list_map[random_key] == random_key);
                    duration_find += measure_find(skip_list_map);
                    duration_find_by_pos += measure_find_by_pos(skip_list_map);
                    duration_erase += measure_erase(skip_list_map);
                    std::this_thread::sleep_for(SLEEP_TIME);
                }
                print_time(duration_insert, duration_find, duration_find_by_pos, duration_erase, iteration_time);
                cout << endl;
            }

            {
                cout << "AvlOrderStatisticTree:" << endl;
                Duration duration_insert = Duration(0), duration_find = Duration(0), duration_find_by_pos = Duration(0),
                         duration_erase = Duration(0);
                for (auto i = iteration_time; i; i--) {
                    AvlOrderStatisticTree<int, int> avl_tree;
                    duration_insert += measure_insert(avl_tree, input);
                    auto random_key = random() % size;
                    assert(avl_tree[random_key] == random_key);
                    duration_find += measure_find(avl_tree);
                    duration_find_by_pos += measure_find_by_pos(avl_tree);
                    duration_erase += measure_erase(avl_tree);
                    std::this_thread::sleep_for(SLEEP_TIME);
                }
                print_time(duration_insert, duration_find, duration_find_by_pos, duration_erase, iteration_time);
                cout << endl;
            }
        }
    }

    return 0;
}
