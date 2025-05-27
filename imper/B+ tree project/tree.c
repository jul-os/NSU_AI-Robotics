#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "disk.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

void *tree_memory = NULL;
size_t tree_memory_size = 0;
int tree_fd = -1;

Node *create_node(int t, bool is_leaf, BTree *tree, bool tree_or, DiskBTree *disk)
{
    // Создаем новый узел
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node)
    {
        perror("malloc failed on node");
        ;
        return NULL;
    }
    // Инициализируем ключи
    new_node->keys = (int *)malloc((2 * t - 1) * sizeof(int));
    if (!new_node->keys)
    {
        perror("malloc failed on node keys");
        free(new_node);
        return NULL;
    }
    // Присваиваем значения прочим
    new_node->n = 0;
    new_node->leaf = is_leaf;
    new_node->disk_block = -1;
    pthread_rwlock_init(&new_node->lock, NULL);

    // Выделяем память для pointers
    if (is_leaf)
    {
        // Для листа - указатели на данные
        new_node->values = (int *)malloc((2 * t - 1) * sizeof(int));
        if (!new_node->values)
        {
            perror("malloc failed on node data pointers");
            free(new_node->keys);
            free(new_node);
            return NULL;
        }
        memset(new_node->values, 0, (2 * t - 1) * sizeof(int));
        new_node->children = NULL; // Листья не имеют дочерних узлов
        new_node->prev = NULL;
        new_node->next = NULL;
    }
    else
    {
        // Для внутреннего узла - указатели на дочерние узлы
        new_node->children = (Node **)malloc(2 * t * sizeof(Node *));
        if (!new_node->children)
        {
            perror("malloc failed on node children pointers");
            free(new_node->keys);
            free(new_node);
            return NULL;
        }
        memset(new_node->children, 0, (2 * t) * sizeof(Node *));
        new_node->values = NULL; // Не используем для данных
        new_node->prev = NULL;
        new_node->next = NULL;
    }
    if (tree_or && disk)
    {

        new_node->disk_block = allocate_block(disk);
        if (new_node->disk_block == -1)
        {
            fprintf(stderr, "Failed to allocate disk block for new node\n");
            // Освобождаем уже выделенную память
            if (is_leaf)
            {
                free(new_node->values);
            }
            else
            {
                free(new_node->children);
            }
            free(new_node->keys);
            free(new_node);
            return NULL;
        }

        // Сохраняем пустой узел на диск
        save_node_to_disk(disk, new_node);

        return new_node;
    }
    // Если дерево связано с диском, сразу выделяем блок
    else if (tree->disk_tree)
    {
        new_node->disk_block = allocate_block(tree->disk_tree);
        if (new_node->disk_block == -1)
        {
            fprintf(stderr, "Failed to allocate disk block for new node\n");
            // Освобождаем уже выделенную память
            if (is_leaf)
            {
                free(new_node->values);
            }
            else
            {
                free(new_node->children);
            }
            free(new_node->keys);
            free(new_node);
            return NULL;
        }

        // Сохраняем пустой узел на диск
        save_node_to_disk(tree->disk_tree, new_node);

        return new_node;
    }
}

BTree *create_tree(int t)
{
    // Создаем дерево и заполняем его
    BTree *tree = (BTree *)malloc(sizeof(BTree));
    /// tree->root = create_node(t, true, tree, false, NULL);
    tree->root = NULL;
    pthread_rwlock_init(&tree->lock, NULL);
    tree->t = t;
    tree->disk_tree = NULL;
    return tree;
}

bool find(DiskBTree *dbt, int search_key, BTree *tree, int *out_value)
{
    // Начинаем с корня, ищем нужный N
    Node *N = tree->root;
    // Блокируем узел в котором стоим те корень
    pthread_rwlock_rdlock(&N->lock);
    while (!N->leaf)
    {
        int i = 0;
        while (i < N->n && search_key >= N->keys[i])
        {
            i++;
        }

        Node *next = N->children[i];

        pthread_rwlock_rdlock(&next->lock);
        pthread_rwlock_unlock(&N->lock);
        N = next;
    }

    // Теперь N это лист, и он заблокирован, ищем в нем ключ
    for (int i = 0; i < N->n; i++)
    {
        if (N->keys[i] == search_key)
        {
            *out_value = get_value_from_disk(dbt, N, i);
            pthread_rwlock_unlock(&N->lock); // разблокировываем
            return true;
        }
    }
    pthread_rwlock_unlock(&N->lock); // разблокировываем
    return false;
}

