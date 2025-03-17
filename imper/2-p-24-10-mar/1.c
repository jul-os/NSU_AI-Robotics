#include <stdio.h>
#include <stdlib.h>
// Расширенный алгоритм Евклида: находит GCD и коэффициенты x, y для уравнения ax + by = GCD(a, b)
long long gcdE(long long a, long long b, long long *x, long long *y)
{
    if (a == 0)
    {
        *x = 0;
        *y = 1;
        return b;
    }
    long long x1, y1;
    long long gcd = gcdE(b % a, a, &x1, &y1);
    *x = y1 - (b / a) * x1;
    *y = x1;
    return gcd;
}

// Нахождение мультипликативного обратного по модулю
long long mod_inverse(long long Mi, long long mi)
{
    long long x, y;
    long long g = gcdE(Mi, mi, &x, &y);
    if (g != 1)
    {
        return -1; // Если обратного нет, возвращаем -1
    }
    else
    {
        return (x % mi + mi) % mi; // Делаем результат положительным
    }
}

long long ModMultiply(long long mult1, long long mult2, long long mod)
{
    long long result = 0;
    mult1 %= mod;
    while (mult2 > 0)
    {
        if (mult2 & 1)
        {
            result = (result + mult1) % mod;
        }
        mult1 = (mult1 * 2) % mod; // сдвиг влево по модулю
        mult2 >>= 1;               // сдвиг вправо(будто ходим по двоичному числу)
    }
    return result;
}

int main(void)
{
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);
    long long n;
    scanf("%lld", &n);
    long long mult_M = 1;
    long long *modules = malloc((n + 1) * sizeof(long long));
    long long *mod_big = malloc((n + 1) * sizeof(long long));
    long long *tri_modules = malloc((n + 1) * sizeof(long long));
    long long *nums = malloc((n + 1) * sizeof(long long));
    for (long long i = 0; i < n; i++)
    {
        scanf("%lld ", &modules[i]);
        mult_M *= modules[i];
    }
    for (long long i = 0; i < n; i++)
    {
        mod_big[i] = mult_M / modules[i]; //O(1)
        tri_modules[i] = mod_inverse(mod_big[i], modules[i]);//through evklid -> O(log(min))
    }
    long long x = 0;
    for (long long i = 0; i < n; i++)
    {
        scanf("%lld ", &nums[i]);
        // x += nums[i] * mod_big[i] * tri_modules[i];
        x += ModMultiply(nums[i] * mod_big[i], tri_modules[i], mult_M);//O(log (max modules))
    }

    x = x % mult_M;
    printf("%lld", x);

    free(modules);
    free(nums);
    fclose(input);
    fclose(output);
    return 0;
}