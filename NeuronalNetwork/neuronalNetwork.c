#include <SDL2/SDL_stdinc.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include "matrix.h"
#include "neuronalNetwork.h"
#include "saveParams.h"

void FreeParameters(parameters *p)
{
    //Free all memory allocated for parameters
    freeMatrix(p->W1);
    freeMatrix(p->W2);
    freeMatrix(p->W3);
    freeMatrix(p->W4);
    freeMatrix(p->b1);
    freeMatrix(p->b2);
    freeMatrix(p->b3);
    freeMatrix(p->b4);

    free(p);
}

void FreeActivations(activations *a)
{
    //Free all memory allocated for activations
    freeMatrix(a->A1);
    freeMatrix(a->A2);
    freeMatrix(a->A3);
    freeMatrix(a->A4);

    free(a);
}

float oneLessX(float x)
{
    //return 1 - x (used to be combined with m_apply_Place_Place in matrix.h)
    return 1 - x;
}

float derivative_relu(float x)
{
    return x > 0 ? 1 : 0;
}

float random11()
{
	//Return a float between -1 and 1
    return  (0.5f - (float)(rand()%1000000)/1000000) / 10;
}

parameters* InitParam(size_t nb_entry, size_t sizeC1, size_t sizeC2, size_t sizeC3, size_t nb_output)
{
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
    if (!p->W2)
    {
        FreeParameters(p);
        return NULL;  // failed to allocate memory
    }
    // Set random values to it
    for (size_t i = 0; i < p->W2->col * p->W2->row; i++)
        p->W2->data[i] = random11();
    
    // Initialize b2 of size sizeC2 * 1 with 0
    p->b2 = MatrixOf(sizeC2, 1, 0);
    if (!p->b2) {
        FreeParameters(p);
        return NULL;  // failed to allocate memory
    }

    // Initialize W3
    // W3 is a sizeC3 * sizeC2 matrix
    p->W3 = Matrix(sizeC3, sizeC2);
    if (!p->W3)
    {
        FreeParameters(p);
        return NULL;  // failed to allocate memory
    }
    // Set random values to it
    for (size_t i = 0; i < p->W3->col * p->W3->row; i++)
        p->W3->data[i] = random11();
    
    // Initialize b3 of size sizeC3 * 1 with 0
    p->b3 = MatrixOf(sizeC3, 1, 0);
    if (!p->b3)
    {
        FreeParameters(p);
        return NULL;  // failed to allocate memory
    }

    // Initialize W4
    // W4 is a nb_output * sizeC3 matrix
    p->W4 = Matrix(nb_output, sizeC3);
    if (!p->W4)
    {
        FreeParameters(p);
        return NULL;  // failed to allocate memory
    }
    // Set random values to it
    for (size_t i = 0; i < p->W4->col * p->W4->row; i++)
        p->W4->data[i] = random11();

    // Initialize b3 of size nb_output * 1 with 0
    p->b4 = MatrixOf(nb_output, 1, 0);
    if (!p->b4)
    {
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
    matrix *Z1 = m_addColumn(W1X, p->b1);
    matrix *Z1S = apply_relu(Z1);
    m_copyTo(Z1S, A->A1);

	//Calcul of A->A2 (ie neuronal network's output)
    matrix *W2A1 = m_mul(p->W2, A->A1);
    matrix *Z2 = m_addColumn(W2A1, p->b2);
    matrix *Z2S = apply_relu(Z2);
    m_copyTo(Z2S, A->A2);

    //Calcul of A->A3 (ie neuronal network's output)
    matrix *W3A2 = m_mul(p->W3, A->A2);
    matrix *Z3 = m_addColumn(W3A2, p->b3);
    matrix *Z3S = apply_relu(Z3);
    m_copyTo(Z3S, A->A3);

    //Calcul of A->A4 (ie neuronal network's output)
    matrix *W4A3 = m_mul(p->W4, A->A3);
    matrix *Z4 = m_addColumn(W4A3, p->b4);
    matrix *Z4S = apply_softmax(Z4);
    m_copyTo(Z4S, A->A4);

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
    freeMatrix(W4A3);
    freeMatrix(Z4);
    freeMatrix(Z4S);
}

void back_propagation(matrix *X, matrix *y, parameters *p, activations *A, parameters *dp)
{
	//Back Propagation method
	//Use results of forward_propagation to calcul the gradients of all parameters
	//return parameters dp wich contains all thoses gradients

	//m = number of training data
    float m = y->col;

	//##### FOURTH LAYER #####
	//Calcul of dZ4
    matrix *dZ4 = m_sub(A->A4, y);
    //calcul of dW4 and db4
    matrix *tA3 = m_transpose(A->A3);
    matrix *dZ4A3 = m_mul(dZ4, tA3);
    m_scalarProd_Place(dZ4A3, 1./m);
    m_copyTo(dZ4A3, dp->W4);

    matrix *sumdZ4 = m_horizontalSum(dZ4);
    m_scalarProd_Place(sumdZ4, 1./m);
    m_copyTo(sumdZ4, dp->b4);

    //##### THIRD LAYER #####
	//Calcul of dZ3
    matrix *tW4 = m_transpose(p->W4);
    matrix *tW4dZ4 = m_mul(tW4, dZ4);
    matrix *dA3 = m_apply(derivative_relu, A->A3);
    matrix *dZ3 = m_LineBLineMul(tW4dZ4, dA3);
    m_scalarProd_Place(dZ3, 1./m);
	//Calcul of dW2 and db2
    matrix *tA2 = m_transpose(A->A2);
    matrix *dZ3A2 = m_mul(dZ3, tA2);
    m_scalarProd_Place(dZ3A2, 1./m);
    m_copyTo(dZ3A2, dp->W3);

    matrix *sumdZ3 = m_horizontalSum(dZ3);
    m_scalarProd_Place(sumdZ3, 1./m);
    m_copyTo(sumdZ3, dp->b3);

	//##### SECOND LAYER #####
	//Calcul of dZ2
    //matrix *oneLessA2 = m_apply(oneLessX,  A->A2);
    //matrix *A2A2 = m_LineBLineMul(A->A2, oneLessA2);
    matrix *tW3 = m_transpose(p->W3);
    matrix *tW3dZ3 = m_mul(tW3, dZ3);
    matrix *dA2 = m_apply(derivative_relu, A->A2);
    matrix *dZ2 = m_LineBLineMul(tW3dZ3, dA2);
    m_scalarProd_Place(dZ2, 1./m);
    //matrix *dZ2 = m_LineBLineMul(tW3dZ3, A2A2);
	//Calcul of dW2 and db2
    matrix *tA1 = m_transpose(A->A1);
    matrix *dZ2A1 = m_mul(dZ2, tA1);
    m_scalarProd_Place(dZ2A1, 1./m);
    m_copyTo(dZ2A1, dp->W2);

    matrix *sumdZ2 = m_horizontalSum(dZ2);
    m_scalarProd_Place(sumdZ2, 1./m);
    m_copyTo(sumdZ2, dp->b2);

	//##### FIRST LAYER #####
	//Calcul of dZ1
    //matrix *oneLessA = m_apply(oneLessX,  A->A1);
    //matrix *A1A1 = m_LineBLineMul(A->A1, oneLessA);
    matrix *tW2 = m_transpose(p->W2);
    matrix *tW2dZ2 = m_mul(tW2, dZ2);
    matrix *dA1 = m_apply(derivative_relu, A->A1);
    matrix *dZ1 = m_LineBLineMul(tW2dZ2, dA1);
    m_scalarProd_Place(dZ1, 1./m);
	//Calcul of dW1 and db1
    matrix *tX = m_transpose(X);
    matrix *dZ1X = m_mul(dZ1, tX);
    m_scalarProd_Place(dZ1X, 1./m);
    m_copyTo(dZ1X, dp->W1);

    matrix *sumdZ1 = m_horizontalSum(dZ1);
    m_scalarProd_Place(sumdZ1, 1./m);
    m_copyTo(sumdZ1, dp->b1);
    
	//Free all
    freeMatrix(dZ4);
    freeMatrix(tA3);
    freeMatrix(dZ4A3);
    freeMatrix(sumdZ4);

    freeMatrix(tW4);
    freeMatrix(tW4dZ4);
    freeMatrix(dA3);
    freeMatrix(dZ3);

    freeMatrix(tA2);
    freeMatrix(dZ3A2);
    freeMatrix(sumdZ3);

    //freeMatrix(oneLessA2);
    //freeMatrix(A2A2);
    freeMatrix(tW3);
    freeMatrix(tW3dZ3);
    freeMatrix(dA2);
    freeMatrix(dZ2);

    freeMatrix(tA1);
    freeMatrix(dZ2A1);
    freeMatrix(sumdZ2);

    //freeMatrix(oneLessA);
    //freeMatrix(A1A1);
    freeMatrix(tW2);
    freeMatrix(tW2dZ2);
    freeMatrix(dA1);
    freeMatrix(dZ1);

    freeMatrix(tX);
    freeMatrix(dZ1X);
    freeMatrix(sumdZ1);
}

void update(parameters *p, parameters *dp, float learning_rate)
{
	//Update (in place) parameters p using dp and lerning_rate
	//Formula p = p - learning_rate*dp

    matrix **current = malloc(sizeof(matrix)*16);
    current[0] = p->W1;
    current[1] = dp->W1;
    current[2] = p->b1;
    current[3] = dp->b1;
    current[4] = p->W2;
    current[5] = dp->W2;
    current[6] = p->b2;
    current[7] = dp->b2;
    current[8] = p->W3;
    current[9] = dp->W3;
    current[10] = p->b3;
    current[11] = dp->b3;
    current[12] = p->W4;
    current[13] = dp->W4;
    current[14] = p->b4;
    current[15] = dp->b4;

    for(size_t id = 0; id < 16; id+=2)
    {
        for(size_t i = 0; i < current[id]->row*current[id]->col; i++)
        {
            current[id]->data[i] -= current[id+1]->data[i] * learning_rate;
        }
    }
}

matrix *predictionVector(matrix *X, parameters *p)
{
	//Predict result for entries contained in X with parameters P
	//return the matrix of outputs
	
    activations *A = malloc(sizeof(activations));
    A->A1 = Matrix(p->W1->row, X->col);
    A->A2 = Matrix(p->W2->row, X->col);
    A->A3 = Matrix(p->W3->row, X->col);
    A->A4 = Matrix(p->W4->row, X->col);

    forward_propagation(X, p, A);

    matrix *A4 = m_copy(A->A4);

    FreeActivations(A);

    return A4;
}

int predict(matrix *X, parameters *p)
{
    matrix *vector = predictionVector(X, p);

    int imax = 0;
    for(size_t i = 1; i < vector->row; i++)
        imax = vector->data[imax] > vector->data[i] ? imax : (int)i;

    return imax;
}

parameters *neuronal_network(datas *data, size_t sizeSC1, size_t sizeSC2, size_t sizeSC3, float learning_rate, size_t nb_iter, int show_debug, parameters *p)
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

    size_t NBI = 10;

    matrix *X = Matrix(data->input->row, NBI);
    matrix *y = Matrix(data->output->row, NBI);
	
	//Init parameters
    if(p == NULL)
        p = InitParam(X->row, sizeSC1, sizeSC2, sizeSC3, y->row);
    parameters *dp = InitParam(X->row, sizeSC1, sizeSC2, sizeSC3, y->row);

    activations *A = malloc(sizeof(activations));
    A->A1 = Matrix(sizeSC1, X->col);
    A->A2 = Matrix(sizeSC2, X->col);
    A->A3 = Matrix(sizeSC3, X->col);
    A->A4 = Matrix(y->row, X->col);

	//Iterations loop
    for(size_t i = 0; i < nb_iter; i++)
    {
        //Pickup random data
        for(size_t i = 0; i < NBI; i++)
        {
            int pickedup = rand() % data->input->col;
            for(size_t x = 0; x < X->row; x++)
                X->data[x*X->col+i] = data->input->data[x*data->input->col+pickedup];
            for(size_t x = 0; x < y->row; x++)
                y->data[x*X->col+i] = data->output->data[x*data->output->col+pickedup];
        }
        
        //shuffle(X, y);
    	//Calcul of activations -> forward propagation
        forward_propagation(X, p, A);
		//Calcul of gradients -> back propagation
        back_propagation(X, y, p, A, dp);
        //Update parameters
        update(p, dp, learning_rate);

        if(i%100 == 0)
        {
            printf("Epoch n°%li\n", i);

            if(i%500 == 0)
                SaveParameters(p, "./BACKUP_SAVED");

            //If show_debug != 0
            if(show_debug)
            {
                float acc = 0;
                for(size_t i = 0; i < y->col; i ++)
                {
                    size_t k = 0;
                    while(y->data[k*y->col+i] != 1)
                        k++;
                    acc += A->A4->data[k*y->col+i]>=0.5;
                }
                printf("acc : %f\n", acc);
                printf("learning rate : %f\n", learning_rate);
                //m_print(y);
                printf("\n\n");
                m_print(A->A4);
                printf("\n");
                m_print(y);
                printf("\n\n");
            }
        }
    }

    freeMatrix(X);
    freeMatrix(y);
    FreeParameters(dp);
    FreeActivations(A);

	//Return parameters
    return p;
}