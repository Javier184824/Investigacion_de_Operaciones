#include "../headers/structs.h"
#include "../headers/dynamic_programing.h"
#include "../headers/greedy_approach.h"
#include "../headers/experiment.h"
#include "../headers/tex_generator.h"

void write_beginning(FILE* fptr) {
    fprintf(fptr, 
      "\\documentclass{report}\n"
      "\\input{texs/packages}\n"
      "\\begin{document}\n"
      "\\input{texs/frontpage}\n"
      "\n"
    );
}

void write_end(FILE* fptr) {
    fprintf(fptr, 
        "\\end{document}"
    );
}

// ===== Functions for writing the equations ======

void write_beginning_equations(FILE* fptr, const int item_amount, const int bag_size, item list[item_amount]) {
   fprintf(fptr, 
      "\\begin{align*}\n"
   );
   fprintf(fptr,
      "Max \\ Z &= "
   );
   for (int i = 0; i < item_amount - 1; i++) {
      fprintf(fptr, "%dx_%d + ", list[i].value, i + 1);
   }
   fprintf(fptr, "%dx_%d \\\\ \n", list[item_amount - 1].value, item_amount);
   fprintf(fptr,
      "such \\ that &: \\ "
   );
   for (int i = 0; i < item_amount - 1; i++) {
      fprintf(fptr, "%dx_%d + ", list[i].cost, i + 1);
   }
   fprintf(fptr, "%dx_%d \\leq %d \\\\ \n & ", list[item_amount - 1].cost, item_amount, bag_size - 1);
   for (int i = 0; i < item_amount - 1; i++) {
      fprintf(fptr, "x_%d, ", i + 1);
   }
   fprintf(fptr, "x_%d \\in \\{0, 1\\}\n", item_amount);
   fprintf(fptr, 
      "\\end{align*}\n\n"
   );
}

void write_table_beginning(FILE* fptr) {
   fprintf(fptr,
      "\\begin{center}\n"
      "\\begin{tabular}{|p{1.5cm}|c|c|c|c|c|c|}\n"
      "\\hline\n"
      "& \\multicolumn{6}{c|}{Items} \\\\\n"
      "\\hline\n"
   );
}

void write_table_ending(FILE* fptr) {
   fprintf(fptr, 
      "\\end{tabular}\n"
      "\\end{center}\n\n"
   );
}

void write_items_row(FILE* fptr, const int item_amount, item list[item_amount]) {
   fprintf(fptr, 
      "Knapsack & \n"
   );
   for (int i = 0; i < item_amount - 1; i++) {
      fprintf(fptr, "$x_%d$ & ", list[i].tag);
   }
   fprintf(fptr, "$x_%d$ \\\\ \n", list[item_amount - 1].tag);
   fprintf(fptr, "\\hline\n");
}

void write_costs_row(FILE* fptr, const int item_amount, item list[item_amount]) {
   fprintf(fptr, "Cost & ");
   for(int i = 0; i < item_amount - 1; i++) {
      if (list[i].use) {
         fprintf(fptr, "\\cellcolor{green}%d & ", list[i].cost);
      }
      else {
         fprintf(fptr, "\\cellcolor{red}%d & ", list[i].cost);
      }
   }
   if (list[item_amount - 1].use) {
      fprintf(fptr, "\\cellcolor{green}%d \\\\ \n", list[item_amount - 1].cost);
   }
   else {
      fprintf(fptr, "\\cellcolor{red}%d \\\\ \n", list[item_amount - 1].cost);
   }
   fprintf(fptr, "\\hline\n");
}

void write_values_row(FILE* fptr, const int item_amount, item list[item_amount]) {
   fprintf(fptr, "Value &");
   for(int i = 0; i < item_amount - 1; i++) {
      fprintf(fptr, "%d & ", list[i].value);
   }
   fprintf(fptr, "%d \\\\ \n", list[item_amount - 1].value);
   fprintf(fptr, "\\hline\n");
}

void write_proportions_row(FILE* fptr, const int item_amount, item list[item_amount]) {
   fprintf(fptr, "Proportion &");
   for(int i = 0; i < item_amount - 1; i++) {
      fprintf(fptr, "%f & ", (float) (list[i].value) / (float) (list[i].cost));
   }
   fprintf(fptr, "%f \\\\ \n", (float) (list[item_amount - 1].value) / (float) (list[item_amount - 1].cost));
   fprintf(fptr, "\\hline\n");
}

