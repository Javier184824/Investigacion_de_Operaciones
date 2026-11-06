#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>
#include "headers/structs.h"
#include "headers/tex_manager.h"
#include "headers/simplex.h"

#define GLADE_FILE "menu.glade"
#define OUT_DIR    "results"
#define OUT_NAME   "main"

// Valores globales
#define MAXN 15
#define MAXM 15
#define NAMELEN 64

enum { OP_LE, OP_GE, OP_EQ };
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

static void on_btn_resolver_clicked(GtkButton *b, gpointer d)
{
    (void)b; (void)d;
    Problem *P = calloc(1, sizeof *P);
    char err[256] = "";

    P->n = get_n();
    P->m = get_m();
    P->max = (gtk_combo_box_get_active(GTK_COMBO_BOX(W("combo_verbo"))) != 1);
    P->show_all = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(W("check_tablas")));
    snprintf(P->title, sizeof P->title, "%s", gtk_entry_get_text(GTK_ENTRY(W("entry_nombre"))));

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

    int desicion = P->n + 1;
    int max_restriction = P->m + 1;
    int max_variable_amount = desicion + max_restriction;
    int max_tries = desicion * 3;
    matrix_value matrix[max_variable_amount][max_restriction];

    for (int i = 0; i < max_variable_amount; i++) {
        for (int j = 0; j < max_restriction; j++) {
            matrix[i][j].tag = 0;
            matrix[i][j].value = 0.0L;
        }
    }

    matrix[0][0].value = 1.0L;
    for (int i = 0; i < P->n; i++) {
        matrix[i + 1][0].value = -P->c[i];
        matrix[i + 1][0].name = P->names[i][0];
        for (int j = 0; j < P->m; j++) {
            matrix[i + 1][j + 1].tag = i + 1;
            matrix[i + 1][j + 1].value = P->A[j][i];
        }
    }
    for (int j = 0; j < P->m; j++) {
        matrix[desicion + j][j + 1].value = 1.0L;
        matrix[max_variable_amount - 1][j + 1].value = P->b[j];
    }

    write_original_problem(P->max, P->title, desicion, max_variable_amount, max_restriction, matrix);
    write_initial_table(desicion, max_variable_amount, max_restriction, matrix);
    solve_simplex(max_variable_amount, desicion, max_restriction, max_tries, P->max, matrix);
    write_final_table(desicion, max_variable_amount, max_restriction, matrix);
    write_solution(desicion, max_variable_amount, max_restriction, matrix);

    if (P->show_all) {
        system("pdflatex -output-directory=results tex_files/main.tex");
        g_spawn_command_line_async("xdg-open " OUT_DIR "/" OUT_NAME ".pdf", NULL);
    }
    else {
        system("pdflatex -output-directory=results tex_files/main_no_intermediate.tex");
        g_spawn_command_line_async("xdg-open " OUT_DIR "/" "main_no_intermediate.pdf", NULL);
    }
    
    #if 0
    if (1 == 0)
    //if (generar_pdf(P, err, sizeof err))
        g_spawn_command_line_async("xdg-open " OUT_DIR "/" OUT_NAME ".pdf", NULL);
    else
        mostrar_error(err);
    #endif

    free(P);
}

int main(int argc, char *argv[])
{
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