#include <stdio.h>
#include <stdlib.h>

/*
Алгоритм Косарайю (шаги):

    Построение транспонированного графа:

        Создается trans_graph где все рёбра развёрнуты в обратном направлении

    Первый проход DFS (dfs1):

        Выполняется обход в глубину исходного графа

        Вершины добавляются в массив order в порядке завершения обработки (пост-порядок)

    Второй проход DFS (dfs2):

        Обход в глубину выполняется на транспонированном графе

        Вершины обрабатываются в порядке, обратном полученному в первом проходе

        Все достижимые вершины в одном обходе образуют компоненту сильной связности

            Первый DFS определяет "времена завершения" обработки вершин

    Второй DFS на транспонированном графе, начиная с вершин с наибольшим временем завершения, находит все вершины, достижимые в обратном графе - это и есть SCC

    Компоненты образуют конденсацию графа (DAG)

Сложность:

    Время: O(V + E) (два обхода DFS)

    Память: O(V + E) (хранение графа и транспонированного графа)

Этот алгоритм эффективен для больших графов и часто используется в задачах анализа 
*/

typedef struct graph_S{
    int *arr;
    int len;
}Graph;

Graph graph[150000];
Graph trans_graph[150000];

int order[150000];
int order_ind = 0;
int component[150000];
int component_ind = 0;
int used[150000];

void dfs1(int v){
    used[v] = 1;
    for(int i = 0; i < graph[v].len; i++)
        if(!used[graph[v].arr[i]])
            dfs1(graph[v].arr[i]);
    order[order_ind++] = v;
}

void dfs2(int v, int col){
    used[v] = 1;
    component[v] = col;
    for(int i = 0; i < trans_graph[v].len; i++){
        if(!used[trans_graph[v].arr[i]]){
            dfs2(trans_graph[v].arr[i], col);
        }
    }
}


int main(){
    FILE* input = freopen("input.txt", "r", stdin);
    FILE* output = freopen("output.txt", "w", stdout);
    int N, M;
    int count = 1;
    scanf("%d %d", &N, &M);
    for(int i = 0; i <= M; i++){
        graph[i].len = 0;
        trans_graph[i].len = 0;
    }
    int a, b;
    for(int i = 0; i < M; i++){
        scanf("%d %d", &a, &b);
        graph[a].arr = (int*) realloc(graph[a].arr, (graph[a].len+1)*sizeof(int));
        trans_graph[b].arr = (int*) realloc(trans_graph[b].arr, (trans_graph[b].len+1)*sizeof(int));
        graph[a].arr[graph[a].len] = b;
        graph[a].len++;
        trans_graph[b].arr[trans_graph[b].len] = a;
        trans_graph[b].len++;
    }
//    for(int i = 0; i <= N; i++){
//        used[i] = 0;
//    }
    for(int i = 1; i <= N; i++){
        if(used[i] == 0){
            dfs1(i);
        }
    }
    for(int i = 0; i <= N; i++){
        used[i] = 0;
    }
    for(int i = N-1; i >= 0; i--){
        int v = order[i];
        if(used[v] == 0){
            dfs2(v, count);
            count++;
        }
    }


    printf("%d\n", count-1);
    for(int i = 1; i <= N; i++){
        printf("%d ", component[i]);
    }
    return 0;
}