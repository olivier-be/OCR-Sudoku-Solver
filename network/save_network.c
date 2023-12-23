// #include "load_picture.h"
#include "network.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void save(struct Neural** network) {
    FILE* inputFile;
    inputFile = fopen("test2.txt", "w");
    if (inputFile == NULL) {
        fprintf(stderr, "Cannot open file \n");
        exit(0);
    }
    size_t n = 0;
    while (n < 3) {
        fprintf(inputFile, "p\n");
        size_t i = 0;
        while (i < e[n]) {
            fprintf(inputFile, "%f ", (*(*(network + n) + i)).biases);
            double* v = (*(*(network + n) + i)).weights;
            if (v != NULL) {
                for (size_t p = 0; p < e[n - 1]; p++) {
                    fprintf(inputFile, "%.10lf ", *(v + p));
                }
            }
            fprintf(inputFile, "\n");
            i++;
        }
        n++;
    }
    fclose(inputFile);
}

double findvalue(FILE* inputFile, int* endofline, char* word2) {
    char word[50];
    size_t pw = 0;
    char ch = 0;
    if (!feof(inputFile)) {
        ch = fgetc(inputFile);
    }
    if (!feof(inputFile) && (ch == '\n' || ch == ' ')) {
        ch = fgetc(inputFile);
    }
    while (!feof(inputFile) && ch != '\n' && ch != ' ') {
        if (pw < 49) {
            word[pw] = ch;
            word2[pw] = ch;
        }
        pw++;
        ch = fgetc(inputFile);
    }
    if (pw < 49) {
        word[pw] = 0;
        word2[pw] = 0;
    } else {
        word[49] = 0;
        word2[49] = 0;
    }
    if (ch == '\n') {
        *endofline = 1;
    }
    double d;
    sscanf(word, "%lf", &d);
    return d;
}

struct Neural** initfromfile(const char* file) {
    FILE* inputFile;
    int endofline = 0;
    inputFile = fopen(file, "r");
    if (inputFile == NULL) {
        fprintf(stderr, "Cannot open file \n");
        exit(0);
    }
    struct Neural** network = malloc(sizeof(struct Neural*) * 3);
    size_t n = 0;
    char ch;
    char word[50];
    while (n < 3) {
        while (!feof(inputFile) && (ch = fgetc(inputFile)) != 'p') { }
        ch = fgetc(inputFile);
        *(network + n) = malloc(sizeof(struct Neural) * e[n]);
        size_t i = 0;
        while (i < e[n]) {

            endofline = 0;
            if (n != 0) {
                (*(*(network + n) + i)).biases
                    = findvalue(inputFile, &endofline, word);
                (*(*(network + n) + i)).previouslayer
                    = (void*)(network + n - 1);
                (*(*(network + n) + i)).value = 1;

                double* v = calloc(e[n - 1], sizeof(double));

                for (size_t p = 0; p < e[n - 1]; p++) {

                    if (endofline == 1 && p + 1 < e[n - 1]) {

                        printf("error bad size pos : %zu %zu %f\n", n, p,
                            findvalue(inputFile, &endofline, word));
                        exit(1);
                    }
                    *(v + p) = findvalue(inputFile, &endofline, word);
                }
                (*(*(network + n) + i)).weights = v;
            } else {
                (*(*(network + n) + i)).biases
                    = findvalue(inputFile, &endofline, word);
                (*(*(network + n) + i)).previouslayer = NULL;
                (*(*(network + n) + i)).value = 0;
                (*(*(network + n) + i)).weights = NULL;
            }
            i++;
        }
        n++;
    }
    fclose(inputFile);
    return network;
}

void save_dataset(
    double** dataset, int* value, size_t nb, size_t len, int type) {
    FILE* inputFile;
    if (type == 0) {
        inputFile = fopen("dataset.txt", "w");
    } else {
        inputFile = fopen("dataset.txt", "a");
    }
    if (inputFile == NULL) {
        fprintf(stderr, "Cannot open file \n");
        exit(0);
    }
    size_t n = 0;
    while (n < nb) {
        size_t i = 0;
        while (i < len) {
            if (*(*(dataset + n) + i) == 0) {
                fprintf(inputFile, "0 ");

            } else {
                fprintf(inputFile, "%f ", *(*(dataset + n) + i));
            }
            i++;
        }
        fprintf(inputFile, "%i\n", *(value + n));

        n++;
    }
    fclose(inputFile);
}

double findvalue_dataset(FILE* inputFile, int* endofline, char* word2) {
    char word[50];
    size_t pw = 0;
    char ch = 0;
    if (!feof(inputFile)) {
        ch = fgetc(inputFile);
    }
    if (!feof(inputFile) && (ch == '\n' || ch == ' ')) {
        ch = fgetc(inputFile);
    }
    while (!feof(inputFile) && ch != '\n' && ch != ' ') {
        if (pw < 49) {
            word[pw] = ch;
            word2[pw] = ch;
        }
        pw++;
        ch = fgetc(inputFile);
    }
    if (pw < 49) {
        word[pw] = 0;
        word2[pw] = 0;
    } else {
        word[49] = 0;
        word2[49] = 0;
    }
    if (ch == '\n') {
        *endofline = 1;
    }
    double d;
    sscanf(word, "%lf", &d);
    return d;
}

void get_dataset(size_t nb, size_t len, double*** dataset_p, int** output_p) {
    double** dataset = *dataset_p;
    int* output = *output_p;
    FILE* inputFile;
    int endofline = 0;
    inputFile = fopen("dataset.txt", "r");
    if (inputFile == NULL) {
        fprintf(stderr, "Cannot open file \n");
        exit(0);
    }
    dataset = malloc(sizeof(double*) * nb);
    output = malloc(sizeof(int) * nb);
    size_t n = 0;
    char word[10];
    while (n < nb) {
        *(dataset + n) = malloc(sizeof(double) * (len + 1));
        size_t i = 0;
        while (i < len + 1) {

            endofline = 0;
            double t = (findvalue_dataset(inputFile, &endofline, word));

            if (endofline == 1) {
                *(output + n) = (int)t;
            } else {
                (*(*(dataset + n) + i)) = t;
            }
            i++;
        }
        n++;
    }
    fclose(inputFile);
    *dataset_p = dataset;
    *output_p = output;
}

void show_dir_content(char* path, int res) {
    DIR* d;
    res = res;
    struct dirent* dir;
    printf(" 8 %s test \n", path);
    d = opendir(path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type != 4 || 0) {
                char d_path[255];
                sprintf(d_path, "%s/%.253s", path, dir->d_name);
               // double* ve = vector_from_image(d_path);
               // save_dataset(&ve, &res, 1, 784, 1);
               // free(ve);
            }
        }
        closedir(d);
    }
}

void add_dataset_dir(char* path __attribute__((unused))) {
    DIR* d;
    struct dirent* dir;
    d = opendir(path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
            if (dir->d_type == 4 && *(dir->d_name) != '.') {
                char d_path[255];
                sprintf(d_path, "%s/%.253s", path, dir->d_name);
                show_dir_content(d_path, *(dir->d_name) - '0');
            }
        }
        closedir(d);
    }
}
