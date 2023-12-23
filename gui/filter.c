#include "filter.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

static void filter_grayscale(FilterInfo* info) {
    for (int i = 0; i < info->width * info->height; i++) {
        double r = info->displayed[info->channels * i];
        double g = info->displayed[info->channels * i + 1];
        double b = info->displayed[info->channels * i + 2];

        double gray = 0.2126 * r + 0.7152 * g + 0.0722 * b;

        info->displayed[info->channels * i] = gray;
        info->displayed[info->channels * i + 1] = gray;
        info->displayed[info->channels * i + 2] = gray;
    }
}

static void filter_smooth(FilterInfo* info) {
    memcpy(info->working, info->displayed,
        info->width * info->height * info->channels);

    for (int i = 0; i < info->width * info->height; i++) {
        double tl, t, tr;
        double l, c, r;
        double dl, d, dr;

        tl = i > info->width
            ? info->working[info->channels * (i - info->width) - info->channels]
                / 255.0
            : 0.0;
        t = i >= info->width
            ? info->working[info->channels * (i - info->width)] / 255.0
            : 0.0;
        tr = i > (info->width - 1)
            ? info->working[info->channels * (i - info->width) + info->channels]
                / 255.0
            : 0.0;

        l = i >= 1 ? info->working[info->channels * i - info->channels] / 255.0
                   : 0.0;
        c = info->working[info->channels * i] / 255.0;
        r = i < (info->width * info->height - 1)
            ? info->working[info->channels * i + info->channels] / 255.0
            : 0.0;

        dl = i < (info->width * info->height - info->width + 1)
            ? info->working[info->channels * i + info->channels * info->width
                  - info->channels]
                / 255.0
            : 0.0;
        d = i < (info->width * info->height - info->width)
            ? info->working[info->channels * i + info->channels * info->width]
                / 255.0
            : 0.0;
        dr = i < (info->width * info->height - info->width - 1)
            ? info->working[info->channels * i + info->channels * info->width
                  + info->channels]
                / 255.0
            : 0.0;

        double final = (tl + 2.0 * t + tr + 2.0 * l + 4.0 * c + 2.0 * r + dl
                           + 2.0 * d + dr)
            / 16.0;

        info->displayed[info->channels * i] = final * 255.9999;
        info->displayed[info->channels * i + 1] = final * 255.9999;
        info->displayed[info->channels * i + 2] = final * 255.9999;
    }
}

static int* filter_get_spectre(FilterInfo* info) {
    int* grayscale = malloc(sizeof(int) * 256);
    memset(grayscale, 0, 256 * sizeof(int));

    for (int i = 0; i < info->width * info->height; i++) {
        grayscale[info->displayed[info->channels * i]] += 1;
    }

    return grayscale;
}

static double cumulative_distribution(
    int* count, int max, int width, int height) {
    double cdf = 0.0;

    for (int i = 0; i < max; i++) {
        cdf += count[i];
    }
    return cdf / ((double)(width * height));
}

void filter_black_and_white(FilterInfo* info) {
    filter_grayscale(info);
    for (int i = 0; i < 0; i++) {
        filter_smooth(info);
    }
    // filter_contrast(info);
}

void filter_sobel(FilterInfo* info) {
    memcpy(info->working, info->displayed,
        info->width * info->height * info->channels);
    for (int i = 0; i < info->width * info->height; i++) {
        double tl, t, tr;
        double l, r;
        double dl, d, dr;

        tl = i > info->width
            ? info->working[info->channels * (i - info->width) - info->channels]
                / 255.0
            : 0.0;
        t = i >= info->width
            ? info->working[info->channels * (i - info->width)] / 255.0
            : 0.0;
        tr = i > (info->width - 1)
            ? info->working[info->channels * (i - info->width) + info->channels]
                / 255.0
            : 0.0;

        l = i >= 1 ? info->working[info->channels * i - info->channels] / 255.0
                   : 0.0;
        r = i < (info->width * info->height - 1)
            ? info->working[info->channels * i + info->channels] / 255.0
            : 0.0;

        dl = i < (info->width * info->height - info->width + 1)
            ? info->working[info->channels * i + info->channels * info->width
                  - info->channels]
                / 255.0
            : 0.0;
        d = i < (info->width * info->height - info->width)
            ? info->working[info->channels * i + info->channels * info->width]
                / 255.0
            : 0.0;
        dr = i < (info->width * info->height - info->width - 1)
            ? info->working[info->channels * i + info->channels * info->width
                  + info->channels]
                / 255.0
            : 0.0;

        double gx = (tl + 2.0 * l + dl) - (tr + 2.0 * r + dr);
        double gy = (tl + 2.0 * t + tr) - (dl + 2.0 * d + dr);
        double final = sqrt(gx * gx + gy * gy) / sqrt(32.0);

        info->displayed[info->channels * i] = final * 255.9999;
        info->displayed[info->channels * i + 1] = final * 255.9999;
        info->displayed[info->channels * i + 2] = final * 255.9999;
    }
}

void filter_contrast(FilterInfo* info) {
    double min = 255;
    double max = 0;
    for (int i = 0; i < info->width * info->height; i++) {
        double v = info->displayed[i * info->channels];
        if (v < min) {
            min = v;
        }
        if (v > max) {
            max = v;
        }
    }

    for (int i = 0; i < info->width * info->height; i++) {
        double v = info->displayed[info->channels * i];

        v = ((v - min) / (max - min)) * 255.9999;
        info->displayed[info->channels * i] = v;
        info->displayed[info->channels * i + 1] = v;
        info->displayed[info->channels * i + 2] = v;
    }
}

static double m0(int t, int* gray, int width, int height) {
    double acc = 0;
    for (int i = 0; i < t; i++) {
        acc += (i * gray[i]);
    }
    return acc
        / ((cumulative_distribution(gray, t, width, height)) * width * height);
}

static double m1(int t, int* gray, int width, int height) {
    double acc = 0;
    for (int i = t; i < 256; i++) {
        acc += (i * gray[i]);
    }
    return acc
        / ((1 - cumulative_distribution(gray, t, width, height)) * width
            * height);
}

static double var(int t, int* gray, int width, int height) {
    double tmp = (m0(t, gray, width, height) - m1(t, gray, width, height));
    return (1 - cumulative_distribution(gray, t, width, height))
        * (cumulative_distribution(gray, t, width, height) * tmp * tmp);
}

int filter_optimal_treshold(FilterInfo* info) {
    int* gray = filter_get_spectre(info);

    double maxv = -INFINITY;
    int m = 0;
    for (int i = 0; i < 256; i++) {
        double tmp = var(i, gray, info->width, info->height);
        if (tmp > maxv) {
            maxv = tmp;
            m = i;
        }
    }
    return m;
}

void filter_treshold(
    FilterInfo* info, int treshold, unsigned char inf, unsigned char sup) {
    for (int j = 0; j < info->height * info->width; j++) {
        int pos = j * info->channels;
        unsigned char result = info->displayed[pos] > treshold ? sup : inf;
        info->displayed[pos] = result;
        info->displayed[pos + 1] = result;
        info->displayed[pos + 2] = result;
    }
}
