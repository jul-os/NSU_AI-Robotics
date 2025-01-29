#include <stdio.h>
#include <stdlib.h>

// merges sorted arrays a[0..ak-1] and b[0..bk-1] into
// one sorted array res[0..rk-1], returning rk from function
int merge(const int *a, int ak, const int *b, int bk, int *res) {
    int idx = bk + ak - 1; // Индекс для записи в res
    int res_len = bk + ak;
    ak--;
    bk--;

    while (ak >= 0 && bk >= 0) {
        if (a[ak] > b[bk]) {
            res[idx] = a[ak];
            ak--;
        } else {
            res[idx] = b[bk];
            bk--;
        }
        idx--;
    }
    
    while (ak >= 0) {
        res[idx] = a[ak];
        idx--;
        ak--;
    }

    while (bk >= 0) {
        res[idx] = b[bk];
        idx--;
        bk--;
    }
    return res_len; 
}


int main()
{
    freopen("input.txt", "rb", stdin);
    freopen("output.txt", "wb", stdout);
    int n, m;
    fread(&n, sizeof(int), 1, stdin);
    fread(&m, sizeof(int), 1, stdin);
    int *n_arr = malloc(n * sizeof(int));
    int *m_arr = malloc(m * sizeof(int));

    fread(n_arr, sizeof(int), n, stdin);
    fread(m_arr, sizeof(int), m, stdin);

    int *res_arr = malloc((n + m) * sizeof(int));
    int res_len = merge(n_arr, n, m_arr, m, res_arr);

    //fwrite(&res_len, sizeof(int), 1, stdout);

    fwrite(res_arr, sizeof(int), res_len, stdout);
    free(n_arr);
    free(m_arr);
    free(res_arr);
    fclose(stdin);
    fclose(stdout);
    return 0;
}