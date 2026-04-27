#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <barrier>
#include <atomic>

using namespace std;
using namespace chrono;

double sequential_init_and_mult(size_t n, double start_val)
{
    double *matrix = new double[n * n];
    double *vec = new double[n];
    double *result = new double[n]();

    for (size_t i = 0; i < n; ++i)
    {
        vec[i] = start_val + static_cast<double>(i);
    }

    for (size_t i = 0; i < n * n; ++i)
    {
        matrix[i] = start_val + static_cast<double>(i);
    }

    // Вычисления + замер
    auto t1 = high_resolution_clock::now();

    for (size_t i = 0; i < n; ++i)
    {
        double sum = 0.0;
        size_t row_start = i * n;
        for (size_t j = 0; j < n; ++j)
        {
            sum += matrix[row_start + j] * vec[j];
        }
        result[i] = sum;
    }

    auto t2 = high_resolution_clock::now();

    delete[] matrix;
    delete[] vec;
    delete[] result;

    return duration<double>(t2 - t1).count();
}

double parallel_init_and_mult(size_t n, double start_val, size_t num_threads)
{
    vector<double> matrix(n * n);
    vector<double> vec(n);
    vector<double> result(n, 0.0);

    for (size_t i = 0; i < n; ++i)
    {
        vec[i] = start_val + static_cast<double>(i);
    }

    // Атомик для хранения максимального времени вычислений среди потоков
    atomic<double> max_compute_time{0.0};

    // Барьер: синхронизация между инициализацией и вычислениями
    barrier sync_point(num_threads, [] {});

    vector<jthread> threads;
    threads.reserve(num_threads);
    size_t rows_per_thread = n / num_threads;

    for (size_t t = 0; t < num_threads; ++t)
    {
        size_t start_row = t * rows_per_thread;
        size_t end_row = (t == num_threads - 1) ? n : start_row + rows_per_thread;
        size_t matrix_start = start_row * n;
        size_t matrix_end = end_row * n;
        double base = start_val + static_cast<double>(matrix_start);

        threads.emplace_back(
            [&sync_point, &matrix, &vec, &result, &max_compute_time, n,
             matrix_start, matrix_end, base, start_row, end_row]()
            {
                for (size_t idx = matrix_start; idx < matrix_end; ++idx)
                {
                    matrix[idx] = base + static_cast<double>(idx - matrix_start);
                }

                // все потоки ждут друг друга
                sync_point.arrive_and_wait();

                auto t1 = high_resolution_clock::now();

                for (size_t i = start_row; i < end_row; ++i)
                {
                    double sum = 0.0;
                    const double *row = matrix.data() + i * n;
                    const double *v = vec.data();
                    for (size_t j = 0; j < n; ++j)
                    {
                        sum += row[j] * v[j];
                    }
                    result[i] = sum;
                }

                auto t2 = high_resolution_clock::now();
                double thread_time = duration<double>(t2 - t1).count();

                double current_max = max_compute_time.load();
                while (thread_time > current_max &&
                       !max_compute_time.compare_exchange_weak(current_max, thread_time))
                {
                    // тут должно быть пусто
                    // compare_exchange_weak обновит current_max, если не удалось установить thread_time
                }
            });
    }

    // Ждём завершения всех потоков
    //  вообзе автоматический join при уничтожении вектора, но явно вызываем для ясности
    for (auto &th : threads)
    {
        th.join();
    }

    // Возвращаем время вычислений  те максимум среди потоковЫ
    return max_compute_time.load();
}

int main(int argc, char *argv[])
{
    size_t n = (argc > 1) ? stoull(argv[1]) : 20000;
    vector<size_t> thread_counts = {2, 4, 7, 8, 16, 20, 40};

    cout << fixed << setprecision(3);

    double baseline_time = sequential_init_and_mult(n, 1.0);
    cout << "Baseline time: " << baseline_time << " s\n\n";

    cout << setw(10) << "Threads" << setw(15) << "Time(s)"
         << setw(15) << "Speedup" << "\n";
    cout << string(50, '-') << "\n";

    for (size_t nt : thread_counts)
    {
        if (nt == 1)
            continue;

        double par_time = parallel_init_and_mult(n, 1.0, nt);
        double speedup = baseline_time / par_time;

        cout << setw(10) << nt
             << setw(15) << par_time
             << setw(15) << speedup << "\n";
    }

    return 0;
}