#include "../headers/structs.h"
#include "../headers/dynamic_programing.h"

int get_optimal_value_dynamic_programming(const int item_amount, const int bag_size, matrix_value table[item_amount][bag_size]) {
   return table[item_amount - 1][bag_size - 1].max_value;
}

void get_final_value(const int item_amount, const int bag_size, item list[item_amount], matrix_value table[item_amount][bag_size]) {
   int residual_cost = bag_size - 1;
   int total_cost = 0;
   for(int i = item_amount - 1; i >= 0; i--){
      if(table[i][residual_cost].use){
         list[i].use = table[i][residual_cost].use;
         residual_cost -= list[i].cost;
         total_cost += list[i].cost;
      }else{
         list[i].use = table[i][residual_cost].use;
      }
   }
}

void solve_with_dynamic_programming(const int item_amount, const int bag_size, item list[item_amount], matrix_value table[item_amount][bag_size]){
   for(int i = 0; i < item_amount; i++){
      for(int j = 0; j < bag_size; j++){
         if(i != 0){ // not the first item
            if(list[i].cost <= j){
               int value = list[i].value + table[i-1][j-list[i].cost].max_value;
               bool use;
               if (value > table[i-1][j].max_value){// check if value is better than previous value
                  use = true;
               }
               else {
                  use = false;
                  value = table[i-1][j].max_value;
               }
               matrix_value table_value = {use, value};
               table[i][j] = table_value;
            }
            else{
               bool use = false;
               int value = table[i-1][j].max_value;
               matrix_value table_value = {use, value};
               table[i][j] = table_value;
            }

         }
         else { // first item
            if(list[i].cost <= j){
               bool use = true;
               int value = list[i].value;
               matrix_value table_value = {use, value};
               table[i][j] = table_value;
            }
            else {
               bool use = false;
               int value = 0;
               matrix_value table_value = {use, value};
               table[i][j] = table_value;
            }
         }
      }
   }
   get_final_value(item_amount, bag_size, list, table);
}