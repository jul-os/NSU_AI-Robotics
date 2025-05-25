#include "tree.h"
#include "logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "disk.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
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
        fflush(stderr);
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
