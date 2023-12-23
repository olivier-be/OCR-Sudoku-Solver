#include "callbacks.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "save_network.h"

#include "img.h"
#include "generate.h"
#include "generatehexa.h"

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

typedef struct Sudoku Sudoku;
struct Sudoku {
    int size;
    int** sudoku;
};

extern struct Neural*** netwok;

void cancel_callback(GtkWidget* widget, GdkEvent* event, GtkWidget* window) {
    gtk_widget_destroy(window);
}

void ok_callback(GtkWidget* widget, GdkEvent* event, OkData* data) {
    const char* file
        = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(data->selector));
    fprintf(stdout, "%s\n", file);
    gtk_widget_destroy(data->window);
}

void open_image_callback(GtkWidget* widget, GtkWidget* notebook) {
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Open image", NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open",
        GTK_RESPONSE_ACCEPT, NULL);
    int res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        const char* file_name
            = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        GtkWidget* label = gtk_label_new("Sudoku");
        GtkWidget* label_internal = gtk_label_new(" base");
        GtkWidget* image = image_new(file_name);

        GtkWidget* content = gtk_notebook_new();
        gtk_notebook_append_page(GTK_NOTEBOOK(content), image, label_internal);

        gtk_notebook_append_page(GTK_NOTEBOOK(notebook), content, label);
        gtk_widget_show_all(notebook);
    }
    gtk_widget_destroy(dialog);
}

void open_hexa_callback(GtkWidget* widget, GtkWidget* notebook) {
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Open Hexa grid", NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open",
        GTK_RESPONSE_ACCEPT, NULL);
    int res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        const char* file_name
            = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        GtkWidget* label = gtk_label_new("Hexadoku");
        GtkWidget* content = gtk_notebook_new();
        
        GtkWidget* label_internal = gtk_label_new(" Display");
        GtkWidget* image = image_from_pixbuf(initialization_hexa(file_name));
        gtk_notebook_append_page(GTK_NOTEBOOK(content), image, label_internal);
		GdkPixbuf* buffer = generate_hexa(file_name);
		GtkWidget* solution =image_from_pixbuf(buffer);
		GtkWidget* label_internal2 = gtk_label_new(" Solution");
		gtk_notebook_append_page(GTK_NOTEBOOK(content), solution, label_internal2);
		
        gtk_notebook_append_page(GTK_NOTEBOOK(notebook), content, label);
        gtk_widget_show_all(notebook);
    }
    gtk_widget_destroy(dialog);
}

void open_sudoku_callback(GtkWidget* widget, GtkWidget* notebook) {
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Open Sudoku grid", NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open",
        GTK_RESPONSE_ACCEPT, NULL);

    int res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        const char* file_name
            = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        GtkWidget* label = gtk_label_new("sudoku");
        GtkWidget* content = gtk_notebook_new();

        GtkWidget* label_internal = gtk_label_new(" Display");
        GdkPixbuf* buffer = initialization((char *)file_name,NULL);
        GtkWidget* image = image_from_pixbuf(buffer);
        gtk_notebook_append_page(GTK_NOTEBOOK(content), image, label_internal);

        GdkPixbuf* buffer2 = generate((char *)file_name,NULL);
        GtkWidget* solution =image_from_pixbuf(buffer2);
        GtkWidget* label_internal2 = gtk_label_new(" Solution");
        gtk_notebook_append_page(GTK_NOTEBOOK(content), solution, label_internal2);

        gtk_notebook_append_page(GTK_NOTEBOOK(notebook), content, label);
        gtk_widget_show_all(notebook);
    }
    gtk_widget_destroy(dialog);
}

void open_ai_callback(GtkWidget* widget, gpointer ptr __attribute__((unused))) {
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Open file", NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open",
        GTK_RESPONSE_ACCEPT, NULL);
    int res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        const char* file_name
            = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        network = initfromfile(file_name);
        if (network) {
            gtk_widget_set_sensitive(widget, FALSE);
        }
    }
    gtk_widget_destroy(dialog);
}

void information_callback(void) {
    GtkWidget* window = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "%p", NULL);

    gtk_window_set_title(GTK_WINDOW(window), "Informations");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    char* text = "Bienvenue sur notre application qui résout une grille de "
                 "sudoku et qui vous propose différentes "
                 "fonctionnalités.\n\nPour utiliser cette application, il "
                 "suffit de cliquer sur File.\n\nEnsuite, choisissez : "
                 "open network. Ensuite, choisissez une image"
                 "de sudoku depuis votre ordinateur.\n\nPour finir, "
                 "une fois votre image sur l'interface, vous pouvez vous "
                 "amuser avec les différentes manipulations proposées.\n";

    gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(window), text);
    gtk_dialog_run(GTK_DIALOG(window));
    gtk_widget_destroy(window);
}

