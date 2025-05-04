#include <stdio.h>
#include <stdlib.h>

int size;

void DFS(char **labyrinth, int row, int col) { // поиск в глубину на лабиринте
    labyrinth[row][col] = '*'; // красим текущую ячейку, чтобы пометить, что мы тут были

    if (col > 0 && labyrinth[row][col - 1] == ' ') { // проверка слева
        DFS(labyrinth, row, col - 1);
    }

    if ((size - col) > 0 && labyrinth[row][col + 1] == ' ') { // проверка справа
        DFS(labyrinth, row, col + 1);
    }

    if (row > 0 && labyrinth[row - 1][col] == ' ') { // проверка сверху
        DFS(labyrinth, row - 1, col);
    }

    if ((size - row) > 0 && labyrinth[row + 1][col] == ' ') { // проверка снизу
        DFS(labyrinth, row + 1, col);
    }
}

int main() {
    FILE *in, *out;

    in = fopen("input.txt", "r");
    out = fopen("output.txt", "w");

    int counter = 0;

    if (fscanf(in, "%d", &size) != 1) {}

    char **labyrinth = (char **) malloc(size * sizeof(char *));

    for (int i = 0; i < size; i++) {
        labyrinth[i] = (char *) malloc(size * sizeof(char *));
    }

    for (int row = 0; row < size; row++) {
        int col = 0;
        while (col < size) {
            char buffer;

            fscanf(in, "%c", &buffer);

            if (buffer == ' ' || buffer == '*') {
                labyrinth[row][col] = buffer;
                col++;
            }
        }
    }

    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            if (labyrinth[row][col] == ' ') {
                counter++;
                DFS(labyrinth, row, col);
            }
        }
    }

    fprintf(out, "%d\n", counter - 1);

    for (int i = 0; i < size; i++) {
        free(labyrinth[i]);
    }
    free(labyrinth);
    
    return 0;
}
