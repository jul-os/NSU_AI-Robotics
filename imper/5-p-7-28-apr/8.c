#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAXN 3001
#define MAXM 600001 // Удвоено для неориентированного графа
#define INF 1000000000

// Структура ребра
typedef struct {
    int to, weight, next;
} Edge;

Edge edges[MAXM];
int head[MAXN], edge_cnt = 0;

void add_edge(int u, int v, int w) {
    edges[edge_cnt].to = v;
    edges[edge_cnt].weight = w;
    edges[edge_cnt].next = head[u];
    head[u] = edge_cnt++;
}

// Куча для Дейкстры
typedef struct {
    int node, dist;
} HeapNode;

HeapNode heap[MAXN * 10];
int heap_size = 0;

void swap(int i, int j) {
    HeapNode temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

void push(int node, int dist) {
    int i = heap_size++;
    heap[i].node = node;
    heap[i].dist = dist;
    while (i > 0 && heap[i].dist < heap[(i - 1) / 2].dist) {
        swap(i, (i - 1) / 2);
        i = (i - 1) / 2;
    }
}

HeapNode pop() {
    HeapNode top = heap[0];
    heap[0] = heap[--heap_size];
    int i = 0;
    while (1) {
        int smallest = i;
        int left = 2 * i + 1, right = 2 * i + 2;
        if (left < heap_size && heap[left].dist < heap[smallest].dist)
            smallest = left;
        if (right < heap_size && heap[right].dist < heap[smallest].dist)
            smallest = right;
        if (smallest == i) break;
        swap(i, smallest);
        i = smallest;
    }
    return top;
}

int dist[MAXN], visited[MAXN], prev[MAXN];

void dijkstra(int start, int n) {
    for (int i = 1; i <= n; ++i) {
        dist[i] = INF;
        visited[i] = 0;
        prev[i] = -1;
    }

    dist[start] = 0;
    heap_size = 0;
    push(start, 0);

    while (heap_size > 0) {
        HeapNode hn = pop();
        int u = hn.node;
        if (visited[u]) continue;
        visited[u] = 1;

        for (int i = head[u]; i != -1; i = edges[i].next) {
            int v = edges[i].to;
            int w = edges[i].weight;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                push(v, dist[v]);
            }
        }
    }
}

int path[MAXN];

void write_path(FILE *out, int to) {
    int len = 0;
    for (int v = to; v != -1; v = prev[v])
        path[len++] = v;
    fprintf(out, "YES %d %d ", dist[to], len);
    for (int i = len - 1; i >= 0; --i)
        fprintf(out, " %d", path[i]);
    fprintf(out, "\n");
}

int main() {
    FILE *in = fopen("input.txt", "r");
    FILE *out = fopen("output.txt", "w");

    int n, m, k;
    fscanf(in, "%d %d %d", &n, &m, &k);

    for (int i = 0; i <= n; ++i) head[i] = -1;

    int queries[k][2];
    for (int i = 0; i < k; ++i)
        fscanf(in, "%d %d", &queries[i][0], &queries[i][1]);

    for (int i = 0; i < m; ++i) {
        int u, v, t;
        fscanf(in, "%d %d %d", &u, &v, &t);
        add_edge(u, v, t);
        add_edge(v, u, t); // двустороннее общение
    }

    for (int i = 0; i < k; ++i) {
        int s = queries[i][0], t = queries[i][1];
        dijkstra(s, n);
        if (dist[t] == INF) {
            fprintf(out, "NO\n");
        } else {
            write_path(out, t);
        }
    }

    fclose(in);
    fclose(out);
    return 0;
}
