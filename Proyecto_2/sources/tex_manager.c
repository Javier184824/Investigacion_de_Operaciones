#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "../headers/structs.h"
#include "../headers/tex_manager.h"
#include "../headers/simplex.h"

#define EPS 1e-9L

FILE* open_file(const char* file_path) {
    FILE* fptr = fopen(file_path, "w");

    if (fptr == NULL) {
        fprintf(stderr, "Could not open %s\n", file_path);
        exit(EXIT_FAILURE);
    }

    return fptr;
}

/* evita imprimir "-0.0000" */
static long double cl(const long double v) {
    return fabsl(v) < 5e-5L ? 0.0L : v;
}

/* "%.4Lf" sin ceros finales (para el coeficiente de M) */
static void trim_num(char* out, const size_t sz, const long double v) {
    snprintf(out, sz, "%.4Lf", v);
    char* p = out + strlen(out) - 1;
    while (p > out && *p == '0') *p-- = 0;
    if (*p == '.') *p = 0;
}

/* Formatea  m*M + v  en LaTeX (sin signos $): "-3M + 2.0000", "M", "5.0000", ... */
void format_coef(char* out, const size_t sz, const long double m, const long double v) {
    if (fabsl(m) < 5e-5L) {
        snprintf(out, sz, "%.4Lf", cl(v));
        return;
    }
    char mp[96], num[64];
    if (fabsl(fabsl(m) - 1) < 5e-5L) {
        snprintf(mp, sizeof mp, "%sM", m < 0 ? "-" : "");
    } else {
        trim_num(num, sizeof num, m);
        snprintf(mp, sizeof mp, "%sM", num);
    }
    if (fabsl(v) < 5e-5L) snprintf(out, sz, "%s", mp);
    else snprintf(out, sz, "%s %c %.4Lf", mp, v < 0 ? '-' : '+', fabsl(v));
}

/* Celda de tabla: numero plano, o en modo matematico si lleva M */
static void print_cell(FILE* f, const matrix_value* c) {
    char buf[160];
    format_coef(buf, sizeof buf, c->m, c->value);
    if (fabsl(c->m) < 5e-5L) fprintf(f, "%s", buf);
    else fprintf(f, "$%s$", buf);
}

/* Etiqueta de columna: nombre guardado, o x_i / s_i por defecto */
static void col_label(FILE* f, const char* name, const int i, const int desicion_v) {
    if (name[0]) fprintf(f, "%s", name);
    else if (i < desicion_v) fprintf(f, "x_{%d}", i);
    else fprintf(f, "s_{%d}", i - desicion_v + 1);
}

/* Escribe texto de usuario en modo texto de LaTeX escapando caracteres especiales */
static void tex_escape(FILE* f, const char* s) {
    for (; *s; s++) {
        switch (*s) {
            case '&': case '%': case '$': case '#': case '_': case '{': case '}':
                fputc('\\', f); fputc(*s, f); break;
            case '~': fputs("\\textasciitilde{}", f); break;
            case '^': fputs("\\textasciicircum{}", f); break;
            case '\\': fputs("\\textbackslash{}", f); break;
            case '\n': case '\r': fputc(' ', f); break;
            default: fputc(*s, f);
        }
    }
}

/* Nombre de una variable de decision (modo matematico); por defecto x_{tag} */
static void var_name(FILE* f, const char* name, const int index) {
    if (name[0]) fprintf(f, "%s", name);
    else fprintf(f, "x_{%d}", index);
}

/* Escribe sum(coef * var) de la fila indicada; sign=-1 para la fila de Z */
static void write_expression(
    FILE* f,
    const int desicion_v,
    const int max_variable_amount,
    const int max_restriction,
    matrix_value matrix[max_variable_amount][max_restriction],
    const int row,
    const long double sign
) {
    bool first = true;
    for (int i = 1; i < desicion_v; i++) {
        long double c = sign * matrix[i][row].value;
        if (fabsl(c) < 1e-12L) continue;
        if (first) {
            if (c < 0) fprintf(f, "-");
        } else {
            fprintf(f, c < 0 ? " - " : " + ");
        }
        long double a = fabsl(c);
        if (fabsl(a - 1) > 1e-12L) fprintf(f, "%Lg\\,", a);
        var_name(f, matrix[i][0].name, i);
        first = false;
    }
    if (first) fprintf(f, "0");
}

