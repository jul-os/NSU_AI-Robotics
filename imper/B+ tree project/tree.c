#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tree.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BLOCK_SIZE 4096
#define INITIAL_TREE_SIZE (BLOCK_SIZE * 256) // 1MB

void *tree_memory = NULL;
size_t tree_memory_size = 0;
int tree_fd = -1;

void bptree_init(int t, int data_fd, int log_fd) {
    tree_fd = data_fd;
    tree_memory_size = INITIAL_TREE_SIZE;

    // Проверим, что файл имеет нужный размер. Если нет — увеличим.
    struct stat st;
    if (fstat(tree_fd, &st) == -1) {
        perror("fstat failed");
        return;
    }
    if (st.st_size < tree_memory_size) {
        if (ftruncate(tree_fd, tree_memory_size) == -1) {
            perror("ftruncate failed");
            return;
        }
    }
    tree_memory = mmap(NULL, tree_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, tree_fd, 0);
    if (tree_memory == MAP_FAILED) {
        perror("mmap failed");
        return;
    }

    // Теперь ты можешь обращаться к tree_memory как к массиву байт
    // Например:
    // int* root_block = (int*)(tree_memory + 0);
    // *root_block = 42;
}

Node * create_node(int t, bool is_leaf){
    Node * new_node = (Node*)malloc(sizeof(Node));
    new_node->keys = (int*)malloc((2 * t - 1) * sizeof(int));
    new_node->t = t;
    //каждый узел содержит не более 2t-1 ключей
    //внутренний узел содержит не более 2t дочерних узлов
    new_node->children =  (Node **)malloc(2*t*sizeof(Node*));
    new_node->n = 0;
    new_node->leaf = is_leaf;
    if (new_node->leaf){
        //TODO их потом обновлять при добавлении и удалении
        new_node->prev = new_node;
        new_node->next= new_node;

    }
    else{
        new_node->prev = NULL;
        new_node->next= NULL;
    }
    return new_node;
}

BTree * create_tree(int t){
    BTree * tree = (BTree*)malloc(sizeof(BTree));
    tree->root = create_node(t, true);
    tree->t = t;
    return tree;
}

int find_in_tree(Node* node, int key){
    int i = 0;
    while (i < node->n && key > node->keys[i]){
        i++;
    }
    if (i < node->n && key == node->keys[i]){
        return;
        //TODO значение;
        //переходим в массив памяти по индексу key???
        // получается так
    }
    if (node->leaf){
        printf("не найдено\n");
        return 0;
    }
    return find_in_tree(node->children[i], key);
}

void no_split_insert(Node * node, int key){
    //TODO
}

void insert(BTree * tree, int key, int val){
    //на диске все ячейки пронумерованы
    //пока на тупую
    Node *r = tree->root;
    if (r->n == 2* tree->t - 1){
        //TODO split
        no_split_insert(r, key);
    }
    else{
        no_split_insert(r, key);
    }
}

int main(){
    create_tree(3);
    return 0;
}