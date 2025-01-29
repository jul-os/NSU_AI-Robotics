#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

typedef unsigned char bitword;

void bitsetZero(bitword *arr, int num)
{
    for (int i = 0; i < num; i++)
    {
        arr[i] = 0;
    }
}

// возвращает значение idx-ого бита (0 или 1)
int bitsetGet(const bitword *arr, int idx)
{
    char mask = 0b10000000;
    mask >>= (idx % 8);
    mask = arr[idx / 8] & mask;
    if (mask)
    {
        return 1;
    }
    return 0;
}

// устанавливает значение idx-ого бита в newval (которое равно 0 или 1)
void bitsetSet(bitword *arr, int idx, int newval)
{
    char mask = 0b10000000;
    mask >>= (idx % 8);

    if (newval == 1)
    {
        arr[idx / 8] |= mask;
    }
    else
    {
        mask = ~mask;
        arr[idx / 8] &= mask;
    }
}

// возвращает 1, если среди битов с номерами k
// для left <= k < right есть единичный, и 0 иначе
int bitsetAny(const bitword *arr, int left, int right)
{
    // for (int i = 0; i < 10; i++)
    // {
    //     printf("%d", (arr[0] >> i) & 1);
    // }
    // printf("\n");
    unsigned char left_mask = 0b11111111;

    left_mask = left_mask >> ((left % 8) - 1);

    // for (int i = 7; i >= 0; i--)
    // {
    //     printf("%d", (left_mask >> i) & 1);
    // }
    // printf("\n");

    unsigned char right_mask = 0b11111111;
    right_mask <<= (8 - right % 8);

    // for (int i = 7; i >= 0; i--)
    // {
    //     printf("%d", (right_mask >> i) & 1);
    // }
    // printf("\n");

    int left_flag = 0, right_flag = 0, in_flag = 0;
    if ((arr[left / 8] & left_mask) > 0)
    {
        if (left != left / 8)
        {
            left_flag = 1;
        }
    }
    if ((arr[right / 8] & right_mask) > 0)
    {
        if (right != right / 8)
        {
            right_flag = 1;
        }
    }
    if (right - left > 8)
    {
        for (int j = (left + 7) / 8; j < right / 8; j++)
        {
            if (arr[j] > 0)
            {
                in_flag = 1;
            }
        }
    }

    if (right_flag || left_flag || in_flag)
    {
        return 1;
    }
    return 0;
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

    bitword *arr = NULL;

    int n, what, len, ind, new_val;
    scanf("%d", &n);

    for (int i = 0; i < n; i++)
    {
        scanf("%d", &what);

        if (what == 0)
        {
            scanf(" %d", &len);
            // потому что предполагается пересоздание битового массива
            if (arr != NULL)
            {
                free(arr);
            }
            arr = (bitword *)malloc((len + 7) / 8);
            bitsetZero(arr, (len + 7) / 8);
        }
        else if (what == 1)
        {
            scanf(" %d", &ind);
            printf("%d\n", bitsetGet(arr, ind));
        }
        else if (what == 2)
        {
            scanf(" %d %d", &ind, &new_val);
            bitsetSet(arr, ind, new_val);
        }
        else
        {
            int left, right;
            scanf(" %d %d", &left, &right);
            if (bitsetAny(arr, left, right) == 1)
            {
                printf("some\n");
            }
            else
            {
                printf("none\n");
            }
        }
    }
    free(arr);
    fclose(input);
    fclose(output);
    return 0;
}