#include "network.h"
#include "save_network.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
double sigmoid_prime(double z) { return sigmoid(z) * (1 - sigmoid(z)); }

double sigmoid_prime_o(double z) { return z * (1 - z); }

void free2d(double** tab, size_t s) {
    free(*(tab));
    free(*(tab + 1));
    if (s == 3) {
        free(*(tab + 2));
    }
    free(tab);
}

double active_func_train(struct Neural neural, size_t j) {
    // sigmoid
    double res = 0;
    res += neural.biases;
    for (size_t p = 0; p < e[j]; p++) {
        res += (*(*((struct Neural**)(neural.previouslayer)) + p)).value
            * *(neural.weights + p);
    }
    return res;
}

void use_network_getz(struct Neural** network, double** z) {

    size_t j = 1;
    size_t k = 0;
    while (k < e[0]) {
        (*(*(z) + k)) = (*(*(network) + k)).value;
        k++;
    }
    while (j < 3) {
        k = 0;
        while (k < e[j]) {
            (*(*(z + j) + k))
                = active_func_train((*(*(network + j) + k)), j - 1);
            (*(*(network + j) + k)).value = sigmoid((*(*(z + j) + k)));
            k++;
        }
        j++;
    }
}

void setgradient(double** gradientb, double** gradientw) {
    size_t init = 0;
    while (init < e[1] * e[0]) {
        *(*(gradientb) + init) = 0;
        *(*(gradientw) + init) = 0;
        init++;
    }
    init = 0;
    while (init < e[1] * e[2]) {
        *(*(gradientb + 1) + init) = 0.0;
        *(*(gradientw + 1) + init) = 0.0;
        init++;
    }
}

double cost(struct Neural** network, size_t exepted) {

    struct Neural* output = *(network + 2);
    double cost = 0;
    for (size_t i = 0; i < e[2]; i++) {
        if (i == exepted) {
            cost += 0.5 * ((*(output + i)).value - 1)
                * ((*(output + i)).value - 1);
        } else {
            cost += 0.5 * (0 - (*(output + i)).value)
                * (0 - (*(output + i)).value);
        }
    }
    return cost;
}

double error_pad(double predicted, double exepted) {
    return -(exepted - predicted);
}
double error(double predicted, double exepted) {
    return 0.5 * (predicted - exepted) * (predicted - exepted);
}

double somme_gradient_weight(
    size_t j, size_t pos, struct Neural** neural, double** gradientb) {
    double wpg = 0;
    size_t k = 0;
    while (k < e[j + 1]) {
        wpg += *(*(gradientb + 1) + k * e[j] + pos)
            * *((*(*(neural + j + 1) + k)).weights + pos);
        k++;
    }
    return wpg;
}

void backpropagation(struct Neural** network, size_t exepted, double* set,
    double** w, double** b, double tvs) {
    double** gradientw = malloc(sizeof(double*) * 2);
    *(gradientw) = malloc(sizeof(double) * e[1] * e[0]);
    *(gradientw + 1) = malloc(sizeof(double) * e[2] * e[1]);

    double** gradientb = malloc(sizeof(double*) * 2);
    *(gradientb) = malloc(sizeof(double) * e[1] * e[0]);
    *(gradientb + 1) = malloc(sizeof(double) * e[2] * e[1]);

    double** z = malloc(sizeof(double*) * 3);
    *(z) = malloc(sizeof(double) * e[0]);
    *(z + 1) = malloc(sizeof(double) * e[1]);
    *(z + 2) = malloc(sizeof(double) * e[2]);

    setgradient(gradientb, gradientw);
    set_network(network, set, tvs);
    use_network_getz(network, z);
    // double coste = cost (network , exepted);
    size_t j = 2, k = 0, p = 0;
    double error;
    while (k < e[j]) {
        p = 0;
        double find = 0.0;
        if (k == exepted) {
            find = 1; // 0.99;
        }
        error = error_pad((*(*(network + j) + k)).value, find)
            * sigmoid_prime_o((*(*(network + j) + k)).value);
        while (p < e[j - 1]) {
            *(*(gradientb + j - 1) + p + k * e[j - 1]) = error;
            *(*(gradientw + j - 1) + p + k * e[j - 1])
                = error * (*(*(network + j - 1) + p)).value;
            p++;
        }
        k++;
    }
    j--;
    k = 0;
    p = 0;
    while (j > 0) {
        k = 0;
        while (k < e[j]) {
            p = 0;
            error = somme_gradient_weight(j, k, network, gradientb)
                * sigmoid_prime(*(*(z + j) + k));
            ;
            // check and it corrected to that point
            while (p < e[j - 1]) {
                *(*(gradientb + j - 1) + k * e[j - 1] + p) = error;
                *(*(gradientw + j - 1) + k * e[j - 1] + p)
                    = error * (*(*(network + j - 1) + p)).value;
                p++;
            }
            k++;
        }
        j--;
    }
    free2d(z, 3);
    k = 0;
    j = 2;
    while (j > 0) {
        k = 0;
        while (k < e[j]) {
            p = 0;
            while (p < e[j - 1]) {
                *(*(b + j - 1) + k * e[j - 1] + p)
                    += *(*(gradientb + j - 1) + k * e[j - 1] + p);
                *(*(w + j - 1) + k * e[j - 1] + p)
                    += *(*(gradientw + j - 1) + k * e[j - 1] + p);
                p++;
            }
            k++;
        }
        j--;
    }
    free2d(gradientb, 2);
    free2d(gradientw, 2);
}

