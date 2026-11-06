#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "../headers/structs.h"
#include "../headers/simplex.h"
#include "../headers/tex_manager.h"

#define EPS 1e-9L

/* Layout de la matriz: matrix[columna][fila]
 *   columna 0              -> columna de Z
 *   columnas 1..n          -> variables de decision
 *   columnas n+1..n+m      -> variables de holgura
 *   ultima columna         -> lado derecho (b)
 *   fila 0                 -> fila de Z, filas 1..m -> restricciones
 */

/* Si la columna es basica devuelve la fila donde esta su 1; si no, 0 */
int basic_row(const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction], const int col){
	(void)max_variable_amount;
	int row = 0;
	for(int j = 0; j < max_restriction; j++){
		long double v = matrix[col][j].value;
		if(fabsl(v) < EPS) continue;
		if(fabsl(v - 1) < EPS && row == 0 && j > 0){
			row = j;
		}else{
			return 0;
		}
	}
	return row;
}

void extract_solution(const int max_variable_amount, const int desicion_v, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction], long double* x){
	for(int i = 1; i < desicion_v; i++){
		int r = basic_row(max_variable_amount, max_restriction, matrix, i);
		x[i - 1] = r ? matrix[max_variable_amount - 1][r].value : 0.0L;
	}
}

/* Prueba de la razon minima sobre la columna col. Devuelve la fila (0 si no hay
 * ninguna entrada positiva). En empates gana la fila de arriba (menor indice). */
static int ratio_row(const int max_variable_amount, const int max_restriction, matrix_value matrix[max_variable_amount][max_restriction], const int col, long double* min_ratio, int* ties){
	int best = 0;
	long double best_ratio = 0;
	int count = 0;

	for(int j = 1; j < max_restriction; j++){
		long double a = matrix[col][j].value;
		if(a > EPS){
			long double r = matrix[max_variable_amount - 1][j].value / a;
			if(best == 0 || r < best_ratio - EPS){
				best = j;
				best_ratio = r;
				count = 1;
			}else if(fabsl(r - best_ratio) <= EPS){
				count++;
			}
		}
	}
	if(min_ratio) *min_ratio = best_ratio;
	if(ties) *ties = count;
	return best;
}

/* Devuelve en i_indexF la columna que entra (0 si ya es optimo) y en j_indexF la
 * fila pivote (0 si la columna no tiene entradas positivas -> no acotado). */
void find_pivot(const int max_variable_amount, const int max_restriction, const bool maximize, int* i_indexF, int* j_indexF, matrix_value matrix[max_variable_amount][max_restriction]){
	*i_indexF = 0;
	*j_indexF = 0;

	long double best = 0;
	for(int i = 1; i < max_variable_amount - 1; i++){
		long double v = matrix[i][0].value;
		if(maximize ? (v < best - EPS) : (v > best + EPS)){
			best = v;
			*i_indexF = i;
		}
	}

	if(*i_indexF == 0){
		return;
	}

	*j_indexF = ratio_row(max_variable_amount, max_restriction, matrix, *i_indexF, NULL, NULL);
}

void canonize(const int max_variable_amount, const int max_restriction, const int i_indexF, const int j_indexF, matrix_value matrix[max_variable_amount][max_restriction]){
	long double divisor_value = matrix[i_indexF][j_indexF].value;

	for(int i = 1; i < max_variable_amount; i++){//canonizing row
		matrix[i][j_indexF].value = matrix[i][j_indexF].value / divisor_value;
	}

	for(int j = 0; j < max_restriction; j++){//canonizing columns
		if(j == j_indexF){
			continue;
		}
		long double actual_mult = matrix[i_indexF][j].value * -1;

		for(int i = 1; i < max_variable_amount; i++){
			matrix[i][j].value = matrix[i][j].value + (matrix[i][j_indexF].value * actual_mult);
		}
	}

	for(int i = 0; i < max_variable_amount; i++){//limpia ruido numerico
		for(int j = 0; j < max_restriction; j++){
			if(fabsl(matrix[i][j].value) < 1e-12L){
				matrix[i][j].value = 0.0L;
			}
		}
	}
}

static void var_label(char* out, const size_t sz, const int desicion_v, const int col, const char* name){
	if(col < desicion_v){
		if(name[0]) snprintf(out, sz, "%s", name);
		else snprintf(out, sz, "x_{%d}", col);
	}else{
		snprintf(out, sz, "s_{%d}", col - desicion_v + 1);
	}
}

