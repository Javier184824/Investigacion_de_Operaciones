#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>
#include <glib/gstdio.h>
#include "headers/structs.h"
#include "headers/tex_manager.h"
#include "headers/simplex.h"

#define GLADE_FILE "menu.glade"
#define OUT_DIR    "pdflatex_output"
#define OUT_NAME   "main"

// Valores globales
#define MAXN 15
#define MAXM 15
#define NAMELEN 64

enum { ST_OPTIMO, ST_NO_ACOTADO, ST_INFACTIBLE, ST_LIMITE };

typedef struct {
    int n, m, max, show_all;
    char title[256];
    char names[MAXN][NAMELEN];
    double c[MAXN];
    double A[MAXM][MAXN];
    int op[MAXM];
    double b[MAXM];
} Problem;

// Conexiones del frontend
static GtkBuilder *builder;

/* Obtiene un widget por id con formato printf, ej: W("spin_c_%d_%d", i, j) */
static GtkWidget *W(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char *id = g_strdup_vprintf(fmt, ap);
    va_end(ap);
    GtkWidget *w = GTK_WIDGET(gtk_builder_get_object(builder, id));
    if (!w) g_printerr("Aviso: no existe el widget '%s' en %s\n", id, GLADE_FILE);
    g_free(id);
    return w;
}

/* Lee un GtkSpinButton asegurando que lo escrito a mano ya fue confirmado */
static double spin_val(GtkWidget *w)
{
    gtk_spin_button_update(GTK_SPIN_BUTTON(w));
    return gtk_spin_button_get_value(GTK_SPIN_BUTTON(w));
}

static void set_vis(GtkWidget *w, gboolean v) { if (w) gtk_widget_set_visible(w, v); }

static int get_n(void) { return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(W("spin_vars"))); }
static int get_m(void) { return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(W("spin_restr"))); }

/* Nombre para mostrar en etiquetas GTK: sin signos '$' */
static char *gui_name(int j)
{
    const char *raw = gtk_entry_get_text(GTK_ENTRY(W("entry_name_%d", j)));
    GString *s = g_string_new(NULL);
    for (; *raw; raw++) if (*raw != '$') g_string_append_c(s, *raw);
    char *r = g_strstrip(g_string_free(s, FALSE));
    if (!r[0]) { g_free(r); r = g_strdup_printf("x%d", j); }
    return r;
}

/* Nombre para LaTeX (modo matematico): sin '$', y x_10 -> x_{10} */
static void make_name(int j, char *out, size_t sz)
{
    const char *raw = gtk_entry_get_text(GTK_ENTRY(W("entry_name_%d", j)));
    char tmp[128]; int k = 0;
    for (; *raw && k < 120; raw++) if (*raw != '$' && *raw != '\n' && *raw != '\r') tmp[k++] = *raw;
    tmp[k] = 0;
    g_strstrip(tmp);
    if (!tmp[0]) { snprintf(out, sz, "x_{%d}", j); return; }
    char *u = strchr(tmp, '_');
    if (u && isdigit((unsigned char)u[1])) {
        int d = 0;
        while (isdigit((unsigned char)u[1 + d])) d++;
        snprintf(out, sz, "%.*s{%.*s}%s", (int)(u - tmp + 1), tmp, d, u + 1, u + 1 + d);
    } else {
        snprintf(out, sz, "%s", tmp);
    }
}

/* Muestra/oculta widgets segun n variables y m restricciones; actualiza etiquetas */
static void refresh_ui(void)
{
    int n = get_n(), m = get_m();
    for (int j = 1; j <= 15; j++) {
        gboolean vj = j <= n;
        set_vis(W("lbl_name_%d", j), vj);
        set_vis(W("entry_name_%d", j), vj);
        set_vis(W("spin_obj_%d", j), vj);
        set_vis(W("lbl_obj_%d", j), vj);
        if (vj) {
            char *nm = gui_name(j);
            char *txt = g_strdup_printf("%s%s", nm, j < n ? " +" : "");
            gtk_label_set_text(GTK_LABEL(W("lbl_obj_%d", j)), txt);
            g_free(txt); g_free(nm);
        }
    }
    for (int i = 1; i <= 15; i++) {
        gboolean vi = i <= m;
        set_vis(W("lbl_r_%d", i), vi);
        set_vis(W("combo_op_%d", i), vi);
        set_vis(W("spin_rhs_%d", i), vi);
        for (int j = 1; j <= 15; j++) {
            gboolean v = vi && j <= n;
            set_vis(W("spin_c_%d_%d", i, j), v);
            set_vis(W("lbl_c_%d_%d", i, j), v);
            if (v) {
                char *nm = gui_name(j);
                char *txt = g_strdup_printf("%s%s", nm, j < n ? " +" : "");
                gtk_label_set_text(GTK_LABEL(W("lbl_c_%d_%d", i, j)), txt);
                g_free(txt); g_free(nm);
            }
        }
    }
}

