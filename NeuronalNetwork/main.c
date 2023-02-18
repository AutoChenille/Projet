#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "matrix.h"
#include "neuronalNetwork.h"
#include "BuildDataImage.h"

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

    matrix *data = get_imgList(argv[1]);
    m_print(data);


    return EXIT_SUCCESS;
}
