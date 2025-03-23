#include "primes.h"
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
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
int main()
{
    sieve_of_eratosthenes();
    assert(isPrime(1) == 0);
    assert(isPrime(0) == 0);
    assert(isPrime(2) == 1);
    assert(isPrime(3) == 1);
    assert(isPrime(10) == 0);
    assert(isPrime(99) == 0);
    assert(isPrime(131) == 1);
    assert(isPrime(1151) == 1);

    assert(findNextPrime(0) == 2);
    assert(findNextPrime(1) == 2);
    assert(findNextPrime(2) == 2);
    assert(findNextPrime(3) == 3);
    assert(findNextPrime(4) == 5);
    assert(findNextPrime(20) == 23);
    assert(findNextPrime(1000) == 1009);
    assert(findNextPrime(17) == 17);

    assert(getPrimesCount(0, 1) == 0);
    assert(getPrimesCount(0, 10) == 4);
    assert(getPrimesCount(10, 10) == 0);
    //printf("%d\n", getPrimesCount(7, 7));
    //assert(getPrimesCount(7, 7) == 1);
    assert(getPrimesCount(10, 20) == 4);
    assert(getPrimesCount(220, 290) == 14);
    assert(getPrimesCount(0, 0) == 0);
    //assert(getPrimesCount(1913, 1913) == 1);

    return 0;
}