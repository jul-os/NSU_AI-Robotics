#include <stdio.h>
#include <stdint.h>

__uint64_t func(__uint64_t s, __uint64_t a, __uint64_t b, __uint64_t c, __uint64_t M) {
    return (s * s * a + s * b + c) % M;
}

// Алгоритм Флойда для поиска цикла
void find_cycle(__uint64_t a, __uint64_t b, __uint64_t c, __uint64_t M, __uint64_t *l, __uint64_t *r) {
    __uint64_t slow = 1, fast = 1;

    // Шаг 1: Найти встречу внутри цикла
    do {
        slow = func(slow, a, b, c, M);
        fast = func(func(fast, a, b, c, M), a, b, c, M);
    } while (slow != fast); //do-while чтобы хотя бы один шаг прошел
    //если они равны то мы находимся внутри цикла
    //потому что если есть повторения то заяц как-то так допрыгает что они с черепахой будут равны

    // Шаг 2: Найти начало цикла (l)
    __uint64_t start = 1, cycle_start = slow;
    *l = 0;
    while (start != cycle_start) {
        start = func(start, a, b, c, M);
        cycle_start = func(cycle_start, a, b, c, M);
        //ищем индекс на ктором начинается повторение, сравнивая элементы с самого начала с черепахой
        (*l)++;
    }

    // Шаг 3: Найти длину цикла (r - l)
    *r = *l + 1;
    slow = func(start, a, b, c, M);
    while (slow != start) {
        slow = func(slow, a, b, c, M);
        (*r)++;
    }
}

int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    __uint64_t M, a, b, c;
    scanf("%lu", &M);
    scanf("%lu %lu %lu", &a, &b, &c);

    __uint64_t l, r;
    find_cycle(a, b, c, M, &l, &r);

    printf("%lu %lu\n", l, r);

    fclose(stdin);
    fclose(stdout);
    return 0;
}
