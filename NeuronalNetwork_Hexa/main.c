#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <time.h>
#include "matrix.h"
#include "neuronalNetwork.h"
#include "saveParams.h"

size_t layerSize = 800;
size_t nb_iter = 60000;

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

char *sizeTToPath(size_t num)
{
    //Convert a size_t num to char './num/
    char *result = malloc(sizeof(char) * 20); // Assumes a maximum size_t of 20 digits
    sprintf(result, "./%zu/", num);
    return result;
}

int Predict(char *img, char *params)
{
    parameters *p = LoadParameters(params);
    /*matrix *m = imageToMatrix(img);
    int i = predict(m, p);
    matrix *v = predictionVector(m, p);
    m_print(v);
    return i;*/
    datas *topredict = get_imgList(img);

    matrix *v = predictionVector(topredict->input, p);

    float *result = calloc(sizeof(int), 16);
    float *attended = calloc(sizeof(int), 16);
    for(size_t j = 0; j < v->col; j++)
    {
        size_t i = 0;
        while(topredict->output->data[i*topredict->output->col+j] != 1)
            i++;
        result[i] += v->data[i*v->col+j] >= 0.5;
        attended[i] += 1;
    }
    printf("##### ACCURACY #####\n");
    for(size_t i = 0; i < 16; i++)
    {
        printf("-> %li : %f%%\n", i, maxf(result[i]/attended[i]*100, 0));
    }

    return 0;
}

void TrainNetwork(char *data, char *savepath)
{
    //Data must contain 10 repo : one for each to treat

    //Get current directory
    char current_dir[1024];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("getcwd() error");
    }

    //Go to data dir
    datas *inputs = get_imgList(data);

    //Come back to normal repo
    chdir(current_dir);

    //Train network
    parameters *p = neuronal_network(inputs, layerSize, layerSize, layerSize, 0.1, nb_iter, 1, NULL);
    //Save parameters to savepath
    SaveParameters(p, savepath);

    Predict("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/", savepath);
   
}

void TrainAgain(char *data, char *loadpath, char *savepath)
{
    //Data must contain 10 repo : one for each to treat

    //Get current directory
    char current_dir[1024];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
        perror("getcwd() error");

    //Go to data dir
    datas *inputs = get_imgList(data);

    //Come back to normal repo
    chdir(current_dir);

    //Train network
    parameters *p = LoadParameters(loadpath);
    p = neuronal_network(inputs, layerSize, layerSize, layerSize, 0.1, nb_iter, 1, p);
    //Save parameters to savepath
    SaveParameters(p, savepath);

    Predict("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/mnist_images/test", savepath);
   
}


int main(int argc, char** argv)
{
    clock_t start = clock();
    // Checks the number of arguments.
    if (argc < 4)
        errx(EXIT_FAILURE, "Usage: -train or -predict");

    if(!strcmp(argv[1], "-train"))
    {
        TrainNetwork(argv[2], argv[3]);
    }

    else if(!strcmp(argv[1], "-tA"))
    {
        TrainAgain(argv[2], argv[3], argv[4]);
    }

    else if(!strcmp(argv[1], "-predict"))
    {
        Predict(argv[2], argv[3]);
    }

    clock_t end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("execution time : %fs\n", cpu_time_used);
    return 0;
}