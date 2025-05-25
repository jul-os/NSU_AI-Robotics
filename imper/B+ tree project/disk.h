#pragma once

#include <stdint.h>

#define BLOCK_SIZE 4096
#define INITIAL_TREE_SIZE (BLOCK_SIZE * 256) // 1MB
#define T_MAX 32                             // Максимальный порядок дерева

typedef struct BTree BTree;
typedef struct Node Node;

#pragma pack(push, 1)
// Структура описания заголовка файла дерева
typedef struct DiskBTreeHeader
{
    int32_t t;                   // Порядок дерева (min degree)
    int32_t root_block;          // Смещение корня в блоках
    int32_t list_of_free_blocks; // Голова списка свободных блоков (-1 если нет)
    int32_t num_blocks;          // Общее количество блоков в файле
    uint8_t reserved[4080];      // Резерв (выравнивание до 4096 байт)
} DiskBTreeHeader;
#pragma pack(pop)

// Структура дерева на диске. Контроллирует диск
typedef struct DiskBTree
{
    int fd;                  // Файловый дескриптор
    void *mmap_ptr;          // Указатель на mmap-область
    size_t mmap_size;        // Текущий размер отображения
    DiskBTreeHeader *header; // Указатель на заголовок
} DiskBTree;

// Структура узла на диске
#pragma pack(push, 1)
typedef struct
{
    uint8_t is_leaf;             // Флаг листа. 1 байт
    int32_t num_keys;            // Количество ключей. 4 байта
    int32_t keys[2 * T_MAX - 1]; // Ключи. (2*32-1)*4 = 252 байта

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

// инициализирует структуры для начала работы с диском
DiskBTree *init_disk(int fd, int t);

// Подключает дерево к дисковому хранилищу
void connect_tree_to_disk(BTree* tree, DiskBTree* dbt);

// Выделяет новый блок (либо из свободных, либо расширяет файл)
int32_t allocate_block(DiskBTree *dbt);

// Удаляет содержимое и озвращает блок в список свободных
void free_block(DiskBTree *dbt, int32_t block_num);

// Инициализирует список свободных блоков связывая их в односвязный список
void init_free_blocks(DiskBTree *dbt, int32_t start_block, int32_t count);

// Записывает узел на диск
void save_node_to_disk(DiskBTree *dbt, Node *node);

// Достает информацию из памяти
Node *load_node_from_disk(DiskBTree *dbt, int32_t block_num);

// Возвращает значение 
int get_value_from_disk(DiskBTree *dbt, Node *leaf, int index);

// Освобождение дискового представления дерева
void free_disk(DiskBTree *dbt);