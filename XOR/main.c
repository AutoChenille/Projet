#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "matrix.h"
#include "xor.h"

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

int main(int argc, char **argv)
{
    if(argc != 3 && argc != 4)
        errx(1, "2 parameters requiered, %i given.\n", argc-1);

    //gettting parameters
    double x0 = string_to_double(argv[1]);
    double x1 = string_to_double(argv[2]);
    //testing them
    if(x0 != 0 && x0 != 1)
        errx(1, "Wrong argument for XOR given.\n");
    if(x1 != 0 && x1 != 1)
        errx(1, "Wrong argument for XOR given.\n");

    //XOR DATA
    matrix *X = Matrix(2, 4);
    double tmp[] = {0, 0, 1, 1, 0, 1, 1, 0};
    X->data = tmp;

    matrix *y = Matrix(1, 4);
    double tmp2[] = {0, 1, 0, 1};
    y->data = tmp2;


    //XOR TRAINING
    int show_debug = argc == 4 ? (int)string_to_double(argv[3]) : 0;
    parameters *p = neuronal_network(X, y, 2, 1, 100000, show_debug);

    if(show_debug)
    {
        printf("#############################################\n");
        printf(  "############# TRAINING ENDED ################");
        printf("\n#############################################\n\n");
    }


    //PREDICTION RESULT

    //Creating entry matrix
    matrix *Xentry = Matrix(2, 1);
    Xentry->data[0] = x0;
    Xentry->data[1] = x1;

    //Predict
    matrix *result = predict(Xentry, p);
    double prob = result->data[0];

    if(show_debug)
    {
        printf("\n\n#############################################\n");
        printf(    "############ PREDICTION RESULTS #############");
        printf(  "\n#############################################\n\n");
        printf("Prediction result : ");
        printf("XOR(%i, %i) = %f\n", (int) x0, (int) x1, prob);
        printf("Return result : ");
        printf("XOR(%i, %i) = %i\n", (int) x0, (int) x1, prob >= 0.5);

    }
    else
        printf("XOR(%i, %i) = %i\n", (int) x0, (int) x1, prob >= 0.5);


    //FREE MATRIX
    //m_free(X);
    //m_free(y);
    free(p);
    m_free(Xentry);
    m_free(result);

    //return
    return prob >= 0.5;

}
