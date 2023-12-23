/**
 * Déclaration de l'ensemble des callbacks
 */
#pragma once

#include <gtk/gtk.h>

///////////////////////////////// MENUBAR FILE MENU ////////////////////////////
void open_image_callback(GtkWidget* widget, GtkWidget* notebook);
void open_ai_callback(GtkWidget* widget, gpointer ptr);
void open_hexa_callback(GtkWidget* widget,GtkWidget* notebook);
void open_sudoku_callback(GtkWidget* widget,GtkWidget* notebook);

///////////////////////////////// MENUBAR HELP MENU ///////////////////////////
void information_callback(void);
void credits_callback(void);

///////////////////////////////// IMAGE FUNCTIONS /////////////////////////////
void image_set_base(GtkWidget* widget, GtkWidget* notebook);
void image_set_grayscale(GtkWidget* widget, GtkWidget* notebook);
void image_set_border(GtkWidget* widget, GtkWidget* notebook);
void image_set_grid_detection(GtkWidget* widget, GtkWidget* notebook);
void image_set_rotation(GtkWidget* widget, GtkWidget* notebook);
void image_auto_rotate(GtkWidget* widget, GtkWidget* notebook);

///////////////////////////////// SUDOKU FUNCTIONS ////////////////////////////
void sudoku_display(GtkWidget* widget, GtkWidget* notebook);
void sudoku_solve(GtkWidget* widget, GtkWidget* notebook);
void save_callback(GtkWidget* widget, GtkWidget* notebook);
