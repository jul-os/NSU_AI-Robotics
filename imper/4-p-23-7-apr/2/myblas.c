void cblas_sgemm(int Order,
                 int TransA, int TransB,
                 const int M, const int N,
                 const int K, const float alpha, const float *A,
                 const int lda, const float *B, const int ldb,
                 const float beta, float *C, const int ldc)
{
    for (int i = 0; i < M; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            float sum = 0.0f;
            for (int h = 0; h < K; ++h)
            {
                sum += A[i * lda + h] * B[h * ldb + j];
            }
            C[i * ldc + j] = sum * alpha + beta * C[i * ldc + j];
        }
    }
}