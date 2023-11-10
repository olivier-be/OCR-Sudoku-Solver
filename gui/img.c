#include "img.h"

#include <math.h>
#include <stdlib.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#ifdef GLIB_DEPRECATED_MACRO_FOR
#undef GLIB_DEPRECATED_MACRO_FOR
#define GLIB_DEPRECATED_MACRO_FOR(x)
#endif

#include "hough.h"
#include "line.h"
#include "mat.h"

#define FILTER_LOW 30
#define FILTER_HIGH 190
#define TRESHOLD 0.1
#define OOI 1000000

#define IMAGE_PRIVATE(obj)                                                     \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), IMAGE_TYPE, ImagePrivate))

#ifdef __clang__
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined __GNUC__
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

typedef struct ImagePrivate ImagePrivate;
struct ImagePrivate {
    GdkPixbuf* origin;
    GdkPixbuf* rotated;
    GdkPixbuf* displayed;
    int origin_w, origin_h;
    int displayed_w, displayed_h;

    size_t line_count;
    Line* lines;

    ImageModifier current;
    double angle;
};

void image_draw(GtkWidget* area, cairo_t* cr, ImagePrivate* data);
G_DEFINE_TYPE(Image, image, GTK_TYPE_WIDGET)

static void image_class_init(ImageClass* self) {
    g_type_class_add_private(self, sizeof(ImagePrivate));
}

static void image_init(Image* self) {
    gtk_widget_set_has_window(GTK_WIDGET(self), FALSE);
    ImagePrivate* private = IMAGE_PRIVATE(self);
    private->origin = NULL;
    private->origin_w = 0;
    private->origin_h = 0;
}

GtkWidget* image_new(const char* file) {
    GtkWidget* self = GTK_WIDGET(g_object_new(image_get_type(), NULL));
    ImagePrivate* private = IMAGE_PRIVATE(self);
    GError* error = NULL;
    private->origin = gdk_pixbuf_new_from_file(file, &error);
    private->current = None;
    if (error) {
        gtk_widget_destroy(self);
        fprintf(stdout, "Failed to create a new image\n");
        return NULL;
    }
    private->origin_w = gdk_pixbuf_get_width(private->origin);
    private->origin_h = gdk_pixbuf_get_height(private->origin);
    private->displayed_w = private->origin_w;
    private->displayed_h = private->origin_h;
    private->displayed = gdk_pixbuf_copy(private->origin);
    private->rotated = gdk_pixbuf_copy(private->origin);
    private->angle = 0.0;
    private->lines = NULL;
    private->line_count = 0;
    g_signal_connect(G_OBJECT(self), "draw", G_CALLBACK(image_draw), private);
    return self;
}

