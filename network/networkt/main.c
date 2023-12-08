#include "load_picture.h"
#include "mnist.h"
#include "network.h"
#include "save_network.h"
#include "train_network.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double randfrom2(double min, double max) {
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

int testnetwork() {
    srand(time(NULL));

    // double array[5] = {2, 2,2,2,2};
    double array[784];
    for (size_t p = 0; p < 784; p++) {
        *(array + p) = randfrom2(0, 255);
        // printf("%f \n",*(v + p));
    }

    struct Neural** network = initnetwork();
    set_network(network, array, 784);

    // printf("pow %i \n",pow2(9,6));
    /*size_t p = 0;
      while (p < 784)
      {
      if ((*(*network + p)).value != 0)
      printf("%f value\n",(*(*network + p)).value); //(*(*network + p)).value);
      p++;
      }*/

    printf("%zu value\n", use_network(network));
    save(network);
    printf("save done\n");
    // free_network(network);
    struct Neural** network2 = initfromfile(e);
    set_network(network2, array, 784);
    printf("%zu value\n", use_network(network2));

    // test for  findvalue in text file
    /*
       FILE * inputFile;
       int endofline= 0;
       inputFile = fopen( "test.txt", "r" );
       char word[50];
       int x =0;
       while (!feof(inputFile))
       {
       x++;
       printf("value find is : %f \n",findvalue(inputFile, &endofline,word));
       }
       fclose( inputFile );
       */

    free_network(network);
    free_network(network2);
    return 0;
}

void trainmnist() {
    load_mnist();
    e[0] = 784;
    srand(time(NULL));
    struct Neural** network = initfromfile(e);
    size_t z = 0;

    double** train2 = malloc(sizeof(double**) * NUM_TRAIN);
    while (z < NUM_TRAIN) {
        *(train2 + z) = malloc(sizeof(double*) * e[0]);
        size_t y = 0;
        while (y < e[0]) {
            *(*(train2 + z) + y) = train_image[z][y];
            y++;
        }
        z++;
    }
/*
    z = 0;
    while (z < 10) {
        train(network, train2, train_label, 60, 60, e[0]);
        z++;
    }
*/
    int i = 0, found = 0;
    while (i < 60000) {
        set_network(network, train2[i], e[0]);
        int res = use_network(network);
        printf("get %i  exepted %i value is egual = %i \n", res, train_label[i],
            (int)res == train_label[i]);
        if (train_label[i] == (int)res) {
            found++;
        }
        i++;
    }
    printf("nb good is %i \n", found);
    save(network);
    free_network(network);
}





void save_d() {

    e[0] = 784;
    load_mnist();
    srand(time(NULL));
    // struct Neural **network = initnetwork();
    size_t z = 0;
    double** train2 = malloc(sizeof(double**) * NUM_TRAIN);
    while (z < NUM_TRAIN) {
        *(train2 + z) = malloc(sizeof(double*) * e[0]);
        size_t y = 0;
        while (y < e[0]) {
            *(*(train2 + z) + y) = train_image[z][y];
            y++;
        }
        z++;
    }

    // free_network(network);
    // save_dataset(train2,train_label,NUM_TRAIN-1,e[0],0);
    // get_dataset(10,e[0],&tests, &o);
    // save_dataset(train2,train_label,NUM_TRAIN,e[0],0);

    // add_dataset_dir("./archive");
}

/*
void openim()
{
    double * t
=vector_from_image("/home/olivier/Documents/epita-prepa-asm-PROJ-OCR-2027-prs-059/network/archive/eights/eight(1).jpg");
    for (size_t y=0 ; y< e[0]; y++)
    {
        printf("%f\n",*(t+y));
    }
    free(t);
}
*/


void makeds()
{
    e[0] = 784;
    load_mnist();
    srand(time(NULL));
    // struct Neural **network = initnetwork();
    size_t z = 0;
    size_t nbt= 0;
    double** train2 = malloc(sizeof(double**) * NUM_TRAIN);
    while (z < nbt) {
        *(train2 + z) = malloc(sizeof(double*) * e[0]);
        size_t y = 0;
        while (y < e[0]) {
            *(*(train2 + z) + y) = train_image[z][y];
            y++;
        }
        z++;
    }

    // free_network(network);
     save_dataset(train2,train_label,nbt,e[0],0);
    // get_dataset(10,e[0],&tests, &o);
    // save_dataset(train2,train_label,NUM_TRAIN,e[0],0);

    add_dataset_dir("./archive");


}


void train_ai() {
    srand(time(NULL));
     struct Neural **network = initnetwork();
    //struct Neural** network = initfromfile(e);

    double** dataset = NULL;
    int* output = NULL;
    size_t nbt = 1000;
    get_dataset(nbt,e[0], &dataset,&output);
    train(network, dataset, output, 10, nbt, e[0]);
    size_t  i = 0;
    int found = 0;
    while (i < nbt) {
        set_network(network, dataset[i], e[0]);
        int res = use_network(network);
        // printf("get %i  exepted %i value is egual = %i \n",res,output[i],res
        // == output[i] );
        if (output[i] == res) {
            found++;
        }
        i++;
    }
    printf("nb good is %i \n", found);
    save(network);
    free_network(network);
    size_t p = 0;
    while (p < nbt) {
        free(*(dataset + p));
        p++;
    }
    free(dataset);
    free(output);
}

int main() {
    // testnetwork();
    // testtrain();
    // xor();
     trainmnist();
    // save_d();
    // openim();
    //train_ai();
    //makeds();
    return 0;
}
