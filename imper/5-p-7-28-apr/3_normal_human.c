#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int row, col;
} Point;

FILE *in, *out;
char **labyrinth;
int **lenWay;
int length, height, startX, startY, finishX, finishY;

Point *queue;
int front = 0, rear = 0;

void BFS() {
    // Начальная позиция
    queue[rear++] = (Point){startY, startX};
    lenWay[startY][startX] = 0;

    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    while (front < rear) {
        Point current = queue[front++];

        for (int i = 0; i < 4; i++) {
            int newRow = current.row + dr[i];
            int newCol = current.col + dc[i];

            if (newRow < 0 || newRow >= height || newCol < 0 || newCol >= length)
                continue;

            if ((labyrinth[newRow][newCol] == '.' || labyrinth[newRow][newCol] == 'F') 
                && lenWay[newRow][newCol] == -1) {
                
                lenWay[newRow][newCol] = lenWay[current.row][current.col] + 1;
                queue[rear++] = (Point){newRow, newCol};

                if (newRow == finishY && newCol == finishX)
                    return;
            }
        }
    }
}

void cleanUp() {
    for (int i = 0; i < height; i++) {
        free(labyrinth[i]);
        free(lenWay[i]);
    }
    free(labyrinth);
    free(lenWay);
    free(queue);
}

void prepare() {
    labyrinth = (char **)malloc(height * sizeof(char *));
    lenWay = (int **)malloc(height * sizeof(int *));
    queue = (Point *)malloc(height * length * sizeof(Point));

    for (int i = 0; i < height; i++) {
        labyrinth[i] = (char *)malloc(length * sizeof(char));
        lenWay[i] = (int *)malloc(length * sizeof(int));
        for (int j = 0; j < length; j++)
            lenWay[i][j] = -1;
    }
}

void fillLabyrinth() {
    for (int row = 0; row < height; row++) {
        int col = 0;
        while (col < length) {
            char buffer;
            fscanf(in, "%c", &buffer);

            if (buffer == '.' || buffer == 'F' || buffer == 'S' || buffer == 'X') {
                labyrinth[row][col] = buffer;

                if (buffer == 'F') {
                    finishX = col;
                    finishY = row;
                } else if (buffer == 'S') {
                    startX = col;
                    startY = row;
                }
                col++;
            }
        }
    }
}

int main() {
    in = fopen("input.txt", "r");
    out = fopen("output.txt", "w");

    fscanf(in, "%d %d\n", &height, &length);
    prepare();
    fillLabyrinth();
    BFS();
    fprintf(out, "%d\n", lenWay[finishY][finishX]);
    cleanUp();

    return 0;
}