#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "img.h"
#include "menu.h"
#include "network.h"
#include "save_network.h"
#include "tools.h"

struct Neural** network = NULL;

int main(int argc, char** argv) {
    GtkWidget* window = NULL;
    GtkWidget* vbox = NULL;
    GtkWidget* tools = NULL;
    GtkWidget* panels = NULL;
    GtkWidget* menu_bar = NULL;
    GtkWidget* notebook = NULL;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sudoku OCR");

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    panels = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    tools = gtk_grid_new();
    notebook = gtk_notebook_new();
    menu_bar = menubar_create(notebook);
    tools_populate(tools, notebook);
    gtk_widget_show_all(notebook);

    gtk_paned_pack1(GTK_PANED(panels), tools, TRUE, TRUE);
    gtk_paned_pack2(GTK_PANED(panels), notebook, TRUE, TRUE);

    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(vbox), panels, TRUE, TRUE, 0);

    g_signal_connect(
        G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    if (network) {
        free_network(network);
    }

    return 0;
}
