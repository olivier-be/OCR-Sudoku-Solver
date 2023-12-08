#include "network.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

size_t e[3] = { 784, 15, 10 };

double randfrom(double min, double max) {
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

struct Neural** initnetwork() {

    struct Neural** network = malloc(sizeof(struct Neural*) * 3);

    size_t n = 0;

    while (n < 3) {
        *(network + n) = malloc(sizeof(struct Neural) * e[n]);
        size_t i = 0;
        while (i < e[n]) {
            if (n != 0) {
                (*(*(network + n) + i)).biases = 1;
                (*(*(network + n) + i)).previouslayer
                    = (void*)(network + n - 1);
                (*(*(network + n) + i)).value = 1;

                double* v = calloc(e[n - 1], sizeof(double));
                for (size_t p = 0; p < e[n - 1]; p++) {
                    *(v + p) = randfrom(-1, 1);
                }
                (*(*(network + n) + i)).weights = v;
            } else {
                (*(*(network + n) + i)).biases = 0;
                (*(*(network + n) + i)).previouslayer = NULL;
                (*(*(network + n) + i)).value = 0;
                (*(*(network + n) + i)).weights = NULL;
            }
            i++;
        }
        n++;
    }
    return network;
}

double sigmoid(double z) { return 1 / (1 + exp(-z)); }

double active_func(struct Neural neural, size_t size) {
    // sigmoid
    double res = 0;
    res += neural.biases;
    for (size_t p = 0; p < size; p++) {
        res += (*(*((struct Neural**)(neural.previouslayer)) + p)).value
            * *(neural.weights + p);
    }
    return res;
}

size_t softmax(struct Neural* neural, size_t size, double* res) {
    double down = 0;
    for (size_t i = 0; i < size; i++) {
        *(res + i) = exp((*(neural + i)).value);
        down += *(res + i);
    }

    for (size_t i = 0; i < size; i++) {
        *(res + i) /= down;
        (*(neural + i)).value = *(res + i);
    }
    double max = res[0];
    size_t p = 0;
    for (size_t i = 0; i < 10; i++) {
        if (max < res[i]) {
            p = i;
            max = res[i];
        }
    }
    return p;
}

size_t use_network(struct Neural** network) {

    size_t n = 1;
    size_t i = 0;
    while (n < 3) {
        i = 0;
        while (i < e[n]) {
            (*(*(network + n) + i)).value
                = sigmoid(active_func((*(*(network + n) + i)), e[n - 1]));
            i++;
        }
        n++;
    }
    double res[10];
    return softmax(*(network + 2), e[2], res);
}

void free_network(struct Neural** network) {
    size_t n = 0, i;
    while (n < 3) {
        i = 0;
        while (i < e[n]) {
            free((*(*(network + n) + i)).weights);
            i++;
        }
        free(*(network + n));
        n++;
    }
    free(network);
}

void set_network(struct Neural** network, double* array, size_t size) {
    size_t p = 0;
    while (p < size && p < e[0]) {
        (*(*network + p)).value = *(array + p);
        p++;
    }
    while (p < e[0]) {
        (*(*network + p)).value = 0;
        p++;
    }
}

int get_res_from_vect(double* vect, struct Neural** network) {
    // struct Neural **network = initnetwork();
    set_network(network, vect, e[0]);
    int res = use_network(network);
    // free_network(network);
    return res;
}
