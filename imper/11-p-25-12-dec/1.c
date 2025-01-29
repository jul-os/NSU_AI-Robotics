#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TABLE_SIZE 100003 // простое число близкое к максимальному N

typedef struct hash_table_element_in_case_of_collisions
{
    int number;
    struct hash_table_element_in_case_of_collisions *next;
} hash_elem; // типа корзинки с числами с этим хеш ключом

typedef struct hash_table
{
    hash_elem *buckets[TABLE_SIZE];
} hash_table;

int hash(int number)
{
    return abs(number) % TABLE_SIZE;
}

hash_table *create_hash_table()
{
    hash_table *table = (hash_table *)malloc(sizeof(hash_table));
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        table->buckets[i] = NULL;
    }
    return table;
}

bool add_number(hash_table *table, int number)
{
    int index = hash(number);                  // используем функцию хеширования
    hash_elem *bucket = table->buckets[index]; // достаем соответсвующую ей корзинку  числами

    // внутри корзинки с таким хешем ищем точно такое же число
    while (bucket != NULL)
    {
        if (bucket->number == number)
        {
            return false; // Число уже есть, это повтор
        }
        bucket = bucket->next; // это перемещение внутри одной цепочки (корзинка)
    }

    // Если числа нет, добавляем его
    hash_elem *newhash_elem = (hash_elem *)malloc(sizeof(hash_elem));
    newhash_elem->number = number;
    newhash_elem->next = table->buckets[index]; // и как бы в начало его пихаем
    table->buckets[index] = newhash_elem;
    return true;
}

int main()
{
    freopen("input.txt", "rb", stdin);
    freopen("output.txt", "wb", stdout);
    int n;
    fread(&n, sizeof(int), 1, stdin);
    int *n_arr = malloc(n * sizeof(int));
    int count = 0;
    fread(n_arr, sizeof(int), n, stdin);

    hash_table *table = create_hash_table();
    int *only_unique = (int *)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++)
    {
        if (add_number(table, n_arr[i]))
        {
            only_unique[count++] = n_arr[i];
        }
    }
    fwrite(&count, sizeof(int), 1, stdout);
    fwrite(only_unique, sizeof(int), count, stdout);
    free(only_unique);
    free(table);
    free(n_arr);
    fclose(stdin);
    fclose(stdout);
    return 0;
}