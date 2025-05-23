#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "disk.h"
#include <sys/mman.h>
#include <sys/stat.h>

int main()
{
    //Открываем файлы
    FILE *input, *output;
    input = fopen("input.txt", "r");
    output = fopen("output.txt", "w");

    //Читаем минимальную степень дерева
    int t;
    if (scanf("%d", &t) != 1)
    {
        fprintf(stderr, "Failed to read tree order\n");
        return 1;
    }

    //читаем названия файлов данных и логов
    char data_file_name[256];
    char log_file_name[256];

    if (scanf("%255s %255s", data_file_name, log_file_name) != 2)
    {
        fprintf(stderr, "Couldn't read file names\n");
        return 1;
    }

    //так как далее будет использоваться убеждение, что файл новый, и это не противоречит требованиям проекта, 
    //то перед началом работы с файлом я его очищаю от того, что могло накопиться во время других тестов

    // Гарантируем новый файл
    unlink(data_file_name); // Игнорируем ошибку если файла нет

    //Открываем файл данных
    int tree_fd = open (data_file_name, O_RDWR | O_CREAT, 0644);
    if (tree_fd == -1) {
        perror("Failed to open tree file");
        fclose(input);
        fclose(output);
        return EXIT_FAILURE;
    }

    DiskBTree* dbt = init_disk(tree_fd, t);
    if (!dbt) {
        close(tree_fd);
        fclose(input);
        fclose(output);
        return EXIT_FAILURE;
    }
    BTree * btree = create_tree(t);
    btree->disk_tree = dbt;

    // Проверяем размер файла для определения, нужно ли инициализировать
    struct stat st;
    if (fstat(tree_fd, &st) == -1) {
        perror("Failed to get file stats");
        close(tree_fd);
        fclose(output);
        fclose(input);
        return EXIT_FAILURE;
    }

    // Если файл новый, инициализируем заголовок
    if (st.st_size == 0) {
        init_empty_tree(data_file_name, t);
        // запись в сам файл то есть mmap будет происходить в файле disk.c тоже
    }


    // Основной цикл обработки команд
    char command[16];
    int key, value, min_key, max_key;
    //fixme как доделаю тут тоже сделать норм
    while (fscanf(input, "%15s", command) == 1) {
        if (strcmp(command, "INSERT") == 0) {
            if (fscanf(input, "%d %d", &key, &value) == 2) {
                // Реализация вставки
                fprintf(output, "INSERT %d %d\n", key, value);
            }
        } else if (strcmp(command, "DELETE") == 0) {
            if (fscanf(input, "%d", &key) == 1) {
                // Реализация удаления
                fprintf(output, "DELETE %d\n", key);
            }
        } else if (strcmp(command, "SEARCH") == 0) {
            if (fscanf(input, "%d", &key) == 1) {
                // Реализация поиска
                fprintf(output, "SEARCH %d\n", key);
            }
        } else if (strcmp(command, "RANGE") == 0) {
            if (fscanf(input, "%d %d", &min_key, &max_key) == 2) {
                // Реализация диапазонного запроса
                fprintf(output, "RANGE %d %d\n", min_key, max_key);
            }
        }
    }
    // bptree_init(t, fileno(data_file), fileno(log_file));
    //  bptree_recover_from_wal();  // если нужно

    // тут можно будет читать операции и вызывать insert/search/delete из tree.c

    return 0;
}
