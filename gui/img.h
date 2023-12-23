#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

#include "line.h"

extern struct Neural** network;

typedef enum ImageModifier {
    None = 0,
    GrayScale,
    Border,
    Grid,
    Force
} ImageModifier;

G_BEGIN_DECLS

#define IMAGE_TYPE (image_get_type())
#define IMAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IMAGE_TYPE, Image))

typedef struct Image Image;
typedef struct ImageClass ImageClass;

struct Image {
    GtkDrawingArea parent;
};

struct ImageClass {
    GtkDrawingAreaClass parent_class;
};

GType image_get_type(void) G_GNUC_CONST;
GtkWidget* image_new(const char* file);
GtkWidget* image_from_pixbuf(GdkPixbuf* buffer);

void image_to_display(Image* image, ImageModifier modifier);
void image_rotate(Image* image, double angle);
double image_detect_rotation(Image* image);
int** image_get_case(Image* image);
void image_draw_line(Image* image, const Line* line);
GdkPixbuf* image_get_displayed(Image* image);

G_END_DECLS
