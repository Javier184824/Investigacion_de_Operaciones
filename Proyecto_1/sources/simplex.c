#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "../headers/structs.h"
#include "../headers/simplex.h"
#include "../headers/tex_manager.h"

void find_pivot(const int max_variable_amount, const int max_restriction, const bool maximize, int* i_indexF, int* j_indexF, matrix_value matrix[max_variable_amount][max_restriction]){

	long double actual_i_value = 1;
	long double actual_j_value = -1;

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
						*i_indexF = i;
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
						*i_indexF = i;
						}
					}
				}
			}
		}
		
		if(*i_indexF == 0){
			return;
		}
		
		for(int j = 1; j < max_restriction; j++){
			if(matrix[*i_indexF][j].value > 0){
				if(actual_j_value < 0){//first value
					actual_j_value = matrix[max_variable_amount][j].value/matrix[*i_indexF][j].value;
					*j_indexF = j;
				}else{
					long double new_value = matrix[max_variable_amount][j].value/matrix[*i_indexF][j].value;
					if(new_value <= actual_j_value){
						actual_j_value = new_value;
						*j_indexF = j;
					}
				}
			}
		}

}

void canonize(const int max_variable_amount, const int max_restriction, const int i_indexF, const int j_indexF, matrix_value matrix[max_variable_amount][max_restriction]){
	long double divisor_value = matrix[i_indexF][j_indexF].value;

	for(int i = 1; i < max_variable_amount; i++){//canonizing row

		long double new_value = matrix[i][j_indexF].value/divisor_value; 
		matrix[i][j_indexF].value = new_value;
	}

	for(int j = 0; j < max_restriction; j++){//canonizing coluumns
		if(j == j_indexF){
			continue;
		}
		long double actual_mult = matrix[i_indexF][j].value *-1;

		for(int i = 1; i < max_variable_amount; i++){
			long double new_value = matrix[i][j].value + (matrix[i][j_indexF].value * actual_mult);
			matrix[i][j].value = new_value;
		}
	}

}

void solve_simplex(const int max_variable_amount, const int desicion_v, const int max_restriction, const int max_tries, const bool maximize, matrix_value matrix[max_variable_amount][max_restriction]){
	FILE* fptr_intermediate = open_file("tex_files/intermediate_tables.tex");
	FILE* fptr_solution = open_file("tex_files/solution.tex");
	
	bool stop = false;//3n, n = desicion_v
	int cont = 0;

	while(!stop && cont <= max_tries){
		int i_index = 0;
		int j_index = 0;

		fprintf(fptr_intermediate, "\\section*{Iteration %d}\n", cont + 1);

		//========find pivot=============

		find_pivot(max_variable_amount, max_restriction, maximize, &i_index, &j_index, matrix);

		if(i_index == 0 && cont == 0){
			fprintf(fptr_intermediate, "No value found for first pivot\n");
			fclose(fptr_intermediate);
			fclose(fptr_solution);
			return;
		}else if(i_index == 0){
			fprintf(fptr_intermediate, "Unbounded problem\n");
			fprintf(fptr_solution, "Unbounded problem\n");
			fclose(fptr_intermediate);
			fclose(fptr_solution);
			return;
		}
		
		fprintf(fptr_intermediate, "\\subsection*{Finding the Pivot}\n");
		write_table_with_pivot(fptr_intermediate, i_index, j_index, desicion_v, max_variable_amount, max_restriction, matrix);
		
		//===========solve===============
		
		canonize(max_variable_amount, max_restriction, i_index, j_index, matrix);

		fprintf(fptr_intermediate, "\\subsection*{Canonize the Pivot column with Pivot value}\n");
		write_table(fptr_intermediate, desicion_v, max_variable_amount, max_restriction, matrix);
		fprintf(fptr_intermediate, "\\newpage\n");

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
	}

	if(cont > max_tries){
		fprintf(fptr_solution, "Solver got stuck in a loop. No solution found.");
		fclose(fptr_intermediate);
		fclose(fptr_solution);
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

		int actual_variables = max_tries/3;

		long double solution_1[actual_variables-1];
		long double solution_2[actual_variables-1];

		//                                        get the first solution
		for(int i = 1; i < actual_variables; i++){
			bool found_value = false;
			for(int j = 1; j < max_restriction; j++){
		    	if(matrix[i][j].value == 1 && matrix[i][0].value == 0){
		    		solution_1[i-1] = matrix[max_variable_amount-1][j].value;
		    		found_value = true;
		    	}
		    	if(!found_value){
		    		solution_1[i-1] = 0;
		    	}
   			}
		}

		#if 0
		for(int j = 0; j < max_restriction; j++){//print the final table
	    	printf("%d\t",j);
		      	for(int i = 0; i < max_variable_amount; i++){
		         	printf("%d,%.4Lf\t", matrix[i][j].tag, matrix[i][j].value);
		      	}
	      	printf("\n");
   		}
   		printf("\n");
		#endif

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

		canonize(max_variable_amount, max_restriction, mult_i, mult_j, matrix);

		//                                        get the second solution
		for(int i = 1; i < actual_variables; i++){
			bool found_value = false;
			for(int j = 1; j < max_restriction; j++){
		    	if(matrix[i][j].value == 1 && matrix[i][0].value == 0){
		    		solution_2[i-1] = matrix[max_variable_amount-1][j].value;
		    		found_value = true;
		    	}
		    	if(!found_value){
		    		solution_2[i-1] = 0;
		    	}
   			}
		}

		#if 0
		printf("another solutions:\n");
		for(int i = 0; i < 3; i++){
			long double a = rand() % 99 +1;
			a = a/100;
			printf("%d solution: ", i+1);
			for(int j = 0; j < actual_variables-1; j++){
				long double solution = solution_1[j]*a + solution_2[j]*(1-a);
				printf("x%d: %.4Lf\t", j+1, solution);
			}
			printf("\n");
		}
		#endif
	}

	fclose(fptr_intermediate);
	fclose(fptr_solution);
}
