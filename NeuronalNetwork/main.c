#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "matrix.h"
#include "neuronalNetwork.h"

char training_src[] = "/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/";
//char training_src[] = "./dataset/TEST/";

double string_to_double(char *string)
{
    double result = 0;
    
    for(size_t i = 0; i < sizeof(string)/8; i++)
    {
        result *= 10;
        result += (double) string[i] - 48;
    }

    return result;
}

int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    /*datas **inputs = malloc(sizeof(datas) * 10);
    for(char n = 0; n < 10; n++)
    {
        char *src = training_src;
        strcat(src, &n);
        inputs[n] = get_imgList(src);
    }*/

    datas **inputs = malloc(sizeof(datas) * 10);
    inputs[0] = get_imgList("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/0/");
    inputs[1] = get_imgList("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/1/");
    inputs[2] = get_imgList("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/2/");
    inputs[3] = get_imgList("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/3/");
    inputs[4] = get_imgList("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/4/");
    inputs[5] = get_imgList("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/5/");
    inputs[6] = get_imgList("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/6/");
    inputs[7] = get_imgList("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/7/");
    inputs[8] = get_imgList("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/8/");
    inputs[9] = get_imgList("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/9/");


    //parameters *p = neuronal_network(inputs, 350, 350, 0.1, 1200, 0);
    parameters *p = neuronal_network(inputs, 150, 150, 0.1, 100, 1);

    matrix *img = imageToMatrix("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/8/8_310.png");
    //matrix *img = imageToMatrix("9.png");
    matrix *result = predict(img, p);
    m_print(result);

    return 0;
}
