// вычислить матрицу (alpha*A*B + beta*C) и записать её в C
// здесь A –- матрица размера m на k, B –- матрица размера k на n,
//  C –- матрица размера m на n
void dgemm(
    int m, int n, int k,
    double alpha, const double *A, const double *B,
    double beta, double *C)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            double sum = 0;
            for (int h = 0; h < k; h++)
            {
                sum += A[i * k + h] * B[h * n + j];
            }
            C[i * n + j] = C[i * n + j] * beta + sum * alpha;
        }
    }
}