#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN_OF_INPUT_NUM 1000
#define MAX_LEN_OF_OUTPUT_NUM 2001

// десятичная арифметика длинного числа. умножение

typedef struct LongNum_s
{
    int len;                        // сколько непустых разрядов в числе
    int arr[MAX_LEN_OF_OUTPUT_NUM]; // массив десятичных цифр числа
} LongNum;

void put_into_ln(LongNum *a, char *input)
{
    a->len = strlen(input);
    for (int i = 0; i < a->len; i++)
    {
        a->arr[i] = input[a->len - 1 - i] - '0'; // little-endian
    }
    for (int i = a->len + 1; i < MAX_LEN_OF_OUTPUT_NUM; i++)
    {
        a->arr[i] = 0;
    }
}

void print_long_int(LongNum *num)
{
    for (int i = num->len - 1; i >= 0; i--)
    {
        printf("%d", num->arr[i]);
    }
}

LongNum long_int_mul(LongNum a, LongNum b)
{
    LongNum res;
    res.len = a.len + b.len;
    for (int i = 0; i < MAX_LEN_OF_OUTPUT_NUM; i++)
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

    char in[1000];
    LongNum a_ln, b_ln, res_ln;
    scanf("%s\n", in);
    put_into_ln(&a_ln, in);

    scanf("%s", in);
    put_into_ln(&b_ln, in);

    // print_long_int(&a_ln);
    // printf("\n");
    // print_long_int(&b_ln);
    // printf("\n");

    res_ln = long_int_mul(a_ln, b_ln);

    print_long_int(&res_ln);

    fclose(input);
    fclose(output);
    return 0;
}