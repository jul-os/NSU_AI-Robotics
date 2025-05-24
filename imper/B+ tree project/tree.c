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

Node *create_node(int t, bool is_leaf)
{
    //Создаем новый узел
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node)
    {
        perror("malloc failed on node");
        ;
        return NULL;
    }
    //Инициализируем ключи
    new_node->keys = (int *)malloc((2 * t - 1) * sizeof(int));
    if (!new_node->keys)
    {
        perror("malloc failed on node keys");
        free(new_node);
        return NULL;
    }
    //Присваиваем значения прочим
    new_node->n = 0;
    new_node->leaf = is_leaf;
    new_node->disk_block = -1;

    // Выделяем память для pointers
    if (is_leaf)
    {
        // Для листа - указатели на данные
        new_node->data_pointers = (void **)malloc((2 * t - 1) * sizeof(void *));
        if (!new_node->data_pointers)
        {
            perror("malloc failed on node data pointers");
            free(new_node->keys);
            free(new_node);
            return NULL;
        }
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
        new_node->data_pointers = NULL; // Не используем для данных
        new_node->prev = NULL;
        new_node->next = NULL;
    }

    return new_node;
}

BTree *create_tree(int t)
{
    //Создаем дерево и заполняем его
    BTree *tree = (BTree *)malloc(sizeof(BTree));
    tree->root = create_node(t, true);
    tree->t = t;
    tree->disk_tree = NULL;
    /*
    TODO
    btree->root->disk_block = allocate_block(dbt);
    dbt->header->root_block = btree->root->disk_block;
    save_node_to_disk(btree->root, dbt);
    */
    return tree;
}

void *find(int search_key, BTree *tree)
{
    Node *N = tree->root;
    while (!N->leaf)
    {
        int i = 0;
        while (i < N->n && search_key <= N->keys[i])
        {
            i++;
        }
        if (i == N->n)
        {
            N = N->children[N->n];
        }
        else if (search_key == N->keys[i])
        {
            N = N->children[i + 1];
        }
        else
        {
            N = N->children[i]; // val < N->keys[i]
        }
    }
    // now N is a leaf
    for (int i = 0; i < N->n; i++)
    {
        if (N->keys[i] == search_key)
        {
            // TODO функция которая достает с диска
            return N->data_pointers[i]; // fixme тип функции другой скорее всего
        }
    }
    return NULL;
}

Node *find_leaf(int search_key, BTree *tree)
{
    // Начинаем с корня
    Node *N = tree->root;
    while (!N->leaf)
    {
        int i = 0;
        while (i < N->n && search_key <= N->keys[i])
        {
            i++;
        }
        if (i == N->n)
        {
            N = N->children[N->n];
        }
        else if (search_key == N->keys[i])
        {
            N = N->children[i + 1];
        }
        else
        {
            N = N->children[i]; // val < N->keys[i]
        }
    }
    // Теперь N это лист, ищем в нем ключ
    for (int i = 0; i < N->n; i++)
    {
        if (N->keys[i] == search_key)
        {
            return N;
        }
    }
    return NULL;
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
        while (i < current->n && current->keys[0] <= current->keys[i])
        {
            i++;
        }
        if (i == current->n)
        {
            current = current->children[current->n];
        }
        else if (current->keys[0] == current->keys[i])
        {
            current = current->children[i + 1];
        }
        else
        {
            current = current->children[i]; // val < N->keys[i]
        }
    }
    return (current == child) ? parent : NULL;
}


void range_query(BTree *tree, int min_k, int max_k)
{
    if (!tree || !tree->root)
    {
        return;
    }
    Node *start_node = find_leaf(min_k, tree);
    if (!start_node)
    {
        return;
    }
    int i = 0;
    // пропускаем ключи коорые меньше min_k
    while (i < start_node->n && start_node->keys[i] < min_k)
    {
        i++;
    }
    while (start_node != NULL)
    {
        for (; i < start_node->n; i++)
        {
            if (start_node->keys[i] > max_k)
            {
                return;
            }
            // else were still in the right diaposon
            // TODO take from mem
        }

        start_node = start_node->next;
        i = 0;
    }
}

