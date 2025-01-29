#include <stdio.h>
#include <errno.h>

int while_bin_search(int n, int arr[], int len)
{
    int left = 0, right = len;
    while (right >= left)
    {
        int mid = (left + right) / 2;
        if (n < arr[mid])
        {
            right = mid - 1;
        }
        else if (n > arr[mid])
        {
            left = mid + 1;
        }
        else
        {
            int aaaaa = 0;
            for (int i = 1; i < len + 1; i++)
            {
                if (arr[mid + i] == arr[mid])
                {
                    aaaaa += 1;
                }
                else
                {
                    break;
                }
            }
            if (aaaaa > 0)
            {
                return mid + aaaaa;
            }
            return mid;
        }
    }
    return -1;
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
    int n, q;
    scanf("%d", &n);
    int nums[n];
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &nums[i]);
    }
    scanf("%d", &q);
    int r_prev = 0, y;
    for (int i = 0; i < q; i++)
    {
        scanf("%d", &y);
        r_prev = while_bin_search(y + r_prev, nums, n);
        printf("%d\n", r_prev);
    }

    fclose(input);
    fclose(output);
    return 0;
}