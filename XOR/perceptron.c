#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "perceptron.h"


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

void p_print(perceptron_param *p)
{
    printf("b : %f\n", p->b);
    printf("W :");
    for(size_t i = 0; i < p->W->row; i++)
    {
        printf(" w%li->%f", i, p->W->data[i]);
    }
    printf("\n\n");
}

perceptron_param *InitParam(size_t nb_entry)
{
    //Return a perceptron_param p
    //All parameters initialized to 1 and p.W.row = nb_entry

    perceptron_param *p = malloc(sizeof(perceptron_param));
    p->W = MatrixOf(nb_entry, 1, 2);
    p->b = 2;

    return p;
}

matrix *model(const matrix *X, const matrix *W, double b)
{
    //Return a matrix containing all the results of the application of the model to each entry
    
    matrix *Z = m_addS( m_mul(X, W), b);
    Z = m_apply(sigmoid, Z);
    return Z;
}

double log_loss(const matrix *A, const matrix *y)
{
    //Return a double, result of application of Log Loss function to A and y

    matrix *L = m_sub( m_mul(m_dot(y, -1), m_apply(log, A) ), m_mul(m_apply(oneLessX, y), m_apply(log, m_apply(oneLessX, A))));

    double loss = (double) 1/y->col * m_sum(L);
    free(L);

    return loss;
}

perceptron_param *gradients(const matrix *A, const matrix *X, const matrix *Y)
{
    //Return a perceptron_param tuple dp containing gradients of W and b
    matrix *dw = m_dot(m_mul(m_transpose(X), m_sub(A, m_transpose(Y))), (double) 1/Y->col); 

    double db = (double) 1/Y->col * m_sum(m_sub(A, m_transpose(Y)));
    
    perceptron_param *p = malloc(sizeof(perceptron_param));
    p->W = dw;
    p->b = db;

    return p;
}

void update_param(perceptron_param *p, const perceptron_param *dp, double learning_rate)
{
    //Update p.W and p.b with their gradients contained in dp

    p->W = m_sub(p->W, m_dot(dp->W, learning_rate));
    p->b -= learning_rate * dp->b;
}

double ceil_prob(double prob)
{
    //Ceil probability prob
    //Return 1 if prob >= 0.5, 0 else
    return prob >= 0.5 ? 1 : 0;
}

matrix *predict(const matrix *X, const matrix *W, double b)
{
    //Apply the model of W and b parameter to X entries

    matrix *A = model(X, W, b);

    return m_apply(ceil_prob, A);
}


perceptron_param *neuron(const matrix *X, const matrix *Y, double learning_rate, int nbr_generation)
{
    //Neuron function
    //X is the matrix of all entries
    //Y is a matrix of associated results
    //learning_rate is a double representing the correction added to each lesson
    //nbr_generation is a double

    matrix *A;
    perceptron_param *p = InitParam(X->col);

    for (int i = 0; i < nbr_generation; i++)
    {
        A = model(X, p->W, p->b);
        perceptron_param *dp = gradients(A, X, Y);
        update_param(p, dp, learning_rate);
        free(dp);
    }

    return p;
}