void credits_callback(void) {
    GtkWidget* window = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "%p", NULL);
    gtk_window_set_title(GTK_WINDOW(window), "Credits");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    char* text = "Ce projet a été créer par:\n\nJayson Vanmarcke\n\nMarc "
                 "Moutarde\n\nVictor Tang\n\nOlivier Bensemhoun";

    gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(window), text);
    gtk_dialog_run(GTK_DIALOG(window));
    gtk_widget_destroy(window);
}

void image_set_base(GtkWidget* widget, GtkWidget* notebook) {
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }

    notebook = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page);
    image_to_display(
        IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 0)), None);
}

void image_set_grayscale(GtkWidget* widget, GtkWidget* notebook) {
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }
    notebook = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page);
    image_to_display(
        IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 0)), GrayScale);
}

void image_set_border(GtkWidget* widget, GtkWidget* notebook) {
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }

    notebook = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page);
    image_to_display(
        IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 0)), Border);
}

void image_set_grid_detection(GtkWidget* widget, GtkWidget* notebook) {
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }

    notebook = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page);
    image_to_display(
        IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 0)), Grid);
}

void image_set_rotation(GtkWidget* widget, GtkWidget* notebook) {
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }

    notebook = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page);
    Image* image = IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 0));
    double value = gtk_range_get_value(GTK_RANGE(widget));
    image_rotate(IMAGE(image), value);
}

void image_auto_rotate(GtkWidget* widget, GtkWidget* notebook) {
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }
    notebook = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page);
    Image* image = IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 0));
    image_detect_rotation(image);
}

void sudoku_display(GtkWidget* widget, GtkWidget* notebook) {
    if (!network) {
        open_ai_callback(NULL, NULL);
    }
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }

    notebook = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page);
    int page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
    if (page_count > 1) {
        return;
    }

    image_to_display(
        IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 0)), Grid);
    int** grid = image_get_case(
        IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 0)));
    if (!grid) {
        return;
    }
    GdkPixbuf* buffer = initialization(NULL, grid);

    GtkWidget* image = image_from_pixbuf(buffer);
    GtkWidget* label = gtk_label_new(" Clean");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), image, label);
    gtk_widget_show_all(notebook);
}

void sudoku_solve(GtkWidget* widget, GtkWidget* notebook) {
    if (!network) {
        open_ai_callback(NULL, NULL);
    }
    GdkPixbuf* buffer;
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }

    notebook = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page);
    int page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
    if (page_count > 2) {
        return;
    }

    image_to_display(
        IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 0)), Grid);
    int** grid = image_get_case(
        IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 0)));
    if (!grid) {
        return;
    }
    if (page_count == 1) {
        buffer = initialization(NULL, grid);
        GtkWidget* image = image_from_pixbuf(buffer);
        GtkWidget* label = gtk_label_new(" Clean");
        gtk_notebook_append_page(GTK_NOTEBOOK(notebook), image, label);
        gtk_widget_show_all(notebook);
    }
    buffer = generate(NULL, grid);

    GtkWidget* image = image_from_pixbuf(buffer);
    GtkWidget* label = gtk_label_new(" Solution");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), image, label);
    gtk_widget_show_all(notebook);
}

void hexadoku_solve(GtkWidget* widget, GtkWidget* notebook) {
    GdkPixbuf* buffer;
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }

    notebook = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page);
    int page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
    if (page_count > 2) {
        return;
    }

    image_to_display(
        IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 0)), Grid);
    int** grid = image_get_case(
        IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 0)));
    if (!grid) {
        return;
    }
    buffer = generate(NULL, grid);

    GtkWidget* image = image_from_pixbuf(buffer);
    GtkWidget* label = gtk_label_new(" Solution");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), image, label);
    gtk_widget_show_all(notebook);
}

void save_callback(GtkWidget* widget, GtkWidget* notebook) {
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == -1) {
        return;
    }
    GtkWidget* notebook2
        = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page);

    int page2 = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook2));
    const char* label
        = gtk_notebook_get_tab_label_text(GTK_NOTEBOOK(notebook), notebook2);
    const char* label3
        = gtk_notebook_get_tab_label_text(GTK_NOTEBOOK(notebook2),
            gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook2), page2));
    char label2[strlen(label) + 1];
    for (size_t i = 0; i < strlen(label) + 1; i++) {
        label2[i] = label[i];
    }
    strcat(label2, label3);
    GdkPixbuf* to_save = image_get_displayed(
        IMAGE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook2), page2)));

    strcat(label2, ".png");
    gdk_pixbuf_save(to_save, label2, "png", NULL, NULL);
}
