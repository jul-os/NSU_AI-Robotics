// gcc -o omp dvemg_1_without_binding.c -fopenmp

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

// Последовательная инициализация вектора
void init_vector_serial(double *b, size_t n)
{
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

// инициализация вектора для параллельного случая, используя вектор из последовательного
// каждый поток имеет свой дупликат вектора
double **create_vector_duplicates_from_original(double *b_original, size_t n, int num_threads)
{
    double **b_duplicates = (double **)malloc(num_threads * sizeof(double *));

    // Выделяем память для каждого дубликата
    for (int i = 0; i < num_threads; i++)
    {
        b_duplicates[i] = (double *)malloc(n * sizeof(double));
        if (b_duplicates[i] == NULL)
        {
            printf("Error allocate memory for duplicate!\n");
            exit(1);
        }
    }

    // Копируем данные
#pragma omp parallel for num_threads(num_threads)
    for (int t = 0; t < num_threads; t++)
    {
        for (size_t j = 0; j < n; j++)
        {
            b_duplicates[t][j] = b_original[j];
        }
    }

    return b_duplicates;
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

// параллельное умножение с использованием дубликатов
void matrix_vector_product_with_duplicates(
    double *a,             // матрица
    double **b_duplicates, // дубликаты вектора
    double *c,             // результат
    size_t m, size_t n,    // размеры
    int num_threads)       // количество потоков
{
#pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        int items_per_thread = m / num_threads;
        // следующие две переменные определяют диапазон строк матрицы которые будет обрабатывать конкретный поток
        int lower_bond = tid * items_per_thread;
        int upper_bond = (tid == num_threads - 1) ? (m - 1) : (lower_bond + items_per_thread - 1);

        double *b_local = b_duplicates[tid];

        for (int i = lower_bond; i <= upper_bond; i++)
        {
            double sum = 0.0;
            for (int j = 0; j < n; j++)
            {
                sum += a[i * n + j] * b_local[j];
            }
            c[i] = sum;
        }
    }
}

// освобождение дубликатов
void free_vector_duplicates(double **b_duplicates, int num_threads)
{
    if (b_duplicates != NULL)
    {
        for (int i = 0; i < num_threads; i++)
        {
            if (b_duplicates[i] != NULL)
            {
                free(b_duplicates[i]);
            }
        }
        free(b_duplicates);
    }
}

double run_test(size_t m, size_t n, int num_threads)
{
    double *a, *b, *c;
    double **b_duplicates = NULL;
    double t_start, t_total;

    a = (double *)malloc(sizeof(*a) * m * n);
    b = (double *)malloc(sizeof(*b) * n);
    c = (double *)malloc(sizeof(*c) * m);

    if (a == NULL || b == NULL || c == NULL)
    {
        printf("Error allocate memory!\n");
        free(a);
        free(b);
        free(c);
        exit(1);
    }

    // Устанавливаем количество потоков
    omp_set_num_threads(num_threads);

    // Инициализация
    if (num_threads > 1)
    {
        init_matrix_parallel(a, m, n);
        init_vector_serial(b, n);
        b_duplicates = create_vector_duplicates_from_original(b, n, num_threads);
    }
    else
    {
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
        matrix_vector_product_with_duplicates(a, b_duplicates, c, m, n, num_threads);
    }
    t_total = cpuSecond() - t_start;

    free(a);
    free(b);
    free(c);

    if (b_duplicates != NULL)
    {
        free_vector_duplicates(b_duplicates, num_threads);
    }

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
        printf("========================================\n");
        printf("| Потоки |   Время (с)   | Ускорение |\n");
        printf("|--------|---------------|-----------|\n");

        // Сначала получаем базовое время для T(1)
        double T1 = run_test(M, N, 1);
        printf("| %6d | %13.6f | %9.2f |\n", 1, T1, 1.0);

        for (int t = 1; t < num_threads_configs; t++)
        {
            int num_threads = threads[t];
            double Tp = run_test(M, N, num_threads);
            double speedup = T1 / Tp;
            printf("| %6d | %13.6f | %9.2f |\n", num_threads, Tp, speedup);
        }
    }

    printf("\n========================================\n");

    return 0;
}