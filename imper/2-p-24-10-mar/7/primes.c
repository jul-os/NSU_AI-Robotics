#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "primes.h"
#define SIZE 10000000

int *primes = NULL;
int count = 0;
bool *prime = NULL;
int *sieve_of_eratosthenes()
{
    prime = malloc((SIZE + 1) * sizeof(bool));
    primes = malloc(SIZE * sizeof(int)); // Массив для простых чисел
    if (!prime || !primes)
    {
        printf("Ошибка выделения памяти\n");
        free(prime);
        free(primes);
        return NULL;
    }

    for (int i = 0; i <= SIZE; i++)
    {
        prime[i] = true;
    }
    prime[0] = prime[1] = false;

    for (int p = 2; p * p <= SIZE; p++)
    {
        if (prime[p])
        {
            for (int i = p * p; i <= SIZE; i += p)
            {
                prime[i] = false;
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
    return primes;
}

int binary(int x, bool *arr)
{
    int left = 0, right = count - 1;
    while (left <= right)
    {
        int middle = left + (right - left) / 2;
        if (middle == x)
            return middle;
        else if (middle < x)
            left = middle + 1;
        else
            right = middle - 1;
    }
}

// returns: 1 if x is prime number, 0 otherwise
int isPrime(int x)
{
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
    int c = 0;
    for (int i = l; i < r; i++)
    {
        if (prime[i] == true)
        {
            c++;
        }
    }
    return c;
}
