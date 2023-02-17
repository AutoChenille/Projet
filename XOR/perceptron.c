#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
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

    srand(time(NULL));
    perceptron_param *p = malloc(sizeof(perceptron_param));
    p->W = MatrixOf(nb_entry, 1, 0.5 - (double)(rand()%10)/10);
;
    p->W->data[1] = 0.5 - (double)(rand()%10)/10;

    p->b = 0;

    return p;
}

matrix *model(const matrix *X, const matrix *W, double b)
{
    //Return a matrix containing all the results of the application of the model to each entry
    
    //printf("model -> mul\n");
    matrix *XW = m_mul(X, W);
    //printf("model -> add_S\n");
    matrix *XWpB = m_scalarSum(XW, b);
    //printf("model -> apply sigm\n");
    matrix *Z = m_apply(sigmoid, XWpB);

    m_free(XW);
    m_free(XWpB);

    return Z;
}

double square(double x)
{
    return x*x;
}

double log_loss(const matrix *A, const matrix *y)
{
    //Return a double, result of application of Log Loss function to A and y

    //matrix *L = m_sub( prodLineBLine(m_dot(y, -1), m_apply(log, A) ), prodLineBLine(m_apply(oneLessX, y), m_apply(log, m_apply(oneLessX, A))));
    
    matrix *oneLessA = m_apply(oneLessX, A);
    matrix *logOneA = m_apply(log, oneLessA);
    matrix *logA = m_apply(log, A);

    matrix *oneLessY = m_apply(oneLessX, y);

    matrix *rightTerm = m_LineBLineMul(oneLessY, logOneA);
    matrix *leftTerm = m_LineBLineMul(y, logA);

    matrix *L = m_add(leftTerm, rightTerm);

    double loss = (double) 1/y->row * m_sum(L);
    
    m_free(oneLessA);
    m_free(logOneA);
    m_free(logA);
    m_free(oneLessY);
    m_free(rightTerm);
    m_free(leftTerm);
    m_free(L);

    //double loss = m_sum(m_apply(square, m_sub(A, y)));


    return loss;
}

perceptron_param *gradients(const matrix *A, const matrix *X, const matrix *Y)
{
    //Return a perceptron_param tuple dp containing gradients of W and b
    
    matrix *AsubY = m_sub(A, Y);
    matrix *X_t = m_transpose(X);
    matrix *X_tAsubY = m_mul(X_t, AsubY);

    matrix *dw = m_scalarProd(X_tAsubY, (double) 1/Y->row); 

    double db = (double) 1/Y->row * m_sum(AsubY);
    
    perceptron_param *dp = InitParam(X->col);
    dp->W = dw;
    dp->b = db;

    m_free(AsubY);
    m_free(X_t);
    m_free(X_tAsubY);

    return dp;
}

void update_param(perceptron_param *p, const perceptron_param *dp, double learning_rate)
{
    //Update p.W and p.b with their gradients contained in dp

    matrix *learnW = m_scalarProd(dp->W, learning_rate);

    p->W = m_sub(p->W, learnW);
    p->b -= learning_rate * dp->b;

    m_free(learnW);
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


void neuron(matrix *X, matrix *Y, double learning_rate, int nbr_generation, perceptron_param *p)
{
    //Neuron function
    //X is the matrix of all entries
    //Y is a matrix of associated results
    //learning_rate is a double representing the correction added to each lesson
    //nbr_generation is a double

    for (int i = 0; i < nbr_generation; i++)
    {
        shuffle_matrixXY(X, Y);
        matrix *A = model(X, p->W, p->b);
        perceptron_param *dp = gradients(A, X, Y);
        update_param(p, dp, learning_rate);
        if(i == 0 || i == nbr_generation-1)
            printf("loss : %f\n", log_loss(A, Y));
        free(dp);
        m_free(A);
    }
}
