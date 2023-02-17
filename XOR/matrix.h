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

//free m1
void m_free(matrix *m1);

//Create a matrix of 0 with row rows and col columns and return it
matrix *Matrix(size_t row, size_t col);

//Shuffle m1 and m2 together
void shuffle_matrixXY(matrix *m1, matrix *m2);

//Create a matrix of x with row rows and col columns and return it
matrix *MatrixOf(size_t row, size_t col, double x);

//Print matrix m1
void m_print(const matrix *m1);

//Print name[](m1->row, m1->col)
void m_printSize(char name[], matrix *m1);

//Return a copy of m1
matrix *m_copy(const matrix *m1);

//Sum m1 and m2 and return the result
matrix *m_add(const matrix *m1, const matrix *m2);

//Add a column matrix m2 to m1 and return the result
matrix *m_addColumn(const matrix *m1, const matrix *m2);

//Sum all elements of m1 with k and return the result
matrix *m_scalarSum(const matrix *m1, double k);

//Substract m2 to m1 and return the result
matrix *m_sub(const matrix *m1, const matrix *m2);

//Multiply m1 by k and return the result
matrix *m_scalarProd(const matrix *m1, double k);

//Multiply m1 by m2 and return the result
matrix *m_mul(const matrix *m1, const matrix *m2);

//Multiply m1 and m2 terms by terms and return the result
matrix *m_LineBLineMul(const matrix *m1, const matrix *m2);

//Return the transposed matrix of m1
matrix *m_transpose(const matrix *m1);

//Apply function f on all elements of m1 and return the result
matrix *m_apply(double (*f)(double), const matrix *m1);

//Return the column matrix containing the sum of all the elements on each line of m1
matrix *m_horizontalSum(const matrix *m1);

//Return the line matrix containing the sum of all the elements on each column of m1
matrix *m_verticalSum(const matrix *m1);

//Return the sum of all the elements on each column of m1
double m_sum(const matrix *m1);
