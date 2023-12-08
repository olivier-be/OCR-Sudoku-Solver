#ifndef TRAIN_NETWORK_H
#define TRAIN_NETWORK_H

void train(struct Neural** network, double** vectorin, int* vectorout,
    size_t nb_batch, size_t nbtest, double tvs);

#endif
