//sum.c

#include <stdlib.h>
#include <stdint.h>

long long *sum;

void Init(const int *array, int length)
{
    sum = (long long *)malloc(length * sizeof(int64_t));

    sum[0] = array[0]; 
    for (int i = 1; i < length; ++i)
    {
        sum[i] = sum[i - 1] + array[i];
    }
}


long long Sum(int left, int right)
{
    if (left == 0)
        return sum[right - 1]; // если от 0, то просто берём сумму

    return sum[right - 1] - sum[left - 1];
}
