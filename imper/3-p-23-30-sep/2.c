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
    int n, sum = 0, step = 0;
    if (fscanf(input, "%d", &n) == 1)
    {
        int array[n];

        for (int i = 0; i < n; i++)
        {
            fscanf(input, "%d", &array[i]);
        }

        for (int i = 1; i <= n; i++)
        {
            for (int j = i - 1; j < n; j += i)//-1 тк в массивах нумерация с 0
            {
                sum += array[j];
            }
            fprintf(output, "%d\n", sum);
            sum = 0;
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}
