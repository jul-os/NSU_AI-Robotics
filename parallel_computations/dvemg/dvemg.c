#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

double cpuSecond()
{
    return omp_get_wtime();
}

// Параллельная инициализация матрицы
void init_matrix_parallel(double *a, size_t m, size_t n)
{
#pragma omp parallel for collapse(2)
    for (size_t i = 0; i < m; i++)
    {
        for (size_t j = 0; j < n; j++)
            a[i * n + j] = i + j;
    }
}

// Параллельная инициализация вектора
void init_vector_parallel(double *b, size_t n)
{
#pragma omp parallel for
    for (size_t j = 0; j < n; j++)
        b[j] = j;
}

// Последовательное умножение матрицы на вектор
void matrix_vector_product(double *a, double *b, double *c, size_t m, size_t n)
{
    for (size_t i = 0; i < m; i++)
    {
        c[i] = 0.0;
        for (size_t j = 0; j < n; j++)
            c[i] += a[i * n + j] * b[j];
    }
}

// Параллельное умножение матрицы на вектор
void matrix_vector_product_omp(double *a, double *b, double *c, size_t m, size_t n)
{
#pragma omp parallel
    {
        int nthreads = omp_get_num_threads();
        int threadid = omp_get_thread_num();
        int items_per_thread = m / nthreads;
        int lb = threadid * items_per_thread;
        int ub = (threadid == nthreads - 1) ? (m - 1) : (lb + items_per_thread - 1);

        for (int i = lb; i <= ub; i++)
        {
            c[i] = 0.0;
            for (int j = 0; j < n; j++)
                c[i] += a[i * n + j] * b[j];
        }
    }
}

double run_test(size_t m, size_t n, int num_threads, int parallel_init)
{
    double *a, *b, *c;
    double t_start, t_total;

    // Выделение памяти
    a = (double *)malloc(sizeof(*a) * m * n);
    b = (double *)malloc(sizeof(*b) * n);
    c = (double *)malloc(sizeof(*c) * m);

    if (a == NULL || b == NULL || c == NULL)
    {
        free(a);
        free(b);
        free(c);
        printf("Error allocate memory!\n");
        exit(1);
    }

    // Устанавливаем количество потоков
    omp_set_num_threads(num_threads);

    // Инициализация (параллельная или последовательная)
    if (parallel_init && num_threads > 1)
    {
        init_matrix_parallel(a, m, n);
        init_vector_parallel(b, n);
    }
    else
    {
        // Последовательная инициализация
        for (size_t i = 0; i < m; i++)
            for (size_t j = 0; j < n; j++)
                a[i * n + j] = i + j;
        for (size_t j = 0; j < n; j++)
            b[j] = j;
    }

    // Вычисления
    t_start = cpuSecond();
    if (num_threads == 1)
    {
        matrix_vector_product(a, b, c, m, n);
    }
    else
    {
        matrix_vector_product_omp(a, b, c, m, n);
    }
    t_total = cpuSecond() - t_start;

    free(a);
    free(b);
    free(c);
    return t_total;
}

int main(int argc, char *argv[])
{
    size_t sizes[2][2] = {{20000, 20000}, {40000, 40000}};
    int threads[] = {1, 2, 4, 7, 8, 16, 20, 40};
    int num_sizes = 2;
    int num_threads_configs = sizeof(threads) / sizeof(threads[0]);

    for (int s = 0; s < num_sizes; s++)
    {
        size_t M = sizes[s][0];
        size_t N = sizes[s][1];

        printf("Размер матрицы: %zux%zu\n", M, N);
        printf("| Потоки | Время | Ускорение |\n");

        // Сначала получаем базовое время для T(1)
        double T1 = run_test(M, N, 1, 0); // последовательная версия
        printf("| %6d | %13.6f | %23.2f |\n", 1, T1, 1.0);

        // Затем для остальных количеств потоков
        for (int t = 1; t < num_threads_configs; t++)
        {
            int num_threads = threads[t];
            double Tp = run_test(M, N, num_threads, 1); // параллельная версия
            double speedup = T1 / Tp;
            printf("| %6d | %13.6f | %23.2f |\n", num_threads, Tp, speedup);
        }
    }

    return 0;
}