#pragma once

#include <stddef.h>

// we use row-major matrix structure

typedef struct 
{
    size_t row;
    size_t col;
    float *data;
}matrix;

float maxf(float a, float b);

//free m1
void freeMatrix(matrix *m1);

//Create a matrix of 0 with row rows and col columns and return it
matrix *Matrix(size_t row, size_t col);

//Shuffle m1 and m2 together
void shuffle_matrixXY(matrix *m1, matrix *m2);
void shuffle(matrix *m1, matrix *m2);

//Create a matrix of x with row rows and col columns and return it
matrix *MatrixOf(size_t row, size_t col, float x);

//Print matrix m1
void m_print(matrix *m1);

//Print name[](m1->row, m1->col)
void m_printSize(char name[], matrix *m1);

//Return a copy of m1
matrix *m_copy(matrix *m1);
void m_copyTo(matrix *src, matrix *dest);

//Sum m1 and m2 and return the result
matrix *m_add(matrix *m1, matrix *m2);
void m_add_Place(matrix *m1, matrix *m2);

//Add a column matrix m2 to m1 and return the result
matrix *m_addColumn(matrix *m1, matrix *m2);
void m_addColumn_Place(matrix *m1, matrix *m2);

//Sum all elements of m1 with k and return the result
matrix *m_scalarSum(matrix *m1, float k);
void m_scalarSum_Place(matrix *m1, float k);

//Substract m2 to m1 and return the result
matrix *m_sub(matrix *m1, matrix *m2);
void m_sub_Place(matrix *m1, matrix *m2);

//Multiply m1 by k and return the result
matrix *m_scalarProd(matrix *m1, float k);
void m_scalarProd_Place(matrix *m1, float k);

//Multiply m1 by m2 and return the result
matrix *m_mul(matrix *m1, matrix *m2);

//Multiply m1 and m2 terms by terms and return the result
matrix *m_LineBLineMul(matrix *m1, matrix *m2);
void m_LineBLine_Place(matrix *m1, matrix *m2);

//Return the transposed matrix of m1
matrix *m_transpose(matrix *m1);

//Apply function f on all elements of m1 and return the result
matrix *m_apply(float (*f)(float), matrix *m1);
void m_apply_Place(float (*f)(float), matrix *m2);

//Return the column matrix containing the sum of all the elements on each line of m1
matrix *m_horizontalSum(matrix *m1);

//Return the line matrix containing the sum of all the elements on each column of m1
matrix *m_verticalSum(matrix *m1);

//Return the sum of all the elements on each column of m1
float m_sum(matrix *m1);

//Return the softmaxed vector computed with m1
matrix *apply_softmax(matrix *m1);

//Divide in place all datas in m1 by the biggest one
void m_normalDiv(matrix *m1);

//Return the relued matrix computed with m1
matrix *apply_relu(matrix *m1);