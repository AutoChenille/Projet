#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <err.h>
#include <math.h>
#include "matrix.h"


void freeMatrix(matrix *m1)
{
    free(m1->data);
    free(m1);
}


matrix *Matrix(size_t row, size_t col)
{
    matrix *m = malloc(sizeof(matrix));
    m->row = row;
    m->col = col;
    m->data = malloc(m->row * m->col * sizeof(float));
    if (!m->data) {
        freeMatrix(m);
        return NULL;
    }

    return m;
}

void shuffle_matrixXY(matrix *m1, matrix *m2)
{
    srand(time(NULL));
    for(size_t i = 0; i < m1->row; i++)
    {
       size_t i2 = rand() % m1->row;

       float x0_temp = m1->data[i*m1->col];
       float x1_temp = m1->data[i*m1->col+1];
       float y_temp = m2->data[i];

       m1->data[i*m1->col] = m1->data[i2*m1->col];
       m1->data[i*m1->col+1] = m1->data[i2*m1->col+1];

       m2->data[i] = m2->data[i2];

       m1->data[i2*m1->col] = x0_temp;
       m1->data[i2*m1->col+1] = x1_temp;

       m2->data[i2] = y_temp;
    }
}

void shuffle(matrix *m1, matrix *m2)
{
    // Initialise le générateur de nombres aléatoires avec une graine aléatoire basée sur l'horloge du système
    srand(time(NULL));
    
    // Mélange les colonnes en échangeant aléatoirement les éléments
    for (size_t i = m1->col - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        if (i != j) {
            for (size_t k = 0; k < m1->row; k++)
            {
                float temp = m1->data[k * m1->col + i];
                m1->data[k * m1->col + i] = m1->data[k * m1->col + j];
                m1->data[k * m1->col + j] = temp;
            }
            for (size_t k = 0; k < m2->row; k++)
            {
                float temp = m2->data[k * m2->col + i];
                m2->data[k * m2->col + i] = m2->data[k * m2->col + j];
                m2->data[k * m2->col + j] = temp;
            }
        }
    }
}

matrix *MatrixOf(size_t row, size_t col, float x)
{
    matrix *m = Matrix(row, col);

    for(size_t i = 0; i < m->row * m->col; i++)
        m->data[i] = x;

    return m;
}

void m_print(matrix *m1)
{
    for(size_t i = 0; i < m1->row; i++)
    {
        printf("|");
        size_t j = 0;
        while(j < m1->col-1)
        {
            printf("%f ", m1->data[i*m1->col + j]);
            j++;
        }
        printf("%f|\n", m1->data[i*m1->col + j]);
    }
}

void m_printSize(char name[], matrix *m1)
{
    printf("%s(%zu, %zu)\n", name, m1->row, m1->col);
}

matrix *m_copy(matrix *m1)
{
    matrix *copy = Matrix(m1->row, m1->col);
    if (!copy) {
        return NULL;
    }

    for(size_t i = 0; i < m1->row * m1->col; i++)
        copy->data[i] = m1->data[i];

    return copy;
}

void m_copyTo(matrix *src, matrix *dest)
{
    if(src->col != dest->col || src->row != dest->row)
        errx(1, "copy_to : src and dest does not have the same sizes.");

    for(size_t i = 0; i < src->row * src->col; i++)
        dest->data[i] = src->data[i];
}

void m_copyTo_destroy(matrix *src, matrix *dest)
{
    if(src->col != dest->col || src->row != dest->row)
        errx(1, "copy_to : src and dest does not have the same sizes.");

    free(dest->data);
    dest->data = src->data;
    src->data = NULL;
}

matrix *m_add(matrix *m1, matrix *m2)
{
    //Add m2 to m1 and return the result
    
    if(m1->row != m2->row || m1->col != m2->col)
    {
        errx(1, "Error sum: wrong dimension matrix\n");
    }
    
    matrix *result = Matrix(m1->row, m1->col);
    
    for(size_t i = 0; i < m2->row * m2->col; i++)
    {
        result->data[i] = m1->data[i] + m2->data[i];
    }

    return result;
}

