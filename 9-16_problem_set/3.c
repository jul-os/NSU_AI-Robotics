#include <stdio.h>

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

    if (fscanf(input, "%d", &n) == 1)
    {
        int res = 0, temp;
        for (int i = 0; i < n; i++)
        {
            if (fscanf(input, "%d", &temp) == 1)
            {
                if (temp % 2 == 0)
                {
                    res += temp;
                }
            }
        }
        fprintf(output, "%d\n", res);
    }

    fclose(input);
    fclose(output);

    return 0;
}
