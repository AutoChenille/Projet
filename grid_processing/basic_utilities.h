#ifndef BASIC_UTILITIES_H
#define BASIC_UTILITIES_H

// Libraries.
// ==============
#include <math.h>
// ==============

double deg_to_rad(double degree);
void make_double_array(int size, double min, double max, double dest[]);
int max_array(const int* arr, int size);
int min_array_index_abs(const double* arr, int size);
int max_array_index_abs(const double* arr, int size);
int distance(int x1, int y1, int x2, int y2);

#endif