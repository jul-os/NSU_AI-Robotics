// gcc -o omp integral.c -fopenmp -lm

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

const double A = -4.0;
const double B = 4.0;
const int NSTEPS = 40000000;

double func(double x)
{
    return exp(-x * x);
}

double integrate_serial(double a, double b, int n)
{
    double h = (b - a) / n;
    double sum = 0.0;
    for (int i = 0; i < n; i++)
    {
        sum += func(a + h * (i + 0.5));
    }
    return sum * h;
}

double integrate_omp(double a, double b, int n)
{
    double h = (b - a) / n;
    double sum = 0.0;

#pragma omp parallel
    {
        int nthreads = omp_get_num_threads();
        int threadid = omp_get_thread_num();

        // Разбиение на части
        int items_per_thread = n / nthreads;
        int lb = threadid * items_per_thread;
        int ub = (threadid == nthreads - 1) ? (n - 1) : (lb + items_per_thread - 1);

        double sumloc = 0.0; // Локальная переменная потока

        for (int i = lb; i <= ub; i++)
        {
            sumloc += func(a + h * (i + 0.5));
        }

// Атомарное сложение локальной суммы в глобальную
#pragma omp atomic
        sum += sumloc;
    }
    return sum * h;
}

int main()
{
    int thread_counts[] = {1, 2, 4, 7, 8, 16, 20, 40};    
    int num_tests = sizeof(thread_counts) / sizeof(thread_counts[0]);

    double serial_time = 0.0;
    double serial_result = 0.0;

    omp_set_num_threads(1);
    double t_start = omp_get_wtime();
    serial_result = integrate_serial(A, B, NSTEPS);
    double t_end = omp_get_wtime();
    serial_time = t_end - t_start;

    printf("Serial Time: %.6f\n", serial_time);

    printf("Threads,Time,Speedup\n");

    for (int i = 0; i < num_tests; i++)
    {
        int threads = thread_counts[i];
        omp_set_num_threads(threads);

        double t_start = omp_get_wtime();
        double parallel_result = integrate_omp(A, B, NSTEPS);
        double t_end = omp_get_wtime();

        double parallel_time = t_end - t_start;
        double speedup = serial_time / parallel_time;
        
        printf("%d,%.6f,%.2f\n", threads, parallel_time, speedup);
    }

    return 0;
}