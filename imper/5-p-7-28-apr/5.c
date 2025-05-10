#include <stdio.h>
#include <stdlib.h>

#define WHITE 0
#define GRAY 1
#define BLACK 2

typedef struct Node {
    int vertex;
    struct Node *next;
} Node;

typedef struct {
    Node *incidentVertices;
    int parent;
    int color;
} Vertex;

Vertex *vertices;
FILE *in, *out;
int cycleIndex, quantityVertices, quantityEdges;

void append(Node **listVertices, int vertex) { // заполнение списка инцидентности конкретной вершины
    Node *buffer = (Node *) malloc(sizeof(Node));

    buffer->next = *listVertices;
    buffer->vertex = vertex;

    *listVertices = buffer;
}

int DFS(int index) { // поиск вглубину
    vertices[index].color = GRAY;
    Node *buffer = vertices[index].incidentVertices;

    while (buffer != NULL) {
        if (vertices[buffer->vertex].color == GRAY) {
            vertices[buffer->vertex].parent = index;
            return index;
        }

        if (vertices[buffer->vertex].color == WHITE) {
            vertices[buffer->vertex].parent = index;
            int foundVertex = DFS(buffer->vertex);
            if (foundVertex != 0) {
                return foundVertex;
            }
        }

        buffer = buffer->next;
    }

    vertices[index].color = BLACK;
    return 0;
}

void prepare() {
    in = fopen("input.txt", "r");
    out = fopen("output.txt", "w");

    fscanf(in, "%d %d", &quantityVertices, &quantityEdges);

    vertices = (Vertex *) calloc(quantityVertices + 1, sizeof(Vertex));

    for (int i = 0; i < quantityEdges; i++) {
        int from, to;
        fscanf(in, "%d %d", &from, &to);

        append(&vertices[from].incidentVertices, to);
    }
}

void findCycle() {
    // проход по всем вершинам и поиск цикла
    for (int i = 0; i < quantityVertices; i++) {
        if (vertices[i].color == WHITE) {
            cycleIndex = DFS(i);
            if (cycleIndex != 0) {
                break;
            }
        }
    }
}

void print() {
    if (cycleIndex == 0) {
        fprintf(out, "-1\n");
    } else {
        int counter = 1;

        // подсчет элементов в цикле
        for (int i = vertices[cycleIndex].parent; i != cycleIndex; i = vertices[i].parent) {
            counter++;
        }

        fprintf(out, "%d\n", counter); // счетчик
        int reversedVertices[counter];
        reversedVertices[0] = cycleIndex;
        int counter_ = 1;

        for (int i = vertices[cycleIndex].parent; i != cycleIndex; i = vertices[i].parent) {
            reversedVertices[counter_] = i;
            counter_++;
        }

        for (int i = counter_ - 1 ; i >= 0; i -- ) {
            fprintf(out, "%d ", reversedVertices[i]);
        }
    }
}

void freeUp() {
    for (int i = 0; i < quantityVertices; i++) {
        Node *next, *buffer = vertices[i].incidentVertices;
        while (buffer != NULL) {
            next = buffer->next;
            free(buffer);
            buffer = next;
        }
    }
    free(vertices);
}

int main() {
    prepare();
    findCycle();
    print();
    freeUp();
    return 0;
}