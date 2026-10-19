#ifndef SIMPLEX_H
#define SIMPLEX_H

#include "../headers/structs.h"

void solve_simplex(const int max_variable_value, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount + 2][max_restriction + 1]);

#endif