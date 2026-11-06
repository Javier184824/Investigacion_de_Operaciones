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

void write_table(
    FILE* fptr, 
    const int desicion_v, 
    const int max_variable_amount, 
    const int max_restriction,
    matrix_value matrix[max_variable_amount][max_restriction]
) {
    fprintf(fptr,
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
        "\\cellcolor{red} $Z$ &"
    );
    for (int i = 1; i < desicion_v; i++) {
        fprintf(fptr, " \\cellcolor{orange} $x_%d$ &", matrix[i][0].tag);
    }
    for (int i = desicion_v; i < max_variable_amount - 1; i++) {
        fprintf(fptr, " \\cellcolor{yellow} $s_%d$ &", i - desicion_v + 1);
    }
    fprintf(fptr, " \\cellcolor{black} \\textcolor{white}{$b$} \\\\ \n\\hline\n");

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
}

void write_table_with_pivot(
    FILE* fptr, 
    const int pivot_row, 
    const int pivot_col, 
    const int desicion_v, 
    const int max_variable_amount, 
    const int max_restriction,
    matrix_value matrix[max_variable_amount][max_restriction]
) {
    fprintf(fptr,
        "\\begin{table}[H]\n"
        "\\centering\n"
    );

    fprintf(fptr, "\\begin{tabular}{|c|");
    for (int j = 1; j < max_variable_amount; j++) {
        fprintf(fptr, "c|");
    }
    fprintf(fptr, 
        "c|}\n"
        "\\hline\n"
        "\\cellcolor{red} $Z$ &"
    );
    for (int i = 1; i < desicion_v; i++) {
        fprintf(fptr, " \\cellcolor{orange} $x_%d$ &", matrix[i][0].tag);
    }
    for (int i = desicion_v; i < max_variable_amount - 1; i++) {
        fprintf(fptr, " \\cellcolor{yellow} $s_%d$ &", i - desicion_v + 1);
    }
    fprintf(fptr, " \\cellcolor{black} \\textcolor{white}{$b$} & $P$ \\\\ \n\\hline\n");

    for(int j = 0; j < max_restriction; j++){
        for(int i = 0; i < max_variable_amount - 1; i++){
            if (i == pivot_row && j == pivot_col) {
                fprintf(fptr, "\\cellcolor{green} %.4Lf & ", matrix[i][j].value);
            } else {
                fprintf(fptr, "%.4Lf & ", matrix[i][j].value);
            }
        }
        if (j > 0) {
            fprintf(fptr, "%.4Lf & ", matrix[max_variable_amount - 1][j].value);
            if (matrix[max_variable_amount - 1][j].value < 0) {
                fprintf(fptr, "- \\\\ \n\\hline\n");
            }
            else if (matrix[pivot_col][j].value <= 0) {
                fprintf(fptr, "- \\\\ \n\\hline\n");
            }
            else {
                fprintf(fptr, "%.4Lf \\\\ \n\\hline\n", matrix[max_variable_amount - 1][j].value / matrix[pivot_col][j].value);
            }
        }
        else {
            fprintf(fptr, "%.4Lf & ", matrix[max_variable_amount - 1][j].value);
            fprintf(fptr, "- \\\\ \n\\hline\n");
        }
    }

    fprintf(fptr,
        "\\end{tabular}\n"
        "\\end{table}\n"
    );
}