Node *find_leaf(int search_key, BTree *tree)
{
    // Начинаем с корня, ищем нужный N
    Node *N = tree->root;
    // Блокируем узел в котором стоим те корень
    pthread_rwlock_rdlock(&N->lock);
    while (!N->leaf)
    {
        int i = 0;
        while (i < N->n && search_key >= N->keys[i])
        {
            i++;
        }
        Node *next = N->children[i];

        pthread_rwlock_rdlock(&next->lock); // блокируем ребёнка
        pthread_rwlock_unlock(&N->lock);    // отпускаем родителя
        N = next;
    }

    // В этой функции освободить найденный узел должна будет та функция которая вызвала find_leaf !
    return N;
}
void range_query(BTree *tree, int min_k, int max_k, DiskBTree *dbt, FILE *output)
{
    if (!tree || !tree->root || !output)
    {
        fprintf(stderr, "Invalid arguments to range_query\n");
        return;
    }
    Node *start_node = find_leaf(min_k, tree);
    if (!start_node)
    {
        fprintf(output, "RANGE %d %d: NO_RESULTS\n", min_k, max_k);
        return;
    }
    int i = 0;
    // пропускаем ключи коорые меньше min_k
    while (i < start_node->n && start_node->keys[i] < min_k)
    {
        i++;
    }
    bool found_any = false;
    fprintf(output, "RANGE %d %d: ", min_k, max_k);
    while (start_node != NULL)
    {
        // идем по узлу, проверяем диапазон
        for (; i < start_node->n; i++)
        {
            if (start_node->keys[i] > max_k)
            {
                if (!found_any)
                {
                    fprintf(output, "NO_RESULTS");
                }

                fprintf(output, "\n");
                pthread_rwlock_unlock(&start_node->lock); // нашли-не нашли, надо разблокировать узел
                return;
            }
            // иначе мы все еще в нужном диапазоне
            int value = get_value_from_disk(dbt, start_node, i);
            fprintf(output, "%d ", value);
            found_any = true;
        }
        // при необходимости переходим к следующему листу
        Node *next = start_node->next;
        if (next != NULL)
        {
            pthread_rwlock_rdlock(&next->lock); // заблокировать следующий узел
        }
        pthread_rwlock_unlock(&start_node->lock); // отпустить текущий узел
        start_node = next;
        i = 0;
    }
    if (!found_any)
    {
        fprintf(output, "NO_RESULTS");
    }
    fprintf(output, "\n");
}

Node *find_parent(BTree *tree, Node *child)
{
    if (tree->root == child)
    {
        return NULL;
    }

    Node *current = tree->root;
    Node *parent = NULL;
    while (!current->leaf && current != child)
    {
        parent = current;
        int i = 0;
        while (i < current->n && child->keys[0] > current->keys[i])
        {
            i++;
        }
        current = current->children[i];
    }
    return (current == child) ? parent : NULL;
}

void insert_into_leaf(DiskBTree *disk, Node *L, int insert_key, int value)
{
    // Найти место для вставки
    int insert_pos = 0;
    while (insert_pos < L->n && insert_key > L->keys[insert_pos])
    {
        insert_pos++;
    }
    // переставить остальные элементы вправо
    // пояснение: мы в insert уже проверили что лист не будет переполнен
    for (int i = L->n; i > insert_pos; i--)
    {
        L->keys[i] = L->keys[i - 1];
        L->values[i] = L->values[i - 1];
    }
    // вставить новый ключ и указатель
    L->keys[insert_pos] = insert_key;
    L->values[insert_pos] = value;
    L->n++;
    // оказалось что вообще-то указатель из родителя в лист не обязан указывать на первый элемент листа поэтому все в этой функции
    if (disk)
    {
        save_node_to_disk(disk, L);
    }
}

