/**
 * Déclaration de l'ensemble des menus
 */
#pragma once

#include <gtk/gtk.h>

GtkWidget* menubar_create(GtkWidget* notebook);
GtkWidget* file_menu_create(GtkWidget* notebook);
GtkWidget* edition_menu_create(GtkWidget* notebook);
GtkWidget* help_menu_create(void);
