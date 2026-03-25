#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <omp.h>

#define EPS 1e-5
#define MAX_ITERATIONS 10000

using namespace std;

int main()
{
    int max_threads = omp_get_max_threads();
    int N;
    cout << "Введите N ";
    cin >> N;

    for (int num_threads = 1; num_threads <= max_threads; num_threads++)
    {
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

#pragma omp parallel num_threads(num_threads) shared(A, b, x, residual, Ax, N, tau, done_flag, bNorm, sum, start_time)
        {
#pragma omp for collapse(2) nowait
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    A[i][j] = (i == j) ? 2.0 : 1.0;
                }
            }

#pragma omp for nowait
            for (int i = 0; i < N; i++)
            {
                b[i] = N + 1.0;
            }

#pragma omp barrier

            sum = 0.0;
#pragma omp for reduction(+ : sum)
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
#pragma omp for
                for (int i = 0; i < N; i++)
                {
                    Ax[i] = 0.0;
                    for (int j = 0; j < N; j++)
                    {
                        Ax[i] += A[i][j] * x[j];
                    }
                }

#pragma omp for nowait
                for (size_t i = 0; i < Ax.size(); i++)
                {
                    residual[i] = Ax[i] - b[i];
                }

                sum = 0.0;
#pragma omp for reduction(+ : sum)
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
                        cout << "Достигнута требуемая точность на итерации " << iter + 1 << endl;
                        done_flag = 1;
                    }
                }

#pragma omp barrier
                if (done_flag)
                    break;

#pragma omp for
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
                double elapsed = std::chrono::duration<double>(end - start_time).count();
                cout << "Полученное решение (потоков: " << num_threads << "):" << endl;
                for (int i = 0; i < 10; i++)
                {
                    cout << "x[" << i << "] = " << fixed << setprecision(6) << x[i] << endl;
                }
                cout << "В этом случае правильным решением системы будет вектор, элементы которого равны 1.0" << endl;
                cout << "Время:" << elapsed << endl;
            }
        }
    }
    return 0;
}