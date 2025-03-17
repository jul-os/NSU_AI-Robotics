#include <stdio.h>
#include <math.h>

#define MAX_N 200
#define EPS 1e-9

int main() {
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);
    int n;
    double a[MAX_N][MAX_N + 1], x[MAX_N];
    
    // Ввод N
    scanf("%d", &n);
    
    // Ввод коэффициентов
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= n; j++) {
            scanf("%lf", &a[i][j]);
        }
    }
    
    // Прямой ход метода Гаусса
    for (int i = 0; i < n; i++) {
        // Поиск главного элемента
        int maxRow = i;
        for (int k = i + 1; k < n; k++) {
            // Функция fabs вычисляет абсолютное значение (модуль) и возвращает его |х|
            if (fabs(a[k][i]) > fabs(a[maxRow][i])) {
                maxRow = k;
            }
        }
        
        // Обмен строк
        for (int k = 0; k <= n; k++) {
            double temp = a[i][k];
            a[i][k] = a[maxRow][k];
            a[maxRow][k] = temp;
        }
        
        // Приведение к треугольному виду
        for (int j = i + 1; j < n; j++) {
            double ratio = a[j][i] / a[i][i];
            for (int k = i; k <= n; k++) {
                a[j][k] -= ratio * a[i][k];
            }
        }
    }
    
    // Обратный ход
    for (int i = n - 1; i >= 0; i--) {
        x[i] = a[i][n];
        for (int j = i + 1; j < n; j++) {
            x[i] -= a[i][j] * x[j];
        }
        x[i] /= a[i][i];
    }
    
    // Вывод решения с точностью до 1e-3
    for (int i = 0; i < n; i++) {
        printf("%.6f\n", x[i]);
    }

    fclose(input);
    fclose(output);
    return 0;
}
