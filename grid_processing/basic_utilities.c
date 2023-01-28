#include "basic_utilities.h"

// Cast degrees in radians.
//
// degree: degree to convert.
// return: result of conversion.
double deg_to_rad(double degree)
{
    return degree * 3.14159265359 / 180;
}

// Makes an array.
//
// size: size of the array.
// max: maximum of the array.
// min: minimum of the array.
// return: nothing.
void make_double_array(int size, double min, double max, double dest[])
{
    // Gets the step between each value.
    double step = (fabs(min) + fabs(max)) / ((double) size);

    // Counter
    double cpt = min;

    // Fills array.
    for (int i = 0; i < size; i++)
    {
        dest[i] = cpt;
        cpt += step;
    }
}

// Returns the maximum integer of an array.
//
// arr: the array with the maximum.
// size: size of the array.
// return: maximum of the array.
int max_array(const int* arr, int size)
{
    int max = arr[0];

    for (int i = 1; i < size; i++)
    {
        if (max < arr[i])
            max = arr[i];

    }

    return max;
}