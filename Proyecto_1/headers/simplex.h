#ifndef SIMPLEX_H
#define SIMPLEX_H

#include "../headers/structs.h"

void solve_simplex(const int max_variable_amount, const int max_restriction, const int max_tries, matrix_value matrix[max_variable_amount][max_restriction]);

#endif