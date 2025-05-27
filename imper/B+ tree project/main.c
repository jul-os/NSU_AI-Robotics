#include "tree.h"
#include "logging.h"
#include "comcerunt_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "disk.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

void print_node(Node *node, int level)
{
    if (!node)
    {
        printf("%*sNULL\n", level * 4, "");
        return;
    }

    // Выводим отступ в зависимости от уровня
    printf("%*sLevel %d %s | n=%d | Keys: ", level * 4, "", level, (node->leaf ? "Leaf" : "Internal"), node->n);

    // Вывод ключей
    for (int i = 0; i < node->n; i++)
    {
        printf("%d ", node->keys[i]);
    }
    printf("\n");

    if (node->leaf)
    {
        // Для листа выводим значения
        printf("%*sValues: ", level * 4, "");
        for (int i = 0; i < node->n; i++)
        {
            printf("%d ", node->values[i]);
        }
        printf("\n");

        // Вывод соседних листов (prev, next) по ключам (если есть)
        if (node->prev)
        {
            printf("%*sPrev leaf keys: ", level * 4, "");
            for (int i = 0; i < node->prev->n; i++)
            {
                printf("%d ", node->prev->keys[i]);
            }
            printf("\n");
        }
        if (node->next)
        {
            printf("%*sNext leaf keys: ", level * 4, "");
            for (int i = 0; i < node->next->n; i++)
            {
                printf("%d ", node->next->keys[i]);
            }
            printf("\n");
        }
    }
    else
    {
        // Для внутренних узлов рекурсивно выводим детей
        for (int i = 0; i <= node->n; i++)
        {
            print_node(node->children[i], level + 1);
        }
    }
}

void print_btree(BTree *tree)
{
    if (!tree)
    {
        printf("Tree is NULL\n");
        return;
    }
    printf("B+ Tree (order = %d):\n", tree->t);
    print_node(tree->root, 0);
}

int main()
{
    // Открываем файлы
    FILE *input, *output;
    input = fopen("input.txt", "r");
    output = fopen("output.txt", "w");
    // Читаем минимальную степень дерева
    int t;
    if (fscanf(input, "%d", &t) != 1)
    {
        fprintf(stderr, "Failed to read tree order\n");
        return 1;
    }
    int num_threads;
    if (fscanf(input, "%d", &num_threads) != 1)
    {
        fprintf(stderr, "Failed to read tree order\n");
        return 1;
    }
    // читаем названия файлов данных и логов
    char data_file_name[256];
    char log_file_name[256];

    if (fscanf(input, "%255s %255s", data_file_name, log_file_name) != 2)
    {
        fprintf(stderr, "Couldn't read file names\n");
        return 1;
    }
    // так как далее будет использоваться убеждение, что файл новый, и это не противоречит требованиям проекта,
    // то перед началом работы с файлом я его очищаю от того, что могло накопиться во время других тестов

    // Гарантируем новый файл
    unlink(data_file_name); // Игнорируем ошибку если файла нет

    // Открываем файл данных
    int tree_fd = open(data_file_name, O_RDWR | O_CREAT, 0644);
    if (tree_fd == -1)
    {
        perror("Failed to open tree file");
        fclose(input);
        fclose(output);
        return EXIT_FAILURE;
    }

    // Проверяем состояние лог-файла
    struct stat log_stat;
    int log_exists = (stat(log_file_name, &log_stat) == 0) && (log_stat.st_size > 0);

    // Открываем лог-файл
    int log_fd = open(log_file_name, O_RDWR | O_CREAT | (log_exists ? 0 : O_TRUNC), 0644);
    if (log_fd == -1)
    {
        perror("Failed to open log file");
        close(tree_fd);
        fclose(input);
        fclose(output);
        return EXIT_FAILURE;
    }

    // Инициализируем структуры
    DiskBTree *dbt = init_disk(tree_fd, t);
    if (!dbt)
    {
        close(tree_fd);
        fclose(input);
        fclose(output);
        return EXIT_FAILURE;
    }
    BTree *btree = create_tree(t);
    connect_tree_to_disk(btree, dbt);
    struct stat st;
    // Проверяем размер. если файл логов не пуст, значит нам нужно восстанавливать из него
    // Иначе мы в него только записываем
    // Восстанавливаем состояние если лог не пустой
    if (log_exists)
    {
        recover_from_log(log_fd, dbt, btree);
        // После успешного восстановления очищаем лог
        ftruncate(log_fd, 0);
        lseek(log_fd, 0, SEEK_SET);
    }

    // Инициализируем новый лог
    wal_log(log_fd, "%d\n", t);

    // Основной цикл обработки команд
    char command[16];
    int key, value, min_key, max_key;
    while (fscanf(input, "%15s", command) == 1)
    {
        // fprintf(stdout, "%15s ", command);
        if (strcmp(command, "INSERT") == 0)
        {
            if (fscanf(input, "%d %d", &key, &value) == 2)
            {
                wal_log(log_fd, "INSERT %d %d\n", key, value);
                insert(btree, key, value);
            }
        }
        else if (strcmp(command, "DELETE") == 0)
        {
            if (fscanf(input, "%d", &key) == 1)
            {
                wal_log(log_fd, "DELETE %d\n", key);
                delete(key, btree);
            }
        }
        else if (strcmp(command, "FIND") == 0)
        {
            if (fscanf(input, "%d", &key) == 1)
            {
                int search_result;
                if (find(dbt, key, btree, &search_result))
                {
                    fprintf(output, "FOR KEY %d FIND FOUND %d\n", key, search_result);
                }
                else
                {
                    fprintf(output, "FIND FOR KEY %d NOT FOUND\n", key);
                }
            }
        }
        else if (strcmp(command, "RANGE") == 0)
        {
            if (fscanf(input, "%d %d", &min_key, &max_key) == 2)
            {
                range_query(btree, min_key, max_key, dbt, output);
            }
        }
    }

    // для демонстрации работы с потоками
    run_concurrent_test(btree, num_threads);
    print_btree(btree);

    // потоков может быть минимум 1 поэтому что-то с ключом 10 обязательно будет
    int search_result;
    for (int i = 1; i <= 4; i++)
    {
        if (find(dbt, (i + 9), btree, &search_result))
        {
            fprintf(output, "FOR KEY %d FIND FOUND %d\n", (i + 9), search_result);
        }
        else
        {
            fprintf(output, "FIND FOR KEY %d NOT FOUND\n", (i + 9));
        }
    }

    // Перед завершением синхронизируем все изменения
    fsync(tree_fd);
    fsync(log_fd);

    // Закрываем ресурсы
    close(tree_fd);
    close(log_fd);
    fclose(output);
    fclose(input);
    free_tree(btree);
    free_disk(dbt);

    return 0;
}
