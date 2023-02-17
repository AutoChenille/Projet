#pragma once

#include "matrix.h"

typedef struct
{
    matrix *W;
    double b;
}perceptron_param;

perceptron_param *InitParam(size_t nb_entry);

//return x apply to sigmoid function
double sigmoid(double x);

//return 1 - x (used to be combined with m_apply in matrix.h)
double oneLessX(double x);

void p_print(perceptron_param *p);

//Return a perceptron_param p with ll parameters initialized to 1
matrix *model(const matrix *X, const matrix *W, double b);

//Return a double, result of application of Log Loss function to A and y
double log_loss(const matrix *A, const matrix *y);   

//Return a perceptron_param tuple dp containing gradients of W and b
perceptron_param *gradients(const matrix *A, const matrix *X, const matrix *Y);

//Update p.W and p.b with their gradients contained in dp
void update_param(perceptron_param *p, const perceptron_param *dp, double learning_rate);

//Ceil probability prob to 0 or 1
double ceil_prob(double prob);

//Apply the model of W and b parameter to X entries
matrix *predict(const matrix *X, const matrix *W, double b);

//Neuron function
////X is the matrix of all entries
//Y is a matrix of associated results
//learning_rate is a double representing the correction added to each lesson
//nbr_generation is a double
void neuron(matrix *X, matrix *Y, double learning_rate, int nbr_generation, perceptron_param *p);
