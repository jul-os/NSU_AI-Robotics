#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

struct node
{
    int key;
    int next; // по индексам
    int prev;
};

struct node nodes_list[200001];

void print_linked_list(int start)
{
    int move = start;
    while (nodes_list[move].next != 0)
    {
        printf("%d\n", nodes_list[move].key);
        move = nodes_list[move].next;
    }
    printf("%d\n", nodes_list[move].key);
}

void delete_after(int index, int n) // ok
{
    if (index == n)
    {
        index = nodes_list[0].next;
    }
    else
    {
        index = nodes_list[index].next;
    }
    // это тот который надо удалить потому что я запуталась уже
    printf("%d\n", nodes_list[index].key);
    nodes_list[nodes_list[index].prev].next = nodes_list[index].next;
    nodes_list[nodes_list[index].next].prev = nodes_list[index].prev;
}

void insert_after(int index, int n)
{
    n+=2; //0 ...(n+1) new
    printf("%d\n", n);
    if (index == 0)
    {
        nodes_list[n].next = nodes_list[0].next;
        nodes_list[nodes_list[0].next].prev = n;
        nodes_list[0].next = n;
        nodes_list[n].prev = 0;
    }
    else
    {
        nodes_list[n].next = nodes_list[index].next;
        nodes_list[nodes_list[index].next].prev = n;
        nodes_list[index].next = n;
        nodes_list[n].prev = index;
    }
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
    int n, nodes_num, first_node, last_node, commands_num;
    scanf("%d", &n);

    for (int i = 0; i < n; i++)
    {
        scanf("%d %d %d %d", &nodes_num, &first_node, &last_node, &commands_num);
        // буфферный элемент
        first_node++;
        last_node++;
        nodes_list[0].key = 0;
        nodes_list[0].next = first_node;
        nodes_list[0].prev = last_node;
        // запихивание в массив
        for (int j = 1; j < nodes_num + 1; j++)
        {
            scanf("%d %d %d", &nodes_list[j].key, &nodes_list[j].next, &nodes_list[j].prev);
            nodes_list[j].next += 1;
            nodes_list[j].prev += 1;
        }

        // комманды
        int what, where, printable = nodes_num;
        for (int j = 0; j < commands_num; j++)
        {
            scanf("%d %d", &what, &where);
            where++;
            if (what == 1)
            {
                printable++;
            }
            else if (what == -1)
            {
                insert_after(where, nodes_num);
                nodes_num++;
                printable++;
            }
            else if (what == 0)
            {
                delete_after(where, nodes_num);
                nodes_num--; //а надо это делать???
            }
        }
        printf("===\n");
        print_linked_list(nodes_list[0].next);
        printf("===\n");
    }

    fclose(input);
    fclose(output);
    return 0;
}