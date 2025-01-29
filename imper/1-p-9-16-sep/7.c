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

    int n, m, p, k, l;
    if (fscanf(input, "%d", &n) == 1)
    {
        if (fscanf(input, "%d %d %d %d", &m, &p, &k, &l) == 4)
        {
            int flat_per_floor = m / ((p - 1) * l + k - 1);

            int flat_per_entr = flat_per_floor * l;
            int p0, k0;
            if (n % flat_per_entr == 0)
            {
                p0 = n / flat_per_entr;
            }
            else
            {
                p0 = n / flat_per_entr + 1;
            }
            if ((n - (p0 - 1) * flat_per_entr) % flat_per_floor == 0)
            {
                k0 = (n - (p0 - 1) * flat_per_entr) / flat_per_floor;
            }
            else
            {
                k0 = (n - (p0 - 1) * flat_per_entr) / flat_per_floor + 1;
            }
            fprintf(output, "%d %d", p0, k0);
        }
    }
    fclose(input);
    fclose(output);

    return 0;
}
