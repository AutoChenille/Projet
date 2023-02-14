#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "perceptron.h"
#include <math.h>
#include <sys/random.h>
#include <time.h>


void getDataSup(int n, matrix *X, matrix *Y)
{   
    srand(time(NULL));
    size_t k= 0;
    for(double i = 0; i < n; i++)
    {
        for(double j = 0; j < n; j++)
        {
            double x0 = (double)(rand() % n)/n;
            double x1 = (double)(rand() % n)/n;
            double y = x0 > x1;
            X->data[k*X->col] = x0;
            X->data[k*X->col + 1] = x1;
            Y->data[k] = y;
            //printf("%f %f --> %f\n", x0, x1, y);
            k++;
        }
    }
}


int main()
{
    size_t size = 100;

    matrix *X = Matrix(size*size, 2);
    matrix *Y = Matrix(size*size, 1);

    getDataSup(size, X, Y);

    perceptron_param *p = InitParam(2);

    p_print(p);

    neuron(X, Y, 0.1, 1000, p);

    printf("final --> ");
    p_print(p);

    double x0 = 0.5;
    double x1 = 0.5 ;
    double w0 = p->W->data[0];
    double w1 = p->W->data[1];
    double b = p->b;
    double z = w0*x0 + w1*x1 + b;
    double a = (double)1/(1+exp(-z));

    //m_print(X);
    //m_print(Y);

    printf("z : %f, a : %f\n",z,a);
}
