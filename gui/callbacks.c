#include "callbacks.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "img.h"

#ifdef __clang__
#pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

typedef struct OkData OkData;
struct OkData {
    GtkWidget* selector;
    GtkWidget* window;
    GtkWidget* notebook;
};

void cancel_callback(GtkWidget* widget, GdkEvent* event, GtkWidget* window) {
    gtk_widget_destroy(window);
}

void ok_callback(GtkWidget* widget, GdkEvent* event, OkData* data) {
    const char* file
        = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(data->selector));
    fprintf(stdout, "%s\n", file);
    gtk_widget_destroy(data->window);
}

void open_callback(GtkWidget* widget, GtkWidget* notebook) {
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Open file", NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open",
        GTK_RESPONSE_ACCEPT, NULL);
    int res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        const char* file_name
            = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        GtkWidget* label = gtk_label_new("Sudoku");
        GtkWidget* image = image_new(file_name);

        gtk_notebook_append_page(GTK_NOTEBOOK(notebook), image, label);
        gtk_widget_show_all(notebook);
    }
    gtk_widget_destroy(dialog);
}

void information_callback(void) {
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Information");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    // TODO ajouter bouton OK et texte

    gtk_widget_show_all(window);
}

void credits_callback(void) {
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Credits");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    // TODO ajouter bouton OK et texte

    gtk_widget_show_all(window);
}

void image_set_base(GtkWidget* widget, GtkWidget* notebook) {
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }

    Image* image
        = IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page));
    image_to_display(image, None);
}

void image_set_grayscale(GtkWidget* widget, GtkWidget* notebook) {
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }

    Image* image
        = IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page));
    image_to_display(image, GrayScale);
}

void image_set_border(GtkWidget* widget, GtkWidget* notebook) {
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }

    Image* image
        = IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page));
    image_to_display(image, Border);
}

void image_set_grid_detection(GtkWidget* widget, GtkWidget* notebook) {
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }

    Image* image
        = IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page));
    image_to_display(image, Grid);
}

void image_set_rotation(GtkWidget* widget, GtkWidget* notebook) {
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }

    Image* image
        = IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page));
    double value = gtk_range_get_value(GTK_RANGE(widget));
    image_rotate(IMAGE(image), value);
}

void image_save_grid(GtkWidget* widget, GtkWidget* notebook) {
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }

    Image* image
        = IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page));
    image_get_case(IMAGE(image));
}
