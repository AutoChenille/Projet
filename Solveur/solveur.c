#include <stdio.h>
#include "solveur.h"



void print_matrix(int matrix[9][9])
{
    for(size_t i = 0; i < 9; i++)
    {
        if(i == 0 || i == 3 || i == 6 )
            printf("\n -----------");

        printf("\n");
        printf("|");
        for(size_t j = 0; j < 9; j++)
        {
            printf("%i", matrix[i][j]);

            if(j == 2 || j == 5)
                printf("|");
        }
        printf("|");

    }

    printf("\n -----------\n");

}

int is_possible(int matrix[9][9], size_t i, size_t j, int n)
{
    //checking lines
    for(size_t k = 0; k < 9; k++)
    {
        if(matrix[i][k] == n && k != j)
            return 0;
    }

    //checking columns
    for(size_t k = 0; k < 9; k++)
    {
        if(matrix[k][j] == n && k != i)
            return 0;
    }

    //checking square
    for(size_t h = i/3; h < i/3 + 3; h++)
    {
        for(size_t k = j/3; k < j/3+1; k++)
        {
            if(matrix[h][k] == n && h != i && k !=j)
                return 0;
        }
    }

    return 1;
}

void solve(int matrix[9][9])
{
    for(size_t i = 0; i < 9; i++)
    {
        for(size_t j = 0; j < 9; j++)
        {
            if(matrix[i][j] == 0)
            {
                for(int n = 1; n < 10; n++)
                {
                    if(is_possible(matrix, i, j, n))
                    {
                        matrix[i][j] = n;
                        solve(matrix);
                        matrix[i][j] = 0;
                    }
                }
                return;
            }
        }
    }
    return;
}