// TODO fix memory leak
void image_to_display(Image* image, ImageModifier modifier) {
    ImagePrivate* private = IMAGE_PRIVATE(image);
    if (private->current == modifier) {
        return;
    }
    private->current = modifier;
    g_object_unref(private->displayed);
    private->displayed = gdk_pixbuf_copy(private->rotated);
    unsigned char* displayed = gdk_pixbuf_get_pixels(private->displayed);
    int width = gdk_pixbuf_get_width(private->displayed);
    int height = gdk_pixbuf_get_height(private->displayed);
    int channels = gdk_pixbuf_get_n_channels(private->displayed);

    unsigned char* working = malloc(width * height * channels);
    memcpy(working, displayed, width * height * channels);

    if (modifier == None) {
        goto end;
    }

    for (int i = 0; i < width * height; i++) {
        double r = displayed[channels * i];
        double g = displayed[channels * i + 1];
        double b = displayed[channels * i + 2];

        double gray = 0.2126 * r + 0.7152 * g + 0.0722 * b;
        displayed[channels * i] = gray;
        displayed[channels * i + 1] = gray;
        displayed[channels * i + 2] = gray;
    }

    if (modifier == GrayScale) {
        goto end;
    }

    for (int i = 0; i < width * height; i++) {
        double tl, t, tr;
        double l, c, r;
        double dl, d, dr;

        tl = i > width ? working[channels * (i - width) - channels] / 255.0
                       : 0.0;
        t = i >= width ? working[channels * (i - width)] / 255.0 : 0.0;
        tr = i > (width - 1)
            ? working[channels * (i - width) + channels] / 255.0
            : 0.0;

        l = i >= 1 ? working[channels * i - channels] / 255.0 : 0.0;
        c = working[channels * i] / 255.0;
        r = i < (width * height - 1) ? working[channels * i + channels] / 255.0
                                     : 0.0;

        dl = i < (width * height - width + 1)
            ? working[channels * i + channels * width - channels] / 255.0
            : 0.0;
        d = i < (width * height - width)
            ? working[channels * i + channels * width] / 255.0
            : 0.0;
        dr = i < (width * height - width - 1)
            ? working[channels * i + channels * width + channels] / 255.0
            : 0.0;

        double final = (tl + 2.0 * t + tr + 2.0 * l + 4.0 * c + 2.0 * r + dl
                           + 2.0 * d + dr)
            / 16.0;
        displayed[channels * i] = final * 255.9999;
        displayed[channels * i + 1] = final * 255.9999;
        displayed[channels * i + 2] = final * 255.9999;
    }

    memcpy(working, displayed, width * height * channels);

    double mini = 1;
    double maxi = 0;
    for (int i = 0; i < width * height; i++) {
        double tl, t, tr;
        double l, r;
        double dl, d, dr;

        tl = i > width ? working[channels * (i - width) - channels] / 255.0
                       : 0.0;
        t = i >= width ? working[channels * (i - width)] / 255.0 : 0.0;
        tr = i > (width - 1)
            ? working[channels * (i - width) + channels] / 255.0
            : 0.0;

        l = i >= 1 ? working[channels * i - channels] / 255.0 : 0.0;
        r = i < (width * height - 1) ? working[channels * i + channels] / 255.0
                                     : 0.0;

        dl = i < (width * height - width + 1)
            ? working[channels * i + channels * width - channels] / 255.0
            : 0.0;
        d = i < (width * height - width)
            ? working[channels * i + channels * width] / 255.0
            : 0.0;
        dr = i < (width * height - width - 1)
            ? working[channels * i + channels * width + channels] / 255.0
            : 0.0;

        double gx = (tl + 2.0 * l + dl) - (tr + 2.0 * r + dr);
        double gy = (tl + 2.0 * t + tr) - (dl + 2.0 * d + dr);
        double final = sqrt(gx * gx + gy * gy) / sqrt(32.0);
        if (final < mini) {
            mini = final;
        }
        if (final > maxi) {
            maxi = final;
        }
        displayed[channels * i] = final * 255.9999;
        displayed[channels * i + 1] = final * 255.9999;
        displayed[channels * i + 2] = final * 255.9999;
    }
#if 1
    for (int i = 0; i < width * height; i++) {
        unsigned char value = displayed[channels * i] > FILTER_LOW ? 255 : 0;
        displayed[channels * i] = value;
        displayed[channels * i + 1] = value;
        displayed[channels * i + 2] = value;
    }
#endif
    if (modifier == Border) {
        goto end;
    }

    private->line_count = 0;
    free(private->lines);
    private->lines = NULL;
    memcpy(working, displayed, width * height * channels);

    HoughParam param
        = { displayed, width, height, channels, 1, G_PI / 180.0, 400 };
    HoughResult result = hough_lines(&param);

    for (size_t i = 0; i < result.line_count; i++) {
        LinePolar line = result.lines[i];
        double a = cos(line.theta);
        double b = sin(line.theta);

        double x0 = a * line.rho;
        double y0 = b * line.rho;

        Line draw = { x0, y0, -b, a };
        image_draw_line(image, &draw);
    }

    free(working);
    free(result.lines);
end:
    gtk_widget_queue_draw(GTK_WIDGET(image));
}

