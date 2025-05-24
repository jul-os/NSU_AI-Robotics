#include <stdio.h>
#include <stdint.h>
#define BLOCK_SIZE 4096
#define INITIAL_TREE_SIZE (BLOCK_SIZE * 256) // 1MB
#define T_MAX 32                             // Максимальный порядок дерева
#define X (BLOCK_SIZE - (1 + 4 + (2 * T_MAX - 1) * 4 + (2 * T_MAX) * 4))

#pragma pack(push, 1)
typedef struct
{
    uint8_t is_leaf;             // Флаг листа 1 байт
    int32_t num_keys;            // Количество ключей 4 байта
    int32_t keys[2 * T_MAX - 1]; // Ключи (2*32-1)*4 = 252 байта

    union
    {
        int32_t children[2 * T_MAX]; // Для внутренних узлов: указатели на блоки. (2*32)*4 = 256 байт
        struct
        {
            int32_t values[2 * T_MAX - 1]; // Для листьев: значения
            int32_t next_leaf;             // Связь с соседним листом
            int32_t prev_leaf;
        };
    };

    uint8_t reserved[3579]; // Выравнивание до BLOCK_SIZE
} DiskNode;
#pragma pack(pop)

int main()
{
    printf("Sizeof DiskNode: %zu bytes\n", sizeof(DiskNode));
    return 0;
}