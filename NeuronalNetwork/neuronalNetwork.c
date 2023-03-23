#include <SDL2/SDL_stdinc.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "matrix.h"
#include "neuronalNetwork.h"

void FreeParameters(parameters *p)
{
    //Free all memory allocated for parameters
    freeMatrix(p->W1);
    freeMatrix(p->W2);
    freeMatrix(p->W3);
    freeMatrix(p->b1);
    freeMatrix(p->b2);
    freeMatrix(p->b3);

    free(p);
}

void FreeActivations(activations *a)
{
    //Free all memory allocated for activations
    freeMatrix(a->A1);
    freeMatrix(a->A2);
    freeMatrix(a->A3);

    free(a);
}

float sigmoid(float x)
{
    //sigmoid function
    return 1 / (1 + exp(-x));
}

float relu(float x)
{
    //sigmoid function
    return x > 0 ? x : 1e-6;
}

float oneLessX(float x)
{
    //return 1 - x (used to be combined with m_apply_Place_Place in matrix.h)
    return 1 - x;
}

matrix *apply_softmax(matrix *input)
{
    matrix *result = Matrix(input->row, input->col);

    for(size_t j = 0; j < input->col; j ++)
    {
        //Find the max for each vect
        float max = input->data[input->col+j];
        for(size_t i = 1; i < input->col; i++)
        {
            if(input->data[i*input->col+j] > max)
                max = input->data[i*input->col+j];
        }

        //Compute the exponential sum
        float sum = 0;
        for(size_t i = 0; i < input->row; i++)
        {
            sum += expf(input->data[i*input->col+j] - max);
        }

        //Compute the probabilities
        for(size_t i = 0; i < input->row; i++)
        {
                result->data[i*result->col+j] = expf(input->data[i*result->col+j] - max)/sum;
        }
    }

    return result;
}

void m_normalDiv(matrix *result)
{
    for(size_t i = 0; i < result->row; i ++)
    {
        float max = fabsf(result->data[i*result->col]);
        for(size_t j = 0; j < result->col; j++)
        {  
            if(fabsf(result->data[i*result->col]) > max)
                max = fabsf(result->data[i*result->col+j]);
        }

        if(max > 1)
        {
            for(size_t j = 0; j < result->col; j++)
                result->data[i*result->col+j] /= max;
        }
    }
}

matrix *apply_relu(matrix *input)
{

    m_normalDiv(input);
    matrix *result = m_copy(input);

    for(size_t i = 0; i < input->row*input->col; i++)
    {
        result->data[i] = input->data[i] > 0 ? input->data[i] : 0;
    }

    //m_print(result);

    m_normalDiv(result);

    return result;
}

float random11()
{
	//Return a float between -1 and 1
    return  (0.5 - (float)(rand()%100)/100);
}

float ceil_prob(float x)
{
    return x;
}

