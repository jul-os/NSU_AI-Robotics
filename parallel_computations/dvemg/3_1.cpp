#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <omp.h>

#define EPS 1e-5
#define MAX_ITERATIONS 10000

using namespace std;

double vectorNorm(const vector<double> &v, const int num_th)
{
    double sum = 0.0;
#pragma omp parallel for reduction(+ : sum) num_threads(num_th)
    for (size_t i = 0; i < v.size(); i++)
    {
        sum += v[i] * v[i];
    }
    return sqrt(sum);
}

vector<double> matrixVectorMultiply(const vector<vector<double>> &A,
                                    const vector<double> &x, const int num_th)
{
    int N = A.size();
    vector<double> result(N, 0.0);
#pragma omp parallel for num_threads(num_th)
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            result[i] += A[i][j] * x[j];
        }
    }

    return result;
}

// Функция для вычисления Ax - b
void computeResidual(const vector<vector<double>> &A,
                               const vector<double> &x,
                               const vector<double> &b,
                               vector<double> &residual,
                               const int num_th)
{
    vector<double> Ax = matrixVectorMultiply(A, x, num_th);
#pragma omp parallel for num_threads(num_th)
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
        {
            cout << "Достигнута требуемая точность на итерации " << iter + 1 << endl;
            return x;
        }

#pragma omp parallel for num_threads(num_th)
        for (int i = 0; i < N; i++)
        {
            x[i] = x[i] - tau * residual[i];
        }
    }

    cout << "Достигнуто максимальное число итераций" << endl;
    return x;
}

int main()
{
    int max_threads = omp_get_max_threads();
    int N;
    cout << "Введите N ";
    cin >> N;
    for (int num_th = 1; num_th <= max_threads; num_th++){
        vector<vector<double>> A(N, vector<double>(N));
#pragma omp parallel for num_threads(num_th)
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j< N; j++){
                i==j ? A[i][j] = 2.0 : A[i][j] = 1.0;
            }
        }

        vector<double> b(N);
#pragma omp parallel for num_threads(num_th)
        for (int i = 0; i < N; i++)
        {
            b[i]= N + 1.0;
        }

        double tau = 0.9 * 2.0/(N+1);

        const auto start{std::chrono::steady_clock::now()};
        vector<double> solution = simpleIterationMethod(A, b, tau, num_th);
        const auto end{std::chrono::steady_clock::now()};
        const std::chrono::duration<double> elapsed_seconds{end - start};
        cout << "Полученное решение:" << endl;
        for (int i = 0; i < 5; i++)
        {
            cout << "x[" << i << "] = " << fixed << setprecision(6) << solution[i] << endl;
        }
        cout << "В этом случае правильным решением системы будет вектор, элементы которого равны 1.0" << endl;
        cout << "Количество потоков:"<< num_th <<"Время:" << elapsed_seconds.count() << endl;
    }
    return 0;
}
