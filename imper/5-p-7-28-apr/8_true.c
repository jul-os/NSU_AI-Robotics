#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAXN 3001
#define MAXM 600001
#define INF 1000000000

typedef struct
{
    int to, weight, next;
} Edge;

Edge edges[MAXM];
int head[MAXN], edge_cnt = 0;

void add_edge_to_graph(int u, int to, int we)
{
    edges[edge_cnt].to = to;
    edges[edge_cnt].weight = we;
    edges[edge_cnt].next = head[u];
    head[u] = edge_cnt++;
}

// куча для дейкстры
typedef struct
{
    int node, dist;
} HeapNode;

HeapNode heap[MAXN * 10];
int heap_size = 0;

void swap(int i, int j)
{
    HeapNode temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

void push(int node, int dist)
{
    int i = heap_size++;
    heap[i].node = node;
    heap[i].dist = dist;
    while (i > 0 && heap[i].dist < heap[(i - 1) / 2].dist)
    {
        swap(i, (i - 1) / 2);
        i = (i - 1) / 2;
    }
}

HeapNode pop()
{
    HeapNode top = heap[0];
    heap[0] = heap[--heap_size];
    int i = 0;
    while (1)
    {
        int smallest = i;
        int left = 2 * i + 1, right = 2 * i + 2;
        if (left < heap_size && heap[left].dist < heap[smallest].dist)
        {
            smallest = left;
        }
        if (right < heap_size && heap[right].dist < heap[smallest].dist)
        {
            smallest = right;
        }
        if (smallest == i)
        {
            break;
        }
        swap(i, smallest);
        i = smallest;
    }
    return top;
}

int dist[MAXN], visited[MAXN], prev[MAXN];

void dijkstra(int start, int n)
{
    for (int i = 1; i <= n; ++i)
    {
        dist[i] = INF;
        visited[i] = 0;
        prev[i] = -1;
    }

    dist[start] = 0;
    heap_size = 0;
    push(start, 0);
    while (heap_size > 0)
    {
        HeapNode hn = pop();
        int u = hn.node;
        if (visited[u])
        {
            continue;
        }
        visited[u] = 1;
        for (int i = head[u]; i != -1; i = edges[i].next)
        {
            int v = edges[i].to;
            int w = edges[i].weight;
            if (dist[u] + w < dist[v])
            {
                dist[v] = dist[u] + w;
                prev[v] = u;
                push(v, dist[v]);
            }
        }
    }
}

int path[MAXN];

void write_path(int to)
{
    int len = 0;
    for (int v = to; v != -1; v = prev[v])
    {
        path[len++] = v;
    }
    printf("YES %d %d  ", dist[to], len);
    for (int i = len - 1; i >= 0; --i)
    {
        printf("%d ", path[i]);
    }
    printf("\n");
}

int main(void)
{
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    int housewives, pairs, n;
    int start, next, time;
    
    scanf("%d %d %d", &housewives, &pairs, &n);
    int quear[n][2];
    for (int i = 0; i <= housewives; ++i)
    {
        head[i] = -1;
    }

    for (int i = 0; i < n; i++)
    {
        scanf("%d %d", &quear[i][0], &quear[i][1]);
    }
    for (int i = 0; i < pairs; ++i)
    {
        int u, v, t;
        scanf("%d %d %d", &u, &v, &t);
        add_edge_to_graph(u, v, t);
        add_edge_to_graph(v, u, t);
    }
    for (int i = 0; i < n; ++i)
    {
        int s = quear[i][0], t = quear[i][1];
        dijkstra(s, housewives);
        if (dist[t] == INF)
        {
            printf("NO\n");
        }
        else
        {
            write_path(t);
        }
    }
    fclose(input);
    fclose(output);
    return 0;
}