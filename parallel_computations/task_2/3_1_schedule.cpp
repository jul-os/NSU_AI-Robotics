#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <omp.h>
#include <string>

#define EPS 1e-5
#define MAX_ITERATIONS 10000

using namespace std;

double vectorNorm(const vector<double> &v, const int num_th)
{
    double sum = 0.0;
#pragma omp parallel for reduction(+ : sum) num_threads(num_th) schedule(runtime)
    for (size_t i = 0; i < v.size(); i++)
    {
        sum += v[i] * v[i];
    }
    return sqrt(sum);
}

vector<double> matrixVectorMultiply(const vector<vector<double>> &A,
                                    const vector<double> &x,
                                    const int num_th)
{
    int N = A.size();
    vector<double> result(N, 0.0);

#pragma omp parallel for num_threads(num_th) schedule(runtime)
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            result[i] += A[i][j] * x[j];
        }
    }
    return result;
}

void computeResidual(const vector<vector<double>> &A,
                     const vector<double> &x,
                     const vector<double> &b,
                     vector<double> &residual,
                     const int num_th)
{
    vector<double> Ax = matrixVectorMultiply(A, x, num_th);

#pragma omp parallel for num_threads(num_th) schedule(runtime)
    for (size_t i = 0; i < Ax.size(); i++)
    {
        residual[i] = Ax[i] - b[i];
    }
}

vector<double> simpleIterationMethod(const vector<vector<double>> &A,
                                     const vector<double> &b,
                                     double tau,
                                     const int num_th)
{
    int N = A.size();
    vector<double> x(N, 0.0);
    vector<double> residual(N);
    double bNorm = vectorNorm(b, num_th);

    for (int iter = 0; iter < MAX_ITERATIONS; iter++)
    {
        computeResidual(A, x, b, residual, num_th);
        double residualNorm = vectorNorm(residual, num_th);
        double stoppingCriterion = residualNorm / bNorm;

        if (stoppingCriterion < EPS)
            break;

#pragma omp parallel for num_threads(num_th) schedule(runtime)
        for (int i = 0; i < N; i++)
        {
            x[i] = x[i] - tau * residual[i];
        }
    }
    return x;
}

string scheduleToString(omp_sched_t type)
{
    switch (type)
    {
    case omp_sched_static:
        return "static";
    case omp_sched_dynamic:
        return "dynamic";
    case omp_sched_guided:
        return "guided";
    case omp_sched_auto:
        return "auto";
    default:
        return "unknown";
    }
}

double runTest(const vector<vector<double>> &A,
               const vector<double> &b,
               int num_th,
               omp_sched_t sched_type,
               int chunk_size)
{
    // Устанавливаем расписание для runtime-директив
    if (chunk_size > 0)
    {
        omp_set_schedule(sched_type, chunk_size);
    }
    else
    {
        omp_set_schedule(sched_type, 0); // default chunk
    }

    double tau = 0.9 * 2.0 / (A.size() + 1);

    const auto start{chrono::steady_clock::now()};
    vector<double> solution = simpleIterationMethod(A, b, tau, num_th);
    const auto end{chrono::steady_clock::now()};

    return chrono::duration<double>(end - start).count();
}

int main()
{
    int N = 9000;
    vector<int> fixed_threads = {2, 4, 7, 8, 16, 20, 40};

    for (int num_threads : fixed_threads)
    {
        vector<vector<double>> A(N, vector<double>(N));

#pragma omp parallel for num_threads(num_threads)
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                i == j ? A[i][j] = 2.0 : A[i][j] = 1.0;
            }
        }

        vector<double> b(N);
#pragma omp parallel for num_threads(num_threads)
        for (int i = 0; i < N; i++)
        {
            b[i] = N + 1.0;
        }

        vector<omp_sched_t> schedule_types = {
            omp_sched_static,
            omp_sched_dynamic,
            omp_sched_guided,
            omp_sched_auto};
        vector<int> chunk_sizes = {0, 1, 4, 16, 64, 256, N / num_threads}; // 0 = default

        cout << "\n"
             << string(75, '=') << endl;
        cout << "OpenMP schedule benchmark | N=" << N << ", threads=" << num_threads << endl;
        cout << string(75, '=') << endl;
        cout << left << setw(12) << "Schedule"
             << setw(10) << "Chunk"
             << setw(15) << "Time (sec)"
             << "Comment" << endl;
        cout << string(75, '-') << endl;

        double best_time = 1e9;
        string best_config = "";

        for (auto sched : schedule_types)
        {
            string sched_name = scheduleToString(sched);

            // auto игнорирует chunk_size
            if (sched == omp_sched_auto)
            {
                double time = runTest(A, b, num_threads, sched, 0);
                cout << left << setw(12) << sched_name
                     << setw(10) << "default"
                     << setw(15) << fixed << setprecision(4) << time
                     << "(chunk ignored)" << endl;
                if (time < best_time)
                {
                    best_time = time;
                    best_config = sched_name + "(default)";
                }
                continue;
            }

            for (int chunk : chunk_sizes)
            {
                double time = runTest(A, b, num_threads, sched, chunk);

                string chunk_str = (chunk == 0) ? "default" : to_string(chunk);
                cout << left << setw(12) << sched_name
                     << setw(10) << chunk_str
                     << setw(15) << fixed << setprecision(4) << time;

                if (time < best_time)
                {
                    best_time = time;
                    best_config = sched_name + "(chunk=" + chunk_str + ")";
                }
                cout << endl;
            }
        }

        cout << string(75, '=') << endl;
        cout << ">>> OPTIMAL: " << best_config << endl;
        cout << ">>> MIN TIME: " << fixed << setprecision(4) << best_time << " sec" << endl;
        cout << string(75, '=') << endl;
    }
    return 0;
}