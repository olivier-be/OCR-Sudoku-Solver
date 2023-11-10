#include "tools.h"

#include "callbacks.h"

void tools_populate(GtkWidget* box, GtkWidget* notebook) {
    GtkWidget* base = NULL;
    GtkWidget* grayscale = NULL;
    GtkWidget* border = NULL;
    GtkWidget* grid_detection = NULL;
    GtkWidget* split_image = NULL;
    GtkWidget* rotation = NULL;

    base = gtk_button_new_with_label("Base");
    grayscale = gtk_button_new_with_label("Grayscale");
    border = gtk_button_new_with_label("Borders");
    grid_detection = gtk_button_new_with_label("Grid detection");
    split_image = gtk_button_new_with_label("Split image");
    rotation = gtk_scale_new(
        GTK_ORIENTATION_HORIZONTAL, gtk_adjustment_new(0, 0, 360, 1, 1, 1));

    gtk_grid_attach(GTK_GRID(box), base, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(box), grayscale, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(box), border, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(box), grid_detection, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(box), split_image, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(box), rotation, 0, 3, 2, 1);

    g_signal_connect(
        G_OBJECT(base), "clicked", G_CALLBACK(image_set_base), notebook);
    g_signal_connect(G_OBJECT(grayscale), "clicked",
        G_CALLBACK(image_set_grayscale), notebook);
    g_signal_connect(
        G_OBJECT(border), "clicked", G_CALLBACK(image_set_border), notebook);
    g_signal_connect(G_OBJECT(grid_detection), "clicked",
        G_CALLBACK(image_set_grid_detection), notebook);
    g_signal_connect(G_OBJECT(split_image), "clicked",
        G_CALLBACK(image_save_grid), notebook);
    g_signal_connect(G_OBJECT(rotation), "value-changed",
        G_CALLBACK(image_set_rotation), notebook);
}
