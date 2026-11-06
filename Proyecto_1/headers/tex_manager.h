#ifndef TEX_MANAGER_H
#define TEX_MANAGER_H

#include "../headers/structs.h"

FILE* open_file(const char* file_path);
void write_original_problem(const int maximizing, const char* problem_name, const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]);
void write_table(FILE* fptr, const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]);
void write_table_with_pivot(FILE* fptr, const int pivot_row, const int pivot_col, const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]);
void write_initial_table(const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]);
void write_final_table(const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]);
void write_solution(const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]);

#endif