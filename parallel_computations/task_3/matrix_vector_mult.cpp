#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <memory>

using namespace std;
using namespace chrono;

using Value = double;

// Последовательное умножение
vector<Value> mat_vec_mult_sequential(const Value* matrix, const Value* vec, size_t n) {
    vector<Value> result(n, 0.0);
    for (size_t i = 0; i < n; ++i) {
        Value sum = 0.0;
        for (size_t j = 0; j < n; ++j) {
            sum += matrix[i * n + j] * vec[j];
        }
        result[i] = sum;
    }
    return result;
}

// Параллельная обработка диапазона строк
void mat_vec_mult_chunk(const Value* matrix, const Value* vec, Value* result, 
                       size_t n, size_t start_row, size_t end_row) {
    for (size_t i = start_row; i < end_row; ++i) {
        Value sum = 0.0;
        const Value* row = matrix + i * n;
        for (size_t j = 0; j < n; ++j) {
            sum += row[j] * vec[j];
        }
        result[i] = sum;
    }
}

// Параллельное умножение с std::jthread
vector<Value> mat_vec_mult_parallel(const Value* matrix, const Value* vec, size_t n, size_t num_threads) {
    vector<Value> result(n, 0.0);
    
    if (num_threads <= 1) {
        return mat_vec_mult_sequential(matrix, vec, n);
    }
    
    vector<jthread> threads;
    threads.reserve(num_threads);
    
    size_t chunk = n / num_threads;
    
    for (size_t t = 0; t < num_threads; ++t) {
        size_t start = t * chunk;
        size_t end = (t == num_threads - 1) ? n : start + chunk;
        
        threads.emplace_back(mat_vec_mult_chunk, 
                           matrix, vec, result.data(), n, start, end);
    }
    // jthread автоматически делает join при разрушении
    return result;
}

// Последовательная инициализация плоского массива
void sequential_init_flat(Value* arr, size_t size, Value start_val) {
    for (size_t i = 0; i < size; ++i) {
        arr[i] = start_val + static_cast<Value>(i);
    }
}

// Параллельная инициализация плоского массива
void parallel_init_flat(Value* arr, size_t size, Value start_val, size_t num_threads) {
    if (num_threads <= 1) {
        sequential_init_flat(arr, size, start_val);
        return;
    }
    
    vector<jthread> threads;
    threads.reserve(num_threads);
    
    size_t chunk = size / num_threads;
    
    for (size_t t = 0; t < num_threads; ++t) {
        size_t start = t * chunk;
        size_t end = (t == num_threads - 1) ? size : start + chunk;
        Value base = start_val + static_cast<Value>(start);
        
        threads.emplace_back([arr, base, start, end]() {
            for (size_t i = start; i < end; ++i) {
                arr[i] = base + static_cast<Value>(i - start);
            }
        });
    }
}

// Проверка результатов
bool verify_results(const Value* ref, const Value* test, size_t n, Value eps = 1e-6) {
    for (size_t i = 0; i < n; ++i) {
        if (fabs(ref[i] - test[i]) > eps * (fabs(ref[i]) + 1.0)) {
            return false;
        }
    }
    return true;
}

// Замер времени выполнения функции
template<typename Func>
double measure_time(Func&& f) {
    auto t1 = high_resolution_clock::now();
    f();
    auto t2 = high_resolution_clock::now();
    return duration<double>(t2 - t1).count();
}

int main(int argc, char* argv[]) {
    size_t n = (argc > 1) ? stoull(argv[1]) : 20000;
    vector<size_t> thread_counts = {2, 4, 7, 8, 16, 20, 40};
    
    cout << fixed << setprecision(4);

    // Выделение памяти
    unique_ptr<Value[]> matrix, vec, ref_result, par_result;
    try {
        matrix = make_unique<Value[]>(n * n);
        vec = make_unique<Value[]>(n);
        ref_result = make_unique<Value[]>(n);
        par_result = make_unique<Value[]>(n);
    } catch (const bad_alloc& e) {
        cerr << "ERROR: Not enough memory for " << n << "x" << n << " matrix!\n";
        return 1;
    }

    // Последовательная инициализация для базового замера
    sequential_init_flat(matrix.get(), n * n, 1.0);
    sequential_init_flat(vec.get(), n, 1.0);
    
    // Замер времени умножения на 1 потоке (это наш baseline)
    double baseline_time = measure_time([&]() {
        auto res = mat_vec_mult_sequential(matrix.get(), vec.get(), n);
        copy(res.begin(), res.end(), ref_result.get());
    });
    
    cout << "Baseline time (1 thread): " << baseline_time << " s\n\n";

    cout << setw(10) << "Threads" << setw(15) << "Time(s)" 
         << setw(15) << "Speedup" << setw(10) << "OK" << "\n";
    cout << string(50, '-') << "\n";
    
    size_t hw_threads = thread::hardware_concurrency();
    if (hw_threads == 0) hw_threads = 1;
    
    for (size_t nt : thread_counts) {
        // Инициализация последовательная для 1 потока параллельная для остальных
        if (nt == 1) {
            sequential_init_flat(matrix.get(), n * n, 1.0);
            sequential_init_flat(vec.get(), n, 1.0);
        } else {
            // Для параллельной инициализации используем min(nt, hw_threads) потоков
            size_t init_threads = nt;
            parallel_init_flat(matrix.get(), n * n, 1.0, init_threads);
            parallel_init_flat(vec.get(), n, 1.0, init_threads);
        }
        
        //Замер времени без инициализации
        double par_time = measure_time([&]() {
            auto res = mat_vec_mult_parallel(matrix.get(), vec.get(), n, nt);
            copy(res.begin(), res.end(), par_result.get());
        });
        
        
        double speedup = baseline_time / par_time;
        
        cout << setw(10) << nt 
             << setw(15) << par_time 
             << setw(15) << (nt > 2 ? speedup : 0.0)<< "\n";
    }
    
    return 0;
}