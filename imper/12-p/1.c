#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(int *arr, int i, int n)
{
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    int max = 0;
    if ((left < n) && (arr[left] > arr[i]))
    {
        max = left;
    }
    else
    {
        max = i;
    }
    if ((right < n) && (arr[right] > arr[max]))
    {
        max = right;
    }
    if (max != i)
    {
        swap(&arr[i], &arr[max]);
        heapify(arr, max, n);
    }
}

void build_max_heap(int *arr, int n)
{
    for (int i = n / 2 - 1; i >= 0; i--)
    {
        heapify(arr, i, n);
    }
}

void heapsort(int *arr, int n)
{
    build_max_heap(arr, n);
    for (int i = n - 1; i >= 0; i--)
    {
        swap(&arr[0], &arr[i]);
        n = n - 1;
        heapify(arr, 0, n);
    }
}

int main()
{
    freopen("input.txt", "rb", stdin);
    freopen("output.txt", "wb", stdout);
    int n;
    fread(&n, sizeof(int), 1, stdin);
    int *n_arr = malloc(n * sizeof(int));
    fread(n_arr, sizeof(int), n, stdin);

    heapsort(n_arr, n);
    fwrite(n_arr, sizeof(int), n, stdout);
    free(n_arr);
    fclose(stdin);
    fclose(stdout);
    return 0;
}
