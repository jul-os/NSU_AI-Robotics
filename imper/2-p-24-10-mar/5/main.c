#include <stdio.h>

typedef struct Factors
{
    int k;          // сколько различных простых в разложении
    int primes[32]; // различные простые в порядке возрастания
    int powers[32]; // в какие степени надо эти простые возводить
} Factors;

void Factorize(int X, Factors *res);

void find(int x)
{
    if (x == 1)
    {
        printf("1 = 1\n");
        return;
    }
    printf("%d = ", x);
    struct Factors res;
    Factorize(x, &res);
    if (res.k == 0){
        printf("%d = 1\n", x);
        return;
    }
    for (int j = 0; j < res.k; j++)
    { // бежим по всем делителям
        printf("%d^%d", res.primes[j], res.powers[j]);
        if (j != res.k - 1)
        { // если j не последний делитель, то выводим *
            printf(" * ");
        }
        else
        { // иначе выводим перевод строки
            printf("\n");
        }
    }
    return;
}

int main(void)
{
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    int n, x;
    scanf("%d", &n);
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &x);
        find(x);
    }

    fclose(input);
    fclose(output);
    return 0;
}