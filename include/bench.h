#pragma once

#include "SkipList.h"
#include <chrono>
#include <iostream>
#include <vector>

extern std::vector<int> random_input;
extern std::vector<int> ordered_input;
extern std::vector<int> reverse_ordered_input;

void init_input(unsigned);

using Duration                      = std::chrono::duration<double, std::nano>;
constexpr Duration INVALID_DURATION = std::chrono::duration<double, std::nano>(-1);

template <typename T>
Duration measure_insert(T &testMap, std::vector<int> const &input) {
    auto start_insert = std::chrono::high_resolution_clock::now();
    for (auto const &i : input) {
        testMap.insert({i, i});
    }
    auto end_insert = std::chrono::high_resolution_clock::now();

    Duration insert_duration = end_insert - start_insert;
    return insert_duration / testMap.size();
}

template <typename T>
Duration measure_find(T &testMap) {
    auto start_find = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < testMap.size(); ++i) {
        volatile auto it = testMap.find(i);
    }
    auto end_find = std::chrono::high_resolution_clock::now();

    Duration find_duration = end_find - start_find;
    return find_duration / testMap.size();
}

template <typename T>
Duration measure_findbypos(T &testMap) {
    auto start_find = std::chrono::high_resolution_clock::now();
    if constexpr (std::is_same<T, SkipList<int, int>>()) {
        for (int pos = 1; pos < testMap.size() + 1; pos++) {
            volatile auto it = testMap.findbypos(pos);
        }
    } else {
        for (int pos = testMap.BASE_INDEX; pos < testMap.size() + testMap.BASE_INDEX; pos++) {
            volatile auto it = testMap.findbypos(pos);
        }
    }
    auto end_find = std::chrono::high_resolution_clock::now();

    Duration find_duration = end_find - start_find;
    return find_duration / testMap.size();
}

template <typename T>
Duration measure_erase(T &testMap) {
    auto start_erase = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < testMap.size(); ++i) {
        testMap.erase(i);
    }
    auto end_erase = std::chrono::high_resolution_clock::now();

    Duration erase_duration = end_erase - start_erase;
    return erase_duration / testMap.size();
}

inline void print_time(
    Duration const &insert, Duration const &find, Duration const &findbypos, Duration const &erase,
    unsigned iteration_time
) {
    std::cout << "Insertion time (per operation): " << insert.count() / iteration_time << " ns" << std::endl;
    std::cout << "Lookup by key time (per operation): " << find.count() / iteration_time << " ns" << std::endl;
    if (findbypos != INVALID_DURATION) {
        std::cout << "Lookup by position time (per operation): " << findbypos.count() / iteration_time << " ns"
                  << std::endl;
    }
    std::cout << "Erase time (per operation): " << erase.count() / iteration_time << " ns" << std::endl;
}
