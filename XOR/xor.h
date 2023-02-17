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

//Predict result for entries contained in X with parameters P
matrix *predict(matrix *X, parameters *p);

//Main funtion of the neuronal network
parameters *neuronal_network(matrix *X, matrix *y, size_t sizeSC, double learning_rate, size_t nb_iter, int show_debug);
