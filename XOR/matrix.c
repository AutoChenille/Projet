#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <err.h>
#include "matrix.h"


matrix *Matrix(size_t row, size_t col)
{
    //Create a matrix with row rows and col columns and return it
    matrix *m = malloc(sizeof(matrix));
    m->row = row;
    m->col = col;
    m->data = malloc(sizeof(double) * m->col * m->row);

    return m;
}


void m_print(const matrix m1)
{
    //Print matrix m1

    for(size_t i = 0; i < m1.row; i++)
    {
        printf("|");
        for(size_t j = 0; j < m1.col-1; j++)
        {
            printf("%lf ", m1.data[i*m1.col + j]);
        }
        printf("%lf|\n", m1.data[i*m1.col+m1.row-1]);
    }
 
}

matrix *m_copy(matrix m1)
{
    matrix *copy = Matrix(m1.row, m1.col);

    for(size_t i = 0; i < m1.row * m1.col; i++)
        copy->data[i] = m1.data[i];

    return copy;
}

matrix *m_sum(matrix m1, matrix m2)
{
    //Add m2 to m1 and return the result
    
    matrix *result = malloc(sizeof(matrix));
    result->row = m1.row;
    result->col = m1.col;
    result->data = malloc(sizeof(double) * result->row * result->col);

    if(m1.row != m2.row || m1.col != m2.col)
        errx(1, "Error : wrong dimension matrix");

    for(size_t i = 0; i < m1.row * m1.col; i++)
    {
        result->data[i] = m1.data[i] + m2.data[i];
    }

    return result;

}

matrix *m_sub(matrix m1, matrix m2)
{
    //Substract m2 to m1 and return the result

    return m_sum(m1, *m_dot(m2, -1));
}


matrix *m_dot(matrix m1, double k)
{
    //Multiply m1 by k and return the result
    
    matrix *result = Matrix(m1.row, m1.col);

    for(size_t i = 0; i < m1.row * m1.col; i++)
            result->data[i] = m1.data[i] * k;

    return result;
}


matrix *m_mul(const matrix m1, const matrix m2)
{
    //Multiply m1 by m2 and return the result

    if(m1.row != m2.col || m1.col != m2.row)
        errx(1, "Error : wrong dimension matrix");

    matrix *result = Matrix(m1.row, m2.col);

    for(size_t i = 0; i < m1.row; i++)
    {
        for(size_t j = 0; j < m2.col; j++)
        {
            double sum = 0;
            for(size_t k = 0; k < m1.col; k++)
            {
                sum += m1.data[i*m1.col + k] * m2.data[k*m2.col + i];
            }
            result->data[i*result->col + j] = sum;
        }
    }

    return result;
}
