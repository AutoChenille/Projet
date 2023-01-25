#include "solveur.h"

void check_possibilities(int matrix[][9][9])
{
    
}

void check_line(int matrix[][9][9], size_t i, size_t j)
{
    for(int n = 0; i < 9; i++)
    {
        matrix[i][j][n] = n+1;
    }
    
    for(size_t n = 0; n < 9; n++)
    {
        if(matrix[i][n][0] == 1)
                return;
    }
}
