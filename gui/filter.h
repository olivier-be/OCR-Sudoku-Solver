#pragma once

typedef struct FilterInfo FilterInfo;
struct FilterInfo {
    int width;
    int height;
    int channels;

    unsigned char* displayed;
    unsigned char* working;
};

void filter_black_and_white(FilterInfo* info);
void filter_sobel(FilterInfo* info);
void filter_contrast(FilterInfo* info);
int filter_optimal_treshold(FilterInfo* info);
void filter_treshold(
    FilterInfo* info, int treshold, unsigned char inf, unsigned char sup);
