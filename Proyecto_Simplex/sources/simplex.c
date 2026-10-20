#include "../headers/structs.h"
#include "../headers/simplex.h"

void solve_simplex(const int max_variable_amount, const int max_restriction, const int max_tries, matrix_value matrix[max_variable_amount][max_restriction]){
	bool stop = false;//3n, n = desicion_v
	int cont = 0;
	while(!stop && cont <= max_tries){
		int i_index = 0;
		int j_index = 0;
		float actual_i_value = 1;
		float actual_j_value = -1;

		//========find pivot=============

		for(int i = 1; i < max_variable_amount; i++){
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
		if(i_index == 0 && cont == 0){
			printf("no value found for first pivot\n");
			break;
		}else if(i_index == 0){
			printf("not bounded problem\n");
			break;
		}
		for(int j = 1; j < max_restriction; j++){
			if(matrix[i_index][j].value > 0){
				if(actual_j_value < 0){//first value
					actual_j_value = matrix[max_variable_amount][j].value/matrix[i_index][j].value;
					j_index = j;
				}else{
					float new_value = matrix[max_variable_amount][j].value/matrix[i_index][j].value;
					if(new_value <= actual_j_value){
						actual_j_value = new_value;
						j_index = j;
					}
				}
			}
		}
		//===========solve===============
		int divisor_value = matrix[i_index][j_index].value;
		for(int i = 1; i < max_variable_amount; i++){//canonizing row
			float new_value = matrix[i][j_index].value/divisor_value; 
			matrix[i][j_index].value = ((int)(new_value * 1000 + 0.5)) / 1000;
		}

		for(int j = 0; j < max_restriction; j++){//canonizing coluumns
			if(j == j_index){
				continue;
			}
			int actual_mult = matrix[i_index][j].value *-1;

			for(int i = 1; i < max_variable_amount; i++){
				float new_value = matrix[i][j].value + (matrix[i][j_index].value * actual_mult);
				matrix[i][j].value = ((int)(new_value * 1000 + 0.5)) / 1000;
			}
		}
		//==========check if done========
		stop = true;
		for(int i = 1; i < max_variable_amount; i++){
			if(matrix[i][0].value < 0){
				stop = false;
			}
		}
		cont++;
		for(int j = 0; j < max_restriction; j++){//print the final table
	      printf("%d\t",j);
		      for(int i = 0; i < max_variable_amount; i++){
		         printf("%d,%f\t", matrix[i][j].tag, matrix[i][j].value);
		      }
	      printf("\n");
   		}
   		printf("\n");
	}
}
