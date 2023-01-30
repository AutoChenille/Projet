#pragma once

#include <stddef.h>

// we use row-major matrix structure

typedef struct 
{
    size_t row;
    size_t col;
    double *data;
}matrix;

typedef struct
{
    matrix *m1;
    matrix *m2;
}matrix_tuple;


void m_free(matrix *m1);

//Print matrix m1
void m_print(const matrix *m1);

//Create a matrix with row rows and col columns and return it
matrix *Matrix(size_t row, size_t col);
matrix *MatrixOf(size_t row, size_t col, double x);

//Return a copy of m1
matrix *m_copy(const matrix *m1);

//Add m2 to m1 and return the result
matrix *m_add(const matrix *m1, const matrix *m2);
matrix *m_addS(const matrix *m1, double k);

//Substract m2 to m1 and return the result
matrix *m_sub(const matrix *m1, const matrix *m2);

//Multiply m1 by k and return the result
matrix *m_dot(const matrix *m1, double k);

//Multiply m1 by m2 and return the result
matrix *m_mul(const matrix *m1, const matrix *m2);

//Return the transposed matrix of m1
matrix *m_transpose(const matrix *m1);

//Return the transposed matrix of m0
matrix *m_transpose(const matrix *m1);

//Apply function f on all elements of m1 and return the resulted matrix
matrix *m_apply(double (*f)(double), const matrix *m1);

//Return the sum of all elements of m1
double m_sum(const matrix *m1);