void write_basic_greedy_table(FILE* fptr, const int item_amount, const int bag_size, item list[item_amount]) {
   write_table_beginning(fptr);
   write_items_row(fptr, item_amount, list);
   write_costs_row(fptr, item_amount, list);
   write_values_row(fptr, item_amount, list);
   write_table_ending(fptr);
}

void write_proportional_greedy_table(FILE* fptr, const int item_amount, const int bag_size, item list[item_amount]) {
   write_table_beginning(fptr);
   write_items_row(fptr, item_amount, list);
   write_costs_row(fptr, item_amount, list);
   write_values_row(fptr, item_amount, list);
   write_proportions_row(fptr, item_amount, list);
   write_table_ending(fptr);
}

void write_max_values(FILE* fptr, const int item_amount, const int bag_size, item list[item_amount], matrix_value table[item_amount][bag_size]) {
   for(int i = 0; i < bag_size; i++) {
      fprintf(fptr, "%d & ", i);
      for (int j = 0; j < item_amount - 1; j++) {
         if (table[j][i].use) {
            fprintf(fptr, "\\cellcolor{green}%d & ", table[j][i].max_value);
         }
         else {
            fprintf(fptr, "\\cellcolor{red}%d & ", table[j][i].max_value);
         }
      }
      if (table[item_amount - 1][i].use) {
         fprintf(fptr, "\\cellcolor{green}%d \\\\\n\\hline\n", table[item_amount - 1][i].max_value);
      }
      else {
         fprintf(fptr, "\\cellcolor{red}%d \\\\\n\\hline\n", table[item_amount - 1][i].max_value);
      }
   }
}

void write_dynamic_programming_table(FILE* fptr, const int item_amount, const int bag_size, item list[item_amount], matrix_value table[item_amount][bag_size]) {
   write_table_beginning(fptr);
   write_items_row(fptr, item_amount, list);
   write_max_values(fptr, item_amount, bag_size, list, table);
   write_table_ending(fptr);
}

void write_time_table(FILE* fptr, float times[EXPERIMENT_BASE_SIZE][EXPERIMENT_BASE_SIZE]) {
   fprintf(fptr, 
      "\\begin{table}[H]\n"
      "\\hspace{-3cm}\n"
      "\\begin{tabular}{|c|c|c|c|c|c|c|c|c|c|c|}\n"
   );
   fprintf(fptr, 
      "\\hline\n"
      "& \\multicolumn{10}{c|}{Amount of items} \\\\\n"
      "\\hline\n"
      "Knapsack & 10 & 20 & 30 & 40 & 50 & 60 & 70 & 80 & 90 & 100 \\\\\n"
   );
   for(int i = 0; i < EXPERIMENT_BASE_SIZE; i++) {
      fprintf(fptr, 
         "\\hline\n"
         "%d & ",
         (i + 1) * 100);
      for (int j = 0; j < EXPERIMENT_BASE_SIZE - 1; j++) {
         fprintf(fptr, "%.4f & ", times[j][i]);
      }
      fprintf(fptr, "%.4f\\\\ \n", times[EXPERIMENT_BASE_SIZE - 1][i]);
   }
   fprintf(fptr, "\\hline\n");
   fprintf(fptr, 
      "\\end{tabular}\n"
      "\\end{table}\n\n"
   );
}

void write_collisions_table(FILE* fptr, float collisions[EXPERIMENT_BASE_SIZE][EXPERIMENT_BASE_SIZE]) {
   fprintf(fptr, 
      "\\begin{table}[H]\n"
      "\\hspace{-3cm}\n"
      "\\begin{tabular}{|c|c|c|c|c|c|c|c|c|c|c|}\n"
   );
   fprintf(fptr, 
      "\\hline\n"
      "& \\multicolumn{10}{c|}{Amount of items} \\\\\n"
      "\\hline\n"
      "Knapsack & 10 & 20 & 30 & 40 & 50 & 60 & 70 & 80 & 90 & 100 \\\\\n"
   );
   for(int i = 0; i < EXPERIMENT_BASE_SIZE; i++) {
      fprintf(fptr, 
         "\\hline\n"
         "%d & ",
         (i + 1) * 100);
      for (int j = 0; j < EXPERIMENT_BASE_SIZE - 1; j++) {
         fprintf(fptr, "%.2f\\%% & ", collisions[j][i]);
      }
      fprintf(fptr, "%.2f\\%% \\\\ \n", collisions[EXPERIMENT_BASE_SIZE - 1][i]);
   }
   fprintf(fptr, "\\hline\n");
   fprintf(fptr, 
      "\\end{tabular}\n"
      "\\end{table}\n\n"
   );
}