void insert_into_parent(BTree *tree, Node *N, int K_prime, Node *N_prime)
{
    pthread_rwlock_wrlock(&N->lock);
    pthread_rwlock_wrlock(&N_prime->lock);

    // Случай 1: N - корень, создаем новый корень
    if (tree->root == N)
    {
        Node *new_root = create_node(tree->t, false, tree, true, tree->disk_tree);
        if (!new_root)
        {
            pthread_rwlock_unlock(&tree->lock);
            pthread_rwlock_unlock(&new_root->lock);
            pthread_rwlock_unlock(&N->lock);
            pthread_rwlock_unlock(&N_prime->lock);
            perror("Failed to create new root node\n");
            return;
        }

        pthread_rwlock_wrlock(&new_root->lock);
        new_root->keys[0] = K_prime;
        new_root->children[0] = N;
        new_root->children[1] = N_prime;
        new_root->n = 1;
        tree->root = new_root;
        // Сохраняем изменения на диск
        if (tree->disk_tree)
        {
            save_node_to_disk(tree->disk_tree, new_root);
            tree->disk_tree->header->root_block = new_root->disk_block;
            msync(tree->disk_tree->header, BLOCK_SIZE, MS_SYNC);

            // Обновляем старых детей (N и N_prime)
            save_node_to_disk(tree->disk_tree, N);
            save_node_to_disk(tree->disk_tree, N_prime);
        }
        pthread_rwlock_unlock(&tree->lock);
        pthread_rwlock_unlock(&new_root->lock);
        pthread_rwlock_unlock(&N->lock);
        pthread_rwlock_unlock(&N_prime->lock);
        return;
    }
    // Случай 2:
    // Находим родителя
    Node *parent = find_parent(tree, N);
    if (!parent)
    {
        perror("Failed to find parent\n");
        pthread_rwlock_unlock(&N->lock);
        pthread_rwlock_unlock(&N_prime->lock);
        return;
    }
    pthread_rwlock_wrlock(&parent->lock);
    // В родителе есть место
    if (parent->n < 2 * tree->t - 1)
    {
        int insert_pos = 0;
        while (insert_pos <= parent->n && parent->children[insert_pos] != N)
        {
            insert_pos++;
        }
        insert_pos++; // Вставляем после N
                      // Сдвигаем элементы вправо
        for (int i = parent->n - 1; i >= insert_pos - 1; i--)
        {
            parent->keys[i + 1] = parent->keys[i];
        }
        for (int i = parent->n; i >= insert_pos; i--)
        {
            parent->children[i + 1] = parent->children[i];
        }
        // Вставляем K_prime и N_prime
        parent->keys[insert_pos - 1] = K_prime;
        parent->children[insert_pos] = N_prime;
        parent->n++;
        // Сохраняем изменения на диск
        if (tree->disk_tree)
        {
            save_node_to_disk(tree->disk_tree, parent);
        }
        pthread_rwlock_unlock(&N->lock);
        pthread_rwlock_unlock(&N_prime->lock);
        pthread_rwlock_unlock(&parent->lock);
        return;
    }
    // Родитель заполнен, нужно разделить
    else
    {
        int total_keys = parent->n + 1;
        int *temp_keys = malloc(total_keys * sizeof(int));
        Node **temp_pointers = malloc((total_keys + 1) * sizeof(Node *));
        if (!temp_keys || !temp_pointers)
        {
            perror("Error: Memory allocation failed\n");
            pthread_rwlock_unlock(&parent->lock);
            pthread_rwlock_unlock(&N->lock);
            pthread_rwlock_unlock(&N_prime->lock);
            free(temp_keys);
            free(temp_pointers);
            return;
        }
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

        // Разделяем родителя
        int split_pos = total_keys / 2;
        int split_pos_node = temp_keys[split_pos];

        Node *P_prime = create_node(tree->t, false, tree, true, tree->disk_tree);
        if (!P_prime)
        {
            perror("Error: Failed to create new node\n");
            pthread_rwlock_unlock(&parent->lock);
            pthread_rwlock_unlock(&N->lock);
            pthread_rwlock_unlock(&N_prime->lock);
            free(temp_keys);
            free(temp_pointers);
            return;
        }
        pthread_rwlock_wrlock(&P_prime->lock);
        // Обновляем исходного родителя
        parent->n = 0;
        for (i = 0; i < split_pos; i++)
        {
            parent->children[i] = temp_pointers[i];
            parent->keys[i] = temp_keys[i];
            parent->n++;
        }
        parent->children[i] = temp_pointers[i];
        // Заполняем нового родителя P_prime
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
        // Сохраняем изменения на диск перед рекурсивным вызовом
        if (tree->disk_tree)
        {
            save_node_to_disk(tree->disk_tree, parent);
            save_node_to_disk(tree->disk_tree, P_prime);
        }

        pthread_rwlock_unlock(&N->lock);
        pthread_rwlock_unlock(&N_prime->lock);
        pthread_rwlock_unlock(&parent->lock);
        pthread_rwlock_unlock(&P_prime->lock);
        //  Рекурсивно вставляем split_pos_node в родителя
        insert_into_parent(tree, parent, split_pos_node, P_prime);
    }
}

