#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "primes.h"


int main()
{
    assert(isPrime(1) == 0);
    assert(isPrime(0) == 0);
    assert(isPrime(2) == 1);
    assert(isPrime(3) == 1);
    assert(isPrime(10) == 0);
    assert(isPrime(99) == 0);
    assert(isPrime(131) == 1);
    assert(isPrime(1151) == 1);
    assert(isPrime(9999973) == 1);
    assert(isPrime(10000000) == 0);
    assert(isPrime(9999937) == 1);
    assert(isPrime(9999749) == 1);
    assert(isPrime(9999751) == 0);
    

    assert(findNextPrime(0) == 2);
    assert(findNextPrime(1) == 2);
    assert(findNextPrime(2) == 2);
    assert(findNextPrime(3) == 3);
    assert(findNextPrime(4) == 5);
    assert(findNextPrime(20) == 23);
    assert(findNextPrime(1000) == 1009);
    assert(findNextPrime(9999739) == 9999739);
    assert(findNextPrime(9999730) == 9999739);
    assert(findNextPrime(9999971) == 9999971);
    assert(findNextPrime(9999972) == 9999973);


    assert(getPrimesCount(0, 1) == 0);
    assert(getPrimesCount(0, 10) == 4);
    assert(getPrimesCount(10, 10) == 0);
    assert(getPrimesCount(7, 7) == 0);
    assert(getPrimesCount(10, 20) == 4);
    assert(getPrimesCount(220, 290) == 14);
    assert(getPrimesCount(0, 0) == 0);
    assert(getPrimesCount(1913, 1913) == 0); //tочка не входит
    //printf("%d\n", getPrimesCount(9999900, 10000000));
    assert(getPrimesCount(9999900, 10000000) == 9);
    assert(getPrimesCount(9999739, 10000000) == 17);
    assert(getPrimesCount(10000000, 10000000) == 0);
    assert(getPrimesCount(99999991, 10000000) == 0);

    return 0;
}