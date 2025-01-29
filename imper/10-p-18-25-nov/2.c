#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

typedef struct
{
    char name[8];
    int length;
} lines;

lines *arr[1000001];

void put_into(int index)
{
    int len = arr[index][0].length;
    arr[index] = realloc(arr[index], (len + 1) * sizeof(lines));
    scanf("%s", arr[index][len].name);
    arr[index][0].length = len + 1;
}

void print_lines()
{
    for (int i = 0; i < 1000001; i++)
    {
        if (arr[i][0].length > 1)
        {
            for (int j = 1; j < arr[i][0].length; j++)
            {
                printf("%d %s\n", i, arr[i][j].name);
            }
        }
    }
}

int main(void)
{
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    int n, ind;
    scanf("%d", &n);

    for (int i = 0; i < 1000001; i++)
    {
        arr[i] = malloc(1 * sizeof(lines));
        arr[i][0].length = 1;
        // будет как бы буфферный элемент для  содержания длины
    }

    for (int i = 0; i < n; i++)
    {
        scanf("%d ", &ind);
        put_into(ind);
    }
    print_lines();

    fclose(input);
    fclose(output);
    return 0;
}