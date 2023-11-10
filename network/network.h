#ifndef NETWORK_H
#define NETWORK_H


struct Neural{
       double biases;
       void *previouslayer;
       double *weights;
       double value;
};

size_t e[3] = {784,15,10};

struct Neural** initnetwork();
double sigmoid(double z);
double active_func(struct Neural neural,size_t size);
size_t softmax(struct Neural *neural,size_t size, double *res);
size_t use_network(struct Neural **network);
void free_network(struct Neural **network);
void set_network( struct Neural **network , double *array ,size_t size);


#endif