static void write_header(
    FILE* fptr,
    const int desicion_v,
    const int max_variable_amount,
    const int max_restriction,
    matrix_value matrix[max_variable_amount][max_restriction],
    const bool with_ratio
) {
    (void)max_restriction;
    fprintf(fptr, "\\begin{table}[H]\n\\centering\n");
    if (max_variable_amount > 20) fprintf(fptr, "\\tiny\\setlength{\\tabcolsep}{2pt}\n");
    else if (max_variable_amount > 13) fprintf(fptr, "\\small\\setlength{\\tabcolsep}{3pt}\n");
    else if (max_variable_amount > 9) fprintf(fptr, "\\setlength{\\tabcolsep}{4pt}\n");

    fprintf(fptr, "\\begin{tabular}{|");
    for (int j = 0; j < max_variable_amount; j++) {
        fprintf(fptr, "c|");
    }
    if (with_ratio) fprintf(fptr, "c|");
    fprintf(fptr, "}\n\\hline\n\\cellcolor{red} $Z$ &");

    for (int i = 1; i < desicion_v; i++) {
        fprintf(fptr, " \\cellcolor{orange} $");
        var_name(fptr, matrix[i][0].name, i);
        fprintf(fptr, "$ &");
    }
    for (int i = desicion_v; i < max_variable_amount - 1; i++) {
        fprintf(fptr, " \\cellcolor{%s} $", matrix[i][0].tag == TAG_ARTIFICIAL ? "cyan" : "yellow");
        col_label(fptr, matrix[i][0].name, i, desicion_v);
        fprintf(fptr, "$ &");
    }
    fprintf(fptr, " \\cellcolor{black} \\textcolor{white}{$b$}%s \\\\ \n\\hline\n",
            with_ratio ? " & $P$" : "");
}

static void write_footer(FILE* fptr) {
    fprintf(fptr, "\\end{tabular}\n\\end{table}\n");
}

void write_table(
    FILE* fptr,
    const int desicion_v,
    const int max_variable_amount,
    const int max_restriction,
    matrix_value matrix[max_variable_amount][max_restriction]
) {
    write_header(fptr, desicion_v, max_variable_amount, max_restriction, matrix, false);

    for (int j = 0; j < max_restriction; j++) {
        for (int i = 0; i < max_variable_amount - 1; i++) {
            print_cell(fptr, &matrix[i][j]);
            fprintf(fptr, " & ");
        }
        print_cell(fptr, &matrix[max_variable_amount - 1][j]);
        fprintf(fptr, " \\\\ \n\\hline\n");
    }

    write_footer(fptr);
}

/* pivot_row = columna que entra (indice i), pivot_col = fila pivote (indice j) */
void write_table_with_pivot(
    FILE* fptr,
    const int pivot_row,
    const int pivot_col,
    const int desicion_v,
    const int max_variable_amount,
    const int max_restriction,
    matrix_value matrix[max_variable_amount][max_restriction]
) {
    write_header(fptr, desicion_v, max_variable_amount, max_restriction, matrix, true);

    for (int j = 0; j < max_restriction; j++) {
        for (int i = 0; i < max_variable_amount - 1; i++) {
            if (i == pivot_row && j == pivot_col) {
                fprintf(fptr, "\\cellcolor{green} \\textbf{%.4Lf} & ", cl(matrix[i][j].value));
            } else if (i == pivot_row) {
                fprintf(fptr, "\\cellcolor{green!20} ");
                print_cell(fptr, &matrix[i][j]);
                fprintf(fptr, " & ");
            } else {
                print_cell(fptr, &matrix[i][j]);
                fprintf(fptr, " & ");
            }
        }
        print_cell(fptr, &matrix[max_variable_amount - 1][j]);
        fprintf(fptr, " & ");

        long double a = matrix[pivot_row][j].value;
        if (j == 0 || a <= EPS) {
            fprintf(fptr, "- \\\\ \n\\hline\n");
        } else {
            fprintf(fptr, "%.4Lf \\\\ \n\\hline\n", cl(matrix[max_variable_amount - 1][j].value / a));
        }
    }

    write_footer(fptr);
}

