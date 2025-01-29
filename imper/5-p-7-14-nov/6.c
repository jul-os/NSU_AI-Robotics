#include <stdio.h>
#include <errno.h>

#define LONG_NUM_MAX_LEN 3001

typedef struct LongNum_s
{
    int len;                   // сколько непустых разрядов в числе
    int arr[LONG_NUM_MAX_LEN]; // массив десятичных цифр числа
} LongNum;

void print_long_int(LongNum *num)
{
    for (int i = num->len - 1; i >= 0; i--)
    {
        printf("%d", num->arr[i]);
    }
}

void initialize_zero_num(LongNum *a)
{
    for (int i = 0; i < LONG_NUM_MAX_LEN; i++)
    {
        a->arr[i] = 0;
    }
}

// сумма сложения будет храниться в a
void long_int_sum(LongNum *a, LongNum *b)
{
    // b < a в этой задаче, если складывать как sum(fib(n), fib(n-2))
    for (int i = 0; i < b->len; i++)
    {
        a->arr[i] += b->arr[i];
        if (a->arr[i] > 9)
        {
            if (i == a->len - 1) // потому что i начинает с 0, а длтна с 1
            {
                a->len += 1;
            }
            a->arr[i + 1] += 1;
            a->arr[i] %= 10;
        }
    }
}

LongNum long_int_mul(LongNum a, LongNum b)
{
    LongNum res;
    res.len = a.len + b.len;
    for (int i = 0; i < LONG_NUM_MAX_LEN; i++)
    {
        res.arr[i] = 0;
    }

    for (int i = 0; i < a.len; i++)
    {
        for (int j = 0; j < b.len; j++)
        {
            res.arr[i + j] += a.arr[i] * b.arr[j];
        }
    }

    for (int i = 0; i < res.len - 1; i++)
    {
        res.arr[i + 1] += res.arr[i] / 10;
        res.arr[i] %= 10;
    }

    while (res.len > 1 && res.arr[res.len - 1] == 0)
    {
        res.len--;
    }

    return res;
}

LongNum find_factorial(int n)
{
    if (n < 2)
    {
        LongNum little_num;
        initialize_zero_num(&little_num);
        little_num.len = 1;
        little_num.arr[0] = 1;
        return little_num;
    }
    // n>=2

    LongNum A, B, just_one;

    A.len = 1;
    A.arr[0] = 1;
    B.len = 1;
    B.arr[0] = 2;

    just_one.len = 1;
    just_one.arr[0] = 1;

    for (int i = 0; i< n - 1; i++){
        LongNum C = long_int_mul(A, B);
        A.len = C.len;
        for (int j = 0; j < C.len; j++){
            A.arr[j] = C.arr[j];
        }
        long_int_sum(&B, &just_one);
    }

    return A;
}

int main(void)
{

    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    if (input == NULL || output == NULL)
    {

        perror("freopen()");
        return errno;
    }
    int n;
    if (scanf("%d", &n) != 1)
    {
        perror("please, input n");
        return errno;
    }

    LongNum fact;

    fact = find_factorial(n);
    print_long_int(&fact);

    fclose(input);
    fclose(output);
    return 0;
}
