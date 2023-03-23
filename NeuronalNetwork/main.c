#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "matrix.h"
#include "neuronalNetwork.h"
#include "BuildDataImage.h"

//char training_src[] = "./dataset/normalizedSACHA/";
char training_src[] = "./dataset/TEST/";

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

    datas *data = get_imgList(training_src);


    parameters *p = neuronal_network(data->input, data->output, 100, 100, 0.1, 1000, 1);

    matrix *img = imageToMatrix("1.png");
    matrix *result = predict(img, p);
    m_print(result);

    return 0;
}