int solve_simplex(const int max_variable_amount, const int desicion_v, const int max_restriction, const int max_tries, const bool maximize, matrix_value matrix[max_variable_amount][max_restriction]){
	FILE* fptr_intermediate = open_file("tex_files/intermediate_tables.tex");
	FILE* fptr_notes = open_file("tex_files/solution_notes.tex");

	const int rhs = max_variable_amount - 1;
	int status = SIMPLEX_OPTIMAL;
	int cont = 0;
	bool degenerate = false;
	char lbl_in[128], lbl_out[128];

	while(true){
		int i_index = 0;
		int j_index = 0;

		//========find pivot=============
		find_pivot(max_variable_amount, max_restriction, maximize, &i_index, &j_index, matrix);

		if(i_index == 0){
			break; // optimo
		}

		fprintf(fptr_intermediate, "\\section*{Iteration %d}\n", cont + 1);
		var_label(lbl_in, sizeof lbl_in, desicion_v, i_index, matrix[i_index][0].name);

		if(j_index == 0){
			status = SIMPLEX_UNBOUNDED;
			fprintf(fptr_intermediate,
				"The entering variable would be $%s$ (coefficient $%.4Lf$ in the $Z$ row), "
				"but every entry of its column in the constraints is less than or equal to zero, "
				"so no row limits its growth: the problem is \\textbf{unbounded}.\n\n",
				lbl_in, matrix[i_index][0].value);
			write_table(fptr_intermediate, desicion_v, max_variable_amount, max_restriction, matrix);
			fprintf(fptr_notes,
				"\\subsection*{Unbounded problem}\n"
				"In iteration %d the variable $%s$ has an improving coefficient in the $Z$ row "
				"($%.4Lf$), but all the entries of its column in the constraint rows are $\\le 0$. "
				"Therefore the ratio test has no candidate row and $%s$ can grow indefinitely "
				"without violating any constraint, improving $Z$ without limit. "
				"The problem has no finite optimal solution.\n",
				cont + 1, lbl_in, matrix[i_index][0].value, lbl_in);
			break;
		}

		if(cont >= max_tries){
			status = SIMPLEX_LOOP;
			fprintf(fptr_intermediate, "Iteration limit reached.\n");
			break;
		}

		long double min_ratio = 0;
		int ties = 0;
		ratio_row(max_variable_amount, max_restriction, matrix, i_index, &min_ratio, &ties);

		int leaving = 0;
		for(int i = 1; i < max_variable_amount - 1; i++){
			if(basic_row(max_variable_amount, max_restriction, matrix, i) == j_index){
				leaving = i;
				break;
			}
		}
		var_label(lbl_out, sizeof lbl_out, desicion_v, leaving, leaving ? matrix[leaving][0].name : "");

		fprintf(fptr_intermediate,
			"Entering variable: $%s$ (%s coefficient in the $Z$ row: $%.4Lf$). "
			"Leaving variable: $%s$ (row %d, minimum ratio $%.4Lf$).\n\n",
			lbl_in, maximize ? "most negative" : "most positive", matrix[i_index][0].value,
			leaving ? lbl_out : "-", j_index, min_ratio);

		if(ties > 1){
			degenerate = true;
			fprintf(fptr_intermediate,
				"\\textbf{Tie:} %d rows share the minimum ratio $%.4Lf$. The tie was resolved by "
				"choosing the topmost row. The next table will have a basic variable equal to zero "
				"(degenerate solution).\n\n", ties, min_ratio);
		}else if(min_ratio < EPS){
			degenerate = true;
			fprintf(fptr_intermediate,
				"\\textbf{Degenerate pivot:} the minimum ratio is zero, so this iteration changes "
				"the basis but not the value of $Z$.\n\n");
		}

		fprintf(fptr_intermediate, "\\subsection*{Finding the Pivot}\n");
		write_table_with_pivot(fptr_intermediate, i_index, j_index, desicion_v, max_variable_amount, max_restriction, matrix);

		//===========solve===============
		canonize(max_variable_amount, max_restriction, i_index, j_index, matrix);

		fprintf(fptr_intermediate, "\\subsection*{Canonize the Pivot column with Pivot value}\n");
		write_table(fptr_intermediate, desicion_v, max_variable_amount, max_restriction, matrix);
		fprintf(fptr_intermediate, "\\newpage\n");

		cont++;
	}

	if(status == SIMPLEX_OPTIMAL && cont == 0){
		fprintf(fptr_intermediate, "The initial table is already optimal: no iterations were needed.\n");
	}

	if(status == SIMPLEX_OPTIMAL){
		//degeneracy in the final table
		for(int j = 1; j < max_restriction; j++){
			if(fabsl(matrix[rhs][j].value) < EPS){
				degenerate = true;
			}
		}
		if(degenerate){
			fprintf(fptr_notes,
				"\\subsection*{Degenerate problem}\n"
				"During the execution (or in the final table) at least one basic variable took the value zero, "
				"so the problem is degenerate: more constraints than necessary meet at the vertex. "
				"Ties in the minimum ratio test were resolved by choosing the topmost row (smallest row index); "
				"ties for the entering column were resolved by choosing the leftmost column. "
				"The algorithm did not cycle.\n");
		}

		//multiple solutions: nonbasic column with zero coefficient in the Z row
		int alt_col = 0;
		for(int i = 1; i < max_variable_amount - 1; i++){
			if(fabsl(matrix[i][0].value) < EPS && basic_row(max_variable_amount, max_restriction, matrix, i) == 0){
				alt_col = i;
				break;
			}
		}

		if(alt_col != 0){
			char lbl_alt[128];
			var_label(lbl_alt, sizeof lbl_alt, desicion_v, alt_col, matrix[alt_col][0].name);

			matrix_value alt[max_variable_amount][max_restriction];
			memcpy(alt, matrix, sizeof alt);

			int alt_row = ratio_row(max_variable_amount, max_restriction, alt, alt_col, NULL, NULL);

			fprintf(fptr_notes,
				"\\subsection*{Multiple solutions}\n"
				"In the final table the non-basic variable $%s$ has a coefficient of $0$ in the $Z$ row. "
				"Entering it into the basis does not change the value of $Z$, so there is more than one optimal solution.\n\n",
				lbl_alt);

			if(alt_row == 0){
				fprintf(fptr_notes,
					"However, every entry of the column of $%s$ in the constraints is $\\le 0$, so $%s$ can grow "
					"indefinitely while $Z$ keeps its optimal value: there are infinitely many optimal solutions "
					"along an unbounded direction.\n", lbl_alt, lbl_alt);
			}else{
				long double x1[desicion_v], x2[desicion_v];
				extract_solution(max_variable_amount, desicion_v, max_restriction, alt, x1);

				canonize(max_variable_amount, max_restriction, alt_col, alt_row, alt);
				extract_solution(max_variable_amount, desicion_v, max_restriction, alt, x2);

				fprintf(fptr_notes,
					"One more pivot (column $%s$, row %d) gives a second final table:\n\n", lbl_alt, alt_row);
				write_table(fptr_notes, desicion_v, max_variable_amount, max_restriction, alt);

				fprintf(fptr_notes, "\\medskip\n\\noindent\\textbf{Solution 1:} $(");
				for(int k = 0; k < desicion_v - 1; k++) fprintf(fptr_notes, "%s%.4Lf", k ? ", " : "", x1[k]);
				fprintf(fptr_notes, ")$\\\\\n\\textbf{Solution 2:} $(");
				for(int k = 0; k < desicion_v - 1; k++) fprintf(fptr_notes, "%s%.4Lf", k ? ", " : "", x2[k]);
				fprintf(fptr_notes, ")$\n\n");

				bool same = true;
				for(int k = 0; k < desicion_v - 1; k++){
					if(fabsl(x1[k] - x2[k]) > 1e-7L) same = false;
				}

				if(same){
					fprintf(fptr_notes,
						"Both tables correspond to the same point (the extra pivot was degenerate), "
						"so no different optimal point was obtained from this pivot.\n");
				}else{
					fprintf(fptr_notes,
						"Any convex combination of both solutions is also optimal:\n"
						"\\[ \\mathbf{x} = \\lambda\\,\\mathbf{x}_1 + (1-\\lambda)\\,\\mathbf{x}_2, \\qquad 0 \\le \\lambda \\le 1. \\]\n"
						"Some additional optimal solutions:\n\\begin{itemize}\n");
					const long double lam[3] = {0.25L, 0.5L, 0.75L};
					for(int t = 0; t < 3; t++){
						fprintf(fptr_notes, "\\item $\\lambda = %.2Lf$: $(", lam[t]);
						for(int k = 0; k < desicion_v - 1; k++){
							fprintf(fptr_notes, "%s%.4Lf", k ? ", " : "", lam[t] * x1[k] + (1 - lam[t]) * x2[k]);
						}
						fprintf(fptr_notes, ")$\n");
					}
					fprintf(fptr_notes, "\\end{itemize}\n");
				}
			}
		}
	}

	fclose(fptr_intermediate);
	fclose(fptr_notes);
	return status;
}
