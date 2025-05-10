#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    int vertex;
    struct Node *next;
} Node;

typedef struct
{
    Node *incidentVertices;
    int pathLength;
    char visited;
} Vertex;

Vertex *vertices;
FILE *input;
int quantityVerticesRaw, quantityEdgesRaw;
int *queue;

void append(Node **listVertices, int vertex)
{ // заполнение списка инцидентности конкретной вершины
    Node *buffer = (Node *)malloc(sizeof(Node));

    buffer->next = *listVertices;
    buffer->vertex = vertex;

    *listVertices = buffer;
}

void BFS()
{ // поиск в ширину
    int queuePosition = 1;

    for (int i = 0; i < quantityVerticesRaw; i++)
    {
        Node *buffer = vertices[queue[i]].incidentVertices;
        while (buffer != NULL)
        {
            if (vertices[buffer->vertex].visited == 0)
            {
                queue[queuePosition] = buffer->vertex;
                vertices[buffer->vertex].visited = 1;
                vertices[buffer->vertex].pathLength = vertices[queue[i]].pathLength + 1;
                queuePosition++;
            }
            buffer = buffer->next;
        }
    }
}

void initQueue(int quantityVertices)
{ // инициализация очереди
    queue = (int *)calloc(quantityVertices, sizeof(int));
    queue[0] = 1;
}

void initVertices(int quantityVertices, int quantityEdges)
{ // инициализация списка вершин
    vertices = (Vertex *)calloc(quantityVertices + 1, sizeof(Vertex));

    for (int i = 0; i < quantityEdges; i++)
    {
        int from, to;
        fscanf(input, "%d %d", &from, &to);

        append(&vertices[from].incidentVertices, to);
    }

    for (int i = 1; i <= quantityVertices; i++)
    {
        vertices[i].pathLength = -1;
    }

    vertices[1].pathLength = 0;
    vertices[1].visited = 1;
}

void prepare()
{ // подготовка к запуску БФС
    input = fopen("input.txt", "r");
    fscanf(input, "%d %d", &quantityVerticesRaw, &quantityEdgesRaw);
    initQueue(quantityVerticesRaw);
    initVertices(quantityVerticesRaw, quantityEdgesRaw);
}

void print(FILE *out, int quantityVertices)
{ // вывод результата
    for (int i = 1; i <= quantityVertices; i++)
    {
        fprintf(out, "%d\n", vertices[i].pathLength);
    }
}

void cleanUp(int quantityVertices)
{ // чистка памяти
    for (int i = 1; i <= quantityVertices; i++)
    {
        free(vertices[i].incidentVertices);
    }
    free(vertices);
    free(queue);
}

int main()
{
    prepare();
    BFS();
    print(fopen("output.txt", "w"), quantityVerticesRaw);
    cleanUp(quantityVerticesRaw);
    return 0;
}
