#pragma once

#include <stdint.h>

#define BLOCK_SIZE 4096
#define INITIAL_TREE_SIZE (BLOCK_SIZE * 256) // 1MB

#pragma pack(push, 1) 
typedef struct DiskBTreeHeader {
    int32_t t;            // Порядок дерева (min degree)
    int32_t root_block;   // Смещение корня (в блоках, не в байтах!)
    int32_t list_of_free_blocks;    // Голова списка свободных блоков (-1 если нет)
    int32_t num_blocks;   // Общее количество блоков в файле
    uint8_t reserved[4076]; // Резерв (выравнивание до 4096 байт)
} DiskBTreeHeader;
#pragma pack(pop)