static void mostrar_error(const char *msg)
{
    GtkWindow *w = GTK_WINDOW(W("main_window"));
    GtkWidget *d = gtk_message_dialog_new(w, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
                                          GTK_BUTTONS_OK, "%s", msg);
    gtk_dialog_run(GTK_DIALOG(d));
    gtk_widget_destroy(d);
}

static void on_main_window_destroy(GtkWidget *w, gpointer d) { (void)w; (void)d; gtk_main_quit(); }
static void on_spin_vars_changed(GtkSpinButton *s, gpointer d) { (void)s; (void)d; refresh_ui(); }
static void on_spin_restr_changed(GtkSpinButton *s, gpointer d) { (void)s; (void)d; refresh_ui(); }
static void on_name_changed(GtkEditable *e, gpointer d) { (void)e; (void)d; refresh_ui(); }

static void on_btn_Clean_clicked(GtkButton *b, gpointer d)
{
    (void)b; (void)d;
    gtk_entry_set_text(GTK_ENTRY(W("entry_nombre")), "");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(W("spin_vars")), 2);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(W("spin_restr")), 2);
    gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_verbo")), 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(W("check_tablas")), FALSE);
    for (int j = 1; j <= 15; j++) {
        char *nm = g_strdup_printf("$x_%d$", j);
        gtk_entry_set_text(GTK_ENTRY(W("entry_name_%d", j)), nm);
        g_free(nm);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(W("spin_obj_%d", j)), 0);
    }
    for (int i = 1; i <= 15; i++) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_op_%d", i)), 0);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(W("spin_rhs_%d", i)), 0);
        for (int j = 1; j <= 15; j++)
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(W("spin_c_%d_%d", i, j)), 0);
    }
    refresh_ui();
}

/* ---------- Nombres de archivo seguros ---------- */

static int eq_nocase(const char *a, size_t n, const char *b)
{
    if (strlen(b) != n) return 0;
    for (size_t i = 0; i < n; i++) {
        char x = a[i], y = b[i];
        if (x >= 'a' && x <= 'z') x -= 32;
        if (y >= 'a' && y <= 'z') y -= 32;
        if (x != y) return 0;
    }
    return 1;
}

/* Nombres reservados en Windows (con o sin extension) */
static int is_reserved(const char *name, size_t len)
{
    size_t base = 0;
    while (base < len && name[base] != '.') base++;

    static const char *fixed[] = { "CON", "PRN", "AUX", "NUL" };
    for (size_t i = 0; i < 4; i++)
        if (eq_nocase(name, base, fixed[i])) return 1;

    return base == 4 && name[3] >= '1' && name[3] <= '9' &&
           (eq_nocase(name, 3, "COM") || eq_nocase(name, 3, "LPT"));
}

/* Convierte cualquier string en un nombre de archivo valido (UTF-8 se conserva).
 * dst_size debe ser >= 2. Devuelve la longitud escrita. */
static size_t sanitize_filename(const char *src, char *dst, size_t dst_size)
{
    if (!src || !dst || dst_size < 2) return 0;

    const size_t max = dst_size - 2;   /* 1 byte libre para un posible prefijo '_' */
    size_t len = 0, i = 0;

    for (; src[i] != '\0' && len < max; i++) {
        unsigned char c = (unsigned char)src[i];

        if (len == 0 && c == ' ') continue;
        if (c < 0x20 || c == 0x7F || strchr("<>:\"/\\|?*", c)) c = '_';
        else if (len == 0 && c == '.') c = '_';

        dst[len++] = (char)c;
    }

    /* No cortar un caracter UTF-8 por la mitad */
    if (src[i] != '\0' && ((unsigned char)src[i] & 0xC0) == 0x80) {
        while (len > 0 && ((unsigned char)dst[len - 1] & 0xC0) == 0x80) len--;
        if (len > 0 && ((unsigned char)dst[len - 1] & 0x80)) len--;
    }

    while (len > 0 && (dst[len - 1] == ' ' || dst[len - 1] == '.')) len--;
    if (len == 0) { dst[0] = '_'; len = 1; }

    if (is_reserved(dst, len)) {
        memmove(dst + 1, dst, len);
        dst[0] = '_';
        len++;
    }
    dst[len] = '\0';
    return len;
}

