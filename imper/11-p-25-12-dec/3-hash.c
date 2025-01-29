#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define TABLE_SIZE 100003

typedef struct hash_elem {
    uint64_t state;     
    uint64_t index;      
    struct hash_elem *next;
} hash_elem;

typedef struct hash_table {
    hash_elem *buckets[TABLE_SIZE];
} hash_table;


uint64_t hash(uint64_t state) {
    return state % TABLE_SIZE;
}


hash_table *create_hash_table() {
    hash_table *table = (hash_table *)malloc(sizeof(hash_table));
    for (uint64_t i = 0; i < TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    return table;
}


bool add_to_table(hash_table *table, uint64_t state, uint64_t index, uint64_t *found_index) {
    uint64_t h = hash(state);
    hash_elem *current = table->buckets[h];
    while (current != NULL) {
        if (current->state == state) {
            *found_index = current->index;
            return true;
        }
        current = current->next;
    }

    hash_elem *new_elem = (hash_elem *)malloc(sizeof(hash_elem));
    new_elem->state = state;
    new_elem->index = index;
    new_elem->next = table->buckets[h];
    table->buckets[h] = new_elem;
    return false;
}

uint64_t func(uint64_t s, uint64_t a, uint64_t b, uint64_t c, uint64_t M) {
    return (s * s * a + s * b + c) % M;
}

int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    uint64_t M, a, b, c;
    scanf("%lu", &M);
    scanf("%lu %lu %lu", &a, &b, &c);

    hash_table *seen = create_hash_table();
    uint64_t state = 1, index = 0, found_index = 0;

    // Шаг 1: Поиск первого повторения
    while (!add_to_table(seen, state, index, &found_index)) {
        state = func(state, a, b, c, M);
        index++;
    }

    // Шаг 2: Найден цикл, выводим начало и конец
    uint64_t l = found_index;
    uint64_t r = index;

    printf("%lu %lu\n", l, r);

    for (uint64_t i = 0; i < TABLE_SIZE; i++) {
        hash_elem *current = seen->buckets[i];
        while (current != NULL) {
            hash_elem *tmp = current;
            current = current->next;
            free(tmp);
        }
    }
    free(seen);
    fclose(stdin);
    fclose(stdout);

    return 0;
}