void insert_into_leaf(Node *L, int insert_key, void *insert_pointer)
{
    // find where to insert
    int insert_pos = 0;
    while (insert_pos < L->n && insert_key > L->keys[insert_pos])
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
    L->keys[insert_pos] = insert_key;
    L->data_pointers[insert_pos] = insert_pointer;
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
    Node *parent = find_parent(tree, N);
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
void insert(BTree *tree, int insert_key, void *insert_pointer)
{
    // Если дерево пустое
    if (tree->root == NULL || tree->root->n == 0)
    // Вставляем узел L который также является корнем
    {
        Node *L = create_node(tree->t, true);
        L->keys[0] = insert_key;
        L->n = 1;
        tree->root = L;
        return;
    }
    // Иначе: найти лист, в который нужно вставить
    Node *L = find_leaf(insert_key, tree);

    // Если в узле еще есть место для вставки, вставляем туда
    if (L->n < 2 * tree->t - 1)
    {
        insert_into_leaf(L, insert_key, insert_pointer);
    }
    else
    {
        // Иначе разбиваем лист
        Node *L_prime = create_node(tree->t, true);
        // Временно перемещаем ключи и указатели
        int total_keys = L->n + 1;
        int *temp_keys = malloc(total_keys * sizeof(int));
        // fixme void pointers or Node pointers????
        void **temp_pointers = malloc(total_keys * sizeof(void *));
        // Копируем ключи и ищем место для нового ключа
        int i = 0, j = 0;
        while (i < L->n && insert_key > L->keys[i])
        {
            temp_keys[j] = L->keys[i];
            temp_pointers[j] = L->data_pointers[i];
            i++;
            j++;
        }
        // Вставляем новый ключ
        temp_keys[j] = insert_key;
        temp_pointers[j] = insert_pointer;
        j++;
        // Копируем оставшиеся ключи
        while (i < L->n)
        {
            temp_keys[j] = L->keys[i];
            temp_pointers[j] = L->data_pointers[i];
            i++;
            j++;
        }
        // Ищем точку разделения
        int split_pos = total_keys / 2;
        int K_prime = temp_keys[split_pos];
        // TODO тут где-то запись на диск еще ))))
        // Обновляем L и L_prime
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
        // ОБновляем отношения между листами
        L_prime->next = L->next;
        if (L->next != NULL)
        {
            L->next->prev = L_prime;
        }
        L->next = L_prime;
        L_prime->prev = L;
        free(temp_keys);
        free(temp_pointers);
        // Обновляем родителя
        insert_into_parent(tree, L, K_prime, L_prime);
    }
}

int find_child_index(Node *parent, Node *child)
{
    int index = 0;
    while (index <= parent->n && parent->children[index != child]) // fixme comparison between pointer and integer
    {
        index++;
    }
    return index;
}

void remove_key_and_pointer(Node *N, int delete_key)
{
    // task а оно может вообще быть не листом?
    int i = 0;
    while (i < N->n && N->keys[i] != delete_key)
    {
        i++;
    }

    // if leaf, shilf keys and data pointers
    if (N->leaf)
    {
        for (; i < N->n - 1; i++)
        {
            N->keys[i] = N->keys[i + 1];
            N->data_pointers[i] = N->data_pointers[i + 1];
        }
    }
    // if interanal node, shift keys and children pointers
    else
    {
        for (; i < N->n - 1; i++)
        {
            N->keys[i] = N->keys[i + 1];
            N->children[i] = N->children[i + 1];
        }
    }
    N->n--;
}

void coalesce_nodes(Node *N, Node *N_prime, Node *parent, int K_prime, BTree *tree)
{
    if (!N->leaf)
    {
        // последний ключ левого узла теперь ключ который разделял левый и правый ключи в родительском узле
        N_prime->keys[N_prime->n] = K_prime;
        N_prime->n++;

        // copy from N to N_prime
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
        for (int i = 0; i < N->n; i++)
        {
            N_prime->keys[N_prime->n + i] = N->keys[i];
            N_prime->data_pointers[N_prime->n + i] = N->data_pointers[i];
        }
        N_prime->n += N->n;

        // Update leaf linked list
        N_prime->next = N->next;
        if (N->next != NULL)
        {
            N->next->prev = N_prime;
        }
        // N_prime-> prev и N_prime->prev->next обновлять не нужно
        // N_prime-> next = N->next
        // N->next->prev = N_prime
        // N->prev удалится при удалении узла ниже
    }
    delete_entry(parent, K_prime, N, tree);
    free(N->keys);
    if (N->leaf)
    {
        free(N->data_pointers);
    }
    else
    {
        free(N->children);
    }
    free(N);
}


void redistribute_nodes(Node *N, Node *N_prime, Node *parent, int K_prime, int N_index)
{
    // Если N_prime стоит слева от N
    if (N_index > 0 && parent->children[N_index - 1] == N_prime)
    {
        //Если N не лист
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
            // same but for keys
            for (int i = N->n; i > 0; i--)
            {
                N->keys[i] = N->keys[i - 1];
                N->data_pointers[i] = N->data_pointers[i - 1];
            }

            N->keys[0] = N_prime->keys[N_prime->n - 1];
            N->data_pointers[0] = N_prime->data_pointers[N_prime->n - 1];
            N->n++;

            // update key in parent
            parent->keys[N_index - 1] = N->keys[0];
            N_prime->n--;
        }
    }
    // else N_prime is right to N
    else
    {
        if (!N->leaf)
        {
            // move first child of N_prime to be last in N
            N->keys[N->n] = K_prime;
            N->children[N->n + 1] = N_prime->children[0];
            N->n++;

            // update key in parent
            parent->keys[N_index] = N_prime->keys[0];

            // Shift keys and children in N_prime
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
            // For leaf nodes
            // Move the first key of N_prime to be the last key of N
            N->keys[N->n] = N_prime->keys[0];
            N->data_pointers[N->n] = N_prime->data_pointers[0];
            N->n++;

            // Update parent's key
            parent->keys[N_index] = N_prime->keys[1];

            // Shift keys and data pointers in N_prime
            for (int i = 0; i < N_prime->n - 1; i++)
            {
                N_prime->keys[i] = N_prime->keys[i + 1];
                N_prime->data_pointers[i] = N_prime->data_pointers[i + 1];
            }
            N_prime->n--;
        }
    }
}

