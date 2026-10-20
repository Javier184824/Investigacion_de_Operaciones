#include "../headers/structs.h"

void create_problem(const int max_variable_value,const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]){
	int slack = 0;
	for (int j = 0; j < max_restriction; ++j){//max_variable_amount + 2, max_restriction +1
		for (int i = 0; i < max_variable_amount; ++i){
			if(j == 0){//Z column aka first colunm
				if(i == 0){//first item of Z
					matrix_value f_value = {i, 1};

					matrix[i][j] = f_value;
				}else if(i < (max_variable_amount - max_restriction)){ // middle columns of Z
					int min = max_variable_value * -1;

					int value = rand() % (max_variable_value - min + 1) + min;

					matrix_value f_value = {i, value};

					matrix[i][j] = f_value;
					
				}else{//last columns of Z
					matrix_value f_value = {i, 0};

					matrix[i][j] = f_value;

				}
			}else {
				if(i == 0){//first item of restriction
					matrix_value f_value = {i, 0};

					matrix[i][j] = f_value;
				}else if(i < (max_variable_amount - max_restriction)){ // middle columns of restriction
					int min = max_variable_value * -1;

					int value = rand() % (max_variable_value - min + 1) + min;

					matrix_value f_value = {i, value};

					matrix[i][j] = f_value;
				}else if(i == max_variable_amount-1){ // last column
					int min = max_variable_value * -1;

					int value = (rand() % max_variable_value) +1;

					matrix_value f_value = {i, value};

					matrix[i][j] = f_value;
				}else{//last columns of restriction - last
					/*int check = desicion_v+1+slack -i;
					printf("s- %d, c- %d\n",slack, check);*/
					if(i == (desicion_v+1+slack) && slack == (j-1)){//sets the first BFS
						matrix_value f_value = {i, 1};

						matrix[i][j] = f_value;
						++slack;
					}else{
						matrix_value f_value = {i, 0};

						matrix[i][j] = f_value;
					}
				}
			}
			
		}
	}
}