void write_original_problem(const int maximizing, const char* problem_name, const int desicion_v, const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction], const int* ops) {
    FILE* fptr = open_file("tex_files/original_problem.tex");

    const int n = desicion_v - 1;
    const int m = max_restriction - 1;
    int n_art = 0;
    for (int i = desicion_v; i < max_variable_amount - 1; i++)
        if (matrix[i][0].tag == TAG_ARTIFICIAL) n_art++;

    fprintf(fptr,
        "\\section*{Description}\n"
        "\\begin{itemize}\n"
        "\\item Problem Name: ");
    tex_escape(fptr, problem_name);
    fprintf(fptr,
        "\n\\item Amount of decision variables: %d\n"
        "\\item Amount of restrictions: %d\n"
        "\\item Total Amount of variables (decision + slack/surplus + artificial): %d\n"
        "\\item Type of problem: %s\n",
        n, m, max_variable_amount - 2, maximizing ? "Maximization" : "Minimization");
    if (n_art > 0) {
        fprintf(fptr,
            "\\item Method: Big $M$ (%d artificial variable%s, penalized with $%sM$ in the objective function)\n",
            n_art, n_art > 1 ? "s" : "", maximizing ? "-" : "+");
    }
    fprintf(fptr, "\\end{itemize}\n");

    fprintf(fptr,
        "\\section*{Equations}\n"
        "\\begin{align*}\n"
        "\\text{%s}\\ Z = {} & ",
        maximizing ? "Max" : "Min");
    write_expression(fptr, desicion_v, max_variable_amount, max_restriction, matrix, 0, -1.0L);
    fprintf(fptr, " \\\\ \n\\text{subject to:} & \\\\ \n");

    for (int j = 1; j < max_restriction; j++) {
        fprintf(fptr, "& ");
        write_expression(fptr, desicion_v, max_variable_amount, max_restriction, matrix, j, 1.0L);
        fprintf(fptr, " %s %Lg \\\\ \n",
                ops[j - 1] == OP_GE ? "\\geq" : ops[j - 1] == OP_EQ ? "=" : "\\leq",
                matrix[max_variable_amount - 1][j].value);
    }

    fprintf(fptr, "& ");
    for (int i = 1; i < desicion_v; i++) {
        if (i > 1) fprintf(fptr, ", ");
        var_name(fptr, matrix[i][0].name, i);
    }
    fprintf(fptr, " \\geq 0 \n\\end{align*}\n\n");

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

void write_solution(const int desicion_v, const int max_variable_amount, const int max_restriction, const int status, matrix_value matrix[max_variable_amount][max_restriction]) {
    FILE* fptr = open_file("tex_files/solution.tex");

    fprintf(fptr, "\\section*{Solution}\n");

    if (status == SIMPLEX_OPTIMAL) {
        const int rhs = max_variable_amount - 1;
        fprintf(fptr, "The optimal solution is:\n\\begin{itemize}\n");
        for (int i = 1; i < max_variable_amount - 1; i++) {
            int r = basic_row(max_variable_amount, max_restriction, matrix, i);
            long double v = r ? matrix[rhs][r].value : 0.0L;
            fprintf(fptr, "\\item $");
            col_label(fptr, matrix[i][0].name, i, desicion_v);
            fprintf(fptr, " = %.4Lf$%s\n", cl(v), r ? "" : " (non-basic)");
        }
        fprintf(fptr, "\\end{itemize}\n");
        fprintf(fptr, "The optimal value of the objective function is: $Z = %.4Lf$\n\n", cl(matrix[rhs][0].value));
    } else if (status == SIMPLEX_INFEASIBLE) {
        fprintf(fptr, "The problem is \\textbf{infeasible}: no point satisfies all the constraints.\n\n");
    } else if (status == SIMPLEX_UNBOUNDED) {
        fprintf(fptr, "The problem is \\textbf{unbounded}: there is no finite optimal solution.\n\n");
    } else {
        fprintf(fptr, "The solver stopped after reaching the iteration limit; no solution was found.\n\n");
    }

    fprintf(fptr, "\\input{tex_files/solution_notes.tex}\n");

    fclose(fptr);
}
