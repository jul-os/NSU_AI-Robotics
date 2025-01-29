#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

struct node
{
    char key[9];
    int next; // по индексам
};

struct node nodes_list[200001];

void print_linked_list(int start)
{
    int move = start;
    while (nodes_list[move].next != -1)
    {
        printf("%s\n", nodes_list[move].key);
        move = nodes_list[move].next;
    }
    printf("%s\n", nodes_list[move].key);
}

void add_first(int *start, int *n)
{
    // printf("%d\n", *n + 1);
    (*n)++;
    scanf("%s", nodes_list[*n].key);
    nodes_list[*n].next = *start;
    *start = *n;
    // printf("add_first: Добавлен узел с key=%s на позицию %d, новый start=%d\n", nodes_list[*n].key, *n, *start);
    for (int i = 0; i <= *n; i++)
    {
        printf("Узел %d: key=%s, next=%d\n", i, nodes_list[i].key, nodes_list[i].next);
    }
}

void add_after(int index, int *n)
{
    // printf("%d\n", *n + 1);
    (*n)++;
    scanf("%s", nodes_list[*n].key);

    int prev_next = nodes_list[index].next;
    nodes_list[*n].next = prev_next;
    nodes_list[index].next = *n;

    // printf("add_after: Добавлен узел с key=%s на позицию %d после узла %d\n", nodes_list[*n].key, *n, index);
    // printf("add_after: Узел %d теперь указывает на %d\n", index, nodes_list[index].next);
    // printf("add_after: Узел %d указывает на %d\n", *n, nodes_list[*n].next);
    for (int i = 0; i <= *n; i++)
    {
        printf("Узел %d: key=%s, next=%d\n", i, nodes_list[i].key, nodes_list[i].next);
    }
}

void delete_first(int *start, int n)
{
    // printf("%s\n", nodes_list[*start].key);
    *start = nodes_list[*start].next;

    // for (int i = 0; i <= n; i++)
    // {
    //     printf("Узел %d: key=%s, next=%d\n", i, nodes_list[i].key, nodes_list[i].next);
    // }
}

void delete_after(int index, int n)
{
    // first index this next
    printf("%s\n", nodes_list[nodes_list[index].next].key);
    nodes_list[index].next = nodes_list[nodes_list[index].next].next;
    // for (int i = 0; i <= n; i++)
    // {
    //     printf("Узел %d: key=%s, next=%d\n", i, nodes_list[i].key, nodes_list[i].next);
    // }
}

int main(void)
{
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    if (input == NULL || output == NULL)
    {
        perror("freopen()");
        return errno;
    }

    int n, nodes_num, first_node, commands_num;
    scanf("%d", &n);
    for (int i = 0; i < n; i++)
    {
        scanf("%d %d %d", &nodes_num, &first_node, &commands_num);

        for (int j = 0; j < nodes_num; j++)
        {
            scanf("%s %d", nodes_list[j].key, &nodes_list[j].next);
        }
        int what, where, printable = nodes_num;
        for (int j = 0; j < commands_num; j++)
        {
            scanf("%d %d", &what, &where);
            if (what == 0)
            {
                if (j == 0)
                {
                    nodes_num -= 1;
                }
                printf("%d\n", printable);
                printable++;
                if (where == -1)
                {
                    add_first(&first_node, &nodes_num);
                }
                else
                {
                    add_after(where, &nodes_num);
                }
            }
            else
            {
                if (where == -1)
                {
                    delete_first(&first_node, nodes_num);
                }
                else
                {
                    delete_after(where, nodes_num);
                }
            }
        }
        printf("===\n");
        print_linked_list(first_node);
        printf("===\n");
    }

    fclose(input);
    fclose(output);
    return 0;
}
