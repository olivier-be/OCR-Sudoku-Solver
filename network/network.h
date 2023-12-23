#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>

struct Neural {
    double biases;
    void* previouslayer;
    double* weights;
    double value;
};

extern size_t e[3];

struct Neural** initnetwork();
double sigmoid(double z);
double active_func(struct Neural neural, size_t size);
int softmax(struct Neural* neural, size_t size, double* res);
int use_network(struct Neural** network);
void free_network(struct Neural** network);
void set_network(struct Neural** network, double* array, size_t size);
int get_res_from_vect(double* vect, struct Neural** network);

#endif
