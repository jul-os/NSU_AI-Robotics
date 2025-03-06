#include <stdio.h>
#include <stdlib.h>
#define MAX_NUM 2000

int columns = MAX_NUM + 1;
int rows = MAX_NUM + 1;


void make_triangle (int **arr, int module) {
    for (int i = 0; i <= MAX_NUM; i++) {
        arr[0][i] = 1;
        arr[i][0] = 1;
    }

    for (int i = 1; i < columns; i++) {
        for (int j = 1; j < rows; j++) {
            arr[i][j] = (arr[i - 1][j] + arr[i][j - 1]) % module;
        }
    }
}

void scan_and_find (int quantity, int ** triangle, FILE * out, FILE *in) {
    for (int i = 0; i < quantity; ++i) {
        int n, k;
        if (fscanf(in, "%d %d", &n, &k) == 0){}

        if (k < 0 || k > n) {
            fprintf(out, "0\n");
        } else {

            fprintf(out, "%d\n", triangle[n - k][k]);
        }
    }
}


int main() {
    FILE *in = fopen("input.txt", "r");
    FILE *out = fopen("output.txt", "w");

    int quantity, module;
    if (fscanf(in, "%d %d", &module, &quantity) != 0) {}

    int **triangle = malloc(columns * sizeof(int*));

    for (int i = 0; i < columns; i++) {
        triangle[i] = malloc(rows * sizeof(int));
    }

    make_triangle(triangle, module);

    scan_and_find(quantity, triangle, out, in);

    for (int i = 0; i < columns; i++) {
        free(triangle[i]);
    }
    free(triangle);

    return 0;
}