void gradient_descent(
    struct Neural** network, double** gradientw, double** gradientb) {
    size_t k = 0, p = 0, j = 2;
    double learning_rate = 0.8;
    while (j > 0) {
        k = 0;
        while (k < e[j]) {
            p = 0;
            (*(*(network + j) + k)).biases
                -= learning_rate * *(*(gradientb + j - 1) + k * e[j - 1] + p);
            while (p < e[j - 1]) {
                *((*(*(network + j) + k)).weights + p) -= learning_rate
                    * *(*(gradientw + j - 1) + k * e[j - 1] + p);
                p++;
            }
            k++;
        }
        j--;
    }
}

size_t test_network(struct Neural** network, double** train_label,
    int* excepted, size_t nbtest) {
    size_t i = 0, found = 0;
    while (i < nbtest) {
        set_network(network, train_label[i], e[0]);
        int res = use_network(network);
        if (excepted[i] == res) {
            found++;
        }
        i++;
    }
    return found;
}

void train(struct Neural** network, double** vectorin, int* vectorout,
    size_t nb_batch, size_t nbtest, double tvs) {

    struct Neural** networkb = network;
    size_t m = 0;

    double** gradientw = malloc(sizeof(double*) * 2);
    *(gradientw) = malloc(sizeof(double) * e[1] * e[0]);
    *(gradientw + 1) = malloc(sizeof(double) * e[2] * e[1]);

    double** gradientb = malloc(sizeof(double*) * 2);
    *(gradientb) = malloc(sizeof(double) * e[1] * e[0]);
    *(gradientb + 1) = malloc(sizeof(double) * e[2] * e[1]);

    size_t pt = 0, same = 0;
    size_t res = 0;
    size_t nb = 0;
    size_t obj = (size_t)(nbtest * 0.95);
    printf("%zu obj \n", obj);
    while (res < obj) {
        if (same == 30) {
            free_network(network);
            network = initnetwork();
        }
        pt = 0;
        while (pt * nb_batch < nbtest) {
            setgradient(gradientb, gradientw);
            size_t nbtestc = 0;
            while (nbtestc < nb_batch) {
                backpropagation(network,
                    (double)(*(vectorout + nbtestc + (pt * nb_batch))),
                    *(vectorin + nbtestc + (pt * nb_batch)), gradientw,
                    gradientb, tvs);
                nbtestc++;
            }
            size_t n = 0;
            while (n < 2) {
                // printf("try add \n")
                size_t pos = 0;
                while (pos < e[n]) {
                    size_t p = 0;
                    while (p < e[n + 1]) {
                        *(*(gradientb + n) + p + pos * e[n + 1]) /= nbtest;

                        *(*(gradientw + n) + p + pos * e[n + 1]) /= nbtest;

                        p++;
                    }
                    pos++;
                }
                n++;
            }
            gradient_descent(network, gradientw, gradientb);
            pt++;
        }
	if (nb == 10)
	{
        size_t r = test_network(network, vectorin, vectorout, nbtest);
         printf("nb g %zu\n",r);
        if (res == r) {
            same++;
        } else {
            same = 0;
            res = r;
        }
        if (same == 10) {
            same = 0;
            network = initnetwork();
            // printf("new done\n ");
            if (r > m) {
                if (networkb != network) {
                    free_network(networkb);
                }
                // printf("change best\n");
                m = r;
                networkb = network;
            }
        }
            if (r > m) {
                if (networkb != network) {
                    free_network(networkb);
                }
                // printf("change best\n");
                m = r;
                networkb = network;
            }
            // printf("make 1000 \n");
            nb = 0;
            save(networkb);
	}
	nb ++;
    }
    free2d(gradientw, 2);
    free2d(gradientb, 2);
}
