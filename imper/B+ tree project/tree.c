#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void *tree_memory = NULL;
size_t tree_memory_size = 0;
int tree_fd = -1;

void bptree_init(int t, int data_fd, int log_fd)
{
    tree_fd = data_fd;
    tree_memory_size = INITIAL_TREE_SIZE;

    // Проверим, что файл имеет нужный размер. Если нет — увеличим.
    struct stat st;
    if (fstat(tree_fd, &st) == -1)
    {
        perror("fstat failed");
        return;
    }
    if (st.st_size < tree_memory_size)
    {
        if (ftruncate(tree_fd, tree_memory_size) == -1)
        {
            perror("ftruncate failed");
            return;
        }
    }
    tree_memory = mmap(NULL, tree_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, tree_fd, 0);
    if (tree_memory == MAP_FAILED)
    {
        perror("mmap failed");
        return;
    }

    // Теперь ты можешь обращаться к tree_memory как к массиву байт
    // Например:
    // int* root_block = (int*)(tree_memory + 0);
    // *root_block = 42;
}

Node *create_node(int t, bool is_leaf)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->keys = (int *)malloc((2 * t - 1) * sizeof(int));
    // Выделяем память для pointers
    if (is_leaf)
    {
        // Для листа - указатели на данные (records)
        new_node->data_pointers = (void **)malloc((2 * t - 1) * sizeof(void *));
        new_node->children = NULL; // Листья не имеют дочерних узлов
    }
    else
    {
        // Для внутреннего узла - указатели на дочерние узлы
        new_node->data_pointers = NULL; // Не используем для данных
        // каждый узел содержит не более 2t-1 ключей
        // внутренний узел содержит не более 2t дочерних узлов
        new_node->children = (Node **)malloc(2 * t * sizeof(Node *));
    }
    new_node->t = t;
    new_node->n = 0;
    new_node->leaf = is_leaf;
    if (new_node->leaf)
    {
        // TODO их потом обновлять при добавлении и удалении
        // TODO points to some data
        new_node->prev = new_node;
        new_node->next = new_node;
    }
    else
    {
        new_node->prev = NULL;
        new_node->next = NULL;
    }
    return new_node;
}

BTree *create_tree(int t)
{
    BTree *tree = (BTree *)malloc(sizeof(BTree));
    tree->root = create_node(t, true);
    tree->t = t;
    return tree;
}

int find_in_tree(Node *node, int key)
{
    int i = 0;
    while (i < node->n && key > node->keys[i])
    {
        i++;
    }
    if (i < node->n && key == node->keys[i])
    {
        return;
        // TODO значение;
    }
    if (node->leaf)
    {
        printf("не найдено\n");
        return 0;
    }
    return find_in_tree(node->children[i], key);
}

void find(int val, BTree *tree)
{
    // start with head node
    Node *C = tree->root;
    while (!C->leaf)
    {
        int i = 0;
        while (i < C->n && val > C->keys[i])
        {
            i++;
        }
        if (i == C->n)
        {
            C = C->children[C->n];
        }
        else if (val == C->keys[i])
        {
            C = C->children[i + 1];
        }
        else
        {
            C = C->children[i];
        }
    }
    for (int i = 0; i < C->n; i++)
    {
        if (C->keys[i] == val)
        {
            // TODO функция которая достает с диска
        }
        // else null
    }
}

// assignment тут короче указатель непонятно на что, когда работу с диском прибавим надо будет посмотреть что здесь должно быть
void insert(BTree *tree, int K, void *P)
{
    // if tree is empty
    if (tree->root == NULL || tree->root->n == 0)
    {
        Node *L = create_node(tree->t, true);
        // leaf L which is also the root
        L->keys[0] = K;
        L->n = 1;
        tree->root = L;
        return;
    }
    // find node in which insert
    Node *L = find_leaf(tree, K);

    // if node has some space
    if (L->n < 2 * tree->t - 1)
    {
        insert_into_leaf(L, K, P);
    }
    else
    {
        // EXPLODE THE NODE
        Node *L_prime = create_node(tree->t, true);
        // temp malloc to store keys and pointers
        int total_keys = L->n + 1;
        int *temp_keys = malloc(total_keys * sizeof(int));
        void **temp_pointers = malloc(total_keys * sizeof(void *));
        // copy keys find place for new key
        int i = 0, j = 0;
        while (i < L->n && K > L->keys[i])
        {
            temp_keys[j] = L->keys[i];
            temp_pointers[j] = L->data_pointers[i];
            i++;
            j++;
        }
        //insert new key
        temp_keys[j] = K;
        temp_pointers[j] = P;
        j++;
        //copy keys that are left
        while(i< L->n){
            temp_keys[j] = L->keys[i];
            temp_pointers[j] = L->data_pointers[i];
            i++;
            j++;
        }
        //find split point
        int split_pos = total_keys/2;
        int K_prime = temp_keys[split_pos];
        //TODO тут где-то запись на диск еще ))))
        // update L and L_prime
        L->n = split_pos;
        for (i = 0; i < split_pos; i++){
            L->keys[i] = temp_keys[i];
            L->data_pointers[i] = temp_pointers[i];
        }
        L_prime->n = total_keys - split_pos;
        for (i = split_pos; i<total_keys; i++){
            L_prime->keys[i - split_pos] = temp_keys[i];
            L_prime->data_pointers[i - split_pos] = temp_pointers[i];
        }
        //update relations between leaves
        L_prime->next = L->next;
        if (L->next != NULL){
            L->next->prev = L_prime;
        }
        L->next = L_prime;
        L_prime->prev = L;
        free(temp_keys);
        free(temp_pointers);

        insert_into_parent(tree, L, K_prime, L_prime);
    }
}

int main()
{
    create_tree(3);
    return 0;
}