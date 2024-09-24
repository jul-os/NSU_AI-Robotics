#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE *input, *output;
    input = fopen("input.txt", "r");
    output = fopen("output.txt", "w");

    if ((input == NULL) || (output == NULL))
    {
        return 1;
    }

    int h, m, s, k;
    int new_h, new_m, new_s;

    if (fscanf(input, "%d %d %d %d", &h, &m, &s, &k) == 4)
    {
        int temp_h, temp_m, temp_s;
        temp_h = k / 3600;
        temp_m = (k - 3600 * temp_h) / 60;
        temp_s = k - 60 * temp_m - temp_h * 3600;
        // printf("temps %d %d %d\n", temp_h, temp_m, temp_s);
        // printf("temps + theirs %d %d %d\n", temp_h + h, temp_m + m, temp_s + s);
        //  seconds first
        if (temp_s + s > 59)
        {
            temp_m += (temp_s + s) / 60;

            new_s = (temp_s + s) % 60;
            // printf("new_s temp_m %d %d\n", new_s, temp_m);
        }
        else
        {
            new_s = temp_s + s;
        }

        if (temp_m + m > 59)
        {
            temp_h += (temp_m + m) / 60;
            new_m = (temp_m + m) % 60;
            // printf("new_m temp_h %d %d\n", new_m, temp_h);
        }
        else
        {
            new_m = temp_m + m;
        }

        if (temp_h + h > 23)
        {
            new_h = (temp_h + h) % 24;
            // printf("new_h %d\n", new_h);
        }
        else
        {
            new_h = temp_h + h;
        }
        // printf("new %d %d %d\n", new_h, new_m, new_s);
        fprintf(output, "%d %d %d", new_h, new_m, new_s);
    }

    fclose(input);
    fclose(output);

    return 0;
}
