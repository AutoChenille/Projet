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

matrix *Matrix(size_t row, size_t col);

void shuffle_matrixXY(matrix *m1, matrix *m2);

matrix *MatrixOf(size_t row, size_t col, double x);

void m_print(const matrix *m1);

matrix *m_copy(const matrix *m1);

matrix *m_add(const matrix *m1, const matrix *m2);

matrix *m_scalarSum(const matrix *m1, double k);
    
matrix *m_sub(const matrix *m1, const matrix *m2);

matrix *m_scalarProd(const matrix *m1, double k);

matrix *m_mul(const matrix *m1, const matrix *m2);

matrix *m_LineBLineMul(const matrix *m1, const matrix *m2);

matrix *m_transpose(const matrix *m1);

matrix *m_apply(double (*f)(double), const matrix *m1);

matrix *m_horizontalSum(const matrix *m1);

matrix *m_verticalSum(const matrix *m1);

double m_sum(const matrix *m1);
