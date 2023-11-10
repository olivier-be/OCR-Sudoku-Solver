/**
 * Déclaration de l'ensemble des callbacks
 */
#pragma once

#include <gtk/gtk.h>

///////////////////////////////// MENUBAR FILE MENU ////////////////////////////
void open_callback(GtkWidget* widget, GtkWidget* notebook);

///////////////////////////////// MENUBAR HELP MENU ///////////////////////////
void information_callback(void);
void credits_callback(void);

///////////////////////////////// IMAGE FUNCTIONS /////////////////////////////
void image_set_base(GtkWidget* widget, GtkWidget* notebook);
void image_set_grayscale(GtkWidget* widget, GtkWidget* notebook);
void image_set_border(GtkWidget* widget, GtkWidget* notebook);
void image_set_grid_detection(GtkWidget* widget, GtkWidget* notebook);
void image_set_rotation(GtkWidget* widget, GtkWidget* notebook);
void image_save_grid(GtkWidget* widget, GtkWidget* notebook);
