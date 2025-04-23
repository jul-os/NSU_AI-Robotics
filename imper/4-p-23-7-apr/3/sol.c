#include "sol.h"
#include <stdlib.h>

#define MAX_VERTICES 300000
#define MAX_EDGES 300000  // Максимальное количество рёбер во входных данных

static Edge edge_buffer[MAX_EDGES];  // Временный буфер для всех рёбер
static Edge edges[MAX_EDGES * 2];    // Основной массив рёбер (каждое ребро учитывается дважды)
static int vertex_edge_start[MAX_VERTICES];  // Начало списка рёбер для вершины
static int vertex_edge_count[MAX_VERTICES];  // Количество рёбер у вершины
static int total_edges = 0;          // Общее количество записанных рёбер

void init() {
    int N = getVerticesCount();
    if (N > MAX_VERTICES) exit(1);

    // 1. Читаем все рёбра в буфер и считаем, сколько рёбер у каждой вершины
    int M = 0;
    Edge edge;
    while (readEdge(&edge)) {
        if (M >= MAX_EDGES) exit(1);
        edge_buffer[M++] = edge;
        
        // Увеличиваем счётчик рёбер для вершины edge.from
        vertex_edge_count[edge.from]++;
        
        // Если ребро не петля, увеличиваем счётчик для вершины edge.to
        if (edge.from != edge.to) {
            vertex_edge_count[edge.to]++;
        } else {
            // Если это петля, учитываем её дважды
            vertex_edge_count[edge.from]++;
        }
    }

    // 2. Вычисляем начальные индексы (префиксные суммы)
    vertex_edge_start[0] = 0;
    for (int i = 1; i < N; i++) {
        vertex_edge_start[i] = vertex_edge_start[i - 1] + vertex_edge_count[i - 1];
        vertex_edge_count[i - 1] = 0;  // Сбрасываем для заполнения
    }
    vertex_edge_count[N - 1] = 0;

    // 3. Заполняем рёбра в основной массив
    for (int i = 0; i < M; i++) {
        Edge e = edge_buffer[i];

        // Добавляем ребро для e.from
        int pos = vertex_edge_start[e.from] + vertex_edge_count[e.from]++;
        edges[pos] = e;

        // Если это не петля, добавляем обратное ребро для e.to
        if (e.from != e.to) {
            pos = vertex_edge_start[e.to] + vertex_edge_count[e.to]++;
            edges[pos] = (Edge){e.to, e.from, e.weight};
        } else {
            // Если это петля, добавляем её ещё раз
            pos = vertex_edge_start[e.from] + vertex_edge_count[e.from]++;
            edges[pos] = e;
        }
    }

    total_edges = vertex_edge_start[N - 1] + vertex_edge_count[N - 1];
}

int getEdgesCount(int iVertex) {
    return vertex_edge_count[iVertex];
}

Edge getIncidentEdge(int iVertex, int iIndex) {
    return edges[vertex_edge_start[iVertex] + iIndex];
}