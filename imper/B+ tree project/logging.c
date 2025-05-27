#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include "disk.h"
#include "tree.h"
#include <string.h>
#include <unistd.h>

#include <pthread.h>

 static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void wal_log(int log_fd, const char *format, ...)
{
    pthread_mutex_lock(&log_mutex); // захват мьютекса
    va_list args;
    va_start(args, format);

    char buffer[256];
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    if (len < 0)
    {
        perror("Failed to format log message");
        va_end(args);
        pthread_mutex_unlock(&log_mutex);
        return;
    }

    // Атомарная запись в лог
    if (write(log_fd, buffer, len) != len)
    {
        perror("Failed to write to log file");
        pthread_mutex_unlock(&log_mutex);
        va_end(args);
        return;
    }

    // Синхронизируем изменения на диск
    if (fsync(log_fd))
    {
        perror("Failed to sync log file");
    }

    va_end(args);
    pthread_mutex_unlock(&log_mutex);
}

// Восстановление состояния из лога
void recover_from_log(int log_fd, DiskBTree *dbt, BTree *btree)
{
    FILE *log_file = fdopen(dup(log_fd), "r");
    rewind(log_file);

    char command[16];
    int t, key, value, min_key, max_key;

    // Читаем начальное значение t
    if (fscanf(log_file, "%d\n", &t) != 1)
    {
        fclose(log_file);
        return;
    }
    // ранее в 

    // Обрабатываем команды из лога
    while (fscanf(log_file, "%15s", command) == 1)
    {
        if (strcmp(command, "INSERT") == 0)
        {
            if (fscanf(log_file, "%d %d", &key, &value) == 2)
            {
                insert(btree, key, value);
            }
        }
        else if (strcmp(command, "DELETE") == 0)
        {
            if (fscanf(log_file, "%d", &key) == 1)
            {
                delete(key, btree);
            }
        }
    }

    fclose(log_file);
}