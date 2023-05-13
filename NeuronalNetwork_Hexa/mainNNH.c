#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <time.h>
#include "../Ressources/matrix.h"
#include "../Ressources/neuronalNetwork.h"
#include "../Ressources/saveParams.h"
#include "../Ressources/BuildDataImage.h"

size_t layerSize16 = 800;
size_t nb_iter16 = 60000;

char** PredictSurface_16x16(SDL_Surface **surface, size_t nbData, char *params)
{
    parameters *p = LoadParameters(params);

    matrix* loaded = LoadFromSurface(surface, nbData);

    matrix *v = predictionVector(loaded, p);

    int* result = malloc(sizeof(int) * nbData);
    for(size_t j = 0; j < v->col; j++)
    {
        result[j] = 0;
        for(size_t i = 1; i < v->row; i++)
        {
            if(v->data[i*v->col+j] > v->data[result[j]*v->col+j])
                result[j] = i;
        }
    }

    char **cresult = malloc(sizeof(char*) * nbData);
    for(size_t i = 0; i < nbData; i++)
    {
        cresult[i] = malloc(sizeof(char) * nbData);
        for(size_t j = 0; j < nbData; j++)
        {
            if(result[i*nbData+j] == 0)
                cresult[i][j] = '.';
            else if(result[i*nbData+j] < 9)
                cresult[i][j] = result[i*nbData+j] + '0';
            else
                cresult[i][j] = result[i*nbData+j] + 'A' - 10;
        }
    }

    return cresult;
}


int Predict16(char *img, char *params)
{
    parameters *p = LoadParameters(params);
    /*matrix *m = imageToMatrix(img);
    int i = predict(m, p);
    matrix *v = predictionVector(m, p);
    m_print(v);
    return i;*/
    datas *topredict = get_imgList(img, 16);

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

void TrainNetwork16(char *data, char *savepath)
{
    //Data must contain 10 repo : one for each to treat

    //Get current directory
    char current_dir[1024];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("getcwd() error");
    }

    //Go to data dir
    datas *inputs = get_imgList(data, 16);

    //Come back to normal repo
    chdir(current_dir);

    //Train network
    parameters *p = neuronal_network(inputs, layerSize16, layerSize16, layerSize16, 0.1, nb_iter16, 1, NULL);
    //Save parameters to savepath
    SaveParameters(p, savepath);

    Predict16("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/", savepath);
   
}

void TrainAgain16(char *data, char *loadpath, char *savepath)
{
    //Data must contain 10 repo : one for each to treat

    //Get current directory
    char current_dir[1024];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
        perror("getcwd() error");

    //Go to data dir
    datas *inputs = get_imgList(data, 16);

    //Come back to normal repo
    chdir(current_dir);

    //Train network
    parameters *p = LoadParameters(loadpath);
    p = neuronal_network(inputs, layerSize16, layerSize16, layerSize16, 0.1, nb_iter16, 1, p);
    //Save parameters to savepath
    SaveParameters(p, savepath);

    Predict16("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/mnist_images/test", savepath);
   
}


int main_NeuronalNetwork_Hexa(int argc, char** argv)
{
    clock_t start = clock();
    // Checks the number of arguments.
    if (argc < 4)
        errx(EXIT_FAILURE, "Usage: -train or -predict");

    if(!strcmp(argv[1], "-train"))
    {
        TrainNetwork16(argv[2], argv[3]);
    }

    else if(!strcmp(argv[1], "-tA"))
    {
        TrainAgain16(argv[2], argv[3], argv[4]);
    }

    else if(!strcmp(argv[1], "-predict"))
    {
        Predict16(argv[2], argv[3]);
    }

    clock_t end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("execution time : %fs\n", cpu_time_used);
    return 0;
}