parameters* InitParam(size_t nb_entry, size_t sizeC1, size_t sizeC2, size_t nb_output) {
    // Initialize network's parameters (W1, b1, W2, b2)
    // nb_entry  -> number of entries taken by the network
    // sizeC1    -> number of neurons in input layer
    // sizeC2    -> number of neurons in output layer
    // return initialized parameters

    // random initialization
    srand(time(NULL));

    // Creates empty parameters
    parameters* p = malloc(sizeof(parameters));
    if (!p) {
        return NULL;  // failed to allocate memory
    }

    // Initialize W1
    // W1 is a sizeC1 * nb_entry matrix
    p->W1 = Matrix(sizeC1, nb_entry);
    if (!p->W1) {
        FreeParameters(p);
        return NULL;  // failed to allocate memory
    }
    // Set random values to it
    for (size_t i = 0; i < p->W1->col * p->W1->row; i++) {
        p->W1->data[i] = random11();
    }
    
    // Initialize b1 of size sizeC1 * 1 with 0
    p->b1 = MatrixOf(sizeC1, 1, 0);
    if (!p->b1) {
        FreeParameters(p);
        return NULL;  // failed to allocate memory
    }

    // Initialize W2
    // W2 is a sizeC2 * sizeC1 matrix
    p->W2 = Matrix(sizeC2, sizeC1);
    if (!p->W2) {
        FreeParameters(p);
        return NULL;  // failed to allocate memory
    }
    // Set random values to it
    for (size_t i = 0; i < p->W2->col * p->W2->row; i++) {
        p->W2->data[i] = random11();
    }
    
    // Initialize b2 of size sizeC2 * 1 with 0
    p->b2 = MatrixOf(sizeC2, 1, 0);
    if (!p->b2) {
        free(p);
        return NULL;  // failed to allocate memory
    }

    // Initialize W3
    // W3 is a nb_output * sizeC2 matrix
    p->W3 = Matrix(nb_output, sizeC2);
    if (!p->W3) {
        FreeParameters(p);
        return NULL;  // failed to allocate memory
    }
    // Set random values to it
    for (size_t i = 0; i < p->W3->col * p->W3->row; i++) {
        p->W3->data[i] = random11();
    }

    // Initialize b3 of size nb_output * 1 with 0
    p->b3 = MatrixOf(nb_output, 1, 0);
    if (!p->b3) {
        FreeParameters(p);
        return NULL;  // failed to allocate memory
    }

    return p;
}

void forward_propagation(matrix *X, parameters *p, activations *A)
{
    //Apply the model with p parameters to X and return the resulted matrix
       
    //Calcul of A->A1 (ie first layer's output)
    matrix *W1X = m_mul(p->W1, X);
    //m_printSize("W1", p->W1);
    //m_print(p->W1);
    //m_printSize("X", X);
    //m_print(X);
    //m_printSize("W1X", W1X);
    //m_print(W1X);
    matrix *Z1 = m_addColumn(W1X, p->b1);
    //m_normalize(Z1);
    matrix *Z1S = apply_relu(Z1);
    //m_printSize("Z1S", Z1S);
    //m_printSize("A1", A->A1);
    m_copyTo(Z1S, A->A1);

	//Calcul of A->A2 (ie neuronal network's output)
    matrix *W2A1 = m_mul(p->W2, A->A1);
    matrix *Z2 = m_addColumn(W2A1, p->b2);
    //m_normalize(Z2);
    matrix *Z2S = apply_relu(Z2);
    //m_printSize("Z2S", Z2S);
    //m_printSize("A2", A->A2);
    m_copyTo(Z2S, A->A2);

    //Calcul of A->A3 (ie neuronal network's output)
    matrix *W3A2 = m_mul(p->W3, A->A2);
    matrix *Z3 = m_addColumn(W3A2, p->b3);
    matrix *Z3S = apply_softmax(Z3);
    //m_printSize("Z3S", Z3S);
    //m_printSize("A3", A->A3);
    m_copyTo(Z3S, A->A3);

	//free all used matrix (freeMatrix defined in matrix.h)
    freeMatrix(W1X);
    freeMatrix(Z1);
    freeMatrix(Z1S);
    freeMatrix(W2A1);
    freeMatrix(Z2);
    freeMatrix(Z2S);
    freeMatrix(W3A2);
    freeMatrix(Z3);
    freeMatrix(Z3S);
}