void write_solution_equations(FILE* fptr, const int item_amount, const int bag_size, item list[item_amount]) {
   fprintf(fptr,
      "\\begin{align*}\n"
   );
   fprintf(fptr,
      "Z = %d, ",
      get_value_sum_from_used_items(item_amount, list)
   );
   for (int i = 0; i < item_amount - 1; i++) {
      fprintf(fptr, "x_%d = %d, ", list[i].tag, list[i].use);
   }
   fprintf(fptr, "x_%d = %d\n", list[item_amount - 1].tag, list[item_amount - 1].use);

   fprintf(fptr, 
      "\\end{align*}\n\n"
   );
}

void create_demo_tex(void) {
   FILE* fptr = fopen("texs/main.tex", "w");
   item list[ITEM_AMMOUNT_DEMO];
   matrix_value table[ITEM_AMMOUNT_DEMO][BAG_SIZE_DEMO];

   if (fptr == NULL) {
      printf("Error: File does not exist\n");
      exit(1);
   }

   create_items(MAX_COST_DEMO, MAX_VALUE_DEMO, ITEM_AMMOUNT_DEMO, list);

   write_beginning(fptr);

   fprintf(fptr, "\\input{texs/demo_explanation}\n");

   fprintf(fptr, "\\chapter*{Demonstration Results}\n\n");

   fprintf(fptr, "\\section*{Problem generated}\n\n");

   write_beginning_equations(fptr, ITEM_AMMOUNT_DEMO, BAG_SIZE_DEMO, list);

   fprintf(fptr, "\\section*{Dynamic Programming Solution}");

   fprintf(fptr, "This table is generated using dynamic programming, based on the 0/1 Knapsack Problem algorithm covered in class. \n");
   solve_with_dynamic_programming(ITEM_AMMOUNT_DEMO, BAG_SIZE_DEMO, list, table);
   write_dynamic_programming_table(fptr, ITEM_AMMOUNT_DEMO, BAG_SIZE_DEMO, list, table);
   write_solution_equations(fptr, ITEM_AMMOUNT_DEMO, BAG_SIZE_DEMO, list);

   reset_items_use(ITEM_AMMOUNT_DEMO, list);
	
   fprintf(fptr, "\\chapter*{Greedy Algorithm}\n\n");

   fprintf(fptr, "\\section*{Basic Greedy Solution}");

   fprintf(fptr, "This is the table for the Basic Greedy Algorithm, in which the most valuable item that fits into the remaining space in the knapsack is selected. \n");
   solve_with_basic_greedy(ITEM_AMMOUNT_DEMO, BAG_SIZE_DEMO, list);
   write_basic_greedy_table(fptr, ITEM_AMMOUNT_DEMO, BAG_SIZE_DEMO, list);
   write_solution_equations(fptr, ITEM_AMMOUNT_DEMO, BAG_SIZE_DEMO, list);

   reset_items_use(ITEM_AMMOUNT_DEMO, list);

   fprintf(fptr, "\\section*{Proportional Greedy Solution}");
	
   fprintf(fptr, "This table employs the Proportional Greedy Algorithm, in which the proportion of each item is calculated as its value divided by the capacity it consumes."); 
   fprintf(fptr, "Then, the item with the highest proportion that fits within the remaining knapsack capacity is selected at each step. \n");
   solve_with_proportional_greedy(ITEM_AMMOUNT_DEMO, BAG_SIZE_DEMO, list);
   write_proportional_greedy_table(fptr, ITEM_AMMOUNT_DEMO, BAG_SIZE_DEMO, list);
   write_solution_equations(fptr, ITEM_AMMOUNT_DEMO, BAG_SIZE_DEMO, list);

   write_end(fptr);
   fclose(fptr);
}

