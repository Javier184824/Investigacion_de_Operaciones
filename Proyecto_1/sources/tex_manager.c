#include <stdio.h>
#include <stdlib.h>
#include "../headers/structs.h"
#include "../headers/tex_manager.h"

FILE* open_file(const char* file_path) {
    FILE* fptr = fopen(file_path, "w");

    if (fptr == NULL) {
        printf("Could not open %s\n", file_path);
        exit(EXIT_FAILURE);
    }

    return fptr;
}

void write_original_problem(const char* problem_name, const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]) {
    FILE* fptr = open_file("tex_files/original_problem.tex");

    // Write the problem description
    fprintf(fptr, 
        "\\section*{Description}\n"
        "\\begin{itemize}\n"
        "   \\item Problem Name: %s\n"
        "   \\item Amount of decision variables: %d\n"
        "   \\item Amount of restrictions: %d\n"
        "   \\item Total Amount of variables: %d\n"
        "\\end{itemize}\n"
        , problem_name
        , desicion_v
        , max_restriction
        , max_variable_amount
    );

    // Write the Target Function
    fprintf(fptr,
        "\\section*{Equations}\n"
        "\\small\n"
        "\\begin{align*}\n"
        "   Max \\ Z = "
    );
    for (int i = 1; i < desicion_v - 1; i++) {
        fprintf(fptr, "%Lfx_%d + ", matrix[i][0].value, matrix[i][0].tag);
    }
    fprintf(fptr, "%Lfx_%d \\\\ \n", matrix[desicion_v - 1][0].value, matrix[desicion_v - 1][0].tag);

    // Write the Restrictions
    fprintf(fptr,
        "   such \\ that: \\\\ \n"
    );
    for (int j = 1; j < max_restriction; j++) {
        fprintf(fptr, "\t");
        for (int i = 1; i < desicion_v - 1; i++) {
            fprintf(fptr, "%Lfx_%d + ", matrix[i][j].value, matrix[i][j].tag);
        }
        fprintf(fptr, "%Lfx_%d \\geq %Lf\\\\ \n", matrix[desicion_v - 1][j].value, matrix[desicion_v - 1][j].tag, matrix[max_variable_amount - 1][j].value);
    }

    fprintf(fptr, 
        "\\end{align*}\n"
        "\n\n"
    );

    fclose(fptr);
}

void write_initial_table(const char* problem_name, const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]) {
    FILE* fptr = open_file("tex_files/initial_table.tex");

    fprintf(fptr, 
        "\\section*{Initial Table}\n"
        "\\begin{table}[H]\n"
        "\\centering\n"
    );

    fprintf(fptr, "\\begin{tabular}{|");
    for (int j = 1; j < max_variable_amount; j++) {
        fprintf(fptr, "c|");
    }
    fprintf(fptr, 
        "c|}\n"
        "\\hline\n"
        "Z &"
    );
    for (int i = 1; i < desicion_v; i++) {
        fprintf(fptr, " $x_%d$ &", matrix[i][0].tag);
    }
    for (int i = desicion_v; i < max_variable_amount - 1; i++) {
        fprintf(fptr, " $s_%d$ &", matrix[i][0].tag);
    }
    fprintf(fptr, " $s_%d$ \\\\ \n\\hline\n", matrix[max_variable_amount - 1][0].tag);

    for(int j = 0; j < max_restriction; j++){
        for(int i = 0; i < max_variable_amount - 1; i++){
            fprintf(fptr, "%.4Lf & ",  matrix[i][j].value);
        }
        fprintf(fptr, "%.4Lf \\\\ \n\\hline\n", matrix[max_variable_amount - 1][j].value);
   }


    fprintf(fptr,
        "\\end{tabular}\n"
        "\\end{table}\n"
    );

    fclose(fptr);
}