#include <stdio.h>
#include <errno.h>

#define LONG_NUM_MAX_LEN 3001

typedef struct LongNum_s
{
    int len;                   // сколько непустых разрядов в числе
    int arr[LONG_NUM_MAX_LEN]; // массив десятичных цифр числа
} LongNum;

void print_long_int(LongNum *Num)
{
    for (int i = Num->len - 1; i >= 0; i--)
    {
        printf("%d", Num->arr[i]);
    }
}

void initialize_zero_num(LongNum *A)
{
    for (int i = 0; i < LONG_NUM_MAX_LEN; i++)
    {
        A->arr[i] = 0;
    }
}

void read_num(char *num, LongNum *Long_Num)
{
    int count = 0;
    while (num[count] != '\0')
    {
        count++;
    }
    Long_Num->len = count;
    for (int i = 0; i < Long_Num->len; i++)
    {
        Long_Num->arr[i] = (int)(num[Long_Num->len - (i + 1)] - 48);
    }
}

// сумма сложения будет храниться в a
void long_int_sum(LongNum *A, LongNum *B)
{
    // b < a в этой задаче, если складывать как sum(fib(n), fib(n-2))
    for (int i = 0; i < B->len; i++)
    {
        A->arr[i] += B->arr[i];
        if (A->arr[i] > 9)
        {
            if (i == A->len - 1) // потому что i начинает с 0, а длтна с 1
            {
                A->len += 1;
            }
            A->arr[i + 1] += 1;
            A->arr[i] %= 10;
        }
    }
}

int A_is_greater_than_B(LongNum *A, LongNum *B)
{
    if (A->len < B->len)
    {
        return 0;
    }
    if (A->len > B->len)
    {
        return 1;
    }
    for (int i = A->len - 1; i >= 0; i--)
    {
        if (A->arr[i] > B->arr[i])
            return 1;
        if (A->arr[i] < B->arr[i])
            return 0;
    }
    return 1;
}

void long_num_sub(LongNum *A, LongNum *B)
{
    for (int i = 0; i< A->len; i++)
    {
        A->arr[i] -= B->arr[i];
    }
    for (int i = 0; i < A->len - 1; i++)
    {
        if (A->arr[i] < 0)
        {
            A->arr[i] += 10;
            A->arr[i + 1]--;
        }
    }
    while (A->arr[A->len - 1] == 0 && A->len > 1)
    {
        A->len--;
    }
}

LongNum long_num_div(LongNum *A, LongNum *B)
{
    LongNum Res, Just_One;

    Just_One.len = 1;
    Just_One.arr[0] = 1;

    Res.len = A->len;

    initialize_zero_num(&Res);
    while (A_is_greater_than_B(A, B))
    {
        long_int_sum(&Res, &Just_One);
        long_num_sub(A, B);
        while (Res.arr[Res.len - 1] == 0 && Res.len > 1)
        {
            Res.len--;
        }
    }
    return Res;
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
    char a[1001], b[1001];
    scanf("%s", a);
    scanf("%s", b);

    LongNum A, B;
    initialize_zero_num(&A);
    initialize_zero_num(&B);
    read_num(a, &A);
    read_num(b, &B);
    LongNum Res = long_num_div(&A, &B);

    print_long_int(&Res);

    fclose(input);
    fclose(output);
    return 0;
}