Node *find_leaf_to_insert(Node *root, int key)
{
    if (root == NULL)
        return NULL;

    Node *current = root;
    pthread_rwlock_rdlock(&current->lock);
    while (!current->leaf)
    {
        int i = 0;
        // Находим первый ключ в узле, который больше или равен искомому
        while (i < current->n && key >= current->keys[i])
        {
            i++;
        }
        // Переходим к соответствующему дочернему узлу
        Node *child = current->children[i];
        pthread_rwlock_rdlock(&child->lock);   // блокируем дочерний узел
        pthread_rwlock_unlock(&current->lock); // отпускаем текущий
        current = child;
    }
    // это лист и мы его отпускаем для чтения но блокируем для записи

    pthread_rwlock_unlock(&current->lock);
    return current;
}

void insert(BTree *tree, int insert_key, int value)
{
    fprintf(stdout, "entered\n");
    // глобальная блокировка дерева чтобы не было гонок при создании корня
    pthread_rwlock_wrlock(&tree->lock);

    fprintf(stdout, "got write lock\n");
    // Если дерево пустое
    if (tree->root == NULL || tree->root->n == 0)
    // Вставляем узел L который также является корнем
    {
        fprintf(stdout, "entered for =%d value=%d\n", insert_key, value);
        Node *L = create_node(tree->t, true, tree, true, tree->disk_tree);

        fprintf(stdout, "left for =%d value=%d\n", insert_key, value);
        if (!L)
        {
            perror("Error: Failed to create root leaf node\n");
            pthread_rwlock_unlock(&tree->lock);
            return;
        }
        pthread_rwlock_wrlock(&L->lock); // блокируем создаваемый узео
        L->keys[0] = insert_key;
        L->values[0] = value;
        L->n = 1;
        tree->root = L;
        // Сохраняем на диск
        if (tree->disk_tree)
        {
            save_node_to_disk(tree->disk_tree, L);
            tree->disk_tree->header->root_block = L->disk_block;
            msync(tree->disk_tree->header, BLOCK_SIZE, MS_SYNC);
        }
        // снимаем блокировку
        pthread_rwlock_unlock(&tree->lock);
        pthread_rwlock_unlock(&L->lock);
        fprintf(stdout, "1  Inserted key=%d value=%d\n", insert_key, value);
        return;
    }
    Node *root = tree->root;
    // если корень у дерева уже есть: найти лист, в который нужно вставить

    Node *L = find_leaf_to_insert(root, insert_key);

    if (!L)
    {
        perror("Error: Failed to find leaf node\n");
        return;
    }
    pthread_rwlock_wrlock(&L->lock);
    //  Если в узле еще есть место для вставки, вставляем туда
    if (L->n < 2 * tree->t - 1)
    {

        fprintf(stdout, "rwrwerwerwe for =%d value=%d\n", insert_key, value);
        insert_into_leaf(tree->disk_tree, L, insert_key, value); // сюда подается заблокированный, там просто заполняется, после снимаем блокировку
        pthread_rwlock_unlock(&L->lock);
        fprintf(stdout, "2   Inserted key=%d value=%d\n", insert_key, value);
    }
    else
    {
        // Иначе разбиваем лист
        fprintf(stdout, "entered for =%d value=%d\n", insert_key, value);
        Node *L_prime = create_node(tree->t, true, tree, true, tree->disk_tree);
        fprintf(stdout, "left for =%d value=%d\n", insert_key, value);
        if (!L_prime)
        {
            perror("Error: Failed to create L_prime node\n");
            pthread_rwlock_unlock(&L->lock);
            return;
        }
        pthread_rwlock_wrlock(&L_prime->lock); // блокируем создаваемый узео
        // Временно перемещаем ключи и указатели
        int total_keys = L->n + 1;
        int *temp_keys = malloc(total_keys * sizeof(int));
        int *temp_values = malloc(total_keys * sizeof(int));
        if (!temp_keys || !temp_values)
        {
            perror("Error: Memory allocation failed during split\n");
            pthread_rwlock_unlock(&L->lock);
            pthread_rwlock_unlock(&L_prime->lock);
            free(temp_keys);
            free(temp_values);
            return;
        }
        // Копируем ключи и ищем место для нового ключа
        int i = 0, j = 0;
        while (i < L->n && insert_key > L->keys[i])
        {
            temp_keys[j] = L->keys[i];
            temp_values[j] = L->values[i];
            i++;
            j++;
        }
        // Вставляем новый ключ
        temp_keys[j] = insert_key;
        temp_values[j] = value;
        j++;
        // Копируем оставшиеся ключи
        while (i < L->n)
        {
            temp_keys[j] = L->keys[i];
            temp_values[j] = L->values[i];
            i++;
            j++;
        }
        // Ищем точку разделения
        int split_pos = total_keys / 2;
        // int K_prime = temp_keys[split_pos];
        //  Обновляем L и L_prime
        L->n = split_pos;
        for (i = 0; i < split_pos; i++)
        {
            L->keys[i] = temp_keys[i];
            L->values[i] = temp_values[i];
        }
        L_prime->n = total_keys - split_pos;
        for (i = split_pos; i < total_keys; i++)
        {
            L_prime->keys[i - split_pos] = temp_keys[i];
            L_prime->values[i - split_pos] = temp_values[i];
        }
        // ОБновляем отношения между листами
        L_prime->next = L->next;
        if (L->next != NULL)
        {
            pthread_rwlock_wrlock(&L->next->lock);
            L->next->prev = L_prime;
            pthread_rwlock_unlock(&L->next->lock);

            if (tree->disk_tree)
            {
                save_node_to_disk(tree->disk_tree, L->next);
            }
        }
        L->next = L_prime;
        L_prime->prev = L;
        free(temp_keys);
        free(temp_values);

        // Ключ для подъёма в родителя — первый ключ нового листа
        int K_prime = L_prime->keys[0];

        // Сохраняем изменения на диск перед обновлением родителя
        if (tree->disk_tree)
        {
            save_node_to_disk(tree->disk_tree, L);
            save_node_to_disk(tree->disk_tree, L_prime);
        }
        // Обновляем родителя
        pthread_rwlock_unlock(&L->lock);
        pthread_rwlock_unlock(&L_prime->lock);
        insert_into_parent(tree, L, K_prime, L_prime); // принимает разблокированные узлы чтобы не создать пролбем при рекурсии

        pthread_rwlock_unlock(&tree->lock);
        fprintf(stdout, "insert done for key %d\n", insert_key);
    }
}

