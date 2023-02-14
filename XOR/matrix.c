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


void shuffle_matrixXY(matrix *m1, matrix *m2)
{
    srand(time(NULL));
    for(size_t i = 0; i < m1->row; i++)
    {
       size_t i2 = rand() % m1->row;

       double x0_temp = m1->data[i*m1->col];
       double x1_temp = m1->data[i*m1->col+1];
       double y_temp = m2->data[i];

       m1->data[i*m1->col] = m1->data[i2*m1->col];
       m1->data[i*m1->col+1] = m1->data[i2*m1->col+1];

       m2->data[i] = m2->data[i2];

       m1->data[i2*m1->col] = x0_temp;
       m1->data[i2*m1->col+1] = x1_temp;

       m2->data[i2] = y_temp;

    }
}

matrix *MatrixOf(size_t row, size_t col, double x)
{
    //Create a matrix with row rows and col columns, fill it with x and return it
    
    matrix *m = Matrix(row, col);

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

void m_printSize(matrix *m1)
{
    printf("(%li, %li)", m1->row, m1->col);
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
    
    //printf("m_add -> init result matrix\n");
    matrix *result = Matrix(m1->row, m1->col);

    //printf("m_add -> test dimensions\n");

    if(m1->row != m2->row || m1->col != m2->col)
    {
        //printf("m1 : %li x %li\n", m1->row, m1->col);
        //printf("m2 : %li x %li\n", m2->row, m2->col);
        errx(1, "Error sum: wrong dimension matrix\n");
    }

    //printf("m_add -> makes sum\n");
    for(size_t i = 0; i < m2->row * m2->col; i++)
    {
        result->data[i] = m1->data[i] + m2->data[i];
    }

    return result;

}

matrix *m_scalarSum(const matrix *m1, double k)
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
    matrix *result = Matrix(m1->row, m2->col);

    if(m1->row != m2->row || m1->col != m2->col)
        errx(1, "Error sum: wrong dimension matrix");

    for(size_t i = 0; i < m2->row * m2->col; i++)
    {
        result->data[i] = m1->data[i] - m2->data[i];
    }

    return result;
}


matrix *m_scalarProd(const matrix *m1, double k)
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
                //printf("m_mul -> calc i:%li x j:%li with %f and %f\n", i, k, m1->data[i*m1->col + k], m2->data[k*m2->col + j]);
                sum += m1->data[i*m1->col + k] * m2->data[k*m2->col + j];
            }
            result->data[i*result->col + j] = sum;
        }
    }

    return result;
}

matrix *m_LineBLineMul(const matrix *m1, const matrix *m2)
{
    if(m1->row != m2->row || m1->col != m2->col)
        errx(1, "Error prod: wrong dimension matrix");

    matrix *result = Matrix(m1->row, m1->col);

    for(size_t i = 0; i < m2->row * m2->col; i++)
    {
        result->data[i] = m1->data[i] * m2->data[i];
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

matrix *m_horizontalSum(const matrix *m1)
{
    //Return the vertical matrix containing the sum of all the elements on each line of m1

    matrix *result = Matrix(m1->row, 1);

    for(size_t i = 0; i < m1->row; i++)
    {
        double sum_line = 0;
        for(size_t j = 0; j < m1->col; j++)
        {
            sum_line += m1->data[i*m1->col+j];
        }
        result->data[i] = sum_line;
    }

    return result;
}

matrix *m_HSum_keepDim(const matrix *m1)
{
    //Return the vertical matrix containing the sum of all the elements on each line of m1

    matrix *result = Matrix(m1->row, m1->col);

    for(size_t i = 0; i < m1->row; i++)
    {
        double sum_line = 0;
        for(size_t j = 0; j < m1->col; j++)
        {
            sum_line += m1->data[i*m1->col+j];
        }
        for(size_t j= 0; j < m1->col; j++)
        {
            result->data[i*m1->col+j] = sum_line;
        }
    }

    return result;
}

matrix *m_verticalSum(const matrix *m1)
{
    //Return the vertical matrix containing the sum of all the elements on each column of m1

    matrix *result = Matrix(1, m1->col);

    for(size_t j = 0; j < m1->col; j++)
    {
        double sum_col = 0;
        for(size_t i = 0; i < m1->row; i++)
        {
            sum_col += m1->data[i*m1->col+j];
        }
        result->data[j] = sum_col;
    }

    return result;
}

double m_sum(const matrix *m1)
{
    //Return the vertical matrix containing the sum of all the elements on each column of m1

    double result = 0;

    for(size_t j = 0; j < m1->col; j++)
    {
        for(size_t i = 0; i < m1->row; i++)
        {
            result += m1->data[i*m1->col+j];
        }
    }

    return result;
}
