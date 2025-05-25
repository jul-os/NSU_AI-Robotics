#define _GNU_SOURCE // Для mremap() с MREMAP_MAYMOVE
#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "disk.h"
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <bits/mman-shared.h>

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

    // Инициализация свободных блоков (блоки с 1 по 255)
    init_free_blocks(dbt, 1, (INITIAL_TREE_SIZE / BLOCK_SIZE) - 1);
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

void init_free_blocks(DiskBTree *dbt, int32_t start_block, int32_t count)
{
    if (count <= 0 || start_block <= 0)
    {
        return;
    }
    // Устанавливаем голову в хэдере
    dbt->header->list_of_free_blocks = start_block;

    for (int i = 0; i < count; i++)
    {
        int32_t current_block = start_block + i;
        int32_t *block_ptr = (int32_t *)((char *)dbt->mmap_ptr + current_block * BLOCK_SIZE);
        *block_ptr = (i == count - 1) ? -1 : (current_block + 1);
    }
    msync(dbt->mmap_ptr + start_block * BLOCK_SIZE, count * BLOCK_SIZE, MS_SYNC);
    msync(dbt->header, BLOCK_SIZE, MS_SYNC);

    // для отладки
    //  Проверяем, что список инициализирован правильно
    int32_t current = dbt->header->list_of_free_blocks;
    int checked_blocks = 0;
    while (current != -1 && checked_blocks < 16)
    {
        int32_t *block_ptr = (int32_t *)((char *)dbt->mmap_ptr + current * BLOCK_SIZE);
        printf("Block %d → next: %d\n", current, *block_ptr);
        current = *block_ptr;
        checked_blocks++;
    }
    assert(checked_blocks == count); // Все блоки должны быть в списке
}

int32_t allocate_block(DiskBTree *dbt)
{
    // Проверить наличие свободных блоков
    if (dbt->header->list_of_free_blocks != -1)
    {
        // Берем номер первого свободного блока
        int32_t free_block = dbt->header->list_of_free_blocks;
        // Получаем указатель на этот блок в mmap области
        int32_t *block_ptr = (int32_t *)((char *)dbt->mmap_ptr + free_block * BLOCK_SIZE);
        // тк в свободных блоках первые 4 байта указывают на следующий свободный блок то вот этим мы усстанвливаем начало списка на следующий
        dbt->header->list_of_free_blocks = *block_ptr;
        msync(dbt->header, BLOCK_SIZE, MS_SYNC);
        return free_block;
    }

    // Если свободных блоков нет расширяем файл

    int32_t new_block = dbt->header->num_blocks++;
    size_t required_size = (new_block + 1) * BLOCK_SIZE;
    // при необходимости расширяем mmap
    if (required_size > dbt->mmap_size)
    {
        size_t new_size = dbt->mmap_size * 2;
        // физическое расширение файла
        ftruncate(dbt->fd, new_size);
        void *new_ptr = mremap(dbt->mmap_ptr, dbt->mmap_size, new_size, new_size, MREMAP_MAYMOVE);
        if (new_ptr == MAP_FAILED)
        {
            perror("Failed to expand file");
            return -1;
        }
        dbt->mmap_ptr = new_ptr;
        dbt->mmap_size = new_size;
        dbt->header = (DiskBTreeHeader *)dbt->mmap_ptr;
    }
    return new_block;
}

void free_block(DiskBTree *dbt, int32_t block_num)
{
    // Валидация номера блока
    if (block_num <= 0 || block_num >= dbt->header->num_blocks) {
        fprintf(stderr, "Invalid block number: %d\n", block_num);
        return;
    }
    // Получаем указатель на блок
    int32_t* block_ptr = (int32_t*)((char*)dbt->mmap_ptr + block_num * BLOCK_SIZE);
    
    // Добавляем блок в начало списка
    *block_ptr = dbt->header->list_of_free_blocks;
    dbt->header->list_of_free_blocks = block_num;
    
    // Синхронизируем изменения
    msync(block_ptr, sizeof(int32_t), MS_SYNC); // Сам блок
    msync(&dbt->header->list_of_free_blocks, sizeof(int32_t), MS_SYNC); // Заголовок
}


/*

// При удалении узла
void delete_node(DiskBTree *dbt, Node *node) {
    if (node->disk_block != -1) {
        free_block(dbt, node->disk_block);
        node->disk_block = -1;
    }
    // ... остальная логика удаления
}

// При создании нового узла
Node* create_node_with_block(DiskBTree *dbt, int t, bool is_leaf) {
    Node *node = create_node(t, is_leaf);
    if (node) {
        node->disk_block = allocate_block(dbt);
        if (node->disk_block == -1) {
            free(node);
            return NULL;
        }
    }
    return node;
}
*/