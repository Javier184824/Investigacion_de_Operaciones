#ifndef DYNAMICPROGRAMING_H
#define DYNAMICPROGRAMING_H

#include "../headers/structs.h"

void solve_with_dynamic_programming(const int item_amount, const int bag_size, item list[item_amount], matrix_value table[item_amount][bag_size]);
int get_optimal_value_dynamic_programming(const int item_amount, const int bag_size, matrix_value table[item_amount][bag_size]);
void get_final_value(const int item_amount, const int bag_size, item list[item_amount], matrix_value table[item_amount][bag_size]);

#endif