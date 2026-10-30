#ifndef TEX_MANAGER_H
#define TEX_MANAGER_H

#include "../headers/structs.h"

void write_original_problem(const char* problem_name, const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]);

#endif