void create_experiment_tex(const int iterations) {
   FILE* fptr = fopen("texs/main.tex", "w");
   if (fptr == NULL) {
      printf("Error: File does not exist\n");
      exit(1);
   }

   time_reports times;
   optimal_values_reports optimal_values;
   collisions_reports collisions;

   clean_time_report(&times);
   clean_optimal_values_reports(&optimal_values);
   clean_collisions_reports(&collisions);

   do_experiment(iterations, &times, &optimal_values, &collisions);

   write_beginning(fptr);

   fprintf(fptr, "\\input{texs/exp_explanation}\n\n");

   fprintf(fptr, "\\chapter*{Experiment Results}\n\n");

   fprintf(fptr, "\\section*{Average execution times in seconds}\n\n");

   fprintf(fptr, "\\subsection*{Dynamic Programming}\n\n");

   fprintf(fptr, "This is the table of times used by the Dynamic Programming Algorithm. \n");
   write_time_table(fptr, times.dynamic_programming);

   fprintf(fptr, "\\newpage\n");

   fprintf(fptr, "\\subsection*{Basic Greedy}\n\n");
   
   fprintf(fptr, "This is the table of times used by the Basic Greedy Algorithm. \n");
   write_time_table(fptr, times.basic_greedy);

   fprintf(fptr, "\\subsection*{Proportional Greedy}\n\n");
   
   fprintf(fptr, "This is the table of times used by the Proportional Greedy Algorithm. \n");
   write_time_table(fptr, times.proportional_greedy);

   fprintf(fptr, "\\input{texs/times_conclusions}\n\n");

   fprintf(fptr, "\\section*{Collisions percentage}\n\n");

   fprintf(fptr, "\\subsection*{Dynamic Programming}\n\n");
   fprintf(fptr, "There is no need to create a table for this algorithm. This algorithms is used to give us the optimal solutions for each problem in the table.\n");

   fprintf(fptr, "\\subsection*{Basic Greedy}\n\n");
   
   fprintf(fptr, " This table shows the percentage of collisions of the Basic Greedy Algorithm compared to the optimal solution.\n");
   write_collisions_table(fptr, collisions.basic_greedy);

   fprintf(fptr, "\\subsection*{Proportional Greedy}\n\n");
   
   fprintf(fptr, "This table shows the percentage of collisions of the Proportional Greedy Algorithm compared to the optimal solution.\n");
   write_collisions_table(fptr, collisions.proportional_greedy);

   fprintf(fptr, "\\input{texs/collisions_conclusions}\n\n");

   write_end(fptr);
   fclose(fptr);
}

int main(int argc, char *argv[]) {
   srand(time(NULL));
   if (argc != 2) {
      fprintf(stderr, "Usage: %s -X | -E=n\n", argv[0]);
      return EXIT_FAILURE;
   }

   if (strcmp(argv[1], "-X") == 0) {
      printf("Option -X selected\n");

      create_demo_tex();
      system("pdflatex -output-directory=others texs/main.tex > /dev/null");
      system("evince --presentation ../Proyecto_0/others/main.pdf");

      return EXIT_SUCCESS;
   }

   if (strncmp(argv[1], "-E=", 3) == 0) {
      char *endptr;
      long n = strtol(argv[1] + 3, &endptr, 10);

      if (argv[1][3] == '\0') {
         fprintf(stderr, "Error: -E requires an integer\n");
         return EXIT_FAILURE;
      }

      if (*endptr != '\0') {
         fprintf(stderr, "Error: '%s' is not a valid integer\n",
                  argv[1] + 3);
         return EXIT_FAILURE;
      }
      printf("Option -E selected with n = %ld\n", n);

      create_experiment_tex(n);
      system("pdflatex -output-directory=others texs/main.tex > /dev/null");
      system("evince --presentation ../Proyecto_0/others/main.pdf");

      return EXIT_SUCCESS;
    }

    fprintf(stderr, "Error: unknown option '%s'\n", argv[1]);
    fprintf(stderr, "Usage: %s -X | -E=n\n", argv[0]);

    return EXIT_FAILURE;
}
