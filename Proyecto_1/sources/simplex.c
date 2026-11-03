#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "../headers/structs.h"
#include "../headers/simplex.h"

void solve_simplex(const int max_variable_amount, const int max_restriction, const int max_tries, const bool maximize, matrix_value matrix[max_variable_amount][max_restriction]){
	bool stop = false;//3n, n = desicion_v
	int cont = 0;
	while(!stop && cont <= max_tries){
		int i_index = 0;
		int j_index = 0;
		long double actual_i_value = 1;
		long double actual_j_value = -1;

		//========find pivot=============
		if(maximize){
			for(int i = 1; i < max_variable_amount-1; i++){
			if(matrix[i][0].value < 0){
				if(matrix[i][0].value < actual_i_value){
					bool valid = false;
					for (int j = 1; j < max_restriction; ++j){
						if(matrix[i][j].value > 0){
							valid = true;
						}
					}
					if(valid){
						actual_i_value = matrix[i][0].value;
						i_index = i;
						}
					}
				}
			}
		}else{
			actual_i_value = -1;
			for(int i = 1; i < max_variable_amount-1; i++){
			if(matrix[i][0].value > 0){
				if(matrix[i][0].value > actual_i_value){
					bool valid = false;
					for (int j = 1; j < max_restriction; ++j){
						if(matrix[i][j].value > 0){
							valid = true;
						}
					}
					if(valid){
						actual_i_value = matrix[i][0].value;
						i_index = i;
						}
					}
				}
			}
		}
		
		if(i_index == 0 && cont == 0){
			printf("no value found for first pivot\n");
			return;
		}else if(i_index == 0){
			printf("not bounded problem\n");
			return;
		}
		for(int j = 1; j < max_restriction; j++){
			if(matrix[i_index][j].value > 0){
				if(actual_j_value < 0){//first value
					actual_j_value = matrix[max_variable_amount][j].value/matrix[i_index][j].value;
					j_index = j;
				}else{
					long double new_value = matrix[max_variable_amount][j].value/matrix[i_index][j].value;
					if(new_value <= actual_j_value){
						actual_j_value = new_value;
						j_index = j;
					}
				}
			}
		}
		//===========solve===============
		long double divisor_value = matrix[i_index][j_index].value;
		for(int i = 1; i < max_variable_amount; i++){//canonizing row

			long double new_value = matrix[i][j_index].value/divisor_value; 
			matrix[i][j_index].value = new_value;
		}

		for(int j = 0; j < max_restriction; j++){//canonizing coluumns
			if(j == j_index){
				continue;
			}
			long double actual_mult = matrix[i_index][j].value *-1;

			for(int i = 1; i < max_variable_amount; i++){
				long double new_value = matrix[i][j].value + (matrix[i][j_index].value * actual_mult);
				matrix[i][j].value = new_value;
			}
		}
		//==========check if done========
		stop = true;
		if(maximize){
			for(int i = 1; i < max_variable_amount; i++){
				if(matrix[i][0].value < 0){
					stop = false;
				}
			}
		}else{
			for(int i = 1; i < max_variable_amount; i++){
				if(matrix[i][0].value > 0){
					stop = false;
				}
			}
		}
		cont++;
		/*//prints the matrix in every step
		for(int j = 0; j < max_restriction; j++){//print the final table
	      printf("%d\t",j);
		      for(int i = 0; i < max_variable_amount; i++){
		         printf("%d,%.4Lf\t", matrix[i][j].tag, matrix[i][j].value);
		      }
	      printf("\n");
   		}
   		printf("\n");
   		*/
	}
	if(cont > max_tries){
		printf("Solver got stuck in a loop. No solution found.");
		return;
	}

	//check if multiple solutions
	int mult_i = 0;
	for (int i = 1; i < max_variable_amount-1; i++){
		if(matrix[i][0].value == 0){
			for (int j = 1; j < max_restriction; ++j){
				if(matrix[i][j].value > 0 && matrix[i][j].value != 1){
					mult_i = i;
				}
			}
		}
	}
	if(mult_i != 0 && cont >0){
		printf("Solver found another solution.\n");

		long double solution_1[(max_tries/3)-1];
		long double solution_2[(max_tries/3)-1];

		//                                        get the first solution
		for(int i = 1; i < (max_tries/3); i++){
			bool found_value = false;
			for(int j = 1; j < max_restriction; j++){
		    	if(matrix[i][j].value == 1 && matrix[i][0].value == 0){
		    		solution_1[i-1] = matrix[max_variable_amount][j].value;
		    		found_value = true;
		    	}
		    	if(!found_value){
		    		solution_1[i-1] = 0;
		    	}
   			}
		}

		for(int j = 0; j < max_restriction; j++){//print the final table
	    	printf("%d\t",j);
		      	for(int i = 0; i < max_variable_amount; i++){
		         	printf("%d,%.4Lf\t", matrix[i][j].tag, matrix[i][j].value);
		      	}
	      	printf("\n");
   		}
   		printf("\n");

   		int mult_j = 0;
   		long double act_mult_j = -1;
   		for(int j = 1; j < max_restriction; j++){
			if(matrix[mult_i][j].value > 0){
				if(act_mult_j < 0){//first value
					act_mult_j = matrix[max_variable_amount][j].value/matrix[mult_i][j].value;
					mult_j = j;
				}else{
					long double new_value = matrix[max_variable_amount][j].value/matrix[mult_i][j].value;
					if(new_value <= act_mult_j){
						act_mult_j = new_value;
						mult_j = j;
					}
				}
			}
		}
		//===========solve===============
		long double divisor_value = matrix[mult_i][mult_j].value;
		for(int i = 1; i < max_variable_amount; i++){//canonizing row

			long double new_value = matrix[i][mult_j].value/divisor_value; 
			matrix[i][mult_j].value = new_value;
		}

		for(int j = 0; j < max_restriction; j++){//canonizing coluumns
			if(j == mult_j){
				continue;
			}
			long double actual_mult = matrix[mult_i][j].value *-1;

			for(int i = 1; i < max_variable_amount; i++){
				long double new_value = matrix[i][j].value + (matrix[i][mult_j].value * actual_mult);
				matrix[i][j].value = new_value;
			}
		}
		//                                        get the second solution
		for(int i = 1; i < (max_tries/3); i++){
			bool found_value = false;
			for(int j = 1; j < max_restriction; j++){
		    	if(matrix[i][j].value == 1 && matrix[i][0].value == 0){
		    		solution_2[i-1] = matrix[max_variable_amount][j].value;
		    		found_value = true;
		    	}
		    	if(!found_value){
		    		solution_2[i-1] = 0;
		    	}
   			}
		}
		printf("another solutions:\n");
		for(int i = 0; i < 3; i++){
			long double a = ((random()%98)+1)/100;
			printf("%d solution: ", i+1);
			for(int j = 0; j < (max_tries/3)-1; j++){
				long double solution = solution_1[j]*a + solution_2[j]*(1-a);
				printf("x%d: %Lf\t", j+1, solution);
			}
			printf("\n");
		}
	}
}
