#ifndef STRUCTS_H
#define STRUCTS_H

#define EXPERIMENT_BASE_SIZE 10

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct{
   bool use;
   int tag;
   int cost;
   int value;
} item;

typedef struct{
   bool use;
   int max_value;
} matrix_value;

typedef struct time_reports {
   float dynamic_programming[EXPERIMENT_BASE_SIZE][EXPERIMENT_BASE_SIZE];
   float basic_greedy[EXPERIMENT_BASE_SIZE][EXPERIMENT_BASE_SIZE];
   float proportional_greedy[EXPERIMENT_BASE_SIZE][EXPERIMENT_BASE_SIZE];
} time_reports;

typedef struct optimal_values_reports {
   int dynamic_programming[EXPERIMENT_BASE_SIZE][EXPERIMENT_BASE_SIZE];
   int basic_greedy[EXPERIMENT_BASE_SIZE][EXPERIMENT_BASE_SIZE];
   int proportional_greedy[EXPERIMENT_BASE_SIZE][EXPERIMENT_BASE_SIZE];
} optimal_values_reports;

typedef struct collisions_reports {
   float basic_greedy[EXPERIMENT_BASE_SIZE][EXPERIMENT_BASE_SIZE];
   float proportional_greedy[EXPERIMENT_BASE_SIZE][EXPERIMENT_BASE_SIZE];
} collisions_reports;

void create_items(const int max_cost, const int max_value, const int item_amount, item list[item_amount]);
void reset_items_use(const int item_amount, item list[item_amount]);
void clean_time_report(time_reports* times);
void clean_optimal_values_reports(optimal_values_reports* optimal_values);
void clean_collisions_reports(collisions_reports* collisions);
void convert_to_averages(const int iterations, time_reports* times);
void convert_to_percentages(const int iterations, collisions_reports* collisions);
int get_value_sum_from_used_items(const int item_amount, item list[item_amount]);

#endif