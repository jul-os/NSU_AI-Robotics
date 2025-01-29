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
    int n, buff, count = 0;
    int all_nums[100001];

    for (int i = 0; i < 100001; i++)
    {
        all_nums[i] = 0;
    }

    if (fscanf(input, "%d", &n) == 1)
    {
        for (int i = 0; i < n; i++)
        {
            fscanf(input, "%d", &buff);
            if (all_nums[buff] == 0)
            {
                count += 1;
                all_nums[buff] = 1;
            }
        }

        fscanf(input, "%d", &n);
        for (int i = 0; i < n; i++)
        {
            fscanf(input, "%d", &buff);
            if (all_nums[buff] == 1)
            {
                count -= 1;
                all_nums[buff] = 0;
            }
        }
        fprintf(output, "%d\n", count);
        for (int i = 0; i < 100001; i++)
        {
            if (all_nums[i] == 1)
            {
                fprintf(output, "%d ", i);
            }
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}