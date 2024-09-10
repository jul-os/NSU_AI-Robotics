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

    int n, res = 0;

    if (fscanf(input, "%d", &n) == 1)
    {
        for (int i = 1; i <= n; i++)
        {
            res += i;
        }
    }

    fprintf(output, "%d\n", res);

    fclose(input);
    fclose(output);
    
    return 0;
}
