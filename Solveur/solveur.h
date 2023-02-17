#include <stddef.h>

#pragma once

typedef struct{
    size_t size;
    int *data;
}Matrix;

void print_matrix(Matrix *matrix);

int is_possible(int matrix[9][9], size_t i, size_t j, int n);
void solve(int matrix[9][9]);
