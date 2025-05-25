#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct DiskBTree DiskBTree;

// Структура узла B+ дерева
typedef struct Node
{
    int *keys;              // Ключи
    struct Node **children; // Для внутренних узлов: казатели на потомков
    int n;                  // Текущее количество ключей
    bool leaf;              // Флаг листа
    int *values;  // ДЛя листов: значения
    struct Node *prev;      // ДЛя листов: указатель на соседние листы
    struct Node *next;
    int32_t disk_block; // Связанный с этим листом блок на дисково пространстве
} Node;

// Структура B+ дерева
typedef struct BTree
{
    int t;                // Порядок дерева (min degree)
    Node *root;           // Корень
    DiskBTree *disk_tree; // Ссылка на дисковое представление
} BTree;

// Создать узел
Node *create_node(int t, bool is_leaf, BTree* tree);
// Создать дерево
BTree *create_tree(int t);

// Поиск в дереве по ключу
bool find(DiskBTree *dbt, int search_key, BTree *tree, int *out_value);
// Поиск узла-листа в дереве по ключу
Node *find_leaf(int search_key, BTree *tree);
// Поиск узла-родителя в дереве по потомку
Node *find_parent(BTree *tree, Node *child);
// Поиск индекса (позиции) узла child среди дочерних узлов его родителя parent
int find_child_index(Node *parent, Node *child);
// Диапазонный запрос
void range_query(BTree *tree, int min_k, int max_k, DiskBTree *dbt, FILE *output);

// Вставка в дерево
void insert(BTree *tree, int insert_key,int value);
// Вставка в лист
void insert_into_leaf(BTree *tree, Node *L, int insert_key, int value);
// Обновление родителя
void insert_into_parent(BTree *tree, Node *N, int K_prime, Node *N_prime);

// Удаление - вспомогательная функция\функция-вызов
void delete(int delete_key, BTree *tree);
// Удаление
void delete_entry(Node *N, int delete_key,  BTree *tree);
// Удаление ключа и указателя из узла
void remove_key_and_value(BTree* tree, Node *N, int delete_key);

// Объединить два узла
void coalesce_nodes(Node *N, Node *N_prime, Node *parent, int K_prime, BTree *tree);
// Перераспределение узлов при заимствовании
void redistribute_nodes(Node *N, Node *N_prime, Node *parent, int K_prime, int N_index, BTree* tree);

//Освобождение узла
void free_node(Node *node);
// Рекурсивная функция для освобождения всех узлов
void free_subtree(Node *node);
// Освобождение всего B+ дерева 
void free_tree(BTree *tree);

Node* find_leaf_to_insert(BTree *tree, int key) ;