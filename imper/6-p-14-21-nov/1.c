#include <stdio.h>
#include <errno.h>

struct node
{
    double key;
    struct node *next;
};

void print_list_of_nodes(struct node *start)
{

    while (start->next != NULL)
    {
        printf("%0.3lf\n", start->key);
        start = start->next;
    }
    printf("%0.3lf\n", start->key);
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

    /*
    Дан односвязный список, который хранится в массиве. Массив состоит из узлов, в каждом
узле лежит какое-то вещественное значение и индекс следующего элемента списка в том же
массиве.
    */

    int n, f;
    if (scanf("%d %d", &n, &f) != 2)
    {
        perror("please input n f");
        return errno;
    }

    struct node list_of_nodes[n];
    double temp_key;
    int temp_next;
    for (int i = 0; i < n; i++)
    {
        scanf("%lf %d", &temp_key, &temp_next);
        if (temp_next == -1)
        {
            list_of_nodes[i].next = NULL;
        }
        else
        {
            list_of_nodes[i].next = &list_of_nodes[temp_next];
        }
        list_of_nodes[i].key = temp_key;
    }
    print_list_of_nodes(&list_of_nodes[f]);

    fclose(input);
    fclose(output);
    return 0;
}