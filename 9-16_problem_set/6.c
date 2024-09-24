#include <stdio.h>
#include <math.h>

int main(void)
{
    FILE *input, *output;
    input = fopen("input.txt", "r");
    output = fopen("output.txt", "w");

    if ((input == NULL) || (output == NULL))
    {
        return 1;
    }

    int n;
    int min = 100000, max = -100000, min_index, max_index, temp;
    if (fscanf(input, "%d", &n) == 1)
    {
        for (int i = 1; i <= n; i++)
        {
            fscanf(input, "%d", &temp);
            if (temp > max)
            {
                max = temp;
                max_index = i;
            }
            if (temp < min)
            {
                min = temp;
                min_index = i;
            }
        }
        fprintf(output, "%d %d %d %d", min, min_index, max, max_index);
    }
    fclose(input);
    fclose(output);

    return 0;
}
