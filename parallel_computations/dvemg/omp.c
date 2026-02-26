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

// Последовательная инициализация матрицы
void init_matrix_serial(double *a, size_t m, size_t n)
{
    for (size_t i = 0; i < m; i++)
    {
        for (size_t j = 0; j < n; j++)
            a[i * n + j] = i + j;
    }
}

// Последовательная инициализация вектора
void init_vector_serial(double *b, size_t n)
{
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

double run_test(size_t m, size_t n, int num_threads)
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

    // Инициализация: параллельная для многопоточных запусков, последовательная для однопоточного
    if (num_threads > 1)
    {
        // Для многопоточных запусков - параллельная инициализация
        init_matrix_parallel(a, m, n);
        init_vector_parallel(b, n);
    }
    else
    {
        // Для однопоточного запуска - последовательная инициализация
        init_matrix_serial(a, m, n);
        init_vector_serial(b, n);
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

void print_system_info()
{
    printf("\n=== Информация о вычислительном узле ===\n");

    // Информация о CPU
    system("lscpu | grep \"Model name\"");
    system("lscpu | grep \"CPU(s)\"");
    system("lscpu | grep \"NUMA\"");

    // Информация о сервере
    printf("\n--- Server Product Name ---\n");
    system("cat /sys/devices/virtual/dmi/id/product_name 2>/dev/null || echo 'Information not available'");

    // NUMA информация
    printf("\n--- NUMA Configuration ---\n");
    system("numactl --hardware 2>/dev/null | head -20 || echo 'numactl not available'");

    // OS информация
    printf("\n--- Operating System ---\n");
    system("cat /etc/os-release | grep \"PRETTY_NAME\"");

    printf("\n====================================\n\n");
}

int main(int argc, char *argv[])
{
    size_t sizes[2][2] = {{20000, 20000}, {40000, 40000}};
    int threads[] = {1, 2, 4, 7, 8, 16, 20, 40};
    int num_sizes = 2;
    int num_threads_configs = sizeof(threads) / sizeof(threads[0]);

    // Выводим информацию о системе
    print_system_info();

    for (int s = 0; s < num_sizes; s++)
    {
        size_t M = sizes[s][0];
        size_t N = sizes[s][1];

        printf("\n========================================\n");
        printf("Размер матрицы: %zux%zu\n", M, N);
        printf("========================================\n");
        printf("| Потоки |   Время (с)   | Ускорение |\n");
        printf("|--------|---------------|-----------|\n");

        // Сначала получаем базовое время для T(1)
        double T1 = run_test(M, N, 1);
        printf("| %6d | %13.6f | %9.2f |\n", 1, T1, 1.0);

        // Затем для остальных количеств потоков
        for (int t = 1; t < num_threads_configs; t++)
        {
            int num_threads = threads[t];
            double Tp = run_test(M, N, num_threads);
            double speedup = T1 / Tp;
            printf("| %6d | %13.6f | %9.2f |\n", num_threads, Tp, speedup);
        }
    }

    printf("\n========================================\n");
    printf("Примечание: параллельная инициализация используется\n");
    printf("только для запусков с num_threads > 1\n");

    return 0;
}