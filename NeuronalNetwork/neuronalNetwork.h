#pragma once

#include "matrix.h"

typedef struct
{
    matrix *W1;
    matrix *b1;

    matrix *W2;
    matrix *b2;

    matrix *W3;
    matrix *b3;

}parameters;

typedef struct
{
    matrix *A1;
    matrix *A2;
    matrix *A3;

}activations;

//Predict result for entries contained in X with parameters P
matrix *predict(matrix *X, parameters *p);

//Main funtion of the neuronal network
parameters *neuronal_network(matrix *X, matrix *y, size_t sizeSC1, size_t sizeSC2, double learning_rate, size_t nb_iter, int show_debug);
