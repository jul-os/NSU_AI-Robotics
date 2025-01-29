#include <stdio.h>
#include <stdlib.h>

typedef struct tree
{
    int key;
    struct tree *left_tree;
    struct tree *right_tree;
} tree;

tree *add_node(int new_key, tree *cur_tree)
{
    if (cur_tree == NULL)
    {
        tree *new = malloc(sizeof(tree));
        new->key = new_key;
        new->left_tree = NULL;
        new->right_tree = NULL;
        cur_tree = new;
        return cur_tree;
    }
    else
    {
        if (new_key <= cur_tree->key)
        {
            cur_tree->left_tree = add_node(new_key, cur_tree->left_tree);
        }
        else
        {
            cur_tree->right_tree = add_node(new_key, cur_tree->right_tree);
        }
    }
    return cur_tree;
}

void tree_obhod(tree *cur_tree, int *arr, int n, int *i)
{
    if (cur_tree != NULL && *i < n)
    {
        tree_obhod(cur_tree->left_tree, arr, n, i);
        arr[*i] = cur_tree->key;
        (*i)++;
        tree_obhod(cur_tree->right_tree, arr, n, i); 
    }
}

void tree_sort(int *arr, int n)
{
    tree *tree_a = NULL;
    for (int i = 0; i < n; i++)
    {
        tree_a = add_node(arr[i], tree_a);
    }
    int i = 0;
    tree_obhod(tree_a, arr, n, &i); 
}

int main()
{
    freopen("input.txt", "rb", stdin);
    freopen("output.txt", "wb", stdout);
    int n;
    fread(&n, sizeof(int), 1, stdin);
    int *n_arr = malloc(n * sizeof(int));
    fread(n_arr, sizeof(int), n, stdin);
    tree_sort(n_arr, n);
    fwrite(n_arr, sizeof(int), n, stdout);
    free(n_arr);
    fclose(stdin);
    fclose(stdout);
    return 0;
}