/* Nombre para -jobname de pdflatex: ademas de ser un nombre de archivo valido,
 * solo permite ASCII alfanumerico, '-', '_' y '.' (y UTF-8). Asi es seguro para
 * el shell y para TeX. Si el titulo esta vacio usa "resultado". */
static void make_jobname(const char *title, char *out, size_t sz)
{
    char tmp[256];
    snprintf(tmp, sizeof tmp, "%s", title ? title : "");
    g_strstrip(tmp);
    if (!tmp[0]) snprintf(tmp, sizeof tmp, "resultado");

    sanitize_filename(tmp, out, sz);

    for (char *p = out; *p; p++) {
        unsigned char c = (unsigned char)*p;
        if (c < 0x80 && !isalnum(c) && c != '-' && c != '_' && c != '.') *p = '_';
    }
}

/* Elimina (in situ) los caracteres especiales de LaTeX: \ { } $ & # ^ _ % ~
 * y los saltos de linea. Luego quita espacios sobrantes en los extremos. */
static void strip_latex_special(char *s)
{
    char *w = s;
    for (const char *r = s; *r; r++)
        if (!strchr("\\{}$&#^_%~\n\r", *r)) *w++ = *r;
    *w = '\0';
    g_strstrip(s);
}

static void on_btn_resolver_clicked(GtkButton *b, gpointer d)
{
    (void)b; (void)d;
    Problem *P = calloc(1, sizeof *P);
    P->n = get_n();
    P->m = get_m();
    P->max = (gtk_combo_box_get_active(GTK_COMBO_BOX(W("combo_verbo"))) != 1);
    P->show_all = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(W("check_tablas")));
    snprintf(P->title, sizeof P->title, "%s", gtk_entry_get_text(GTK_ENTRY(W("entry_nombre"))));
    strip_latex_special(P->title);

    for (int j = 1; j <= P->n; j++) {
        make_name(j, P->names[j - 1], NAMELEN);
        P->c[j - 1] = spin_val(W("spin_obj_%d", j));
    }
    for (int i = 1; i <= P->m; i++) {
        for (int j = 1; j <= P->n; j++)
            P->A[i - 1][j - 1] = spin_val(W("spin_c_%d_%d", i, j));
        int op = gtk_combo_box_get_active(GTK_COMBO_BOX(W("combo_op_%d", i)));
        P->op[i - 1] = (op < 0 || op > 2) ? OP_LE : op;
        P->b[i - 1] = spin_val(W("spin_rhs_%d", i));
    }

    /* Gran M. RHS negativo: se multiplica la fila por -1 y se invierte la desigualdad */
    for (int i = 0; i < P->m; i++) {
        if (P->b[i] < 0) {
            P->b[i] = -P->b[i];
            for (int j = 0; j < P->n; j++) P->A[i][j] = -P->A[i][j];
            if (P->op[i] == OP_LE)      P->op[i] = OP_GE;
            else if (P->op[i] == OP_GE) P->op[i] = OP_LE;
        }
    }

    /* "<=" -> holgura; ">=" -> exceso + artificial; "=" -> artificial */
    int n_slack = 0, n_art = 0;
    for (int i = 0; i < P->m; i++) {
        if (P->op[i] != OP_EQ) n_slack++;
        if (P->op[i] != OP_LE) n_art++;
    }

    int desicion = P->n + 1;                                   /* Z + variables de decision */
    int max_restriction = P->m + 1;                            /* fila Z + restricciones */
    int max_variable_amount = desicion + n_slack + n_art + 1;  /* +1 = columna b */
    int rhs = max_variable_amount - 1;
    int max_tries = 1000;
    matrix_value matrix[max_variable_amount][max_restriction];

    memset(matrix, 0, sizeof matrix);

    matrix[0][0].value = 1.0L;
    for (int i = 0; i < P->n; i++) {
        matrix[i + 1][0].value = -P->c[i];
        matrix[i + 1][0].tag = i + 1;
        snprintf(matrix[i + 1][0].name, sizeof matrix[i + 1][0].name, "%s", P->names[i]);
        for (int j = 0; j < P->m; j++) {
            matrix[i + 1][j + 1].tag = i + 1;
            matrix[i + 1][j + 1].value = P->A[j][i];
        }
    }

    /* Holguras/excesos primero, luego artificiales (s_i y a_i = numero de restriccion) */
    int col_slack = desicion;
    int col_art   = desicion + n_slack;
    const long double pen = P->max ? 1.0L : -1.0L;   /* coeficiente de M en la fila Z */

    for (int j = 0; j < P->m; j++) {
        int row = j + 1;
        matrix[rhs][row].value = P->b[j];

        if (P->op[j] != OP_EQ) {
            matrix[col_slack][row].value = (P->op[j] == OP_LE) ? 1.0L : -1.0L;
            snprintf(matrix[col_slack][0].name, sizeof matrix[col_slack][0].name,
                     P->op[j] == OP_LE ? "s_{%d}" : "e_{%d}", row);
            col_slack++;
        }
        if (P->op[j] != OP_LE) {
            matrix[col_art][row].value = 1.0L;
            matrix[col_art][0].m = pen;               /* penalizacion: +-M */
            matrix[col_art][0].tag = TAG_ARTIFICIAL;
            snprintf(matrix[col_art][0].name, sizeof matrix[col_art][0].name, "a_{%d}", row);

            /* Forma canonica: fila Z -= (+-M) * fila de la restriccion (deja la artificial en 0) */
            for (int k = 1; k <= rhs; k++)
                matrix[k][0].m -= pen * matrix[k][row].value;

            col_art++;
        }
    }

    write_original_problem(P->max, P->title, desicion, max_variable_amount, max_restriction, matrix, P->op);
    write_initial_table(desicion, max_variable_amount, max_restriction, matrix);
    int status = solve_simplex(max_variable_amount, desicion, max_restriction, max_tries, P->max, matrix);
    write_final_table(desicion, max_variable_amount, max_restriction, matrix);
    write_solution(desicion, max_variable_amount, max_restriction, status, matrix);

    const char *tex = P->show_all ? "tex_files/main.tex" : "tex_files/main_no_intermediate.tex";

    char jobname[128];
    make_jobname(P->title, jobname, sizeof jobname);

    char *cmd = g_strdup_printf(
        "pdflatex -interaction=nonstopmode -halt-on-error -jobname=%s -aux-directory=pdflatex_aux -output-directory=" OUT_DIR " %s > " OUT_DIR "/pdflatex_output.txt 2>&1",
        jobname, tex);
    int rc = system(cmd);
    g_free(cmd);

    if (rc == 0) {
        char *pdf_path = g_strdup_printf(OUT_DIR "/%s.pdf", jobname);
        char *argv[] = { "evince", "--presentation", pdf_path, NULL };
        g_spawn_async(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL);
        g_free(pdf_path);
    } else {
        mostrar_error("No se pudo generar el PDF. Revise que pdflatex este instalado y mire "
                      OUT_DIR "/pdflatex_output.txt para ver el error.");
    }

    free(P);
}

