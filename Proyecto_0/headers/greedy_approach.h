#ifndef GREEDY_APPROACH_H
#define GREEDY_APPROACH_H

#include <stdlib.h>
#include "../headers/structs.h"

void solve_with_basic_greedy(const int item_amount, const int bag_size, item list[item_amount]);
void solve_with_proportional_greedy(const int item_amount, const int bag_size, item list[item_amount]);

#endif