int find_child_index(Node *parent, Node *child)
{
    int index = 0;
    while (index <= parent->n && parent->children[index] != child)
    {
        index++;
    }
    return index;
}

void remove_key_and_value(BTree *tree, Node *N, int delete_key)
{
    // Находим позицию ключа
    int i = 0;
    while (i < N->n && N->keys[i] != delete_key)
    {
        i++;
    }
    // Проверяем, найден ли ключ
    if (i >= N->n)
    {
        fprintf(stderr, "Error: Key %d not found in node\n", delete_key);
        return;
    }

    // Для листа Сдвигаем ключи и указатели на данные
    if (N->leaf)
    {
        for (; i < N->n - 1; i++)
        {
            N->keys[i] = N->keys[i + 1];
            N->values[i] = N->values[i + 1];
        }
        N->keys[N->n - 1] = 0;
        N->values[N->n - 1] = NULL;
    }
    // Для внутреннего узла Сдвигаем ключи и указатели на потомков
    else
    {
        for (; i < N->n - 1; i++)
        {
            N->keys[i] = N->keys[i + 1];
            N->children[i] = N->children[i + 1];
        }
        // Сдвигаем последний указатель на ребенка
        N->children[N->n - 1] = N->children[N->n];
        N->children[N->n] = NULL;

        // Очищаем последний ключ
        N->keys[N->n - 1] = 0;
    }
    N->n--;
    // Сохраняем изменения на диск
    if (tree->disk_tree)
    {
        save_node_to_disk(tree->disk_tree, N);
    }
}

