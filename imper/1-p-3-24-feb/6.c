#include <stdio.h>
#include <stdlib.h>

#define MAX 1000000
#define MODULE 1000000007

//обратный элемент по модулю
//O(logM)
long long *gcdE(long long num, long long module) { // расширенный алгоритм Евклида
    long long *li = malloc(3 * sizeof(long long)), x, y, x1, y1;

    if (num == 0) { // если число равно нулю, то возвращаем модуль, коэфф. Х и коэфф. Y; ax + by = gcd(a,b),
        // т.к. num1 * x + num2 * y = y в том случае, если y = 1, а левое произведение равно нулю
        li[0] = module;
        li[1] = 0;
        li[2] = 1;

        return li;
    }

    li = gcdE(module % num, num);

    x1 = li[1]; // буфер
    y1 = li[2];

    x = y1 - (module / num) * x1; // коэфф. Х
    y = x1; // коэфф. Y

    li[1] = x;
    li[2] = y;

    return li; // возвращаем делитель и 2 коэффициента
}

long long fact(long long num, long long *array) {
    return array[num];
}

//заранее вычисляет факториалы по модулю
//O(N)
long long *facts(long long range, long long module) {
    long long *res = malloc((range + 1) * sizeof(long long));
    res[0] = 1;
    for (int num = 1; num <= range; num ++) {
        res[num] = (((num * res[num - 1]) % module) + module) % module;
    }
    return res;
}
//ответ на запрос О(1)
long long C(long long n, long long k, long long module, long long *facts) {
    long long up, down;
    if (n == 0) {
        up = 1;
    } else {
        up = fact(n, facts);
    }
    if (k == 0) {
        down = fact(n - k, facts);
    } else {
        down = (((fact(k, facts) * fact(n - k, facts)) % module) + module) % module;
    }
    long long *li = gcdE(down, module);
    long long inverseDown = ((li[1] % module) + module) % module;
    return (((up * inverseDown) % module) + module) % module;
}

int main() {
    //в итоге О(N*log M)
    FILE *in = fopen("input.txt", "r");
    FILE *out = fopen("output.txt", "w");

    long long quantity, module = MODULE, num1, num2, *facts_ = facts(MAX, module);

    fscanf(in, "%lld", &quantity);

    for (int i = 0; i < quantity; i++) {
        fscanf(in, "%lld %lld", &num1, &num2);
        fprintf(out, "%lld\n", C(num1, num2, module, facts_));
    }
    free(facts_);
    return 0;
}