// TODO fix memory leak
void image_rotate(Image* image, double angle) {
    ImagePrivate* private = IMAGE_PRIVATE(image);
    private->angle = angle;

    int x, y, new_width, new_height;
    double rad = angle * G_PI / 180.0;
    double abs_cos = fabs(cos(rad));
    double abs_sin = fabs(sin(rad));

    new_width
        = (int)(private->origin_w * abs_cos + private->origin_h * abs_sin);
    new_height
        = (int)(private->origin_w * abs_sin + private->origin_h * abs_cos);

    cairo_surface_t* origin
        = gdk_cairo_surface_create_from_pixbuf(private->origin, 1, NULL);
    cairo_surface_t* result = cairo_image_surface_create(
        CAIRO_FORMAT_ARGB32, new_width, new_height);
    cairo_t* transform = cairo_create(result);
    cairo_set_source_rgb(transform, 0, 0, 0);
    cairo_paint(transform);
    if (angle < 0) {
        while (angle < 0) {
            angle += 360;
        }
    }
    if (angle < 90 && angle >= 0) {
        x = new_width - private->origin_w * abs_cos;
        y = 0;
    } else if (angle < 180 && angle >= 90) {
        x = new_width;
        y = new_height - private->origin_h * abs_sin;
    } else if (angle < 270 && angle >= 180) {
        x = private->origin_w * abs_cos;
        y = new_height;
    } else {
        x = 0;
        y = private->origin_w * abs_sin;
    }

    cairo_translate(transform, x, y);
    cairo_rotate(transform, rad);
    cairo_translate(transform, 0, 0);

    cairo_set_source_surface(transform, origin, 0, 0);
    cairo_paint(transform);

    GdkPixbuf* final
        = gdk_pixbuf_get_from_surface(result, 0, 0, new_width, new_height);
    g_object_unref(private->rotated);
    private->rotated = final;
    private->displayed_w = new_width;
    private->displayed_h = new_height;

    ImageModifier save = private->current;
    private->current = Force;
    cairo_surface_destroy(result);
    cairo_surface_destroy(origin);
    cairo_destroy(transform);

    image_to_display(image, save);
}

void image_split(Image* self) {
    ImagePrivate* private = IMAGE_PRIVATE(self);
    if (!private->lines) {
        return;
    }

    gtk_widget_queue_draw(GTK_WIDGET(self));
}

static Matrix12 intersection(Line* l0, Line* l1) {
    Matrix12 tmp = { l0->x - l1->x, l0->y - l1->y };
    Matrix22 binv = { l1->dir_x, -l0->dir_x, l1->dir_y, -l0->dir_y };

    // Lines are more or less orthogonal, so det != 0
    double det = binv.m00 * binv.m11 + binv.m10 * binv.m01;
    if (fabs(det) < TRESHOLD) {
        return (Matrix12) { -1, -1 };
    }

    Matrix22 inv
        = { binv.m11 / det, -binv.m01 / det, -binv.m10 / det, binv.m00 / det };
    Matrix12 res = { tmp.m00 * inv.m00 + tmp.m10 * inv.m01,
        tmp.m00 * inv.m10 + tmp.m10 * inv.m11 };
    return (
        Matrix12) { l0->x + res.m10 * l0->dir_x, l0->y + res.m10 * l0->dir_y };
}

static int max_x(const Quad* q) {
    double min0 = q->x0 > q->x1 ? q->x0 : q->x1;
    double min1 = q->x2 > q->x3 ? q->x0 : q->x3;
    return round(min0 > min1 ? min0 : min1);
}

static int min_x(const Quad* q) {
    double min0 = q->x0 < q->x1 ? q->x0 : q->x1;
    double min1 = q->x2 < q->x3 ? q->x0 : q->x3;
    return round(min0 < min1 ? min0 : min1);
}

static int max_y(const Quad* q) {
    double min0 = q->y0 > q->y1 ? q->y0 : q->y1;
    double min1 = q->y2 > q->y3 ? q->y0 : q->y3;
    return round(min0 > min1 ? min0 : min1);
}

static int min_y(const Quad* q) {
    double min0 = q->y0 < q->x1 ? q->y0 : q->y1;
    double min1 = q->y2 < q->x3 ? q->y0 : q->y3;
    return round(min0 < min1 ? min0 : min1);
}

static void save_image(GdkPixbuf* buffer, const Quad* quad, const char* name) {
    int x0 = min_x(quad);
    int x1 = max_x(quad);

    int y0 = min_y(quad);
    int y1 = max_y(quad);

    int width = x1 - x0;
    int height = y1 - y0;

    GdkPixbuf* tmp
        = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, width, height);
    gdk_pixbuf_copy_area(buffer, x0, y0, width, height, tmp, 0, 0);

    GError* error = NULL;
    gdk_pixbuf_save(tmp, name, "png", &error, NULL);
}

