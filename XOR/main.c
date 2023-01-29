#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
//#include "perceptron.h"

int main()
{
    matrix m1 = *Matrix(2, 2);
    double tmp[] = {1, 2, 3, 4};
    m1.data =  tmp;

    m_print(m1);
    printf("\n");


    matrix m2;
    m2.row = 2;
    m2.col = 2;
    double tmp2[] = {2, 2, 2, 2};
    m2.data = tmp2;

    m_print(m2);
    printf("\n");

    /*matrix result;
    result.row = 2;
    result.col = 2;
    double tmp3[] = {0, 0, 0, 0};
    result.data = tmp3;*/

    printf("\n");


    m_print(*m_sum(m1, m2));

    return 0;
}