void delete_entry(Node *N, int delete_key, void *delete_pointer, BTree *tree)
{
    // remove key and pointers from the node
    remove_key_and_pointer(N, delete_key);

    // if (N is the root and N has only one remaining child)
    // then make the child of N the new root of the tree and delete N
    if (N == tree->root && N->n == 0 && !N->leaf)
    {
        // в общем n = 0 и 1 ребенок это норм потому что кол-во детей = n + 1
        // но 1 ребенок может бть только у корня если что
        Node *new_root = N->children[0];
        free(N->keys);
        free(N->children);
        free(N);
        tree->root = new_root;
        return;
    }

    // if after deletion node has too few keys/pointers
    if (!N->leaf && N->n < tree->t - 1)
    {
        Node *parent = find_parent(tree, N);
        int N_index = find_child_index(parent, N);

        Node *left_sibling = (N_index > 0) ? parent->children[N_index - 1] : NULL;
        Node *right_sibling = (N_index < parent->n) ? parent->children[N_index + 1] : NULL;

        // try borrowing from the left sibling
        if (left_sibling && left_sibling->n > tree->t - 1)
        {
            redistribute_nodes(N, left_sibling, parent, parent->keys[N_index - 1], N_index);
        }
        // try borrowing from the right sibling
        else if (right_sibling && right_sibling->n > tree->t - 1)
        {
            redistribute_nodes(N, right_sibling, parent, parent->keys[N_index], N_index);
        }
        // if cant borrow merge
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
    // now if N doesnt have too few keys/pointers
    else if (N->leaf && N->n < tree->t - 1)
    {
        Node *parent = find_parent(tree, N);
        if (parent)
        {
            int N_index = find_child_index(parent, N);

            // Find left and right siblings
            Node *left_sibling = (N_index > 0) ? parent->children[N_index - 1] : NULL;
            Node *right_sibling = (N_index < parent->n) ? parent->children[N_index + 1] : NULL;

            // Try to borrow from left sibling
            if (left_sibling && left_sibling->n > tree->t - 1)
            {
                redistribute_nodes(N, left_sibling, parent, parent->keys[N_index - 1], N_index);
            }
            // Try to borrow from right sibling
            else if (right_sibling && right_sibling->n > tree->t - 1)
            {
                redistribute_nodes(N, right_sibling, parent, parent->keys[N_index], N_index);
            }
            // If can't borrow, merge with a sibling
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

void delete(int delete_key, void *delete_pointer, BTree *tree)
{
    Node *leaf = find_leaf(delete_key, tree);
    delete_entry(leaf, delete_key, delete_pointer, tree);
}

int main()
{
    BTree *tre = create_tree(3);
    insert(tre, 10, 20);
    return 0;
}