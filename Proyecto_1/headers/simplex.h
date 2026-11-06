#ifndef SIMPLEX_H
#define SIMPLEX_H

#include <stdbool.h>
#include "../headers/structs.h"

enum { SIMPLEX_OPTIMAL = 0, SIMPLEX_UNBOUNDED = 1, SIMPLEX_LOOP = 2 };

int  solve_simplex(const int max_variable_amount, const int decision_v, const int max_restriction, const int max_tries, const bool maximize, matrix_value matrix[max_variable_amount][max_restriction]);
void find_pivot(const int max_variable_amount, const int max_restriction, const bool maximize, int* i_indexF, int* j_indexF, matrix_value matrix[max_variable_amount][max_restriction]);
void canonize(const int max_variable_amount, const int max_restriction, const int i_indexF, const int j_indexF, matrix_value matrix[max_variable_amount][max_restriction]);
int  basic_row(const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction], const int col);
void extract_solution(const int max_variable_amount, const int desicion_v, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction], long double* x);

#endif
