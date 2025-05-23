#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct DiskBTree DiskBTree;

typedef struct Node
{
    int * keys;
    int t; // Minimum degree (defines the range for number of keys)
    struct Node **children;
    int n; // Current number of keys
    bool leaf;
    void **data_pointers; // points to data i guess
    struct Node *prev;
    struct Node *next;
    int32_t disk_block; 
} Node;

typedef struct BTree
{
    int t;
    Node *root;
    DiskBTree *disk_tree;  // Ссылка на дисковое представление
} BTree;

Node *create_node(int t, bool is_leaf);

BTree *create_tree(int t);

// Ключи и значения: 32-битные целые числа (int).
void *find(int val, BTree *tree);
Node *find_leaf(int val, BTree *tree);
Node *find_parent(BTree *tree, Node *child);
int find_child_index(Node *parent, Node *child);

void insert(BTree *tree, int K, void *P);
void insert_into_leaf(Node *L, int K, void *P);
void insert_into_parent(BTree *tree, Node *N, int K_prime, Node *N_prime);

void delete(int key, void *point, BTree *tree);
void delete_entry(Node *N, int K, void *P, BTree *tree);
void remove_key_and_pointer(Node *N, int K);

void coalesce_nodes(Node *N, Node *N_prime, Node *parent, int K_prime, BTree *tree);
void redistribute_nodes(Node *N, Node *N_prime, Node *parent, int K_prime, int N_index);
