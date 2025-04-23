#include "sol.h"
#include <stdio.h>

// Тестовые данные
#define MAX_EDGES 6

Edge test_edges[MAX_EDGES] = {
    {0, 2, 178},
    {3, 4, 207},
    {1, 1, 356},  // Петля
    {2, 0, 101},
    {4, 1, 286},
    {4, 1, 213}
};

int current_edge = 0;  // Текущее ребро для чтения

// Заглушка для getVerticesCount()
int getVerticesCount() {
    return 5;  // В графе 5 вершин
}

// Заглушка для readEdge()
int readEdge(Edge *oEdge) {
    if (current_edge >= MAX_EDGES) {
        return 0;  // Рёбра закончились
    }
    *oEdge = test_edges[current_edge];
    current_edge++;
    return 1;
}

// Тестовая функция для проверки
void test() {
    init();  // Инициализируем граф

    // Проверяем количество рёбер у вершины 0 (должно быть 2)
    printf("EdgesCount(0) = %d\n", getEdgesCount(0));

    // Печатаем рёбра, инцидентные вершине 0
    for (int i = 0; i < getEdgesCount(0); i++) {
        Edge e = getIncidentEdge(0, i);
        printf("Edge %d: [%d, %d, %d]\n", i, e.from, e.to, e.weight);
    }

    // Проверяем петлю у вершины 1 (должна быть один раз, но в списке дважды)
    printf("\nEdgesCount(1) = %d\n", getEdgesCount(1));
    for (int i = 0; i < getEdgesCount(1); i++) {
        Edge e = getIncidentEdge(1, i);
        printf("Edge %d: [%d, %d, %d]\n", i, e.from, e.to, e.weight);
    }
}

int main() {
    test();
    return 0;
}