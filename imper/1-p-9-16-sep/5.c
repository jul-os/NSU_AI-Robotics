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

    int a, b;

    if (fscanf(input, "%d %d", &a, &b) == 2)
    {
        // вниз
        int down;
        if (a % b == 0 || (a % b != 0 && a > 0))
        {
            down = a / b;
            fprintf(output, "%d ", a / b);
        }
        else if (a % b != 0 && a < 0)
        {
            down = a / b - 1;
            fprintf(output, "%d ", a / b - 1);
        }
        // вверх
        if (a % b == 0 || (a % b != 0 && a < 0))
            fprintf(output, "%d ", a / b);
        else if (a % b != 0 && a > 0)
            fprintf(output, "%d ", a / b + 1);
        fprintf(output, "%d ", a / b);                    // в сторону нуля
        fprintf(output, "%d", a - down * b); // R
    }

    fclose(input);
    fclose(output);

    return 0;
}
