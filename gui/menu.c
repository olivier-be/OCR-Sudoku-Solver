#include "menu.h"

#include <gtk/gtk.h>

#include "callbacks.h"

GtkWidget* menubar_create(GtkWidget* window) {
    GtkWidget* result = gtk_menu_bar_new();

    GtkWidget* file_menu = file_menu_create(window);
    GtkWidget* edition_menu = edition_menu_create();
    GtkWidget* help_menu = help_menu_create();

    gtk_menu_shell_append(GTK_MENU_SHELL(result), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(result), edition_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(result), help_menu);
    return result;
}

GtkWidget* file_menu_create(GtkWidget* notebook) {
    GtkWidget* result = gtk_menu_item_new_with_label("File");
    GtkWidget* item_open = gtk_menu_item_new_with_label("Open");
    GtkWidget* item_quit = gtk_menu_item_new_with_label("Quit");

    GtkWidget* menu_items = gtk_menu_new();

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(result), menu_items);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), item_open);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), item_quit);

    g_signal_connect(
        G_OBJECT(item_open), "activate", G_CALLBACK(open_callback), notebook);
    g_signal_connect(
        G_OBJECT(item_quit), "activate", G_CALLBACK(gtk_main_quit), NULL);
    return result;
}

GtkWidget* edition_menu_create(void) {
    GtkWidget* result = gtk_menu_item_new_with_label("Edition");
    return result;
}

GtkWidget* help_menu_create(void) {
    GtkWidget* result = gtk_menu_item_new_with_label("Help");
    GtkWidget* item_info = gtk_menu_item_new_with_label("Information");
    GtkWidget* item_credits = gtk_menu_item_new_with_label("Credits");

    GtkWidget* menu_items = gtk_menu_new();

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(result), menu_items);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), item_info);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), item_credits);

    g_signal_connect(G_OBJECT(item_info), "activate",
        G_CALLBACK(information_callback), NULL);
    g_signal_connect(
        G_OBJECT(item_credits), "activate", G_CALLBACK(credits_callback), NULL);
    return result;
}
