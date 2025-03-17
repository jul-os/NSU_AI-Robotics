#include "modular.h"

int MOD = 10; //ьез этого не хочет

int pnorm(int num)
{
    return (num % MOD + MOD) % MOD;
}
//Эта реализация гарантирует, что результат всегда будет неотрицательным

int power(int num, int power) {
    long long ans = 1;
    long long longNum = (long long) num;
    while (power > 0) {
        if (power % 2 != 0) {
            ans *= longNum;
            ans %= MOD;
        }
        longNum *= longNum;
        longNum %= MOD;
        power /= 2;
    }
    return (int) ans;
}

int padd(int num1, int num2) {
    return (num1 + num2) % MOD;
}

int psub(int num1, int num2) {
    return pnorm(num1 - num2);
}

int pdiv(int num1, int num2) {
    //// x ^ (−1) mod(m) = x ^ (m − 2) mod(m)
    return pmul(num1, pnorm(power(num2, MOD - 2)));
}

int pmul(int num1, int num2) {
    return pnorm((int) (((long long) num1 * num2) % MOD));
}