void back_propagation(matrix *X, matrix *y, parameters *p, activations *A, parameters *dp)
{
	//Back Propagation method
	//Use results of forward_propagation to calcul the gradients of all parameters
	//return parameters dp wich contains all thoses gradients

	//m = number of training data
    float m = y->col;

	//##### THIRD LAYER #####
	//Calcul of dZ3
    matrix *dZ3 = m_sub(A->A3, y);
    //calcul of dW3 and db3
    matrix *tA2 = m_transpose(A->A2);
    matrix *dZ3A2 = m_mul(dZ3, tA2);
    dp->W3 = m_scalarProd(dZ3A2, 1/m);
    matrix *sumdZ3 = m_horizontalSum(dZ3);
    matrix *SP3 = m_scalarProd(sumdZ3, 1/m);
    //m_printSize("SP3", SP3);
    //m_printSize("dp->b3", dp->b3);
    m_copyTo(SP3, dp->b3);

	//##### SECOND LAYER #####
	//Calcul of dZ2
    matrix *oneLessA2 = m_apply(oneLessX,  A->A2);
    matrix *A2A2 = m_LineBLineMul(A->A2, oneLessA2);
    matrix *tW3 = m_transpose(p->W3);
    matrix *tW3dZ3 = m_mul(tW3, dZ3);
    matrix *dZ2 = m_LineBLineMul(tW3dZ3, A2A2);
	//Calcul of dW2 and db2
    matrix *tA1 = m_transpose(A->A1);
    matrix *dZ2A1 = m_mul(dZ2, tA1);
    dp->W2 = m_scalarProd(dZ2A1, 1/m);
    matrix *sumdZ2 = m_horizontalSum(dZ2);
    matrix *SP2 = m_scalarProd(sumdZ2, 1/m);
    //m_printSize("SP2", SP2);
    //m_printSize("dp->b2", dp->b2);
    m_copyTo(SP2, dp->b2);

	//##### FIRST LAYER #####
	//Calcul of dZ1
    matrix *oneLessA = m_apply(oneLessX,  A->A1);
    matrix *A1A1 = m_LineBLineMul(A->A1, oneLessA);
    matrix *tW2 = m_transpose(p->W2);
    matrix *tW2dZ2 = m_mul(tW2, dZ2);
    matrix *dZ1 = m_LineBLineMul(tW2dZ2, A1A1);
	//Calcul of dW1 and db1
    matrix *tX = m_transpose(X);
    matrix *dZ1X = m_mul(dZ1, tX);
    dp->W1 = m_scalarProd(dZ1X, 1/m);
    matrix *sumdZ1 = m_horizontalSum(dZ1);
    matrix *SP1 = m_scalarProd(sumdZ1, 1/m);
    //m_printSize("SP1", SP1);
    //m_printSize("dp->b1", dp->b1);
    m_copyTo(SP1, dp->b1);

    m_normalDiv(dp->b1);
    m_normalDiv(dp->W1);
    m_normalDiv(dp->b2);
    m_normalDiv(dp->W2);
    m_normalDiv(dp->b3);
    m_normalDiv(dp->W3);
    
	//Free all
    freeMatrix(dZ3);
    freeMatrix(tA2);
    freeMatrix(dZ3A2);
    freeMatrix(sumdZ3);
    freeMatrix(SP3);

    freeMatrix(oneLessA2);
    freeMatrix(A2A2);
    freeMatrix(tW3);
    freeMatrix(tW3dZ3);
    freeMatrix(dZ2);

    freeMatrix(tA1);
    freeMatrix(dZ2A1);
    freeMatrix(sumdZ2);
    freeMatrix(SP2);

    freeMatrix(oneLessA);
    freeMatrix(A1A1);
    freeMatrix(tW2);
    freeMatrix(tW2dZ2);
    freeMatrix(dZ1);

    freeMatrix(tX);
    freeMatrix(dZ1X);
    freeMatrix(sumdZ1);
    freeMatrix(SP1);
}

void update(parameters *p, parameters *dp, float learning_rate)
{
	//Update (in place) parameters p using dp and lerning_rate
	//Formula p = p - learning_rate*dp
	
    m_scalarProd_Place(dp->W1, learning_rate);
    m_sub_Place(p->W1, dp->W1);

    m_scalarProd_Place(dp->b1, learning_rate);
    m_sub_Place(p->b1, dp->b1);

    m_scalarProd_Place(dp->W2, learning_rate);
    m_sub_Place(p->W2, dp->W2);

    m_scalarProd_Place(dp->b2, learning_rate);
    m_sub_Place(p->b2, dp->b2);

    m_scalarProd_Place(dp->W3, learning_rate);
    m_sub_Place(p->W3, dp->W3);

    m_scalarProd_Place(dp->b3, learning_rate);
    m_sub_Place(p->b3, dp->b3);
}

