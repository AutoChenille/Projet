#include <stddef.h>

#pragma once

// we use row-major matrix structure

typedef struct 
{
    size_t row;
    size_t col;
    double *data;
}matrix;

//Print matrix m1
void m_print(matrix m1);

//Create a matrix with row rows and col columns and return it
matrix *Matrix(size_t row, size_t col);

//Return a copy of m1
matrix *m_copy(matrix m1);

//Add m2 to m1 and return the result
matrix *m_sum(matrix m1, matrix m2);

//Substract m2 to m1 and return the result
matrix *m_sub(matrix m1, matrix m2);

//Multiply m1 by k and return the result
matrix *m_dot(matrix m1, double k);

//Multiply m1 by m2 and return the result
matrix *m_mul(const matrix m1, const matrix m2);

//Return the transposed matrix of m1
matrix *m_transpose(matrix m1);
