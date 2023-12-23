#include "tools.h"

#include "callbacks.h"

void tools_populate(GtkWidget* box, GtkWidget* notebook) {
    GtkWidget* base = NULL;
    GtkWidget* grayscale = NULL;
    GtkWidget* border = NULL;
    GtkWidget* grid_detection = NULL;
    GtkWidget* rotation = NULL;
    GtkWidget* auto_rotate = NULL;
    GtkWidget* display = NULL;
    GtkWidget* solution = NULL;

    base = gtk_button_new_with_label("Base");
    grayscale = gtk_button_new_with_label("Grayscale");
    border = gtk_button_new_with_label("Borders");
    grid_detection = gtk_button_new_with_label("Grid detection");
    rotation = gtk_scale_new(
        GTK_ORIENTATION_HORIZONTAL, gtk_adjustment_new(0, 0, 360, 1, 1, 1));
    auto_rotate = gtk_button_new_with_label("Auto rotation");
    display = gtk_button_new_with_label("Display");
    solution = gtk_button_new_with_label("Solution");

    gtk_grid_attach(GTK_GRID(box), base, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(box), grayscale, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(box), border, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(box), grid_detection, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(box), rotation, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(box), auto_rotate, 0, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(box), display, 0, 4, 2, 1);
    gtk_grid_attach(GTK_GRID(box), solution, 0, 5, 2, 1);

    g_signal_connect(
        G_OBJECT(base), "clicked", G_CALLBACK(image_set_base), notebook);
    g_signal_connect(G_OBJECT(grayscale), "clicked",
        G_CALLBACK(image_set_grayscale), notebook);
    g_signal_connect(
        G_OBJECT(border), "clicked", G_CALLBACK(image_set_border), notebook);
    g_signal_connect(G_OBJECT(grid_detection), "clicked",
        G_CALLBACK(image_set_grid_detection), notebook);
    g_signal_connect(G_OBJECT(rotation), "value-changed",
        G_CALLBACK(image_set_rotation), notebook);
    g_signal_connect(G_OBJECT(auto_rotate), "clicked",
        G_CALLBACK(image_auto_rotate), notebook);
    g_signal_connect(
        G_OBJECT(display), "clicked", G_CALLBACK(sudoku_display), notebook);
    g_signal_connect(
        G_OBJECT(solution), "clicked", G_CALLBACK(sudoku_solve), notebook);
}
