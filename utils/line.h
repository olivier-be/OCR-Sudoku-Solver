#pragma once

typedef struct Line Line;
struct Line {
    double x, y;
    double dir_x, dir_y;
};

typedef struct LinePolar LinePolar;
struct LinePolar {
    double rho, theta;
};

typedef struct Quad Quad;
struct Quad {
    double x0, y0;
    double x1, y1;
    double x2, y2;
    double x3, y3;
};
