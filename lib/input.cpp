#include <algorithm>
#include <random>

#include "bench.h"

std::vector<int> random_input;
std::vector<int> ordered_input;
std::vector<int> reverse_ordered_input;

void init_input(unsigned size) {
    ordered_input.resize(size);
    std::iota(ordered_input.begin(), ordered_input.end(), 0);

    reverse_ordered_input.resize(size);
    std::iota(reverse_ordered_input.rbegin(), reverse_ordered_input.rend(), 0);

    random_input.resize(size);
    std::iota(random_input.begin(), random_input.end(), 0);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(random_input.begin(), random_input.end(), std::default_random_engine(seed));
}