void write_original_problem(const int maximizing, const char* problem_name, const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]) {
    FILE* fptr = open_file("tex_files/original_problem.tex");

    // Write the problem description
    fprintf(fptr, 
        "\\section*{Description}\n"
        "\\begin{itemize}\n"
        "\\item Problem Name: %s\n"
        "\\item Amount of decision variables: %d\n"
        "\\item Amount of restrictions: %d\n"
        "\\item Total Amount of variables: %d\n"
        , problem_name
        , desicion_v
        , max_restriction
        , max_variable_amount
    );

    fprintf(fptr, 
        "\\item Type of problem: %s\n"
        "\\end{itemize}\n"
        , maximizing ? "Maximization" : "Minimization"
    );

    // Write the Target Function
    fprintf(fptr,
        "\\section*{Equations}\n"
        "\\begin{adjustwidth}{-5cm}{-2cm}\n"
        "\\begin{align*}\n"
        "%s \\ Z = \\ & "
        , maximizing ? "Max" : "Min"
    );
    if (matrix[1][0].value > 0) {
        fprintf(fptr, "-%Lfx_%d", matrix[1][0].value, matrix[1][0].tag);
    } 
    else {
        fprintf(fptr, "%Lfx_%d", -matrix[1][0].value, matrix[1][0].tag);
    }
    for (int i = 2; i < desicion_v - 1; i++) {
        if (matrix[i][0].value > 0) {
            fprintf(fptr, " - %Lfx_%d", matrix[i][0].value, matrix[i][0].tag);
        } 
        else {
            fprintf(fptr, " + %Lfx_%d", -matrix[i][0].value, matrix[i][0].tag);
        }
    }
    if (matrix[desicion_v - 1][0].value > 0) {
        fprintf(fptr, " + %Lfx_%d \\\\ \n", matrix[desicion_v - 1][0].value, matrix[desicion_v - 1][0].tag);
    }
    else {
        fprintf(fptr, " - %Lfx_%d \\\\ \n", -matrix[desicion_v - 1][0].value, matrix[desicion_v - 1][0].tag);
    }

    // Write the Restrictions
    fprintf(fptr,
        "such \\ that: \\ & \\\\ \n"
    );
    for (int j = 1; j < max_restriction; j++) {
        fprintf(fptr, "& ");
        fprintf(fptr, "%Lfx_%d", matrix[1][j].value, matrix[1][j].tag);
        for (int i = 2; i < desicion_v - 1; i++) {
            if (matrix[i][j].value >= 0) {
                fprintf(fptr, " + %Lfx_%d", matrix[i][j].value, matrix[i][j].tag);
            } 
            else {
                fprintf(fptr, " - %Lfx_%d", -matrix[i][j].value, matrix[i][j].tag);
            }
        }
        if (matrix[desicion_v - 1][j].value >= 0) {
            fprintf(fptr, " + %Lfx_%d \\geq %Lf\\\\ \n", matrix[desicion_v - 1][j].value, matrix[desicion_v - 1][j].tag, matrix[max_variable_amount - 1][j].value);
        } 
        else {
            fprintf(fptr, " - %Lfx_%d \\geq %Lf\\\\ \n", -matrix[desicion_v - 1][j].value, matrix[desicion_v - 1][j].tag, matrix[max_variable_amount - 1][j].value);
        }
    }

    fprintf(fptr, 
        "\\end{align*}\n"
        "\\end{adjustwidth}\n"
        "\n\n"
    );

    fclose(fptr);
}

void write_initial_table(const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]) {
    FILE* fptr = open_file("tex_files/initial_table.tex");
    write_table(fptr, desicion_v, max_variable_amount, max_restriction, matrix);
    fclose(fptr);
}

void write_final_table(const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]) {
    FILE* fptr = open_file("tex_files/final_table.tex");
    write_table(fptr, desicion_v, max_variable_amount, max_restriction, matrix);
    fclose(fptr);
}

void write_solution(const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction]) {
    FILE* fptr = open_file("tex_files/solution.tex");

    fprintf(fptr, "\\section*{Solution}\n");
    fprintf(fptr, "The optimal solution is:\n");
    fprintf(fptr, "\\begin{itemize}\n");
    for (int i = 1; i < desicion_v; i++) {
        fprintf(fptr, "\\item $x_%d = %.4Lf$\n", matrix[i][0].tag, matrix[max_variable_amount - 1][i].value);
    }
    fprintf(fptr, "\\end{itemize}\n");
    fprintf(fptr, "The optimal value of the objective function is: $Z = %.4Lf$\n", matrix[max_variable_amount - 1][0].value);

    fclose(fptr);
}