void m_add_Place(matrix *m1, matrix *m2)
{
    //Add m2 to m1 and return the result
    
    if(m1->row != m2->row || m1->col != m2->col)
    {
        errx(1, "Error sum: wrong dimension matrix\n");
    }
    
    for(size_t i = 0; i < m2->row * m2->col; i++)
    {
        m1->data[i] = m1->data[i] + m2->data[i];
    }
}

matrix *m_addColumn(matrix *m1, matrix *m2)
{
    //Add a column matrix m2 to m1 and return the result
    //Each column Ci of result is the sum of the ith column of m1 with m2
    
    if(m1->row != m2->row || m2->col != 1)
    {
        errx(1, "Error sum: wrong dimension matrix\n");
    }
    
    matrix *result = Matrix(m1->row, m1->col);

    for(size_t i = 0; i < m1->row; i++)
    {
        for(size_t j = 0; j < m1->col; j++)
        {
            result->data[i*result->col+j] = m1->data[i*m1->col+j] + m2->data[i];
        }
    }

    return result;
}

void m_addColumn_Place(matrix *m1, matrix *m2)
{
    //Add a column matrix m2 to m1 and return the result
    //Each column Ci of result is the sum of the ith column of m1 with m2
    
    if(m1->row != m2->row || m2->col != 1)
    {
        errx(1, "Error sum: wrong dimension matrix\n");
    }

    for(size_t i = 0; i < m1->row; i++)
    {
        for(size_t j = 0; j < m1->col; j++)
        {
            m1->data[i*m1->col+j] = m1->data[i*m1->col+j] + m2->data[i];
        }
    }
}

matrix *m_scalarSum(matrix *m1, float k)
{
    //Sum all elements of m1 with k and return the result
    
    matrix *result = Matrix(m1->row, m1->col);

    for(size_t i = 0; i < m1->row * m1->col; i++)
    {
        result->data[i] = m1->data[i] + k;
    }

    return result;
}

void m_scalarSum_Place(matrix *m1, float k)
{
    //Sum all elements of m1 with k and return the result

    for(size_t i = 0; i < m1->row * m1->col; i++)
    {
        m1->data[i] = m1->data[i] + k;
    }
}

matrix *m_sub(matrix *m1, matrix *m2)
{
    //Substract m2 to m1 and return the result
    
    matrix *result = Matrix(m1->row, m2->col);

    if(m1->row != m2->row || m1->col != m2->col)
        errx(1, "Error sub: wrong dimension matrix");

    for(size_t i = 0; i < m2->row * m2->col; i++)
    {
        result->data[i] = m1->data[i] - m2->data[i];
    }

    return result;
}

void m_sub_Place(matrix *m1, matrix *m2)
{
    //Substract m2 to m1 and return the result

    if(m1->row != m2->row || m1->col != m2->col)
        errx(1, "Error sub: wrong dimension matrix");

    for(size_t i = 0; i < m2->row * m2->col; i++)
    {
        m1->data[i] -= m2->data[i];
    }
}


matrix *m_scalarProd(matrix *m1, float k)
{
    //Multiply m1 by k and return the result
    
    matrix *result = Matrix(m1->row, m1->col);

    for(size_t i = 0; i < m1->row * m1->col; i++)
            result->data[i] = m1->data[i] * k;

    return result;
}

void m_scalarProd_Place(matrix *m1, float k)
{
    //Multiply m1 by k and return the result

    for(size_t i = 0; i < m1->row * m1->col; i++)
            m1->data[i] = m1->data[i] * k;
}

matrix *m_mul(matrix *m1, matrix *m2)
{
    //Multiply m1 by m2 and return the result
    
    if(m1->col != m2->row)
        errx(1, "Error mul: wrong dimension matrix");

    matrix *result = Matrix(m1->row, m2->col);

    for(size_t i = 0; i < m1->row; i++)
    {
        for(size_t j = 0; j < m2->col; j++)
        {
            float sum = 0;
            for(size_t k = 0; k < m1->col; k++)
            {
                sum += m1->data[i*m1->col + k] * m2->data[k*m2->col + j];
            }
            result->data[i*result->col + j] = sum;
        }
    }

    return result;
}

