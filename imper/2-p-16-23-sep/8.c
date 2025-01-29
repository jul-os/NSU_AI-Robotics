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
    int d, m, y, k, days_count = 0, count = 1;
    if (fscanf(input, "%d %d %d %d", &d, &m, &y, &k) == 4)
    {
        while (k != 0)
        {
            d++;
            if (((((y % 4) == 0) && ((y % 100) != 0)) || ((y % 400) == 0)) && (m == 2))
            {
                if (d > 29)
                {
                    m++;
                    d = 1;
                }
            }
            else if (m == 2)
            {
                if (d > 28)
                {
                    m++;
                    d = 1;
                }
            }
            if ((m == 1) || (m == 3) || (m == 5) || (m == 7) || (m == 8) || (m == 10) || (m == 12))
            {
                if (d > 31)
                {
                    m++;
                    d = 1;
                }
            }
            else if ((m == 4) || (m == 6) || (m == 9) || (m == 11))
            {
                if (d > 30)
                {
                    m++;
                    d = 1;
                }
            }
            if (m > 12)
            {
                m = m - 12;
                y++;
            }
            k--;
        }
    }

    fprintf(output, "%d %d %d", d, m, y);

    fclose(input);
    fclose(output);
    return 0;
}