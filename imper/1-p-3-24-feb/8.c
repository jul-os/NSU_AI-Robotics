#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_N 300
#define MODULO 1000000007

int n, k, P;
int a[MAX_N][MAX_N + 1];
int x[MAX_N];
bool free_var[MAX_N]; // true, если переменная свободная

// Быстрое возведение в степень по модулю
int power(int base, int exp, int mod) {
    int result = 1;
    while (exp > 0) {
        if (exp % 2 == 1)
            result = (1LL * result * base) % mod;
        base = (1LL * base * base) % mod;
        exp /= 2;
    }
    return result;
}

// Обратное число по модулю (т.к. P — простое, можно использовать теорему Ферма)
int mod_inv(int num, int mod) {
    return power(num, mod - 2, mod);
}

// Приведение к ступенчатому виду
int gauss() {
    int rank = 0;
    for (int col = 0; col < n; col++) {
        int pivot = -1;
        for (int row = rank; row < k; row++) {
            if (a[row][col] != 0) {
                pivot = row;
                break;
            }
        }
        
        if (pivot == -1) {
            free_var[col] = true;
            continue;
        }
        
        // Меняем строки
        if (pivot != rank) {
            for (int j = 0; j <= n; j++) {
                int temp = a[rank][j];
                a[rank][j] = a[pivot][j];
                a[pivot][j] = temp;
            }
        }
        
        // Приведение ведущего элемента к 1
        int inv = mod_inv(a[rank][col], P);
        for (int j = 0; j <= n; j++) {
            a[rank][j] = (1LL * a[rank][j] * inv) % P;
        }
        
        // Обнуление остальных элементов в колонке
        for (int row = 0; row < k; row++) {
            if (row != rank && a[row][col] != 0) {
                int factor = a[row][col];
                for (int j = 0; j <= n; j++) {
                    a[row][j] = (a[row][j] - 1LL * factor * a[rank][j] % P + P) % P;
                }
            }
        }
        
        rank++;
    }
    
    // Проверка на несовместность (ранг < k, но есть ненулевой правый столбец)
    for (int i = rank; i < k; i++) {
        if (a[i][n] != 0) {
            return -1;
        }
    }
    return rank;
}

void find_solution(int rank) {
    for (int i = 0; i < n; i++) {
        x[i] = 0;
    }
    
    for (int i = rank - 1; i >= 0; i--) {
        int leading_var = -1;
        for (int j = 0; j < n; j++) {
            if (a[i][j] != 0) {
                leading_var = j;
                break;
            }
        }
        if (leading_var == -1) continue;
        
        x[leading_var] = a[i][n];
        for (int j = leading_var + 1; j < n; j++) {
            x[leading_var] = (x[leading_var] - 1LL * a[i][j] * x[j] % P + P) % P;
        }
    }
}

int main() {
    scanf("%d %d %d", &n, &k, &P);
    
    for (int i = 0; i < k; i++) {
        for (int j = 0; j <= n; j++) {
            scanf("%d", &a[i][j]);
        }
    }
    
    int rank = gauss();
    
    if (rank == -1) {
        printf("0\n");
        return 0;
    }
    
    int free_vars = 0;
    for (int i = 0; i < n; i++) {
        if (free_var[i]) free_vars++;
    }
    
    if (free_vars == 0) {
        find_solution(rank);
        for (int i = 0; i < n; i++) {
            printf("%d\n", x[i]);
        }
    } else {
        int solutions = power(P, free_vars, MODULO);
        printf("%d\n", solutions);
    }
    return 0;
}
