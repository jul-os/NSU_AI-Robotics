// main.c

#include <stdio.h>
#include <stdlib.h>

void Init(const int *array, int length);

int Query(int left, long long sumPredict);

int length = 0;

int main(void)
{
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    if (!input)
    {
        perror("Error opening input file");
        return 1;
    }

    output = freopen("output.txt", "w", stdout);

    int questions;
    scanf("%d %d", &length, &questions);

    int *array = (int *)malloc(length * sizeof(int));

    for (int i = 0; i < length; i++)
    {
        scanf("%d", array + i);
    }

    Init(array, length);
    free(array);

    for (int i = 0; i < questions; ++i)
    {
        int left;
        long long sumPredict;

        scanf("%d%lld", &left, &sumPredict);

        int answer = Query(left, sumPredict);

        printf("%d\n", answer);
    }
    fclose(input);
    fclose(output);
    return 0;
}