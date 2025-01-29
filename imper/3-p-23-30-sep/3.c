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
    int n;
    if (fscanf(input, "%d", &n) == 1)
    {
        int array[10001], buffer[n];

        for (int i = 0; i <= 10000; i++)
        {
            array[i] = 0;
        }

        for (int i = 0; i < n; i++)
        {
            fscanf(input, "%d", &buffer[i]);
        }

        for (int i = 0; i < n; i++)
        {
            array[buffer[i]] += 1;
        }

        for (int i = 0; i <= 10000; i++)
        {
            if (array[i] != 0)
            {
                fprintf(output, "%d: %d\n", i, array[i]);
            }
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}