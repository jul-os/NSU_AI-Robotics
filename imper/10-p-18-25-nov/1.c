#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int compare(const void *a, const void *b)
{
    if (*(int *)a < *(int *)b)
        return -1;
    if (*(int *)a == *(int *)b)
        return 0;
    return 1;
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
    int n, m;
    scanf("%d", &n);
    int *n_arr = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &n_arr[i]);
    }
    qsort(n_arr, n, sizeof(int), compare);
    scanf("%d", &m);
    int *m_arr = malloc(m * sizeof(int));
    for (int i = 0; i < m; i++)
    {
        scanf("%d", &m_arr[i]);
    }
    qsort(m_arr, m, sizeof(int), compare);

    int *res = (int *)malloc(n * sizeof(int));

    int i = 0, j = 0, res_len = 0;
    while (i < n)
    {
        while (m_arr[j] < n_arr[i] && j < m)
        {
            j++;
        }
        if ((n_arr[i] != m_arr[j]) && (n_arr[i] != res[res_len - 1]))
        {
            res[res_len++] = n_arr[i];
        }
        i++;
    }

    printf("%d\n", res_len);
    for (int i = 0; i < res_len; i++)
    {
        printf("%d ", res[i]);
    }

    free(n_arr);
    free(m_arr);
    free(res);
    fclose(input);
    fclose(output);
    return 0;
}