#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "xor.h"

double sigmoid(double x)
{
    //return x apply to sigmoid function
    return 1 / (1 + exp(-x));
}

double oneLessX(double x)
{
    //return 1 - x (used to be combined with m_apply in matrix.h)
    return 1 - x;
}

double random11()
{
    return  0.5 - (double)(rand()%10)/10;
}

double ceil_prob(double x)
{
    return x;
}

parameters *InitParam(size_t nb_entry, size_t sizeC1, size_t sizeC2)
{
    //Return a perceptron_param p
    //All parameters initialized to 1 and p.W.row = nb_entry

    srand(time(NULL));
    parameters *p = malloc(sizeof(parameters));

    p->W1 = Matrix(sizeC1, nb_entry);
    for(size_t i = 0; i < p->W1->col*p->W1->row; i++)
        p->W1->data[i] = random11();
    p->b1 = Matrix(sizeC1, 1);

    p->W2 = Matrix(sizeC2, sizeC1);
    for(size_t i = 0; i < p->W2->col*p->W2->row; i++)
        p->W2->data[i] = random11();
    p->b2 = Matrix(sizeC2, 1);

    return p;
}

activations *forward_propagation(matrix *X, parameters *p)
{
    //Return a matrix containing all the results of the application of the model to each entry
    
    activations *A = malloc(sizeof(activations));
       
    matrix *W1X = m_mul(p->W1, X);
    matrix *Z1 = m_addColumn(W1X, p->b1);
    A->A1 = m_apply(sigmoid, Z1);

    matrix *W2A1 = m_mul(p->W2, A->A1);
    matrix *Z2 = m_addColumn(W2A1, p->b2);
    A->A2 = m_apply(sigmoid, Z2);

    m_free(W1X);
    m_free(Z1);
    m_free(W2A1);
    m_free(Z2);

    return A;
}

parameters *back_propagation(matrix *X, matrix *y, parameters *p, activations *A)
{
    parameters *dp = InitParam(X->col, p->b1->row, p->b2->row);

    double m = y->col;

    matrix *dZ2 = m_sub(A->A2, y);
    matrix *tA1 = m_transpose(A->A1);
    matrix *dZ2A1 = m_mul(dZ2, tA1);
    dp->W2 = m_scalarProd(dZ2A1, 1/m);
    matrix *sumdZ2 = m_horizontalSum(dZ2);
    dp->b2 = m_scalarProd(sumdZ2, 1/m);


    matrix *oneLessA = m_apply(oneLessX, A->A1);
    matrix *A1A1 = m_LineBLineMul(A->A1, oneLessA);
    matrix *tW2 = m_transpose(p->W2);
    matrix *tW2dZ2 = m_mul(tW2, dZ2);
    matrix *dZ1 = m_LineBLineMul(tW2dZ2, A1A1);

    matrix *tX = m_transpose(X);
    matrix *dZ1X = m_mul(dZ1, tX);
    dp->W1 = m_scalarProd(dZ1X, 1/m);
    matrix *sumdZ1 = m_horizontalSum(dZ1);
    dp->b1 = m_scalarProd(sumdZ1, 1/m);

    m_free(dZ2);
    m_free(tA1);
    m_free(dZ2A1);
    m_free(sumdZ2);

    m_free(oneLessA);
    m_free(A1A1);
    m_free(tW2);
    m_free(tW2dZ2);
    m_free(dZ1);

    m_free(tX);
    m_free(dZ1X);
    m_free(sumdZ1);

    return dp;
}

void update(parameters *p, parameters *dp, double learning_rate)
{

    matrix *dW1 = m_scalarProd(dp->W1, learning_rate);
    p->W1 = m_sub(p->W1, dW1);

    matrix *db1 = m_scalarProd(dp->b1, learning_rate);
    p->b1 = m_sub(p->b1, db1);

    matrix *dW2 = m_scalarProd(dp->W2, learning_rate);
    p->W2 = m_sub(p->W2, dW2);

    matrix *db2 = m_scalarProd(dp->b2, learning_rate);
    p->b2 = m_sub(p->b2, db2);
}

matrix *predict(matrix *X, parameters *p)
{
    activations *A = forward_propagation(X, p);
    matrix *A2 = m_apply(ceil_prob, A->A2);
    free(A);
    return A2;
}

parameters *neuronal_network(matrix *X, matrix *y, size_t sizeSC, double learning_rate, size_t nb_iter)
{
    parameters *p = InitParam(X->row, sizeSC, y->row);

    for(size_t i = 0; i < nb_iter; i++)
    {
        //shuffle_matrix(X, y);
        activations *A = forward_propagation(X, p);

        parameters *dp = back_propagation(X, y, p, A);
        update(p, dp, learning_rate);

        free(A);
        free(dp);
    }

    return p;
}