void coalesce_nodes(Node *N, Node *N_prime, Node *parent, int K_prime, BTree *tree)
{
    // все узлы подаются уже с write-log
    if (!N->leaf)
    {
        // Для внутренних узлов:
        // последний ключ левого узла теперь ключ который разделял левый и правый ключи в родительском узле
        N_prime->keys[N_prime->n] = K_prime;
        N_prime->n++;

        // Копируем ключи и детей из N в N_prime
        for (int i = 0; i < N->n; i++)
        {
            N_prime->keys[N_prime->n + i] = N->keys[i];
            N_prime->children[N_prime->n + i] = N->children[i];
        }
        N_prime->children[N_prime->n + N->n] = N->children[N->n];
        N_prime->n += N->n;
    }
    else
    {
        // Для листовых узлов:
        //  Копируем ключи и данные
        for (int i = 0; i < N->n; i++)
        {
            N_prime->keys[N_prime->n + i] = N->keys[i];
            N_prime->values[N_prime->n + i] = N->values[i];
        }
        N_prime->n += N->n;

        // Обновляем связи между листьями
        N_prime->next = N->next;
        if (N->next != NULL)
        {
            N->next->prev = N_prime;
            if (tree->disk_tree)
            {
                save_node_to_disk(tree->disk_tree, N->next);
            }
        }
        // N_prime-> prev и N_prime->prev->next обновлять не нужно
        // N_prime-> next = N->next
        // N->next->prev = N_prime
        // N->prev удалится при удалении узла ниже
    }
    // Сохраняем изменения на диск перед удалением
    if (tree->disk_tree)
    {
        save_node_to_disk(tree->disk_tree, N_prime);
        save_node_to_disk(tree->disk_tree, parent);
    }

    delete_entry(parent, K_prime, tree); // в delete_entry узел спыскался уже с блокировкой
                                         // Освобождаем от блокировки
    pthread_rwlock_unlock(&N->lock);
    pthread_rwlock_unlock(&N_prime->lock);
    pthread_rwlock_unlock(&parent->lock);
    // Освобождаем память и дисковые ресурсы
    if (tree->disk_tree && N->disk_block != -1)
    {
        free_block(tree->disk_tree, N->disk_block);
    }
    free(N->keys);
    if (N->leaf)
    {
        free(N->values);
    }
    else
    {
        free(N->children);
    }
    free(N);
}

