#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "perceptron.h"
#include <math.h>
#include <sys/random.h>
#include <time.h>


void getDataSup(size_t n, matrix *X, matrix *Y)
{   
    for(size_t i = 0; i < n; i++)
    {
        X->data[i*X->col] = (double)rand();
        X->data[i*X->col + 1] = (double)rand();
        Y->data[i] = X->data[i*X->col] > X->data[i*X->col+1];
    }
}


int main()
{
    size_t size = 5000;

    matrix *X = Matrix(size, 2);
    matrix *Y = Matrix(1, size);

    getDataSup(size, X, Y);

    perceptron_param *p = neuron(X, Y, 0.01, 10000);
    
    p_print(p);

    double x1 = 2.1;
    double x2 = 1;
    double w1 = p->W->data[0];
    double w2 = p->W->data[1];
    double b = p->b;
    double z = w1*x1 + w2*x2 + b;
    double a = 1/(1+exp(-z));

    m_print(X);
    printf("%f", a);
}
