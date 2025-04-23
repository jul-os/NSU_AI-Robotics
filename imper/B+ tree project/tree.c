#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tree.h"



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