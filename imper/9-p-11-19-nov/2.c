#include <stdio.h>
#include <stdlib.h>

int partition(int *a, int n, int pivot)
{
    int *before_arr = malloc(n * sizeof(int));
    int b_ind = 0, a_ind = 0;
    int *after_arr = malloc(n * sizeof(int));
    int flag_left = 0;
    for (int i = 0; i < n; i++)
    {
        if (a[i] == pivot)
        {
            if (flag_left == 0)
            {
                before_arr[b_ind] = a[i];
                b_ind++;
                flag_left = 1;
            }
            else
            {
                after_arr[a_ind] = a[i];
                a_ind++;
                flag_left = 0;
            }
        }
        else if (a[i] < pivot)
        {
            before_arr[b_ind] = a[i];
            b_ind++;
        }
        else
        {
            after_arr[a_ind] = a[i];
            a_ind++;
        }
    }
    fwrite(&b_ind, sizeof(int), 1, stdout);

    fwrite(before_arr, sizeof(int), b_ind, stdout);
    //в примере у нас почему-то before записано как было в изначальном порядке
    //after отсортировано. чего-то как-то почему........
    fwrite(after_arr, sizeof(int), a_ind, stdout);

    return b_ind;
}

int main()
{
    freopen("input.tx", "rb", stdin);
    freopen("output.txt", "wb", stdout);
    int n, pivot;
    fread(&n, sizeof(int), 1, stdin);
    fread(&pivot, sizeof(int), 1, stdin);
    int *n_arr = malloc(n * sizeof(int));

    fread(n_arr, sizeof(int), n, stdin);

    int left_res = partition(n_arr, n, pivot);
    free(n_arr);
    fclose(stdin);
    fclose(stdout);
    return 0;
}