int main(int argc, char *argv[])
{
    /* Los archivos (glade, tex_files, results, figures) se buscan relativos al ejecutable */
    char *exe = g_file_read_link("/proc/self/exe", NULL);
    if (exe) {
        char *dir = g_path_get_dirname(exe);
        if (g_chdir(dir) != 0) g_printerr("Aviso: no se pudo cambiar a %s\n", dir);
        g_free(dir);
        g_free(exe);
    }

    g_mkdir_with_parents(OUT_DIR, 0755);

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    GError *error = NULL;
    if (!gtk_builder_add_from_file(builder, GLADE_FILE, &error)) {
        g_printerr("Error cargando %s: %s\n", GLADE_FILE, error->message);
        g_error_free(error);
        return 1;
    }

    gtk_builder_add_callback_symbols(builder,
        "on_main_window_destroy", G_CALLBACK(on_main_window_destroy),
        "on_spin_vars_changed",   G_CALLBACK(on_spin_vars_changed),
        "on_spin_restr_changed",  G_CALLBACK(on_spin_restr_changed),
        "on_name_changed",        G_CALLBACK(on_name_changed),
        "on_btn_Clean_clicked",   G_CALLBACK(on_btn_Clean_clicked),
        "on_btn_resolver_clicked",G_CALLBACK(on_btn_resolver_clicked),
        NULL);
    gtk_builder_connect_signals(builder, NULL);

    refresh_ui();

    /* gtk_widget_show (no show_all) para respetar los widgets ocultos */
    gtk_widget_show(W("main_window"));
    gtk_main();
    return 0;
}