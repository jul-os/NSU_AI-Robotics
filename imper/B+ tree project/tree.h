#pragma once

typedef struct Node
{
    int *keys;
    int t;     // Minimum degree (defines the range for number of keys)
    struct Node **children;
    int n;     // Current number of keys
    bool leaf;
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct BTree {
    Node* root;    // Pointer to root node
    int t;    // Minimum degree
} BTree;

Node * create_node(int t, bool is_leaf);

BTree * create_tree(int t);

//Ключи и значения: 32-битные целые числа (int).
int find_in_tree(Node* node, int key);

void insert(BTree * tree, int key, int val);

void no_split_insert(Node * node, int key);