void redistribute_nodes(Node *N, Node *N_prime, Node *parent, int K_prime, int N_index, BTree *tree)
{

    // Если N_prime стоит слева от N
    if (N_index > 0 && parent->children[N_index - 1] == N_prime)
    {
        // Если N не лист
        if (!N->leaf)
        {
            // Переместить последнего потомка N_prime, чтобы он был первым потомком N
            for (int i = N->n; i > 0; i--)
            {
                N->keys[i] = N->keys[i - 1];
                N->children[i + 1] = N->children[i];
            }
            N->children[1] = N->children[0];
            /*тут логика такая
            K_prime это ключ который стоит между узлами N_prime и N в родительском узле
            и мы хотим чтобы на место этого ключа в родительском узле встал нужный нам ключ из N_prime
            а K_prime из ролителя спускаем в N
            из N_prime при этом он как бы уходит
            */
            N->children[0] = N_prime->children[N_prime->n];
            N->keys[0] = K_prime;
            N->n++;

            // Обновить ключ в родителе
            parent->keys[N_index - 1] = N_prime->keys[N_prime->n - 1];
            N_prime->n--;
        }
        // Если N лист
        else
        {
            // перемещаем ключи и указатели
            for (int i = N->n; i > 0; i--)
            {
                N->keys[i] = N->keys[i - 1];
                N->values[i] = N->values[i - 1];
            }

            N->keys[0] = N_prime->keys[N_prime->n - 1];
            N->values[0] = N_prime->values[N_prime->n - 1];
            N->n++;

            // Обновляем ключ в родителе
            parent->keys[N_index - 1] = N->keys[0];
            N_prime->n--;
        }
    }
    // инчае N_prime стоит справа от N
    else
    {
        if (!N->leaf)
        {
            // перемещаем первого потомка N_prime на последнее место в N
            N->keys[N->n] = K_prime;
            N->children[N->n + 1] = N_prime->children[0];
            N->n++;

            // обновить ключ в родителе
            parent->keys[N_index] = N_prime->keys[0];

            // переместить ключи и потомков в N_prime
            for (int i = 0; i < N_prime->n - 1; i++)
            {
                N_prime->keys[i] = N_prime->keys[i + 1];
                N_prime->children[i] = N_prime->children[i + 1];
            }
            N_prime->children[N_prime->n - 1] = N_prime->children[N_prime->n];
            N_prime->n--;
        }
        else
        {
            // ДЛя листов
            // перемещаем первый ключ N_prime на последнее место в N
            N->keys[N->n] = N_prime->keys[0];
            N->values[N->n] = N_prime->values[0];
            N->n++;

            // обновить ключ в родителе
            parent->keys[N_index] = N_prime->keys[1];

            // перемещаем ключи и указатели
            for (int i = 0; i < N_prime->n - 1; i++)
            {
                N_prime->keys[i] = N_prime->keys[i + 1];
                N_prime->values[i] = N_prime->values[i + 1];
            }
            N_prime->n--;
        }
    }
    // Сохраняем изменения на диск
    if (tree->disk_tree)
    {
        save_node_to_disk(tree->disk_tree, N_prime);
        save_node_to_disk(tree->disk_tree, N);
        save_node_to_disk(tree->disk_tree, parent);
    }

    // Принудительная синхронизация
    msync(tree->disk_tree->mmap_ptr, tree->disk_tree->mmap_size, MS_SYNC);

    // Освобождаем от блокировки
    pthread_rwlock_unlock(&N->lock);
    pthread_rwlock_unlock(&N_prime->lock);
    pthread_rwlock_unlock(&parent->lock);
}

