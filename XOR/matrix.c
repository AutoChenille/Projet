//xor
//0 0 -> 0
//1 0 -> 1
//0 1 -> 1
//1 1 -> 0

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"

#define NumImputs 2
#define NumHiddenNodes 2
#define NumOutputs 1
#define NumTrainingSets 4 //Can be changed


//Recommanded video (in french): https://www.youtube.com/watch?v=VlMm4VZ6lk4&t=511s

double initWeights()
{
	return (double)rand() / (double)RAND_MAX;//random number between 0 and 1
}


double sigmoid(double x)//sigmoid function to normalize values(probability)
{
	return 1.0f / (1.0f + exp(-x));
}

double sigmoidDerivative(double x) //derivative of sigmoid
{
	return x * (1.0f - x);
}

void shuffle(int *array, size_t n)//shuffle array
{
	//Shuffle an array of integers. For each index, swap the value at that index. 
	//with a random value between itself and the end of the array.
	//for more info: https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
	if (n > 1)
	{
		size_t i;
		for (i = 0; i < n - 1; i++)
		{
			size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
			int temp = array[j];
			array[j] = array[i];
			array[i] = temp;
		}
	}
}

int main()
{

	//Initializes the weights and biases for the neural network.
	//It also initializes the training inputs and outputs.
	//It works with 2 inputs, 2 hidden nodes and 1 output.
	
	const double lr = 0.1f;// learning rate can change to train faster or slower

	double hiddenLayer[NumHiddenNodes];
	double outputLayer[NumOutputs];

	double hiddenLayerBias[NumHiddenNodes];
	double outputLayerBias[NumOutputs];

	double hiddenLayerWeights[NumImputs][NumHiddenNodes]; //2x2, beause 2 inputs and 2 hidden nodes
	double outputLayerWeights[NumHiddenNodes][NumOutputs]; //2x1, because 2 hidden nodes and 1 output

	double training_inputs[NumTrainingSets][NumImputs] = { {0.0f,0.0f},{0.0f,1.0f},{1.0f,0.0f},{1.0f,1.0f} }; //only 4 cases.
	double training_outputs[NumTrainingSets][NumOutputs] = { {0.0f},{1.0f},{1.0f},{0.0f} }; //only 4 cases.

	for (int i = 0; i < NumImputs; i++)
	{
		for (int j = 0; j < NumHiddenNodes; j++)
		{
			hiddenLayerWeights[i][j] = initWeights();
		}

	}

	for (int i = 0; i < NumHiddenNodes; i++)
	{
		for (int j = 0; j < NumOutputs; j++)
		{
			outputLayerWeights[i][j] = initWeights();
		}
	}

	for (int i = 0; i < NumHiddenNodes; i++)
	{
		hiddenLayerBias[i] = initWeights();
	}

	for (int i = 0; i < NumOutputs; i++)
	{
		outputLayerBias[i] = initWeights();
	}


	int trainingSetOrder[NumTrainingSets] = { 0,1,2,3 }; //array to shuffle the training sets

	int numEpochs = 10000; //number of times the training set will be used to train the network can be changed

	//TRAINING

	for(int epoch = 0; epoch < numEpochs; epoch++)
	{
		shuffle(trainingSetOrder, NumTrainingSets); //shuffle the training sets
		for(int x = 0; x < NumTrainingSets; x++)
		{
			int i = trainingSetOrder[x]; //get the training set

			//Forward Propagation

			//Activation of hidden layer
			for (int j = 0; j < NumHiddenNodes; j++)
			{
				double activation = hiddenLayerBias[j];
				for (int k = 0; k < NumImputs; k++)
				{
					activation += training_inputs[i][k] * hiddenLayerWeights[k][j];
				}
				hiddenLayer[j] = sigmoid(activation);
			}

			//Activation of output layer
			for (int j = 0; j < NumOutputs; j++)
			{
				double activation = outputLayerBias[j];
				for (int k = 0; k < NumHiddenNodes; k++)
				{
					activation += hiddenLayer[k] * outputLayerWeights[k][j];
				}
				outputLayer[j] = sigmoid(activation);//only one output
			}
		
			printf("Epoch : %d Input: %f %f Output: %f Expected: %f \n",
				epoch, training_inputs[i][0], training_inputs[i][1], outputLayer[0], training_outputs[i][0]);

		


			//Backpropagation

			//Calculate change in output layer weights

			double deltaOutput[NumOutputs];

			for(int j = 0; j < NumOutputs; j++)
			{
				double error = training_outputs[i][j] - outputLayer[j];
				deltaOutput[j] = error * sigmoidDerivative(outputLayer[j]);
			}

			//Calculate change in hidden layer weights

			double deltaHidden[NumHiddenNodes];

			for (int j = 0; j < NumHiddenNodes; j++)
			{
				double error = 0.0f;
				for (int k = 0; k < NumOutputs; k++)
				{
					error += deltaOutput[k] * outputLayerWeights[j][k];
				}
				deltaHidden[j] = error * sigmoidDerivative(hiddenLayer[j]);
			}

			//Update output layer weights
			for(int j = 0; j < NumOutputs; j++)
			{
				outputLayerBias[j] += lr * deltaOutput[j];
				for (int k = 0; k < NumHiddenNodes; k++)
				{
					outputLayerWeights[k][j] += lr * deltaOutput[j] * hiddenLayer[k];
				}
			}

			//Update hidden layer weights
			for(int j = 0; j < NumHiddenNodes; j++)
			{
				hiddenLayerBias[j] += lr * deltaHidden[j];
				for (int k = 0; k < NumImputs; k++)
				{
					hiddenLayerWeights[k][j] += lr * deltaHidden[j] * training_inputs[i][k];
				}
			}
                        /* debugging
			fputs("]\nFinal hidden weights:\n[", stdout);
			for(int j = 0; j < NumImputs; j++)
			{
				fputs("[", stdout);
				for(int k = 0; k < NumHiddenNodes; k++)
				{
					printf("%f ", hiddenLayerWeights[j][k]);
				}
				fputs("]", stdout);
			}

			fputs("]\nFinal hidden bias:\n[", stdout);
			for(int j = 0; j < NumHiddenNodes; j++)
			{
				printf("%g ", hiddenLayerBias[j]);
			}


			fputs("]\nFinal output weights:\n[", stdout);
			for(int j = 0; j < NumHiddenNodes; j++)
			{
				fputs("[", stdout);
				for(int k = 0; k < NumOutputs; k++)
				{
					printf("%f ", outputLayerWeights[j][k]);
				}
				fputs("] \n", stdout);
			}

			fputs("]\nFinal output bias:\n[", stdout);
			for(int j = 0; j < NumOutputs; j++)
			{
				printf("%f ", outputLayerBias[j]);
			}

			fputs("] \n", stdout);
                        */
		}

	}

	return 0;
}
