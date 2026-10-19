#include "../headers/structs.h"
//#include "../headers/simplex.h"


int main(int argc, char *argv[]) {
   srand(time(NULL));

   int max_restriction = 3; //(rand() % 13) + 3
   int max_variable_amount = 4 + (max_restriction-1); //(rand() % 13) + 4 + max_restriction
   int max_variable_value = rand() % 100;

   matrix_value matrix[max_variable_amount][max_restriction];

   create_problem(max_variable_value, max_variable_amount, max_restriction, matrix);

   for(int j = 0; j < max_restriction; j++){//print the final table
      printf("%d\t",j);
      for(int i = 0; i < max_variable_amount; i++){
         printf("x_%d,%d\t", matrix[i][j].tag, matrix[i][j].value);
      }
      printf("\n");
   }

   return 0;
}