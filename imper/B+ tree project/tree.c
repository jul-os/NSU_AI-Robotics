#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

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

Node * find(int val, BTree *tree)
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
            return C;
        }
        // else null
    }
}

void insert_into_leaf(Node *L, int K, void *P)
{
    // find where to insert
    int insert_pos = 0;
    while (insert_pos < L->n && K > L->keys[insert_pos])
    {
        insert_pos++;
    }
    // move other elements to the right
    // пояснение: мы в insert уже проверили что лист не будет переполнен
    for (int i = L->n; i > insert_pos; i--)
    {
        L->keys[i] = L->keys[i - 1];
        L->data_pointers[i] = L->data_pointers[i - 1];
    }
    // insert the new key and pointer
    L->keys[insert_pos] = K;
    L->data_pointers[insert_pos] = P;
    L->n++;
    // оказалось что вообще-то указатель из родителя в лист не обязан указывать на первый элемент листа поэтому типа все в этой функции
}

void insert_into_parent(BTree *tree, Node *N, int K_prime, Node *N_prime)
{
    if (tree->root == N)
    {
        Node *new_root = create_node(tree->t, false);
        new_root->keys[0] = K_prime;
        new_root->children[0] = N;
        new_root->children[1] = N_prime;
        new_root->n = 1;
        tree->root = new_root;
        return;
    }
    Node *parent = find_parent(N); // TODO
    if (parent->n < 2 * tree->t - 1)
    {
        int insert_pos = 0;
        while (insert_pos <= parent->n && parent->children[insert_pos] != N)
        {
            insert_pos++;
        }
        insert_pos++; // inserting after N
        // move to the right
        for (int i = parent->n; i >= insert_pos; i--)
        {
            parent->keys[i] = parent->keys[i - 1];
        }
        for (int i = parent->n + 1; i > insert_pos; i--)
        {
            parent->children[i] = parent->children[i - 1];
        }
        // insert K_prime & N_prime
        parent->keys[insert_pos - 1] = K_prime;
        parent->children[insert_pos] = N_prime;
        parent->n++;
    }
    // else parent doesnt have enough dpace
    else
    {
        int total_keys = parent->n + 1;
        int *temp_keys = malloc(total_keys * sizeof(int));
        Node **temp_pointers = malloc((total_keys + 1) * sizeof(Node *));

        // Копируем существующие данные во временный массив
        int i = 0, j = 0;
        while (i <= parent->n && parent->children[i] != N)
        {
            temp_pointers[j] = parent->children[i];
            if (i < parent->n)
            {
                temp_keys[j] = parent->keys[i];
            }
            i++;
            j++;
        }
        // Вставляем K_prime и N_prime после N
        temp_pointers[j] = N;
        temp_keys[j] = K_prime;
        j++;
        temp_pointers[j] = N_prime;
        i++;

        // Копируем оставшиеся элементы
        while (i <= parent->n)
        {
            temp_pointers[j] = parent->children[i];
            if (i < parent->n)
            {
                temp_keys[j] = parent->keys[i];
            }
            i++;
            j++;
        }

        // Определяем точку разделения
        int split_pos = total_keys / 2;
        int K_double_prime = temp_keys[split_pos];

        Node *P_prime = create_node(tree->t, false);
        parent->n = 0;
        for (i = 0; i < split_pos; i++)
        {
            parent->children[i] = temp_pointers[i];
            parent->keys[i] = temp_keys[i];
            parent->n++;
        }
        parent->children[i] = temp_pointers[i];

        P_prime->n = 0;
        for (i = split_pos + 1, j = 0; i < total_keys; i++, j++)
        {
            P_prime->children[j] = temp_pointers[i];
            P_prime->keys[j] = temp_keys[i];
            P_prime->n++;
        }
        P_prime->children[j] = temp_pointers[i];
        free(temp_keys);
        free(temp_pointers);

        insert_into_parent(tree, parent, K_double_prime, P_prime);
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
    Node *L = find_leaf(tree, K); // TODO

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
        // fixme void pointers or Node pointers????
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
        // insert new key
        temp_keys[j] = K;
        temp_pointers[j] = P;
        j++;
        // copy keys that are left
        while (i < L->n)
        {
            temp_keys[j] = L->keys[i];
            temp_pointers[j] = L->data_pointers[i];
            i++;
            j++;
        }
        // find split point
        int split_pos = total_keys / 2;
        int K_prime = temp_keys[split_pos];
        // TODO тут где-то запись на диск еще ))))
        //  update L and L_prime
        L->n = split_pos;
        for (i = 0; i < split_pos; i++)
        {
            L->keys[i] = temp_keys[i];
            L->data_pointers[i] = temp_pointers[i];
        }
        L_prime->n = total_keys - split_pos;
        for (i = split_pos; i < total_keys; i++)
        {
            L_prime->keys[i - split_pos] = temp_keys[i];
            L_prime->data_pointers[i - split_pos] = temp_pointers[i];
        }
        // update relations between leaves
        L_prime->next = L->next;
        if (L->next != NULL)
        {
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