matrix *predict(matrix *X, parameters *p)
{
	//Predict result for entries contained in X with parameters P
	//return the matrix of outputs
	
    activations *A = malloc(sizeof(activations));
    A->A1 = Matrix(p->W1->row, X->col);
    A->A2 = Matrix(p->W2->row, X->col);
    A->A3 = Matrix(p->W3->row, X->col);

    forward_propagation(X, p, A);

    matrix *A3 = m_apply(ceil_prob, A->A3);

    FreeActivations(A);

    return A3;
}

parameters *neuronal_network(matrix *data_entry, matrix *data_output, size_t sizeSC1, size_t sizeSC2, float learning_rate, size_t nb_iter, size_t buffer_size, int show_debug)
{
	//Main funtion of the neuronal network
	//X -> training inputs
	//y -> training results
	//sizeSC -> number of neurons in first layer
	//learning_rate
	//nb_iter -> number of training iterations
	//show_debug (boolean) -> if != 0 then print training results
	//Return final parameters


    srand(time(NULL));

    m_printSize("entry", data_entry);
    m_print(data_entry);
    m_printSize("ouput", data_output);
    m_print(data_output);

    matrix *X = Matrix(data_entry->row, buffer_size);
    matrix *y = Matrix(data_output->row, buffer_size);
	
	//Init parameters
    parameters *p = InitParam(X->row, sizeSC1, sizeSC2, y->row);
    
    parameters *dp = InitParam(X->row, sizeSC1, sizeSC2, y->row);

    activations *A = malloc(sizeof(activations));
    A->A1 = Matrix(sizeSC1, X->col);
    A->A2 = Matrix(sizeSC2, X->col);
    A->A3 = Matrix(y->row, y->col);

	//Iterations loop
    for(size_t i = 0; i < nb_iter; i++)
    {
        //Pickup random data
        for(size_t k = 0; k < buffer_size; k++)
        {
            int pickedup = rand() % data_entry->col;
            pickedup = k;
            printf("pickedup : %i\n", pickedup);
            for(size_t x = 0; x < data_entry->row; x++)
                X->data[x*X->col+k] = data_entry->data[x*data_entry->col+pickedup];
            for(size_t x = 0; x < data_output->row; x++)
                y->data[x*y->col+k] = data_output->data[x*data_output->col+pickedup];
        }

        m_printSize("DATABUFFERED", X);
        m_print(X);
        m_printSize("OUTPUTBUFFERED", y);
        m_print(y);
        printf("\n");


        shuffle(X, y);
    	//Calcul of activations -> forward propagation
        //printf("forward\n");
        forward_propagation(X, p, A);
		//Calcul of gradients -> back propagation
        //printf("back\n");
        back_propagation(X, y, p, A, dp);
        //Update parameters
        //printf("update\n");
        update(p, dp, learning_rate);

        printf("\n\nEpoch n°%li\n", i);
		//If show_debug != 0
        if(show_debug)
        {
            float acc = 0;
            for(size_t i = 0; i < y->col; i ++)
            {
                size_t k = 0;
                while(y->data[k*y->col+i] != 1)
                    k++;
                acc += A->A3->data[k*y->col+i]>=0.5;
            }
            printf("acc : %f\n", acc);
            //m_print(y);
            printf("\n\n");
            m_print(A->A1);
            printf("\n");
            m_print(A->A2);
            printf("\n");
            m_print(A->A3);
            printf("\n");
            m_print(y);
            printf("\n\n");
        }
    }

    FreeParameters(dp);
    FreeActivations(A);

	//Return parameters
    return p;
}

