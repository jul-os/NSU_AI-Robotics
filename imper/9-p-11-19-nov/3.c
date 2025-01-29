#include <stdio.h>
#include <stdlib.h>

// Merges two subarrays
void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void merge_sort(int *array, int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;

        merge_sort(array, l, m);
        merge_sort(array, m + 1, r);

        merge(array, l, m, r);
    }
}

long long int calc_min_sum(int n, int *array)
{
    long long int sum = 0;
    for (int i = 0; i < n; i++)
    {
        sum += array[i] * (n - i - 1);
    }
    return sum;
}

int main(void)
{
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    int n;
    scanf("%d", &n);
    int *arr = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++)
    {
        scanf("%d", &arr[i]);
    }
    merge_sort(arr, 0, n - 1);
    long long int sum = calc_min_sum(n, arr);
    printf("%lld", sum);

    free(arr);

    fclose(input);
    fclose(output);
    return 0;
}