matrix *m_LineBLineMul(matrix *m1, matrix *m2)
{
	//Multiply m1 and m2 terms by terms and return the result
	//For all (i, j) : result[i,j] = m1[i,j] * m2[i,j]
	
    if(m1->row != m2->row || m1->col != m2->col)
        errx(1, "Error LineBLineMul: wrong dimension matrix");

    matrix *result = Matrix(m1->row, m1->col);

    for(size_t i = 0; i < m2->row * m2->col; i++)
    {
        result->data[i] = m1->data[i] * m2->data[i];
    }

    return result;
}

void m_LineBLineMul_Place(matrix *m1, matrix *m2)
{
	//Multiply m1 and m2 terms by terms and return the result
	//For all (i, j) : result[i,j] = m1[i,j] * m2[i,j]
	
    if(m1->row != m2->row || m1->col != m2->col)
        errx(1, "Error LineBLineMul: wrong dimension matrix");

    for(size_t i = 0; i < m2->row * m2->col; i++)
    {
        m1->data[i] = m1->data[i] * m2->data[i];
    }
}

matrix *m_transpose(matrix *m1)
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

matrix *m_apply(float (*f)(float), matrix *m1)
{
    //Apply function f on all elements of m1 and return the result
    
    matrix *result = Matrix(m1->row, m1->col);

    for(size_t i = 0; i < m1->row * m1->col; i++)
            result->data[i] = (*f)(m1->data[i]);

    return result;
}

matrix *m_horizontalSum(matrix *m1)
{
    //Return the column matrix containing the sum of all the elements on each line of m1

    matrix *result = Matrix(m1->row, 1);

    for(size_t i = 0; i < m1->row; i++)
    {
        float sum_line = 0;
        for(size_t j = 0; j < m1->col; j++)
        {
            sum_line += m1->data[i*m1->col+j];
        }
        result->data[i] = sum_line;
    }

    return result;
}

matrix *m_verticalSum(matrix *m1)
{
    //Return the line matrix containing the sum of all the elements on each column of m1

    matrix *result = Matrix(1, m1->col);

    for(size_t j = 0; j < m1->col; j++)
    {
        float sum_col = 0;
        for(size_t i = 0; i < m1->row; i++)
        {
            sum_col += m1->data[i*m1->col+j];
        }
        result->data[j] = sum_col;
    }

    return result;
}

float m_sum(matrix *m1)
{
    //Return the sum of all the elements on each column of m1

    float result = 0;

    for(size_t j = 0; j < m1->col; j++)
    {
        for(size_t i = 0; i < m1->row; i++)
        {
            result += m1->data[i*m1->col+j];
        }
    }

    return result;
}

matrix *apply_softmax(matrix *m1)
{
    //Return the softmaxed vector computed with m1 

    matrix *result = Matrix(m1->row, m1->col);

    for(size_t j = 0; j < m1->col; j ++)
    {
        //Find the max for each vect
        float max = m1->data[m1->col+j];
        for(size_t i = 1; i < m1->row; i++)
        {
            if(m1->data[i*m1->col+j] > max)
                max = m1->data[i*m1->col+j];
        }

        //Compute the exponential sum
        float sum = 0;
        for(size_t i = 0; i < m1->row; i++)
        {
            sum += expf(m1->data[i*m1->col+j] - max);
        }

        //Compute the probabilities
        for(size_t i = 0; i < m1->row; i++)
        {
                result->data[i*result->col+j] = expf(m1->data[i*result->col+j] - max)/sum;
        }
    }

    return result;
}

void m_normalDiv(matrix *m1)
{
    //Divide in place all datas in m1 by the biggest one
    for(size_t j = 0; j < m1->col; j ++)
    {
        float max = m1->data[m1->col + j];
        for(size_t i = 0; i < m1->row; i++)
        {  
            if(m1->data[i*m1->col+j] > max)
                max = m1->data[i*m1->col+j];
        }

        if(max > 1)
        {
            for(size_t i = 0; i < m1->row; i++)
                m1->data[i*m1->col+j] /= max;
        }
    }
}

float maxf(float x, float y)
{
    if(x > y)
        return x;
    return y;
}

matrix *apply_relu(matrix *m1)
{
    //Return the relued matrix computed with m1
    
    //m_normalDiv(m1);
    matrix *result = m_copy(m1);

    for(size_t i = 0; i < m1->row*m1->col; i++)
    {
        result->data[i] = maxf(m1->data[i], 0);
    }

    //m_print(result);

    m_normalDiv(result);

    return result;
}