#include "array.h"

void compress(int a[], size_t len)
{
    /*
        place digits in array a:
            -> != 0 to beginning
            -> == 0 to end
    */

    size_t d = 0;
    for(size_t i = 0; i+d < len; i++)
    {
        if(a[i+d] == 0)
            d ++;
        else
        {
            a[i] = a[i+d];
            a[i+d] = 0;
            i ++;
        }
    }
}

