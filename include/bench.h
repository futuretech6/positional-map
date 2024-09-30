#pragma once

#include <cassert>
#include <chrono>
#include <iostream>
#include <ratio>
#include <vector>

extern std::vector<int> random_input;
extern std::vector<int> ordered_input;
extern std::vector<int> reverse_ordered_input;

void init_input(unsigned);

template <typename T>
void time_insert(T &testMap, std::vector<int> const &input) {
    auto start_insert = std::chrono::high_resolution_clock::now();
    for (auto const &i : input) {
        testMap.insert({i, i});
    }
    auto end_insert = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::nano> insert_duration = end_insert - start_insert;
    std::cout << "Insertion time (avg): " << insert_duration.count() / testMap.size() << " ns" << std::endl;
}

template <typename T>
void time_find_by_key(T &testMap) {
    auto start_find = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < testMap.size(); ++i) {
        volatile auto it = testMap.find(i);
    }
    auto end_find = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::nano> find_duration = end_find - start_find;
    std::cout << "Lookup by key time (avg): " << find_duration.count() / testMap.size() << " ns" << std::endl;
}

template <typename T>
void time_find_by_pos(T &testMap) {
    auto start_find = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= testMap.size(); ++i) {
        volatile auto it = testMap.find_by_pos(i);
    }
    auto end_find = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::nano> find_duration = end_find - start_find;
    std::cout << "Lookup by position time (avg): " << find_duration.count() / testMap.size() << " ns" << std::endl;
}

template <typename T>
void time_erase(T &testMap) {
    auto start_erase = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < testMap.size(); ++i) {
        testMap.erase(i);
    }
    auto end_erase = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::nano> erase_duration = end_erase - start_erase;
    std::cout << "Erase time (avg): " << erase_duration.count() / testMap.size() << " ns" << std::endl;
}
