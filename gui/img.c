#include "img.h"

#include <math.h>
#include <stdlib.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#ifdef GLIB_DEPRECATED_MACRO_FOR
#undef GLIB_DEPRECATED_MACRO_FOR
#define GLIB_DEPRECATED_MACRO_FOR(x)
#endif

#include "filter.h"
#include "hough.h"
#include "line.h"
#include "mat.h"
#include "network.h"

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
    private->angle = 90.0;
    private->lines = NULL;
    private->line_count = 0;
    g_signal_connect(G_OBJECT(self), "draw", G_CALLBACK(image_draw), private);
    return self;
}

GtkWidget* image_from_pixbuf(GdkPixbuf* buffer) {
    GtkWidget* self = GTK_WIDGET(g_object_new(image_get_type(), NULL));
    ImagePrivate* private = IMAGE_PRIVATE(self);
    GError* error = NULL;
    private->origin = buffer;
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
    private->angle = 90.0;
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

    FilterInfo info = { width, height, channels, displayed, working };

    if (modifier == None) {
        goto end;
    }

    {
        filter_black_and_white(&info);
        filter_treshold(&info, filter_optimal_treshold(&info), 255, 0);
    }

    if (modifier == GrayScale) {
        goto end;
    }

    filter_sobel(&info);
    filter_contrast(&info);

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
    cairo_set_source_rgb(transform, 255, 255, 255);
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
        y = private->origin_h * abs_cos;
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

double image_detect_rotation(Image* image) {
    ImagePrivate* private = IMAGE_PRIVATE(image);
    g_object_unref(private->displayed);
    private->displayed = gdk_pixbuf_copy(private->rotated);
    unsigned char* displayed = gdk_pixbuf_get_pixels(private->displayed);
    int width = gdk_pixbuf_get_width(private->displayed);
    int height = gdk_pixbuf_get_height(private->displayed);
    int channels = gdk_pixbuf_get_n_channels(private->displayed);

    unsigned char* working = malloc(width * height * channels);
    memcpy(working, displayed, width * height * channels);

    FilterInfo info = { width, height, channels, displayed, working };
    {
        filter_black_and_white(&info);
        filter_treshold(&info, filter_optimal_treshold(&info), 255, 0);
        filter_sobel(&info);
        filter_contrast(&info);
    }
    private->line_count = 0;
    free(private->lines);
    private->lines = NULL;
    memcpy(working, displayed, width * height * channels);

    HoughParam param
        = { displayed, width, height, channels, 1, G_PI / 180.0, 400 };
    HoughResult result = hough_lines(&param);
    double r = 0.0;

    for (size_t i = 0; i < result.line_count; i++) {
        LinePolar line = result.lines[i];
        double a = cos(line.theta);
        r = a;

        image_rotate(image, (2.0 * G_PI - line.theta) * 180.0 / G_PI);
        break;
    }

    free(working);
    free(result.lines);
    return r;
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
    double det = binv.m00 * binv.m11 - binv.m10 * binv.m01;
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
    double max0 = q->x0 > q->x1 ? q->x0 : q->x1;
    double max1 = q->x2 > q->x3 ? q->x2 : q->x3;
    return round(max1 > max0 ? max1 : max0);
}

static int min_x(const Quad* q) {
    double min0 = q->x0 < q->x1 ? q->x0 : q->x1;
    double min1 = q->x2 < q->x3 ? q->x2 : q->x3;
    return round(min0 < min1 ? min0 : min1);
}

static int max_y(const Quad* q) {
    double max0 = q->y0 > q->y1 ? q->y0 : q->y1;
    double max1 = q->y2 > q->y3 ? q->y2 : q->y3;
    return round(max0 > max1 ? max0 : max1);
}

static int min_y(const Quad* q) {
    double min0 = q->y0 < q->y1 ? q->y0 : q->y1;
    double min1 = q->y2 < q->y3 ? q->y2 : q->y3;
    return round(min0 < min1 ? min0 : min1);
}

static int detect_digit(GdkPixbuf* buffer, const Quad* quad) {
    int x0 = min_x(quad);
    int x1 = max_x(quad);

    int y0 = min_y(quad);
    int y1 = max_y(quad);

    int width = x1 - x0;
    int height = y1 - y0;

    // fprintf(stdout, "Min coord (%d, %d)\n", x0, y0);
    // fprintf(stdout, "Max coord (%d, %d)\n", x1, y1);
    // fprintf(stdout, "Width: %d\nHeight: %d\n", width, height);

    GdkPixbuf* tmp = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(buffer),
        gdk_pixbuf_get_has_alpha(buffer),
        gdk_pixbuf_get_bits_per_sample(buffer), width, height);
    gdk_pixbuf_copy_area(buffer, x0, y0, width, height, tmp, 0, 0);
    unsigned char* pixels = gdk_pixbuf_get_pixels(tmp);
    int channels = gdk_pixbuf_get_n_channels(tmp);

    g_assert(gdk_pixbuf_get_colorspace(tmp) == GDK_COLORSPACE_RGB);

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < height; j++) {
            pixels[channels * (width * j + i)] = 0;
            pixels[channels * (width * j + i) + 1] = 0;
            pixels[channels * (width * j + i) + 2] = 0;
        }
    }
    for (int i = width - 15; i < width; i++) {
        for (int j = 0; j < height; j++) {
            pixels[channels * (width * j + i)] = 0;
            pixels[channels * (width * j + i) + 1] = 0;
            pixels[channels * (width * j + i) + 2] = 0;
        }
    }
    for (int j = 0; j < 15; j++) {
        for (int i = 0; i < width; i++) {
            pixels[channels * (width * j + i)] = 0;
            pixels[channels * (width * j + i) + 1] = 0;
            pixels[channels * (width * j + i) + 2] = 0;
        }
    }
    for (int j = height - 15; j < height; j++) {
        for (int i = 0; i < width; i++) {
            pixels[channels * (width * j + i)] = 0;
            pixels[channels * (width * j + i) + 1] = 0;
            pixels[channels * (width * j + i) + 2] = 0;
        }
    }

    GdkPixbuf* tmp_r = gdk_pixbuf_scale_simple(tmp, 28, 28, GDK_INTERP_HYPER);
    // gdk_pixbuf_save(tmp_r, name, "png", &error, NULL);
    pixels = gdk_pixbuf_get_pixels(tmp_r);
    channels = gdk_pixbuf_get_n_channels(tmp_r);

    width = 28;
    height = 28;
    int found = 0;

    double* vect = malloc(width * height * sizeof(double));
    size_t it = 0;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            double v_pixel = pixels[channels * (j * height + i)];
            vect[it++] = v_pixel / 128.0;
            if (j > 2 && j < height - 2 && i > 2 && i < width - 2
                && v_pixel > 0.1) {
                found += 1;
            }
        }
    }

    int t = found;
    if (t > 15) {
        t = get_res_from_vect(vect, network);
    } else {
        t = 0;
    }
    fprintf(stdout, "Final: %d\n", t);
    return t;
}

