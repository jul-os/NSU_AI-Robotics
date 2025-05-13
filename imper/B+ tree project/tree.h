#pragma once
#include <stdbool.h>
#include <stdint.h>

#define BLOCK_SIZE 4096
#define INITIAL_TREE_SIZE (BLOCK_SIZE * 256) // 1MB
/*
// Структура для заголовка файла
typedef struct
{
    int32_t t;           // Порядок дерева
    int32_t root_block;  // Номер блока с корневым узлом
    int32_t block_count; // Общее количество блоков
    char padding[BLOCK_SIZE - 3 * sizeof(int32_t)];
} TreeHeader;

// Структура узла для дискового хранения (ровно 4096 байт)
typedef struct
{
    int32_t is_leaf;  // 1 для листа, 0 для внутреннего узла
    int32_t num_keys; // Количество ключей в узле

    // Объединение для разных типов узлов
    union
    {
        struct
        {
            int32_t keys[2 * t - 1]; // Ключи
            int32_t children[2 * t]; // Индексы дочерних блоков
        } internal;
        struct
        {
            int32_t keys[2 * t - 1];   // Ключи
            int32_t values[2 * t - 1]; // Значения
            int32_t next_leaf;         // Индекс следующего листа
            int32_t prev_leaf;         // Индекс предыдущего листа
        } leaf;
    } data;

    char padding[BLOCK_SIZE - 2 * sizeof(int32_t) - ...]; // Дополнение до 4096
} DiskNode;

typedef struct BTree
{
    int t;              // Порядок дерева
    int root_block;     // Индекс корневого узла
    void* mmap_ptr;     // Указатель на начало mmap-области
    size_t mmap_size;   // Раздел mmap-области
    int fd;             // Файловый дескриптор
} BTree;
*/

typedef struct Node
{
    int *keys;
    int t; // Minimum degree (defines the range for number of keys)
    struct Node **children;
    int n; // Current number of keys
    bool leaf;
    void **data_pointers; // points to data i guess
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct BTree
{
    int t;
    Node *root;
} BTree;

Node *create_node(int t, bool is_leaf);

BTree *create_tree(int t);

// Ключи и значения: 32-битные целые числа (int).
void * find(int val, BTree *tree);
Node* find_leaf(int val, BTree *tree);
Node *find_parent(BTree *tree, Node *child);
int find_child_index(Node *parent, Node *child);

void insert(BTree *tree, int K, void *P);
void insert_into_leaf(Node *L, int K, void *P);
void insert_into_parent(BTree *tree, Node *N, int K_prime, Node *N_prime);

void delete(int key, void * point, BTree* tree);
void delete_entry(Node *N, int K, void *P, BTree *tree);
void remove_key_and_pointer(Node *N, int K);

void coalesce_nodes(Node *N, Node * N_prime, Node *parent, int K_prime, BTree * tree);
void redistribute_nodes(Node * N, Node* N_prime, Node* parent, int K_prime, int N_index);
