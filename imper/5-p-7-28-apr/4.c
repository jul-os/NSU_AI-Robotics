#include <stdio.h>
#include <stdlib.h>

FILE *in, *out;

#define WHITE 0
#define GRAY 1
#define BLACK 2

typedef struct Node {
    int vertex;
    struct Node *next;
} Node;

typedef struct {
    Node *incidentVertices;
    int answer;
    char color;
} Vertex;

Vertex *vertices;
int answer = 1, quantityVariables, quantityExpressions, flag = 1;

void append(Node **listVertices, int vertex) { // заполнение списка инцидентности конкретной вершины
    Node *buffer = (Node *) malloc(sizeof(Node));

    buffer->next = *listVertices;
    buffer->vertex = vertex;

    *listVertices = buffer;
}

int BFS(Vertex *variable) { // поиск в ширину
    variable->color = GRAY;

    Node *buffer = variable->incidentVertices;

    while (buffer != NULL) {
        if (vertices[buffer->vertex].color == GRAY) {
            return 0;
        }

        if ((vertices[buffer->vertex].color == WHITE) &&
            (BFS(vertices + buffer->vertex) == 0)) {
            return 0;
        }

        buffer = buffer->next;
    }

    variable->answer = answer++;

    variable->color = BLACK;

    return 1;
}

void prepare() { // подготовка
    vertices = (Vertex *) calloc(quantityVariables + 1, sizeof(Vertex));

    for (int i = 0; i < quantityExpressions; i++) {
        int from, to;
        scanf("%d %d", &from, &to);

        append(&vertices[to].incidentVertices, from);
    }
}

void freeUp() {
    for (int i = 0; i < quantityVariables; i++) {
        Node *next, *buffer = vertices[i].incidentVertices; //смешно но с i+1 тоже проходит бот
        //но вроде бы такк правильнее
        while (buffer != NULL) {
            next = buffer->next;
            free(buffer);
            buffer = next;
        }
    }
    free(vertices);
}

int main() {
    in = freopen("input.txt", "r", stdin);
    out = freopen("output.txt", "w", stdout);

    scanf("%d %d", &quantityVariables, &quantityExpressions);

    prepare();

    // поиск в ширину
    for (int i = 1; i <= quantityVariables; i++) {
        if (vertices[i].color == WHITE) {
            if (BFS(&vertices[i]) == 0) {
                flag = 0;
                break;
            }
        }
    }

    // вывод
    if (flag == 0) {
        printf("NO\n");
    } else {
        printf("YES\n");
        for (int i = 1; i <= quantityVariables; i++) {
            printf("%d ", vertices[i].answer);
        }
    }

    freeUp();

    return 0;
}