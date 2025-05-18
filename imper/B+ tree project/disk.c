#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "disk.h"
#include <sys/mman.h>
#include <sys/stat.h>

//task спросить какой вариант памяти лучше выбрать


void init_empty_tree(int fd, int t) {
    // 1. Установить размер файла
    ftruncate(fd, BLOCK_SIZE);
    
    // 2. Отобразить файл в память
    void* ptr = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    
    // 3. Инициализировать заголовок
    DiskBTreeHeader* header = (DiskBTreeHeader*)ptr;
    *header = (DiskBTreeHeader){
        .t = t,
        .root_block = -1,
        .list_of_free_blocks = -1,
        .num_blocks = 1
    };
    memset(header->reserved, 0, sizeof(header->reserved));
    
    // 4. Убрать маппинг
    munmap(ptr, BLOCK_SIZE);
}