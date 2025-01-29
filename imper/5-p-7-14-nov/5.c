#include <stdio.h>
#include <errno.h>

// десятичная арифметика длинного числа. сложение

typedef struct LongNum_s
{
    int len;      // сколько непустых разрядов в числе
    int arr[500]; // массив десятичных цифр числа
} LongNum;

void initialize_zero_num(LongNum *a)
{
    for (int i = 0; i < 500; i++)
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

void copy_long_num(LongNum *dest, LongNum *src)
{
    dest->len = src->len;
    for (int i = 0; i < src->len; i++)
    {
        dest->arr[i] = src->arr[i];
    }
}

void print_long_int(LongNum *num)
{
    for (int i = num->len - 1; i >= 0; i--)
    {
        printf("%d", num->arr[i]);
    }
}

LongNum find_fibb(int n)
{
    // для рекурсии задать начало
    if (n < 3)
    {
        LongNum start_fibb;
        initialize_zero_num(&start_fibb);
        start_fibb.len = 1;
        start_fibb.arr[0] = 1;
        return start_fibb;
    }

    // n >= 3
    // будем хранить три числа фибоначчи чтобы с их помощью считать
    LongNum nth, minusth, minus_minusth;
    initialize_zero_num(&nth);
    initialize_zero_num(&minusth);
    initialize_zero_num(&minus_minusth);

    nth.arr[0] = 2;
    minusth.arr[0] = 1;
    minus_minusth.arr[0] = 1;
    nth.len = 1;
    minusth.len = 1;
    minus_minusth.len = 1;

    for (int i = 0; i < n - 3; i++)
    { // отсчет до n-ного числа
        // n - 2 = n - 1; n - 1 = n; n = long_int_sum(n, n - 2)
        copy_long_num(&minus_minusth, &minusth);
        copy_long_num(&minusth, &nth);
        long_int_sum(&nth, &minus_minusth);

        // printf("%d ", i);
        // print_long_int(&nth);
        // printf(" %d\n", nth.len);
    }

    // find length - length of nth number is updated in long_int_sum?

    return nth;
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
    if (scanf("%d", &n) == 1)
    {
        LongNum fib_num = find_fibb(n);
        print_long_int(&fib_num);
    }

    fclose(input);
    fclose(output);
    return 0;
}