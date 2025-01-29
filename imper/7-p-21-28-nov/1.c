#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

typedef struct key_st
{
    int num;
    char name[8];
} key_st;

void InsertionSort(int n, key_st *arr)
{
    key_st newElement;
    int location;

    for (int i = 1; i < n; i++)
    {
        newElement = arr[i];
        location = i - 1;
        while (location >= 0 && arr[location].num > newElement.num)
        {
            arr[location + 1] = arr[location];
            location = location - 1;
        }
        arr[location + 1] = newElement;
    }
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
    scanf("%d", &n);

    key_st *arr = malloc(n * sizeof(key_st));

    for (int i = 0; i < n; i++)
    {
        scanf("%d %s", &arr[i].num, arr[i].name);
    }
    InsertionSort(n, arr);
    for (int i = 0; i < n; i++)
    {
        printf("%d %s\n", arr[i].num, arr[i].name);
    }
    fclose(input);
    fclose(output);
    return 0;
}
