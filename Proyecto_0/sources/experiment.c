#include "../headers/experiment.h"
#include "../headers/dynamic_programing.h"
#include "../headers/greedy_approach.h"
#include "../headers/tex_generator.h"
#include "../headers/structs.h"

float timedifference_msec(struct timeval t0, struct timeval t1){
   return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

void do_experiment(const int iterations, time_reports* times, optimal_values_reports* optimal_values, collisions_reports* collisions) {
   struct timeval t0, t1;
   float elapsed;
   int item_amount, bag_size;
   for (int k = 0; k < iterations; k++) {
      for (int i = 0; i < EXPERIMENT_BASE_SIZE; i++) {
         item_amount = 10*(i + 1);
         for (int j = 0; j < EXPERIMENT_BASE_SIZE; j++) {//initialize the listsa and matrix
            bag_size = 100*(j + 1);
            item list[item_amount];
            matrix_value table[item_amount][bag_size];

            create_items((int) (bag_size*0.4), 100, item_amount, list);
            
            gettimeofday(&t0, 0);
            solve_with_dynamic_programming(item_amount, bag_size, list, table);
            gettimeofday(&t1, 0);
            times->dynamic_programming[i][j] += timedifference_msec(t0, t1);
            optimal_values->dynamic_programming[i][j] = get_value_sum_from_used_items(item_amount, list);

            reset_items_use(item_amount, list);

            gettimeofday(&t0, 0);
            solve_with_basic_greedy(item_amount, bag_size, list);
            gettimeofday(&t1, 0);
            times->basic_greedy[i][j] += timedifference_msec(t0, t1);
            optimal_values->basic_greedy[i][j] = get_value_sum_from_used_items(item_amount, list);

            reset_items_use(item_amount, list);

            gettimeofday(&t0, 0);
            solve_with_proportional_greedy(item_amount, bag_size, list);
            gettimeofday(&t1, 0);
            times->proportional_greedy[i][j] += timedifference_msec(t0, t1);
            optimal_values->proportional_greedy[i][j] = get_value_sum_from_used_items(item_amount, list);

            if (optimal_values->dynamic_programming[i][j] == optimal_values->basic_greedy[i][j]) {
               collisions->basic_greedy[i][j] += 1.0f;
            }

            if (optimal_values->dynamic_programming[i][j] == optimal_values->proportional_greedy[i][j]) {
               collisions->proportional_greedy[i][j] += 1.0f;
            }
         }
      }
   }
   convert_to_averages(iterations, times);
   convert_to_percentages(iterations, collisions);
}
