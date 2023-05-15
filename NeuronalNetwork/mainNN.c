#include <SDL2/SDL_surface.h>
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

 

size_t layerSize = 800;
size_t nb_iter = 1000;

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

int compareAndGetBest(char nb, size_t x1, size_t y1, size_t x2, size_t y2, matrix *prevVect)
{
    float pv1 = prevVect->data[(nb-'0')*81+x1*9+y1];
    float pv2 = prevVect->data[(nb-'0')*81+x2*9+y2];
    return pv1 > pv2 ? 1:0;
}

void opti_conflicts9(char **grid, matrix *prevVect)
{
    for(size_t x = 0; x < 9; x++)
    {
        for(size_t y = 0; y < 9; y++)
        {
            char current = grid[x][y];

            if(current != '.')
            {
                // Check column.
                for(size_t i = 0; i < 9; i++)
                {
                    if(grid[i][y] == current && i != x)
                    {
                        printf("1 - conflict beetween %li:%li and %li:%li\n", x, y, i, y);
                        int best = compareAndGetBest(current, x, y, i, y, prevVect);
                        if(best) //if best == current
                            grid[i][y] = '.';
                        else
                            grid[x][y] = '.';
                        opti_conflicts9(grid, prevVect);

                    }
                }

                // Check lines.
                for(size_t j = 0; j < 9; j++)
                {
                    if(grid[x][j] == current && j != y)
                    {
                        printf("2 - conflict beetween %li:%li and %li:%li\n", x, y, x, j);
                        size_t best = compareAndGetBest(current, x, y, x, j, prevVect);
                        if(best) //if best == current
                            grid[x][j] = '.';
                        else
                            grid[x][y] = '.';
                        opti_conflicts9(grid, prevVect);
                    }
                }

                // Check box.
                for(size_t i = x/3 * 3; i < (x/3 + 1) * 3; i++)
                {
                    for(size_t j = y/3 * 3; j < (y/3 + 1) * 3; j++)
                    {
                        if(grid[i][j] == current && (i != x || j != y))
                        {
                            printf("3 - conflict beetween %li:%li and %li:%li\n", x, y, i, j);
                            size_t best = compareAndGetBest(current, x, y, i, j, prevVect);
                            if(best) //if best == current
                                grid[i][j] = '.';
                            else
                                grid[x][y] = '.';
                            opti_conflicts9(grid, prevVect);
                        }
                    }
                }
            }
        }
    }
}


char** PredictSurface_9x9(SDL_Surface **surface, size_t nbData, char *params, matrix* v)
{
    parameters *p = LoadParameters(params);
    if(p == NULL)
        printf("Error loading params\n");

    matrix* loaded = LoadFromSurface(surface, nbData);

    v = predictionVector(loaded, p);

    int* result = malloc(sizeof(int) * nbData * nbData);
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
            else if(result[i*nbData+j] <= 9)
                cresult[i][j] = result[i*nbData+j] + '0';
            else
                cresult[i][j] = result[i*nbData+j] + 'A' - 10;
        }
    }

    opti_conflicts9(cresult, v);

    return cresult;
}

int Predict(char *img, char *params)
{
    parameters *p = LoadParameters(params);
    /*matrix *m = imageToMatrix(img);
    int i = predict(m, p);
    matrix *v = predictionVector(m, p);
    m_print(v);
    return i;*/
    datas *topredict = get_imgList(img, 10);

    matrix *v = predictionVector(topredict->input, p);

    float *result = calloc(sizeof(int), 10);
    float *attended = calloc(sizeof(int), 10);
    for(size_t j = 0; j < v->col; j++)
    {
        size_t i = 0;
        while(topredict->output->data[i*topredict->output->col+j] != 1)
            i++;
        result[i] += v->data[i*v->col+j] >= 0.5;
        attended[i] += 1;
    }
    printf("##### ACCURACY #####\n");
    for(size_t i = 0; i < 10; i++)
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
    datas *inputs = get_imgList(data, 10);

    //Come back to normal repo
    if (chdir(current_dir) != 0) {
        // Handle the error condition, e.g., display an error message or take appropriate action
        perror("chdir failed");
    }

    //Train network
    parameters *p = neuronal_network(inputs, layerSize, layerSize, layerSize, 1, nb_iter, 1, NULL);
    //Save parameters to savepath
    SaveParameters(p, savepath);

    Predict("./bestDataset/sudoku_sb/test", savepath);
   
}

void TrainAgain(char *data, char *loadpath, char *savepath)
{
    //Data must contain 10 repo : one for each to treat

    //Get current directory
    char current_dir[1024];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
        perror("getcwd() error");

    //Go to data dir
    datas *inputs = get_imgList(data, 10);

    //Come back to normal repo
    if (chdir(current_dir) != 0) {
        // Handle the error condition, e.g., display an error message or take appropriate action
        perror("chdir failed");
    }

    //Train network
    parameters *p = LoadParameters(loadpath);
    p = neuronal_network(inputs, layerSize, layerSize, layerSize, 0.1, nb_iter, 1, p);
    //Save parameters to savepath
    SaveParameters(p, savepath);

    Predict("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/mnist_images/test", savepath);
   
}


int main_NeuronalNetwork(int argc, char** argv)
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
