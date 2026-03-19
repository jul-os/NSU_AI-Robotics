#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

#define EPS 1e-5
#define MAX_ITERATIONS 10000

using namespace std;

double vectorNorm(const vector<double> &v)
{
    double sum = 0.0;
    for (size_t i = 0; i < v.size(); i++)
    {
        sum += v[i] * v[i];
    }
    return sqrt(sum);
}

vector<double> matrixVectorMultiply(const vector<vector<double>> &A,
                                    const vector<double> &x)
{
    int N = A.size();
    vector<double> result(N, 0.0);
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
vector<double> computeResidual(const vector<vector<double>> &A,
                               const vector<double> &x,
                               const vector<double> &b)
{
    vector<double> Ax = matrixVectorMultiply(A, x);
    vector<double> residual(Ax.size());

    for (size_t i = 0; i < Ax.size(); i++)
    {
        residual[i] = Ax[i] - b[i];
    }

    return residual;
}

vector<double> simpleIterationMethod(const vector<vector<double>> &A,
                                     const vector<double> &b,
                                     double tau)
{
    int N = A.size();

    vector<double> x(N, 0.0);

    double bNorm = vectorNorm(b);

    for (int iter = 0; iter < MAX_ITERATIONS; iter++)
    {
        vector<double> residual = computeResidual(A, x, b);
        double residualNorm = vectorNorm(residual);
        double stoppingCriterion = residualNorm / bNorm;

        if (stoppingCriterion < EPS)
        {
            cout << "Достигнута требуемая точность на итерации " << iter + 1 << endl;
            return x;
        }

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
    int N;
    cout << "Введите N ";
    cin >> N;

    vector<vector<double>> A(N, vector<double>(N, 1.0));
    for (int i = 0; i < N; i++)
    {
        A[i][i] = 2.0;
    }

    vector<double> b(N, N + 1.0);

    double tau;
    cout << "Введите параметр tau (рекомендуется 0.01 или -0.01) ";
    cin >> tau;

    vector<double> solution = simpleIterationMethod(A, b, tau);
    cout << "Полученное решение:" << endl;
    for (int i = 0; i < N; i++)
    {
        cout << "x[" << i << "] = " << fixed << setprecision(6) << solution[i] << endl;
    }
    cout << "В этом случае правильным решением системы будет вектор, элементы которого равны 1.0" << endl;
    return 0;
}