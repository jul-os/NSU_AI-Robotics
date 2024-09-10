#include <stdio.h>

int surface_area(int x, int y, int z)
{
    if (x <= 0)
    {
        return -1;
    }
    if (y <= 0)
    {
        return -2;
    }
    if (z <= 0)
    {
        return -3;
    }
    return 2 * (x * y + y * z + x * z);
}

int main(void)
{
    FILE *input, *output;
    input = fopen("input.txt", "r");
    output = fopen("output.txt", "w");

    if ((input == NULL) || (output == NULL))
    {
        return 1;
    }

    int x, y, z;

    if (fscanf(input, "%d %d %d", &x, &y, &z) == 3)
    {
        fprintf(output, "%d\n", surface_area(x, y, z));
    }

    fclose(input);
    fclose(output);

    return 0;
}
