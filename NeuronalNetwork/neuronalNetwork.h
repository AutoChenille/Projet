#pragma once

#include "matrix.h"
#include "BuildDataImage.h"

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

float sigmoid(float x);

void FreeParameters(parameters *p);

matrix *apply_relu(matrix *input);

void FreeActivations(activations *a);

parameters* InitParam(size_t nb_entry, size_t sizeC1, size_t sizeC2, size_t nb_output);

//Predict result for entries contained in X with parameters P
matrix *predict(matrix *X, parameters *p);

//Main funtion of the neuronal network
parameters *neuronal_network(datas **data, size_t sizeSC1, size_t sizeSC2, float learning_rate, size_t nb_iter, int show_debug);