#ifndef STRUCTS_H
#define STRUCTS_H

/* Tipo de restriccion */
enum { OP_LE, OP_GE, OP_EQ };

/* tag de las columnas artificiales (Gran M). Decision: tag >= 1. Holgura/exceso: tag = 0 */
#define TAG_ARTIFICIAL (-1)

typedef struct{
   char name[64];
   int tag;
   long double value;   /* parte constante */
   long double m;       /* coeficiente de M (Gran M): el valor total es  m*M + value */
} matrix_value;

void create_problem(const int max_variable_value, const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]);

#endif