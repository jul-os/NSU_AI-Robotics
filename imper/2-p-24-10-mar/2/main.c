#include "modular.h"
#include <assert.h>
#include <stdio.h>

// Предполагается, что функции padd, psub, pmul и pdiv определены в другом файле
extern int pnorm(int num);
extern int padd(int num1, int num2);
extern int psub(int num1, int num2);
extern int pmul(int num1, int num2);
extern int pdiv(int num1, int num2);

int main()
{

    MOD = 2;
    assert(pnorm(0) == 0);
    assert(pnorm(1) == 1);
    assert(pnorm(2) == 0);
    assert(pnorm(3) == 1);
    assert(pnorm(-1) == 1);
    assert(pnorm(-2) == 0);
    assert(padd(0, 0) == 0);
    assert(padd(1, 0) == 1);
    assert(padd(0, 1) == 1);
    assert(padd(1, 1) == 0);
    assert(psub(0, 0) == 0);
    assert(psub(1, 0) == 1);
    assert(psub(0, 1) == 1); // 0 - 1 ≡ 1 (mod 2), т.к. -1 ≡ 1 (mod 2)
    assert(psub(1, 1) == 0);
    assert(pmul(0, 0) == 0);
    assert(pmul(1, 0) == 0);
    assert(pmul(0, 1) == 0);
    assert(pmul(1, 1) == 1);
    assert(pmul(2, 3) == 0); // Любое чётное число умножится и даст 0 (mod 2)
    assert(pmul(3, 3) == 1); // 3 * 3 = 9 ≡ 1 (mod 2)
    assert(pdiv(0, 1) == 0);
    assert(pdiv(1, 1) == 1);

    MOD = 1000000000 - 1;
    assert(pnorm(0) == 0);
    assert(pnorm(5) == 5);
    assert(pnorm(-5) == MOD - 5);
    assert(pnorm(MOD) == 0);
    assert(pnorm(-MOD) == 0);
    assert(pnorm(2 * MOD) == 0);
    assert(pnorm(-2 * MOD) == 0);
    assert(padd(0, 0) == 0);
    assert(padd(1, MOD - 1) == 0);
    assert(psub(10, 5) == 5);
    assert(psub(5, 10) == MOD - 5);
    assert(psub(MOD - 1, 1) == MOD - 2);
    assert(psub(0, 1) == MOD - 1);
    assert(pmul(1, 1) == 1);
    assert(pmul(2, 3) == 6);
    assert(pdiv(0, 5) == 0);

    return 0;
}