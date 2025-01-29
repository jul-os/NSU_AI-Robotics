#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

void resheto_er(int n, int *nums)
{
    for (int i = 0; i < n + 1; i++)
    {
        nums[i] = i;
    }
    nums[1] = 0;

    for (int p = 2; p < n + 1; p++)
    {
        if (nums[p] != 0)
        {
            for (int j = p * p; j <= n && j > 0; j += p)
            {
                nums[j] = 0;
            }
        }
    }
}

int main(void)
{

    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    if (input == NULL || output == NULL)
    {

        perror("freopen()");
        return errno;
    }
    int n, q, q_i;

    if (scanf("%d %d", &n, &q) == 2)
    {
        int *nums_arr = malloc((n + 1) * sizeof(int));
        if (nums_arr == NULL)
        {
            printf("Memory allocation failed\n");
            return 1;
        }

        resheto_er(n, nums_arr);

        for (int i = 0; i < q; i++)
        {
            scanf("%d", &q_i);
            if (nums_arr[q_i] == 0)
            {
                printf("%d not\n", q_i);
            }
            else
            {
                printf("%d prime\n", q_i);
            }
        }

        free(nums_arr);
    }

    fclose(input);
    fclose(output);
    return 0;
}