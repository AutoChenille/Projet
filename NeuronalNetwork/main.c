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

    //m_print(data->input);


    parameters *p = neuronal_network(data->input, data->output, 100, 100, 0.1, 1000, 9, 1);

    //matrix *img = imageToMatrix("9_1548354109.3305757.png");
    matrix *img = imageToMatrix("9.png");
    matrix *result = predict(img, p);
    m_print(result);

    return 0;
}
