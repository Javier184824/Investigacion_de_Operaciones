#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "../headers/tex_manager.h"
#include "../headers/structs.h"
#include "../headers/simplex.h"

int main(int argc, char *argv[]) {
   srand(time(NULL));

   int max_restriction = (rand() % 13) + 3;//3;//
   int desicion = (rand() % 13) + 2;//2;//

   int max_tries = desicion*3;
   int max_variable_amount =  desicion + 2 + (max_restriction-1);
   int max_variable_value = rand() % 100;

   matrix_value matrix[max_variable_amount][max_restriction];

   create_problem(max_variable_value, desicion, max_variable_amount, max_restriction, matrix);

   for(int j = 0; j < max_restriction; j++){//print the initial table
      printf("%d\t",j);
      for(int i = 0; i < max_variable_amount; i++){
         printf("%d,%.4Lf\t", matrix[i][j].tag, matrix[i][j].value);
      }
      printf("\n");
   }
   printf("\n");

   #if 0
   solve_simplex(max_variable_amount, max_restriction, max_tries, matrix);
   printf("\n");

   for(int j = 0; j < max_restriction; j++){//print the final table
      printf("%d\t",j);
      for(int i = 0; i < max_variable_amount; i++){
         printf("%d,%.4Lf\t", matrix[i][j].tag, matrix[i][j].value);
      }
      printf("\n");
   }
   #endif

   write_original_problem("Some name", desicion, max_variable_amount, max_restriction, matrix);

   return 0;
}