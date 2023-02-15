#pragma once

#include "matrix.h"

typedef struct
{
    matrix *W1;
    matrix *b1;

    matrix *W2;
    matrix *b2;

}parameters;

typedef struct
{
    matrix *A1;
    matrix *A2;

}activations;

parameters *InitParam(size_t nb_entry, size_t sizeC1, size_t sizeC2);

activations *forward_propagation(matrix *X, parameters *p);

parameters *back_propagation(matrix *X, matrix *y, parameters *p, activations *A);

void update(parameters *p, parameters *dp, double learning_rate);

matrix *predict(matrix *X, parameters *p);

parameters *neuronal_network(matrix *X, matrix *y, size_t sizeSC, double learning_rate, size_t nb_iter, int show_debug);
