#include <stdio.h>
#include <stdlib.h>
#include "primes.h"
#define SIZE 10000030

static int *primes = NULL;
int count = 0;
int *prime = NULL;

//вызывать решето в каждой функции но создать в ней флаг который поднимается при первом проходе
//удалить нафиг второй массив
int flag = 0;

static void sieve_of_eratosthenes()
{
    if (flag == 1){
        return;
    }
    prime = malloc((SIZE + 1) * sizeof(int));
    primes = malloc(SIZE * sizeof(int)); // Массив для простых чисел
    if (!prime || !primes)
    {
        printf("Ошибка выделения памяти\n");
        free(prime);
        free(primes);
        return;
    }

    for (int i = 0; i <= SIZE; i++)
    {
        prime[i] = 1;
    }
    prime[0] = prime[1] = 0;

    for (int p = 2; p * p <= SIZE; p++)
    {
        if (prime[p])
        {
            for (int i = p * p; i <= SIZE; i += p)
            {
                prime[i] = 0;
            }
        }
    }

    count = 0; // Количество простых чисел
    for (int i = 2; i <= SIZE; i++)
    {
        if (prime[i])
        {
            primes[count++] = i;
        }
    }
    flag = 1;
    return;
}


int isPrime(int x)
{
    sieve_of_eratosthenes();
    int left = 0, right = count - 1;
    while (left <= right)
    {
        int middle = left + (right - left) / 2;
        if (primes[middle] == x)
            return 1; // Нашли число
        else if (primes[middle] < x)
            left = middle + 1;
        else
            right = middle - 1;
    }
    return 0; // Число не простое
}

// returns minimal prime number p such that p >= x
int findNextPrime(int x)
{
    sieve_of_eratosthenes();
    int left = 0, right = count - 1;

    while (left < right)
    {
        int middle = left + (right - left) / 2;
        if (primes[middle] < x)
            left = middle + 1;
        else
            right = middle;
    }

    // Теперь `left` указывает на первое простое число, которое >= x
    return primes[left];
}

// returns the number of primes x such that l <= x < r
int getPrimesCount(int l, int r)
{
    sieve_of_eratosthenes();
    int c = 0;
    for (int i = l; i < r; i++)
    {
        if (prime[i] == 1)
        {
            c++;
        }
    }
    return c;
}