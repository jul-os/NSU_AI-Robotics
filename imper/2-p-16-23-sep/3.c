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
        double neg = 0.0, nul = 0.0, pos = 0.0;
        int temp;
        for (int i = 0; i < n; i++)
        {
            fscanf(input, "%d", &temp);
            if (temp < 0)
            {
                neg += 1.0;
            }
            else if (temp == 0)
            {
                nul += 1.0;
            }
            else
            {
                pos += 1.0;
            }
        }
        fprintf(output, "%0.15lf %0.15lf %0.15lf", neg/n, nul/n, pos/n);
    }
    fclose(input);
    fclose(output);
    return 0;
}