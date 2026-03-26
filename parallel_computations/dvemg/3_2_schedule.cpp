#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <omp.h>
#include <string>
#include <algorithm>

#define EPS 1e-5
#define MAX_ITERATIONS 10000

using namespace std;

string scheduleToString(omp_sched_t type) {
    switch(type) {
        case omp_sched_static:  return "static";
        case omp_sched_dynamic: return "dynamic";
        case omp_sched_guided:  return "guided";
        case omp_sched_auto:    return "auto";
        default:                return "unknown";
    }
}

double runIterationWithSchedule(int N, int num_threads, omp_sched_t sched_type, int chunk_size)
{
    // Устанавливаем расписание для runtime-директив
    if (chunk_size > 0) {
        omp_set_schedule(sched_type, chunk_size);
    } else {
        omp_set_schedule(sched_type, 0);
    }
    
    vector<vector<double>> A(N, vector<double>(N));
    vector<double> b(N);
    vector<double> x(N, 0.0);
    vector<double> residual(N);
    vector<double> Ax(N);
    double tau = 0.9 * 2.0 / (N + 1);
    volatile int done_flag = 0;
    double bNorm = 0.0;
    double sum = 0.0;
    std::chrono::steady_clock::time_point start_time;
    double elapsed = 0.0;

#pragma omp parallel num_threads(num_threads) shared(A, b, x, residual, Ax, N, tau, done_flag, bNorm, sum, start_time)
    {
        #pragma omp for collapse(2) nowait schedule(runtime)
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i][j] = (i == j) ? 2.0 : 1.0;
            }
        }

        #pragma omp for nowait schedule(runtime)
        for (int i = 0; i < N; i++)
        {
            b[i] = N + 1.0;
        }

        #pragma omp barrier

        sum = 0.0;
        #pragma omp for reduction(+ : sum) schedule(runtime)
        for (size_t i = 0; i < b.size(); i++)
        {
            sum += b[i] * b[i];
        }
        #pragma omp single
        {
            bNorm = sqrt(sum);
            start_time = std::chrono::steady_clock::now();
        }

        #pragma omp barrier

        for (int iter = 0; iter < MAX_ITERATIONS; iter++)
        {
            #pragma omp for schedule(runtime)
            for (int i = 0; i < N; i++)
            {
                Ax[i] = 0.0;
                for (int j = 0; j < N; j++)
                {
                    Ax[i] += A[i][j] * x[j];
                }
            }

            #pragma omp for nowait schedule(runtime)
            for (size_t i = 0; i < Ax.size(); i++)
            {
                residual[i] = Ax[i] - b[i];
            }

            sum = 0.0;
            #pragma omp for reduction(+ : sum) schedule(runtime)
            for (size_t i = 0; i < N; i++)
            {
                sum += residual[i] * residual[i];
            }

            double residualNorm, stoppingCriterion;
            #pragma omp single
            {
                residualNorm = sqrt(sum);
                stoppingCriterion = residualNorm / bNorm;

                if (stoppingCriterion < EPS && done_flag == 0)
                {
                    done_flag = 1;
                }
            }

            #pragma omp barrier
            if (done_flag)
                break;

            #pragma omp for schedule(runtime)
            for (int i = 0; i < N; i++)
            {
                if (done_flag == 0)
                {
                    x[i] = x[i] - tau * residual[i];
                }
            }
        }
        #pragma omp single
        {
            const auto end{std::chrono::steady_clock::now()};
            elapsed = std::chrono::duration<double>(end - start_time).count();
        }
    }
    return elapsed;
}

int main()
{
    int N = 9000;
    int max_threads = omp_get_max_threads();
    vector<int> fixed_threads = { 2, 4, 7, 8, 16, 20, 40};
    
    vector<omp_sched_t> schedule_types = {
        omp_sched_static, 
        omp_sched_dynamic, 
        omp_sched_guided, 
        omp_sched_auto
    };
    
    // Chunk sizes: 0=default, плюс стандартные значения, плюс N/threads
    
    cout << "\n# CSV_FORMAT: threads,schedule,chunk,time_seconds" << endl;
    
    for (int num_threads : fixed_threads)
    {
        vector<int> base_chunks = {0, 1, 4, 16, 64, 256, N/num_threads};
        if (num_threads > max_threads) continue;
        
        vector<int> chunk_sizes = base_chunks;
        
        cout << "\n" << string(80, '-') << endl;
        cout << ">>> Тестирование: threads = " << num_threads << endl;
        cout << string(80, '-') << endl;
        cout << left << setw(12) << "Schedule" 
             << setw(10) << "Chunk" 
             << setw(15) << "Time (sec)" 
             << "Note" << endl;
        cout << string(80, '-') << endl;
        
        double best_time = 1e9;
        string best_config = "";
        
        for (auto sched : schedule_types) {
            string sched_name = scheduleToString(sched);
            
            // auto игнорирует chunk_size — тестируем только с 0
            if (sched == omp_sched_auto) {
                cout << left << setw(12) << sched_name << setw(10) << "default";
                cout.flush();
                
                double time = 0.0;
                #pragma omp parallel num_threads(num_threads)
                {
                    #pragma omp single
                    time = runIterationWithSchedule(N, num_threads, sched, 0);
                }
                
                cout << setw(15) << time << "(auto)" << endl;
                cout << num_threads << "," << sched_name << ",default," << time << endl;
                
                if (time < best_time) {
                    best_time = time;
                    best_config = sched_name + "(default)";
                }
                continue;
            }
            
            // Тестируем static/dynamic/guided с разными chunk
            for (int chunk : chunk_sizes) {
                cout << left << setw(12) << sched_name;
                string chunk_str = (chunk == 0) ? "default" : to_string(chunk);
                cout << setw(10) << chunk_str;
                cout.flush();
                
                double time = 0.0;

                time = runIterationWithSchedule(N, num_threads, sched, chunk);
                                
                cout << setw(15) << fixed << setprecision(4) << time << endl;
                
                if (time < best_time) {
                    best_time = time;
                    best_config = sched_name + "(chunk=" + chunk_str + ")";
                    cout << "   ← BEST" << endl;
                }
            }
        }        
        cout << "\n>>> OPTIMAL для " << num_threads << " потоков: " << best_config << endl;
        cout << ">>> MIN TIME: " << fixed << setprecision(4) << best_time << " sec" << endl;
    }
    return 0;
}