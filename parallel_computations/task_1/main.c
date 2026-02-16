#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#ifdef USE_FLOAT_TYPE
typedef float real_t;
#define REAL_SIN sinf
#define REAL_PRINT "%.10f"
#define REAL_PI 3.14159265358979323846f
#else
typedef double real_t;
#define REAL_SIN sin
#define REAL_PRINT "%.30lf"
#define REAL_PI 3.14159265358979323846
#endif

int main()
{
    const long long N = 10000000;
    real_t *array = (real_t *)malloc(N * sizeof(real_t));

    if (!array)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    const real_t step = 2.0 * REAL_PI / (real_t)N;

    for (long long i = 0; i < N; i++)
    {
        array[i] = REAL_SIN((real_t)i * step);
    }

    /*
    c таким вариантом получалась большая разница значений
    Double Sum: -0.0000000007
    Float Sum: 0.3492122889
    real_t sum = 0;
    for (long long i = 0; i < N; i++)
    {
        sum += array[i];
    }
    */
    // поэтому решила попробовать компенсационное суммирование
    // чтобы уменьшить погрешность округления
    real_t sum = 0;
    real_t c = 0;
    for (long long i = 0; i < N; i++)
    {
        real_t y = array[i] - c;
        real_t t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }
    printf("Sum: " REAL_PRINT "\n", sum);
    printf("Type size: %zu bytes\n", sizeof(real_t));

    free(array);
    return 0;
}