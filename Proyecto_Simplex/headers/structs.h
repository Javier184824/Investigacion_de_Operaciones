#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

typedef struct{
   int tag;
   int value;
} matrix_value;

void create_problem(const int max_variable_value, const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount + 2][max_restriction + 1]);

#endif