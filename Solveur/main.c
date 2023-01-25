#include <stdio.h>
#include "solveur.h"
#include "array.h"

int main()
{
    int a[5] = {1, 0, 3, 4, 0};
    compress(a, (size_t)5);

    for(int i = 0; i < 5; i++)
    {
        printf("%i,", a[i]);
    }
    printf("\n");

    return 0;
}
