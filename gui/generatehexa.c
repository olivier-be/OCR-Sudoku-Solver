#include <err.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>

#include "generatehexa.h"
#include "hexa.h"

#include "img/img_0.h"
#include "img/img_1.h"
#include "img/img_2.h"
#include "img/img_3.h"
#include "img/img_4.h"
#include "img/img_5.h"
#include "img/img_6.h"
#include "img/img_7.h"
#include "img/img_8.h"
#include "img/img_9.h"
#include "img/img_A.h"
#include "img/img_B.h"
#include "img/img_C.h"
#include "img/img_D.h"
#include "img/img_E.h"
#include "img/img_F.h"

#include "img/img_not_solved.h"
#include "img/img_plain.h"

void add_number_hexa(
    guchar* final_img, char number, int x, int y, char mask[], int color) {
    if (mask[x + y * 16] == 0) {
        if (number != -1) {
            mask[x + y * 16] = 1;
        }
        int depx = (10 + 105 * (x % 4) + 425 * (x / 4)) * 3;
        int depy = (10 + 105 * (y % 4) + 425 * (y / 4)) * 3;
        char pixelcolor;
        if (color == 0) {
            pixelcolor = 0;
        } else {
            pixelcolor = 255;
        }

        gchar* number_img = NULL;
        switch (number) {
        case -1:
            number_img = image_plain;
            break;
        case 1:
            number_img = image_1;
            break;
        case 2:
            number_img = image_2;
            break;
        case 3:
            number_img = image_3;
            break;
        case 4:
            number_img = image_4;
            break;
        case 5:
            number_img = image_5;
            break;
        case 6:
            number_img = image_6;
            break;
        case 7:
            number_img = image_7;
            break;
        case 8:
            number_img = image_8;
            break;
        case 9:
            number_img = image_9;
            break;
        case 0:
            number_img = image_0;
            break;
        case 10:
            number_img = image_A;
            break;
        case 11:
            number_img = image_B;
            break;
        case 12:
            number_img = image_C;
            break;
        case 13:
            number_img = image_D;
            break;
        case 14:
            number_img = image_E;
            break;
        case 15:
            number_img = image_F;
            break;
        }

        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                char pixel[3];
                HEADER_PIXEL(number_img, pixel);
                if ((unsigned char)pixel[0] < 255) {
                    final_img[depy * 1710 + depx + i * 1710 * 3 + j * 3]
                        = pixelcolor;
                    final_img[depy * 1710 + depx + i * 1710 * 3 + j * 3 + 1]
                        = 0;
                    final_img[depy * 1710 + depx + i * 1710 * 3 + j * 3 + 2]
                        = 0;

                } else {
                    final_img[depy * 1710 + depx + i * 1710 * 3 + j * 3] = 255;
                    final_img[depy * 1710 + depx + i * 1710 * 3 + j * 3 + 1]
                        = 255;
                    final_img[depy * 1710 + depx + i * 1710 * 3 + j * 3 + 2]
                        = 255;
                }
            }
        }
    }
}
GdkPixbuf* initialization_hexa(const char* grid_path) {
    guchar* final_img = calloc(1710 * 1710 * 3, sizeof(char));
    GdkPixbuf* result;
    /* obtenir grid*/
    int** grid = malloc(sizeof(int*) * 16);
    for (int i = 0; i < 16; i++) {
        grid[i] = malloc(sizeof(int*) * 16);
    }
    readhexadoku(grid_path, grid);

    char* mask = calloc(256, sizeof(char));
    for (int i = 0; i < 16; i++) // x
    {
        for (int j = 0; j < 16; j++) // y
        {
            add_number_hexa(final_img, (char)grid[j][i], i, j, mask, 0);
        }
    }
    result = gdk_pixbuf_new_from_data(final_img, GDK_COLORSPACE_RGB, FALSE, 8,
        1710, 1710, 1710 * 3, NULL, NULL);
    //free(final_img);
    free(mask);
    return result;
}

GdkPixbuf* generate_hexa(const char* grid_path) {
    guchar* final_img = calloc(1710 * 1710 * 3, sizeof(char));
    GdkPixbuf* result;
    /* obtenir grid*/
    int** grid = malloc(sizeof(int*) * 16);
    for (int i = 0; i < 16; i++) {
        grid[i] = malloc(sizeof(int*) * 16);
    }
    readhexadoku(grid_path, grid);

    char* mask = calloc(256, sizeof(char));
    for (int i = 0; i < 16; i++) // x
    {
        for (int j = 0; j < 16; j++) // y
        {
            add_number_hexa(final_img, (char)grid[j][i], i, j, mask, 0);
        }
    }

    // resolution de la grille
    if (solver_hexadoku(grid)) {
        for (int i = 0; i < 16; i++) // x
        {
            for (int j = 0; j < 16; j++) // y
            {
                add_number_hexa(final_img, (char)grid[j][i], i, j, mask, 1);
            }
        }

        for (int i = 0; i < 16; i++) {
            free(grid[i]);
        }
        free(grid);

        free(mask);

        result = gdk_pixbuf_new_from_data(final_img, GDK_COLORSPACE_RGB, FALSE,
            8, 1710, 1710, 1710 * 3, NULL, NULL);
    } else {
        guchar* image2 = get_not_solved_hexa_img();
        result = gdk_pixbuf_new_from_data(image2, GDK_COLORSPACE_RGB, FALSE, 8,
            970, 970, 970 * 3, NULL, NULL);
        free(image2);
    }
    //free(final_img);
    return result;
}

guchar* get_not_solved_hexa_img() {

    guchar* final_img = calloc(970 * 970 * 3, sizeof(char));
    for (int i = 0; i < 970; i++) {
        for (int j = 0; j < 970; j++) {
            char pixel[3];
            HEADER_PIXEL(image_not_solved, pixel);
            if ((unsigned char)pixel[0] < 255) {
                final_img[i * 970 * 3 + j * 3] = 0;
                final_img[i * 970 * 3 + j * 3 + 1] = 0;
                final_img[i * 970 * 3 + j * 3 + 2] = 0;
            } else {
                final_img[i * 970 * 3 + j * 3] = 255;
                final_img[i * 970 * 3 + j * 3 + 1] = 255;
                final_img[i * 970 * 3 + j * 3 + 2] = 255;
            }
        }
    }
    return final_img;
}

#ifndef BUILD_GUI
int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);
    generate_hexa("/home/jaysonvanmarcke/epita-prepa-asm-PROJ-OCR-2027-prs-059/"
                  "test/grid_02");
    //  GtkWidget *window;
    //  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //  g_signal_connect(window,"delete_event",G_CALLBACK(gtk_main_quit),NULL);
    //  gtk_widget_show_all(window); gtk_main();
    return 0;
}
#endif
