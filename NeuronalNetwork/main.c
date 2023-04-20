#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include "matrix.h"
#include "neuronalNetwork.h"
#include "saveParams.h"

size_t layerSize = 200;
size_t nb_iter = 10000;

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
    parameters *p = neuronal_network(inputs, layerSize, layerSize, 1, nb_iter, 1);
    //Save parameters to savepath
    SaveParameters(p, savepath);

    datas *topredict = get_imgList("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/Predict/");
    matrix *v = predictionVector(topredict->input, p);
    int result = 0;
    for(size_t j = 1; j < v->col; j++)
    {
        size_t max = 0;
        for(size_t i = 1; i < 10; i++)
        {
            if(v->data[i*v->col+j] > v->data[max*v->col+j])
                max = i;
        }
        if(topredict->output->data[max*topredict->output->col+j] == 1)
            result++;
    }
    printf("accuracy : %i/%li\n", result, topredict->input->col);
}

int Predict(char *img, char *params)
{
    parameters *p = LoadParameters(params);
    /*matrix *m = imageToMatrix(img);
    int i = predict(m, p);
    matrix *v = predictionVector(m, p);
    m_print(v);
    return i;*/

    size_t *nbData = malloc(sizeof(size_t));
    datas *topredict = get_imgList(img);

    matrix *v = predictionVector(topredict->input, p);

    int result = 0;
    for(size_t j = 1; j < v->col; j++)
    {
        size_t max = 0;
        for(size_t i = 1; i < 10; i++)
        {
            if(v->data[i*v->col+j] > v->data[max*v->col+j])
                max = i;
        }
        if(topredict->output->data[max*topredict->output->col+j] == 1)
            result++;
    }
    printf("accuracy : %i/%li\n", result, topredict->input->col);

    return 0;
}


int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 4)
        errx(EXIT_FAILURE, "Usage: image-file");

    if(!strcmp(argv[1], "-train"))
    {
        TrainNetwork(argv[2], argv[3]);
    }

    else if(!strcmp(argv[1], "-predict"))
    {
        int result = Predict(argv[2], argv[3]);
        printf("%i\n", result);
    }

    return 0;
}