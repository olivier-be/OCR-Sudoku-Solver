#pragma once

#include <stddef.h>

#include "line.h"

typedef struct HoughResult HoughResult;
struct HoughResult {
    LinePolar* lines;
    size_t line_count;
};

typedef struct HoughParam HoughParam;
struct HoughParam {
    unsigned char* image;
    int width, height, channels;

    double rho_resolution, theta_resolution;
    int treshold;
};

HoughResult hough_lines(HoughParam* info);
