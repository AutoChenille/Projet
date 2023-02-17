#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "xor.h"

double sigmoid(double x)
{
    //sigmoid function
    return 1 / (1 + exp(-x));
}

double oneLessX(double x)
{
    //return 1 - x (used to be combined with m_apply in matrix.h)
    return 1 - x;
}

double random11()
{
	//Return a double between -1 and 1
    return  0.5 - (double)(rand()%10)/10;
}

double ceil_prob(double x)
{
    return x;
}

parameters *InitParam(size_t nb_entry, size_t sizeC1, size_t sizeC2)
{
    //Initialize network's parameters (W1, b1, W2, b2)
    //nb_entry	-> number of entries taken by the network
    //sizeC1	-> number of neurons in input layer
    //sizeC2	-> number of neurons in output layer 
    //return initialized parameters

	//random initialization
    srand(time(NULL));
    
    //Creates empty parameters
    parameters *p = malloc(sizeof(parameters));

	//Initialize W1
	//W1 is a sizeC1 * nb_entry matrix
    p->W1 = Matrix(sizeC1, nb_entry);
    //Set random values to it
    for(size_t i = 0; i < p->W1->col*p->W1->row; i++)
        p->W1->data[i] = random11();
    //Initialize b1 of size sizeC1 * 1 with 0
    p->b1 = Matrix(sizeC1, 1);

	//Initialize W2
	//W2 is a sizeC2 * sizeC1 matrix
    p->W2 = Matrix(sizeC2, sizeC1);
    //Set random values to it
    for(size_t i = 0; i < p->W2->col*p->W2->row; i++)
        p->W2->data[i] = random11();
    //Initialize b2 of size sizeC2 * 1 with 0
    p->b2 = Matrix(sizeC2, 1);

    return p;
}

activations *forward_propagation(matrix *X, parameters *p)
{
    //Apply the model with p parameters to X and return the resulted matrix
    
    //Initialize the struct activations where A->A1(resp 2) will contain the output of the layer 1(resp 2)
    activations *A = malloc(sizeof(activations));
       
    //Calcul of A->A1 (ie first layer's output)
    matrix *W1X = m_mul(p->W1, X);
    matrix *Z1 = m_addColumn(W1X, p->b1);
    A->A1 = m_apply(sigmoid, Z1);

	//Calcul of A->A2 (ie neuronal network's output)
    matrix *W2A1 = m_mul(p->W2, A->A1);
    matrix *Z2 = m_addColumn(W2A1, p->b2);
    A->A2 = m_apply(sigmoid, Z2);

	//free all used matrix (m_free defined in matrix.h)
    m_free(W1X);
    m_free(Z1);
    m_free(W2A1);
    m_free(Z2);

    return A;
}

parameters *back_propagation(matrix *X, matrix *y, parameters *p, activations *A)
{
	//Back Propagation method
	//Use results of forward_propagation to calcul the gradients of all parameters
	//return parameters dp wich contains all thoses gradients
	
	//Initialize dp
    parameters *dp = InitParam(X->col, p->b1->row, p->b2->row);

	//m = number of training data
    double m = y->col;

	//##### SECOND LAYER #####
	//Calcul of dZ2
    matrix *dZ2 = m_sub(A->A2, y);
    //calcul of dW2 and db2
    matrix *tA1 = m_transpose(A->A1);
    matrix *dZ2A1 = m_mul(dZ2, tA1);
    dp->W2 = m_scalarProd(dZ2A1, 1/m);
    matrix *sumdZ2 = m_horizontalSum(dZ2);
    dp->b2 = m_scalarProd(sumdZ2, 1/m);

	//##### FIRST LAYER #####
	//Calcul of dZ1
    matrix *oneLessA = m_apply(oneLessX, A->A1);
    matrix *A1A1 = m_LineBLineMul(A->A1, oneLessA);
    matrix *tW2 = m_transpose(p->W2);
    matrix *tW2dZ2 = m_mul(tW2, dZ2);
    matrix *dZ1 = m_LineBLineMul(tW2dZ2, A1A1);
	//Calcul of dW1 and db2
    matrix *tX = m_transpose(X);
    matrix *dZ1X = m_mul(dZ1, tX);
    dp->W1 = m_scalarProd(dZ1X, 1/m);
    matrix *sumdZ1 = m_horizontalSum(dZ1);
    dp->b1 = m_scalarProd(sumdZ1, 1/m);
    
	//Free all
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

	//Return gradients
    return dp;
}

void update(parameters *p, parameters *dp, double learning_rate)
{
	//Update (in place) parameters p using dp and lerning_rate
	//Formula p = p - learning_rate*dp
	
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
	//Predict result for entries contained in X with parameters P
	//return the matrix of outputs
	
    activations *A = forward_propagation(X, p);
    matrix *A2 = m_apply(ceil_prob, A->A2);
    free(A);
    return A2;
}

parameters *neuronal_network(matrix *X, matrix *y, size_t sizeSC, double learning_rate, size_t nb_iter, int show_debug)
{
	//Main funtion of the neuronal network
	//X -> training inputs
	//y -> training results
	//sizeSC -> number of neurons in first layer
	//learning_rate
	//nb_iter -> number of training iterations
	//show_debug (boolean) -> if != 0 then print training results
	//Return final parameters
	
	//Init parameters
    parameters *p = InitParam(X->row, sizeSC, y->row);

	//Iterations loop
    for(size_t i = 0; i < nb_iter; i++)
    {
    	//Calcul of activations -> forward propagation
        activations *A = forward_propagation(X, p);
		//Calcul of gradients -> back propagation
        parameters *dp = back_propagation(X, y, p, A);
        //Update parameters
        update(p, dp, learning_rate);

		//If show_debug != 0
        if(show_debug)
        {
            printf("\n#############################################\n\n");
            printf("Data gived :\n");
            m_print(X);
            printf("\nWaited results :\n");
            m_print(y);
            printf("\nPrediction result :\n");
            m_print(A->A2);
            printf("\n");
        }

		//free matrix
        free(A);
        free(dp);
    }

    if(show_debug)
        printf("\n");

	//Return parameters
    return p;
}

