#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct node
{
    char key[8];
    struct node *next; // по индексам
};

// массив указателей на списки, указывает на start каждого
struct node lists[1000001];

void print_list_of_nodes(struct node *start, int index)
{
    while (start != NULL)
    {
        printf("%d %s\n", index, start->key);
        start = start->next;
    }
}

int put_into(struct node *start, int index)
{
    struct node *ptr, *new_node;
    char val[8];
    scanf("%s", val);

    // Создаем новый узел
    new_node = (struct node *)malloc(sizeof(struct node));
    strncpy(new_node->key, val, sizeof(new_node->key) - 1);
    new_node->key[sizeof(new_node->key) - 1] = '\0';
    new_node->next = NULL;

    if (strcmp(start->key, "") == 0)
    {
        *start = *new_node;
        free(new_node);
        return 0;
    }

    ptr = start;
    while (ptr->next != NULL)
        ptr = ptr->next;
    ptr->next = new_node;
    return 0;
}

void print_lists(void)
{
    for (int i = 0; i < 1000001; i++)
    {
        if ((strcmp(lists[i].key, "") != 0))
        {
            print_list_of_nodes(&lists[i], i);
        }
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
    int n;
    scanf("%d", &n);
    for (int i = 0; i < 1000000; i++)
    {
        lists[i].next = NULL;
        strcpy(lists[i].key, "");
    }

    int index;
    for (int i = 0; i < n; i++)
    {
        scanf("%d ", &index);
        put_into(&lists[index], index);
    }
    print_lists();

    fclose(input);
    fclose(output);
    return 0;
}