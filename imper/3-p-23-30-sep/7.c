#include <stdio.h>
#include <errno.h>

void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// An optimized version of Bubble Sort
void bubbleSort(int arr[], int n)
{
    int i, j;
    int swapped;
    for (i = 0; i < n - 1; i++)
    {
        swapped = 0;
        for (j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                swap(&arr[j], &arr[j + 1]);
                swapped = 1;
            }
        }
        // If no two elements were swapped by inner loop,
        // then break
        if (swapped == 0)
            break;
    }
}

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
    int n, temp;
    if (fscanf(input, "%d", &n) == 1)
    {
        int nums[n];

        for (int i = 0; i < n; i++)
        {
            fscanf(input, "%d", &temp);
            nums[i] = temp;
        }

        bubbleSort(nums, n);

        for (int i = 0; i < n; i++)
        {
            fprintf(output, "%d ", nums[i]);
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}