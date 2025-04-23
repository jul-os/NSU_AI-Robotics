#include "cblas.h"
#include "pthread.h"
#include <stdlib.h>
// Computes R = A * A;
// Here A and R are square matrices N x N.
// Every matrix is stored in row-major layout,
// i.e. A[i*n+j] is the element in i-th row and j-th column.

void MatrixSqr(int n, const double *A, double *R)
{
    float *a = (float *)malloc(n * n * sizeof(float));
    float *r = (float *)malloc(n * n * sizeof(float));
    for (int i = 0; i < n * n; i++) {
        a[i] = (float)A[i];
    }
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n, n, n, 1.0f, a, n, a, n, 0.0f, r, n);
    for (int i = 0; i < n * n; i++) {
        R[i] = (double)r[i];
    }
    free(a);
    free(r);
}