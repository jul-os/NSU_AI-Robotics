// вообще есть прикольная структура данных для этого(дерево отрезков), но тут ленивая реализация без нее
#include <stdio.h>
#include <errno.h>

int main(void)
{

    FILE *input, *output;
    input = fopen("input.txt", "r");
    output = fopen("output.txt", "w");

    if (input == NULL || output == NULL)
    {

        perror("fopen()");
        return errno;
    }

    int n, start = 0, stop = 0, max_sum = -100000, sum;
    
    if (fscanf(input, "%d", &n) == 1)
    {
        int li[n];

        for (int i = 0; i < n; i++)
        {
            fscanf(input, "%d", &li[i]);
        }

        for (int i = 0; i < n; i++)
        {
            sum = li[i];
            for (int j = i; j < n; j++)
            {
                if (i != j)
                {
                    sum += li[j];
                }
                if (sum > max_sum)
                {
                    max_sum = sum;
                    start = i;
                    stop = j;
                }
            }
        }
    }

    fprintf(output, "%d %d %d", start, stop, max_sum);
    fclose(input);
    fclose(output);
    return 0;
}