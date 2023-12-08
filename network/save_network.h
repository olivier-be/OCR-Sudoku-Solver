#ifndef SAVE_NETWORK_H
#define SAVE_NETWORK_H

void add_dataset_dir(char* path);
void get_dataset(size_t nb, size_t len, double*** dataset_p, int** output_p);
void save_dataset(
    double** dataset, int* value, size_t nb, size_t len, int type);
void save(struct Neural** network);
struct Neural** initfromfile();

#endif