int** image_get_case(Image* image) {
    ImagePrivate* private = IMAGE_PRIVATE(image);

    if (private->line_count == 0) {
        return NULL;
    }
    Line* vert = NULL;
    int vert_count = 0;
    Line* hor = NULL;
    int hor_count = 0;
    int** grid = NULL;

    for (size_t i = 0; i < private->line_count; i++) {
        Line line = private->lines[i];
        if (fabs(1.0 + line.dir_x) <= TRESHOLD) {
            vert = realloc(vert, (vert_count + 1) * sizeof(Line));
            vert[vert_count] = line;
            vert_count++;
        } else if (fabs(1.0 - line.dir_y) <= TRESHOLD) {
            hor = realloc(hor, (hor_count + 1) * sizeof(Line));
            hor[hor_count] = line;
            hor_count++;
        }
    }

    if (vert_count == 0 || hor_count == 0) {
        return NULL;
    }

    for (int i = 0; i < hor_count; i++) {
        int mini = i;
        for (int j = i; j < hor_count; j++) {
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

    for (int i = 0; i < vert_count; i++) {
        int mini = i;
        for (int j = i; j < vert_count; j++) {
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
    GdkPixbuf* buffer = gdk_pixbuf_copy(private->rotated);
    unsigned char* displayed = gdk_pixbuf_get_pixels(buffer);
    int width = gdk_pixbuf_get_width(buffer);
    int height = gdk_pixbuf_get_height(buffer);
    int channels = gdk_pixbuf_get_n_channels(buffer);
    FilterInfo info = { width, height, channels, displayed, NULL };
    {
        filter_black_and_white(&info);
        filter_treshold(&info, filter_optimal_treshold(&info), 255, 0);
    }

    Quad* quads = malloc((hor_count - 1) * (vert_count - 1) * sizeof(Quad));
    size_t quad_count = 0;
    grid = malloc(sizeof(int*) * 9);
    fprintf(stdout, "%d %d\n", (vert_count), hor_count);
    for (int i = 0; i < 9; i++) {
        grid[i] = malloc(sizeof(int) * 9);
        for (int j = 0; j < 9; j++) {
            Matrix12 p0, p1, p2, p3;
            p0 = intersection(&vert[i], &hor[j]);
            p1 = intersection(&vert[i + 1], &hor[j]);
            p2 = intersection(&vert[i + 1], &hor[j + 1]);
            p3 = intersection(&vert[i], &hor[j + 1]);

            // if (p0.m00 < 0 || p1.m00 < 0 || p2.m00 < 0 || p3.m00 < 0) {
            //     continue;
            // }
            //
            Quad q = { p0.m00, p0.m10, p1.m00, p1.m10, p2.m00, p2.m10, p3.m00,
                p3.m10 };

            // fprintf(stdout, "|%f %f|%f %f|%f %f|%f %f|\n", q.x0, q.y0, q.x1,
            //     q.y1, q.x2, q.y2, q.x3, q.y3);
            quads[quad_count] = q;
            quad_count++;

            grid[i][j] = detect_digit(buffer, &q);
        }
    }

    fprintf(stdout, "\nQuad count: %zu\n", quad_count);
    return grid;
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
        if (fabs(1.0 + line.dir_x) <= TRESHOLD) {
            cairo_set_source_rgb(cr, 0, 1, 0);
        } else if (fabs(1.0 - line.dir_y) <= TRESHOLD) {
            cairo_set_source_rgb(cr, 1, 0, 1);
        } else {
            cairo_set_source_rgb(cr, 1, 0, 0);
        }
        cairo_move_to(
            cr, line.x - 10000.0 * line.dir_x, line.y - 10000.0 * line.dir_y);
        cairo_line_to(
            cr, line.x + 10000.0 * line.dir_x, line.y + 10000.0 * line.dir_y);

        cairo_stroke(cr);
    }
}

GdkPixbuf* image_get_displayed(Image* image) {
    ImagePrivate* private = IMAGE_PRIVATE(image);
    return private->displayed;
}