void delete_entry(Node *N, int delete_key, BTree *tree)
{
    // сюда узел спускается уже с блокировкой
    //   Удаляем ключ и значение из узла
    remove_key_and_value(tree, N, delete_key); // Тоже спускаем с write блокировкой

    // Если N корень и имеет только одного потомка
    // то пусть его потомок будет новым деревом а  N удалить
    if (N == tree->root && N->n == 0 && !N->leaf)
    {
        // в общем n = 0 и 1 ребенок это норм потому что кол-во детей = n + 1
        // но 1 ребенок может бть только у корня если что
        Node *new_root = N->children[0];
        pthread_rwlock_wrlock(&new_root->lock); // Блокируем новый корень
        // Освобождаем ресурсы старого корня
        if (tree->disk_tree && N->disk_block != -1)
        {
            free_block(tree->disk_tree, N->disk_block);
        }
        free(N->keys);
        free(N->children);
        pthread_rwlock_unlock(&N->lock); // отпускаем N, внтури этой функции иначе может упасть тк сейчас освободим память тоже
        free(N);

        tree->root = new_root;
        // Обновляем корень на диске
        if (tree->disk_tree)
        {
            tree->disk_tree->header->root_block = new_root->disk_block;
            msync(tree->disk_tree->header, BLOCK_SIZE, MS_SYNC);
        }
        pthread_rwlock_unlock(&new_root->lock); // Отпускаем новый корень
        return;
    }

    // Если после удаления у листа N останется слишком мало ключей или указателей
    if (!N->leaf && N->n < tree->t - 1)
    {
        Node *parent = find_parent(tree, N);
        if (parent)
        {

            pthread_rwlock_wrlock(&parent->lock); // Блокируем родителя
            int N_index = find_child_index(parent, N);

            Node *left_sibling = (N_index > 0) ? parent->children[N_index - 1] : NULL;
            Node *right_sibling = (N_index < parent->n) ? parent->children[N_index + 1] : NULL;
            // блокировки на соседей
            if (left_sibling)
                pthread_rwlock_wrlock(&left_sibling->lock);
            if (right_sibling)
                pthread_rwlock_wrlock(&right_sibling->lock);

            // ПОЯСНЕНИЕ:  в этом и следующем сценарии будем освобождать от блокировки узлы N,родителя, и соседей внутри самих функций
            // чтобы избежать поломки при случае в функции coalesce когда она полностью удаляет N

            // попробуем заимствовать у левого соседа
            if (left_sibling && left_sibling->n > tree->t - 1)
            {
                redistribute_nodes(N, left_sibling, parent, parent->keys[N_index - 1], N_index, tree);
            }

            // попробуем заимствовать у правого соседа
            else if (right_sibling && right_sibling->n > tree->t - 1)
            {
                redistribute_nodes(N, right_sibling, parent, parent->keys[N_index], N_index, tree);
            }
            // если не можем заимствовать будем сливать
            else
            {
                if (left_sibling)
                {
                    coalesce_nodes(N, left_sibling, parent, parent->keys[N_index - 1], tree);
                }
                else if (right_sibling)
                {
                    coalesce_nodes(right_sibling, N, parent, parent->keys[N_index], tree);
                }
            }
        }
    }
    // если у N достаточно ключей
    else if (N->leaf && N->n < tree->t - 1)
    {
        Node *parent = find_parent(tree, N);
        if (parent)
        {
            pthread_rwlock_wrlock(&parent->lock); // Блокируем родителя
            int N_index = find_child_index(parent, N);

            // Найти соседей
            Node *left_sibling = (N_index > 0) ? parent->children[N_index - 1] : NULL;
            Node *right_sibling = (N_index < parent->n) ? parent->children[N_index + 1] : NULL;
            // блокировки на соседей
            if (left_sibling)
                pthread_rwlock_wrlock(&left_sibling->lock);
            if (right_sibling)
                pthread_rwlock_wrlock(&right_sibling->lock);
            // попробуем заимствовать у левого соседа
            if (left_sibling && left_sibling->n > tree->t - 1)
            {
                redistribute_nodes(N, left_sibling, parent, parent->keys[N_index - 1], N_index, tree);
            }
            // попробуем заимствовать у правого соседа
            else if (right_sibling && right_sibling->n > tree->t - 1)
            {
                redistribute_nodes(N, right_sibling, parent, parent->keys[N_index], N_index, tree);
            }
            // если не можем заимствовать будем сливать
            else
            {
                if (left_sibling)
                {
                    coalesce_nodes(N, left_sibling, parent, parent->keys[N_index - 1], tree);
                }
                else if (right_sibling)
                {
                    coalesce_nodes(right_sibling, N, parent, parent->keys[N_index], tree);
                }
            }
        }
    }
}

void delete(int delete_key, BTree *tree)
{
    Node *leaf = find_leaf(delete_key, tree); // find_leaf возвращает узел с блокировкой на чтение, а тут мы собираем вносить изменения в узел
    // отпускаем блокировку на чтение
    pthread_rwlock_unlock(&leaf->lock);
    // ставим блокировку write
    pthread_rwlock_wrlock(&leaf->lock);
    // Находим указатель для удаления
    int delete_pointer = 0;
    for (int i = 0; i < leaf->n; i++)
    {
        if (leaf->keys[i] == delete_key)
        {
            delete_pointer = leaf->values[i];
            break;
        }
    }
    delete_entry(leaf, delete_key, tree);
}

void free_node(Node *node)
{
    if (!node)
        return;

    free(node->keys);
    pthread_rwlock_destroy(&node->lock);

    if (!node->leaf)
    {
        free(node->children);
    }
    else
    {
        free(node->values);
    }

    free(node);
}

void free_subtree(Node *node)
{
    if (!node)
    {
        return;
    }

    if (!node->leaf)
    {
        for (int i = 0; i <= node->n; i++)
        {
            free_subtree(node->children[i]);
        }
    }

    free_node(node);
}

void free_tree(BTree *tree)
{
    if (!tree)
    {
        return;
    }
    pthread_rwlock_destroy(&tree->lock);
    free_subtree(tree->root);
    free(tree);
}