void image_get_case(Image* image) {
    ImagePrivate* private = IMAGE_PRIVATE(image);

    if (private->line_count == 0) {
        return;
    }
    Line* vert = NULL;
    size_t vert_count = 0;
    Line* hor = NULL;
    size_t hor_count = 0;

    for (size_t i = 0; i < private->line_count; i++) {
        Line line = private->lines[i];
        if (fabs(line.dir_x) <= TRESHOLD) {
            vert = realloc(vert, (vert_count + 1) * sizeof(Line));
            vert[vert_count] = line;
            vert_count++;
        } else {
            hor = realloc(hor, (hor_count + 1) * sizeof(Line));
            hor[hor_count] = line;
            hor_count++;
        }
    }

    for (size_t i = 0; i < hor_count; i++) {
        size_t mini = i;
        for (size_t j = i; j < hor_count; j++) {
            if (hor[j].y < hor[mini].y) {
                mini = j;
            }
        }

        if (mini != i) {
            Line tmp = hor[mini];
            hor[mini] = hor[i];
            hor[i] = tmp;
        }
    }

    for (size_t i = 0; i < vert_count; i++) {
        size_t mini = i;
        for (size_t j = i; j < vert_count; j++) {
            if (vert[j].y < vert[mini].y) {
                mini = j;
            }
        }

        if (mini != i) {
            Line tmp = vert[mini];
            vert[mini] = vert[i];
            vert[i] = tmp;
        }
    }

    Quad* quads = malloc((hor_count - 1) * (vert_count - 1) * sizeof(Quad));
    size_t quad_count = 0;
    for (size_t i = 0; i < (hor_count - 1); i++) {
        for (size_t j = 0; j < (vert_count - 1); j++) {
            Matrix12 p0, p1, p2, p3;
            p0 = intersection(&private->lines[i], &private->lines[j]);
            p1 = intersection(&private->lines[i + 1], &private->lines[j]);
            p2 = intersection(&private->lines[i + 1], &private->lines[j + 1]);
            p3 = intersection(&private->lines[i], &private->lines[j + 1]);

            if (p0.m00 < 0 || p1.m00 < 0 || p2.m00 < 0 || p3.m00 < 0) {
                continue;
            }

            Quad q = { p0.m00, p0.m10, p1.m00, p1.m10, p2.m00, p2.m10, p3.m00,
                p3.m10 };

            fprintf(stdout, "|%f %f|%f %f|%f %f|%f %f|\n", q.x0, q.y0, q.x1,
                q.y1, q.x2, q.y2, q.x3, q.y3);
            quads[quad_count] = q;
            quad_count++;

            char name[512];
            sprintf(name, "img_%zu.png", quad_count);
            save_image(private->rotated, &q, name);
        }
    }

    fprintf(stdout, "Quad count: %zu\n", quad_count);

    gtk_widget_queue_draw(GTK_WIDGET(image));
}

void image_draw_line(Image* image, const Line* line) {
    ImagePrivate* private = IMAGE_PRIVATE(image);
    private->lines
        = realloc(private->lines, (private->line_count + 1) * sizeof(Line));
    private->lines[private->line_count] = *line;
    private->line_count++;
}

void image_draw(GtkWidget* area, cairo_t* cr, ImagePrivate* data) {
    int width, height;

    height = gtk_widget_get_allocated_height(area);
    width = gtk_widget_get_allocated_width(area);

    double scale_w = ((double)width) / ((double)data->displayed_w);
    double scale_h = ((double)height) / ((double)data->displayed_h);

    double final = scale_h > scale_w ? scale_w : scale_h;

    // TODO center the image

    cairo_scale(cr, final, final);
    gdk_cairo_set_source_pixbuf(cr, data->displayed, 0, 0);
    cairo_paint(cr);

    cairo_set_line_width(cr, 3);
    cairo_set_source_rgb(cr, 1, 0, 0);
    for (size_t i = 0; i < data->line_count; i++) {
        Line line = data->lines[i];
        if (fabs(line.dir_x) <= TRESHOLD) {
            cairo_set_source_rgb(cr, 0, 1, 0);
        } else {
            cairo_set_source_rgb(cr, 1, 0, 1);
        }
        cairo_move_to(
            cr, line.x - 10000.0 * line.dir_x, line.y - 10000.0 * line.dir_y);
        cairo_line_to(
            cr, line.x + 10000.0 * line.dir_x, line.y + 10000.0 * line.dir_y);

        cairo_stroke(cr);
    }
}
