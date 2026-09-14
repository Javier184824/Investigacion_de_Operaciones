#include "../headers/greedy_approach.h"

int compare_by_value(const void* ptr_0, const void* ptr_1) {
    int value_0 = (*(item*) ptr_0).value;
    int value_1 = (*(item*) ptr_1).value;
    return value_1 - value_0;
}

int compare_by_proportion(const void* ptr_0, const void* ptr_1) {
    item item_0 = (*(item*) ptr_0);
    item item_1 = (*(item*) ptr_1);
    float proportion_0 = item_0.value / item_0.cost;
    float proportion_1 = item_1.value / item_1.cost;
    if (proportion_0 < proportion_1) return 1;
    if (proportion_0 > proportion_1) return -1;
    return 0;
}

void solve_with_basic_greedy(const int item_amount, const int bag_size, item list[item_amount]) {
    qsort(list, item_amount, sizeof(item), compare_by_value);
    int leftover_bag_size = bag_size - 1;
    for (int i = 0; i < item_amount; i++) {
        if (leftover_bag_size - list[i].cost >= 0) {
            list[i].use = true;
            leftover_bag_size -= list[i].cost;
        }
        else {
            list[i].use = false;
        }
    }
}

void solve_with_proportional_greedy(const int item_amount, const int bag_size, item list[item_amount]) {
    qsort(list, item_amount, sizeof(item), compare_by_proportion);
    int leftover_bag_size = bag_size - 1;
    for (int i = 0; i < item_amount; i++) {
        if (leftover_bag_size - list[i].cost >= 0) {
            list[i].use = true;
            leftover_bag_size -= list[i].cost;
        }
        else {
            list[i].use = false;
        }
    }
}

