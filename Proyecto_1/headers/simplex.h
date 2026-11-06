#ifndef SIMPLEX_H
#define SIMPLEX_H

#include "../headers/structs.h"

void solve_simplex(const int max_variable_amount, const int decision_v, const int max_restriction, const int max_tries, const bool maximize, matrix_value matrix[max_variable_amount][max_restriction]);
void find_pivot(const int max_variable_amount, const int max_restriction, const bool maximize, int* i_indexF, int* j_indexF, matrix_value matrix[max_variable_amount][max_restriction]);
void canonize(const int max_variable_amount, const int max_restriction, const int i_indexF, const int j_indexF, matrix_value matrix[max_variable_amount][max_restriction]);

#endif