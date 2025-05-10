#include <stdio.h>
#include <stdlib.h>

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