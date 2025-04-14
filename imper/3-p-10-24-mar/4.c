#include <stdio.h>
#include <math.h>

double my_sqrt(double x)
{
    if (x < 0)
    {
        return -1;
    }

    double guess = x;
    double epsilon = 1e-10;

    while ((guess * guess - x) > epsilon || (x - guess * guess) > epsilon)
    {
        guess = (guess + x / guess) / 2.0;
    }

    return guess;
}

long double find_integral(double l, double r)
{
    long double x[4], y[4], z[4];
    scanf("%Le %Le %Le %Le", &x[0], &x[1], &x[2], &x[3]);
    scanf("%Le %Le %Le %Le", &y[0], &y[1], &y[2], &y[3]);
    scanf("%Le %Le %Le %Le", &z[0], &z[1], &z[2], &z[3]);
    // t это какое-то промежуточная точка
    // пусть используем формулу Котеса

    // разобьем отрезок на еще точки в которых будем искать функцию
    // пусть их будет типа 100. или не сто хз но умножим на 100
    int n =  ((int) (r - l) + 1)*100;
    long double point_interval = (r - l) / n;
    long double res = 0;

    for (int i = 0; i <= n; ++i)
    {
        int Cotes_coeff;
        if ((i == 0) || (i == n))
        {
            Cotes_coeff = 1;
        }
        else if (i % 2 == 0)
        {
            Cotes_coeff = 2;
        }
        else
        {
            Cotes_coeff = 4;
        }
        long double t_min_l = point_interval * i;
        long double x_der = x[1] + 2.0 * x[2] * t_min_l + 3.0 * x[3] * t_min_l * t_min_l;
        long double y_der = y[1] + 2.0 * y[2] * t_min_l + 3.0 * y[3] * t_min_l * t_min_l;
        long double z_der = z[1] + 2.0 * z[2] * t_min_l + 3.0 * z[3] * t_min_l * t_min_l;
        long double sqrt_sum = sqrt((x_der * x_der + y_der * y_der + z_der * z_der));
        res += Cotes_coeff * sqrt_sum;
        // ужасно. кто придумал математику
    }
    res *= point_interval / 3.0;
    return res;
}

int main(void)
{
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    int n;
    double l, r;
    long double sum = 0;
    scanf("%d", &n);

    for (int i = 0; i < n; i++)
    {
        scanf("%le %le", &l, &r);
        sum += find_integral((long double)l, (long double)r);
    }
    printf("%0.20Lf", sum);

    fclose(input);
    fclose(output);
    return 0;
}