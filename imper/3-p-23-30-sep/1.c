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

    int n, buffer, count;
    if (fscanf(input, "%d", &n) == 1)
    {
        int array[n];

        for (int i = 0; i < n; i++)
        {
            fscanf(input, "%d", &array[i]);
        }

        for (int i = 0; i < n; i++)
        {
            buffer = array[i];
            count = 0;
            for (int j = i; j < n; j++)
            {
                if (buffer > array[j])
                {
                    buffer = array[i];
                    count += 1;
                }
            }

            fprintf(output, "%d ", count);
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}