#include "hough.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265359

static int numangle(double min, double max, double step) {
    int angle = floor((max - min) / step) + 1;
    if (angle > 1 && fabs(PI - (angle - 1) * step) < (step / 2.0)) {
        angle--;
    }
    return angle;
}

static void trig_table(double* sin_table, double* cos_table, int angle,
    double min, double step, double irho) {
    for (int i = 0; i < angle; i++) {
        sin_table[i] = sin(min * irho);
        cos_table[i] = cos(min * irho);

        min += step;
    }
}

static void local_maximum(int rho, int angle, int treshold, const int* accu,
    int** sort_buffer, size_t* buffer_size) {
    size_t size = 0;
    int* sort = NULL;

    for (int r = 0; r < rho; r++) {
        for (int n = 0; n < angle; n++) {
            int base = (n + 1) * (rho + 2) + r + 1;
            if (accu[base] > treshold && accu[base] > accu[base - 1]
                && accu[base] >= accu[base + 1]
                && accu[base] > accu[base - rho - 2]
                && accu[base] >= accu[base + rho + 2]) {
                sort = realloc(sort, (size + 1) * sizeof(int));
                sort[size] = base;
                size++;
            }
        }
    }
    *sort_buffer = sort;
    *buffer_size = size;
}

static void array_sort(int* array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        size_t mini = i;
        for (size_t j = i; j < size; j++) {
            if (array[j] < array[mini]) {
                mini = j;
            }
        }

        if (mini != i) {
            int tmp = array[mini];
            array[mini] = array[i];
            array[i] = tmp;
        }
    }
}

struct FilterLines {
    LinePolar* lines;
    size_t len;
};

static void filter_lines(HoughResult* result) {
    double rho_treshold = 50.0;
    double theta_treshold = 0.7;
    struct FilterLines* filter_out
        = malloc(result->line_count * sizeof(struct FilterLines));
    for (size_t i = 0; i < result->line_count; i++) {
        filter_out[i] = (struct FilterLines) { NULL, 0 };
    }

    for (size_t i = 0; i < result->line_count; i++) {
        for (size_t j = 0; j < result->line_count; j++) {
            if (i == j) {
                continue;
            }

            LinePolar l0 = result->lines[i];
            LinePolar l1 = result->lines[j];

            if (fabs(l0.rho - l1.rho) < rho_treshold
                && fabs(l0.theta - l1.theta) < theta_treshold) {
                filter_out[i].lines = realloc(filter_out[i].lines,
                    (filter_out[i].len + 1) * sizeof(struct FilterLines));
                filter_out[i].lines[filter_out[i].len] = l1;
            }
        }
    }

    int* indices = malloc(result->line_count * sizeof(int));
    char* flags = malloc(result->line_count);
    for (size_t i = 0; i < result->line_count; i++) {
        flags[i] = 1;
        indices[i] = i;
    }

    for (size_t i = 0; i < result->line_count; i++) {
        size_t mini = i;
        for (size_t j = i; j < result->line_count; j++) {
            if (filter_out[indices[mini]].len > filter_out[indices[j]].len) {
                mini = j;
            }
        }

        if (i != mini) {
            int tmp = indices[mini];
            indices[mini] = indices[i];
            indices[i] = tmp;
        }
    }

    for (size_t i = 0; i < result->line_count - 1; i++) {
        if (!flags[i]) {
            continue;
        }

        for (size_t j = i + 1; j < result->line_count; j++) {
            if (!flags[j]) {
                continue;
            }

            LinePolar l0 = result->lines[indices[i]];
            LinePolar l1 = result->lines[indices[j]];
            if (fabs(l0.rho - l1.rho) < rho_treshold
                && fabs(l0.theta - l1.theta) < theta_treshold) {
                flags[indices[j]] = 0;
            }
        }
    }

    size_t it = 0;
    for (size_t i = 0; i < result->line_count; i++) {
        if (flags[i]) {
            result->lines[it] = result->lines[i];
            it++;
        }
    }
    free(indices);
    free(flags);
    for (size_t i = 0; i < result->line_count; i++) {
        free(filter_out[i].lines);
    }
    free(filter_out);
    result->line_count = it;
}

HoughResult hough_lines(HoughParam* info) {
    HoughResult result = { NULL, 0 };

    double irho = 1.0 / info->rho_resolution;
    int max_rho = info->width + info->height;
    int min_rho = -max_rho;

    int angle = numangle(0, PI, info->theta_resolution);
    int rho = round(((max_rho - min_rho) + 1) / info->rho_resolution);

    int* accumulator = malloc((angle + 2) * (rho + 2) * sizeof(int));
    double* cos_table = malloc(angle * sizeof(double));
    double* sin_table = malloc(angle * sizeof(double));
    for (int i = 0; i < ((angle + 2) * (rho + 2)); i++) {
        accumulator[i] = 0;
    }
    trig_table(sin_table, cos_table, angle, 0, info->theta_resolution, irho);

    for (int i = 0; i < info->height; i++) {
        for (int j = 0; j < info->width; j++) {
            if (info->image[i * info->width * info->channels
                    + j * info->channels]) {
                for (int n = 0; n < angle; n++) {
                    int r = round(j * cos_table[n] + i * sin_table[n]);
                    r += (rho - 1) / 2;
                    accumulator[(n + 1) * (rho + 2) + r + 1]++;
                }
            }
        }
    }

    int* sort_buffer = NULL;
    size_t sort_size = 0;
    local_maximum(
        rho, angle, info->treshold, accumulator, &sort_buffer, &sort_size);
    array_sort(sort_buffer, sort_size);

    double scale = 1.0 / (rho + 2);
    result.lines = malloc(sort_size * sizeof(LinePolar));
    result.line_count = sort_size;
    for (size_t i = 0; i < sort_size; i++) {
        LinePolar line;
        int idx = sort_buffer[i];
        int n = floor(idx * scale) - 1;
        int r = idx - (n + 1) * (rho + 2) - 1;
        line.rho = (r - (rho - 1) * 0.5) * info->rho_resolution;
        line.theta = n * info->theta_resolution;

        result.lines[i] = line;
    }

    free(accumulator);
    free(cos_table);
    free(sin_table);
    filter_lines(&result);
    return result;
}
