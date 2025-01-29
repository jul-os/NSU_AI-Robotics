#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

struct node
{
    int key;
    int next; 
};

struct node nodes_list[5001]; 

void remove_after(int *current, int *count)
{
    int to_remove = nodes_list[*current].next;
    printf("%d\n", nodes_list[to_remove].key);
    nodes_list[*current].next = nodes_list[to_remove].next;
    (*count)--;
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

    int n, k;
    scanf("%d %d", &n, &k);

    for (int i = 1; i <= n; i++)
    {
        nodes_list[i].key = i;
        if (i != n)
        {
            nodes_list[i].next = i + 1;
        }
        else
        {
            nodes_list[i].next = 1; 
        }
    }

    int count = n;
    int current = n;
    while (count > 0)
    {
        for (int i = 0; i < k - 1; i++)
        {
            current = nodes_list[current].next;
        }

        remove_after(&current, &count);
    }

    fclose(input);
    fclose(output);
    return 0;
}
