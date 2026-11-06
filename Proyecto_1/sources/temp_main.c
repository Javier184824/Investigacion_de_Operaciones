#if 0

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "../headers/tex_manager.h"
#include "../headers/structs.h"
#include "../headers/simplex.h"

#define MAX_DECISION_VARIABLES 15
#define MAX_RESTRICTIONS 15
#define MIN_DECISION_VARIABLES 2
#define MIN_RESTRICTIONS 2

int main(int argc, char *argv[]) {
   srand(time(NULL));

   //int max_restriction = (rand() % (MAX_RESTRICTIONS - MIN_RESTRICTIONS + 1)) + MIN_RESTRICTIONS;
   //int desicion = (rand() % (MAX_DECISION_VARIABLES - MIN_DECISION_VARIABLES + 1)) + MIN_DECISION_VARIABLES;

   int max_restriction = 3 + 1;
   int desicion = 3 + 1;

   int max_tries = desicion*3;
   int max_variable_amount = desicion + max_restriction;
   int max_variable_value = rand() % 100 + 1;

   bool maximize = true;

   matrix_value matrix[max_variable_amount][max_restriction];

   create_problem(max_variable_value, desicion, max_variable_amount, max_restriction, matrix);

   #if 0
   for(int j = 0; j < max_restriction; j++){//print the initial table
      printf("%d\t",j);
      for(int i = 0; i < max_variable_amount; i++){
         printf("%d,%.4Lf\t", matrix[i][j].tag, matrix[i][j].value);
      }
      printf("\n");
   }
   printf("\n");

   for(int j = 0; j < max_restriction; j++){//print the final table
      printf("%d\t",j);
      for(int i = 0; i < max_variable_amount; i++){
         printf("%d,%.4Lf\t", matrix[i][j].tag, matrix[i][j].value);
      }
      printf("\n");
   }
   #endif

   write_original_problem(1, "Some name", desicion, max_variable_amount, max_restriction, matrix);
   write_initial_table(desicion, max_variable_amount, max_restriction, matrix);
   
   solve_simplex(max_variable_amount, desicion, max_restriction, max_tries, maximize, matrix);

   write_final_table(desicion, max_variable_amount, max_restriction, matrix);
   
   return 0;
}

#endif