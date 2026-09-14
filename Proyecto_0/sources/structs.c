#include "../headers/structs.h"

void create_items(const int max_cost, const int max_value, const int item_amount, item list[item_amount]) {
    for(int i = 0; i < item_amount; i++) {
        list[i].use = false;
        list[i].tag = i + 1;
        list[i].cost = (rand() % max_cost) + 1;
        list[i].value = (rand() % max_value) + 1;
    }
}

void reset_items_use(const int item_amount, item list[item_amount]) {
    for (int i = 0; i < item_amount; i++) {
        list[i].use = false;
    }
}

void clean_time_report(time_reports* times) {
    for (int i = 0; i < EXPERIMENT_BASE_SIZE; i++) {
        for (int j = 0; j < EXPERIMENT_BASE_SIZE; j++) {
            times->dynamic_programming[i][j] = 0.0f;
            times->basic_greedy[i][j] = 0.0f;
            times->proportional_greedy[i][j] = 0.0f;
        }
    }
}

void clean_optimal_values_reports(optimal_values_reports* optimal_values) {
    for (int i = 0; i < EXPERIMENT_BASE_SIZE; i++) {
        for (int j = 0; j < EXPERIMENT_BASE_SIZE; j++) {
            optimal_values->dynamic_programming[i][j] = 0;
            optimal_values->basic_greedy[i][j] = 0;
            optimal_values->proportional_greedy[i][j] = 0;
        }
    }
}

void clean_collisions_reports(collisions_reports* collisions) {
    for (int i = 0; i < EXPERIMENT_BASE_SIZE; i++) {
        for (int j = 0; j < EXPERIMENT_BASE_SIZE; j++) {
            collisions->basic_greedy[i][j] = 0;
            collisions->proportional_greedy[i][j] = 0;
        }
    }
}

int get_value_sum_from_used_items(const int item_amount, item list[item_amount]) {
    int sum = 0;
    for (int i = 0; i < item_amount; i++) {
        if (list[i].use) {
            sum += list[i].value;
        }
    }
    return sum;
}