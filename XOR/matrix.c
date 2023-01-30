#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <err.h>
#include "matrix.h"


void m_free(matrix *m1)
{
    free(m1->data);
    free(m1);
}

matrix *Matrix(size_t row, size_t col)
{
    //Create a matrix with row rows and col columns and return it
    matrix *m = malloc(sizeof(matrix));
    m->row = row;
    m->col = col;
    m->data = malloc(sizeof(double) * m->col * m->row);

    return m;
}


matrix *MatrixOf(size_t row, size_t col, double x)
{
    //Create a matrix with row rows and col columns, fill it with x and return it
    
    matrix *m = malloc(sizeof(matrix));
    m->row = row;
    m->col = col;
    m->data = malloc(sizeof(double) * m->col * m->row);

    for(size_t i = 0; i < m->row * m->col; i++)
        m->data[i] = x;

    return m;
}


void m_print(const matrix *m1)
{
    //Print matrix m1

    for(size_t i = 0; i < m1->row; i++)
    {
        printf("|");
        size_t j = 0;
        while(j < m1->col-1)
        {
            printf("%lf ", m1->data[i*m1->col + j]);
            j++;
        }
        printf("%lf|\n", m1->data[i*m1->col + j]);
    }
 
}

matrix *m_copy(const matrix *m1)
{
    matrix *copy = Matrix(m1->row, m1->col);

    for(size_t i = 0; i < m1->row * m1->col; i++)
        copy->data[i] = m1->data[i];

    return copy;
}

matrix *m_add(const matrix *m1, const matrix *m2)
{
    //Add m2 to m1 and return the result
    
    matrix *result = Matrix(m1->row, m2->col);

    if(m1->row != m2->row || m1->col != m2->col)
        errx(1, "Error sum: wrong dimension matrix");

    for(size_t i = 0; i < m2->row * m2->col; i++)
    {
        result->data[i] = m1->data[i] + m2->data[i];
    }

    return result;

}

matrix *m_addS(const matrix *m1, double k)
{
    //Add m2 to m1 and return the result
    
    matrix *result = Matrix(m1->row, m1->col);

    for(size_t i = 0; i < m1->row * m1->col; i++)
    {
        result->data[i] = m1->data[i] + k;
    }

    return result;

}

matrix *m_sub(const matrix *m1, const matrix *m2)
{
    //Substract m2 to m1 and return the result

    matrix *inv = m_dot(m2, -1);
    matrix *result =  m_add(m1, inv);
    free(inv);
    return result;
}


matrix *m_dot(const matrix *m1, double k)
{
    //Multiply m1 by k and return the result
    
    matrix *result = Matrix(m1->row, m1->col);

    for(size_t i = 0; i < m1->row * m1->col; i++)
            result->data[i] = m1->data[i] * k;

    return result;
}


matrix *m_mul(const matrix *m1, const matrix *m2)
{
    //Multiply m1 by m2 and return the result

    if(m1->col != m2->row)
        errx(1, "Error mul: wrong dimension matrix");

    matrix *result = Matrix(m1->row, m2->col);

    for(size_t i = 0; i < m1->row; i++)
    {
        for(size_t j = 0; j < m2->col; j++)
        {
            double sum = 0;
            for(size_t k = 0; k < m1->col; k++)
            {
                sum += m1->data[i*m1->col + k] * m2->data[k*m2->col + i];
            }
            result->data[i*result->col + j] = sum;
        }
    }

    return result;
}

matrix *m_transpose(const matrix *m1)
{
    //Return the transposed matrix of m1

    matrix *T = Matrix(m1->col, m1->row);

    for(size_t i = 0; i < m1->row; i++)
    {
        for(size_t j = 0; j < m1->col; j++)
        {
            T->data[j*T->col + i] = m1->data[i*m1->col + j];
        }
    }

    return T;
}

matrix *m_apply(double (*f)(double), const matrix *m1)
{
    //Apply function f on all elements of m1 and return the resulted matrix
    matrix *result = Matrix(m1->row, m1->col);

    for(size_t i = 0; i < m1->row * m1->col; i++)
            result->data[i] = (*f)(m1->data[i]);

    return result;
}

double m_sum(const matrix *m1)
{
    //Return the sum of all elements of m1

    double result = 0;

    for(size_t i = 0; i < m1->row * m1->col; i++)
        result += m1->data[i];

    return result;
}

