#pragma once
typedef struct DiskBTree DiskBTree;
typedef struct BTree BTree;

// Функция для записи в лог
void wal_log(int log_fd, const char *format, ...);
// Восстановление состояния из лога
void recover_from_log(int log_fd, DiskBTree *dbt, BTree *btree);