#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "disk.h"
#include <sys/mman.h>
#include <sys/stat.h>

DiskBTree *init_disk(int fd, int t)
{
    // Установить размер файла
    if (ftruncate(fd, INITIAL_TREE_SIZE) == -1)
    {
        perror("ftruncate failed");
        close(fd);
        return NULL;
    }

    // Отобразить файл в память
    void *mmap_ptr = mmap(NULL, INITIAL_TREE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmap_ptr == MAP_FAILED)
    {
        perror("mmap failed");
                close(fd);
        return NULL;
    }

    // Инициализировать структуру управления
    DiskBTree *dbt = malloc(sizeof(DiskBTree));
    dbt->fd = fd;
    dbt->mmap_ptr = mmap_ptr;
    dbt->mmap_size = INITIAL_TREE_SIZE;

    // Инициализировать и заполнить заголовок
    dbt->header = (DiskBTreeHeader *)mmap_ptr;
    *dbt->header = (DiskBTreeHeader){
        .t = t,
        .root_block = -1,
        .list_of_free_blocks = -1,
        .num_blocks = 1};

    // Занулить резервную область
    memset(dbt->header->reserved, 0, sizeof(dbt->header->reserved));

    // Принудительная запись на диск
    if (msync(dbt->header, BLOCK_SIZE, MS_SYNC) == -1)
    {
        perror("msync failed");
        munmap(mmap_ptr, INITIAL_TREE_SIZE);
        close(fd);
        free(dbt);
        return NULL;
    }

    return dbt;
}