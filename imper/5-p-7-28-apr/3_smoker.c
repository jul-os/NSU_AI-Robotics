#include <stdio.h>
#include <stdlib.h>

FILE *in, *out;

char *field;
int **lenWay;
int length, height, startX, startY, finishX, finishY;
char **labyrinth;

void DFS(int row, int col) {
    // Проверка слева
    if (col > 0 && (labyrinth[row][col - 1] == '.' || labyrinth[row][col - 1] == 'F') &&
        (lenWay[row][col - 1] > lenWay[row][col] + 1 || lenWay[row][col - 1] == -1)) {
        lenWay[row][col - 1] = lenWay[row][col] + 1;
        DFS(row, col - 1);
    }

    // Проверка справа
    if ((length - col) > 1 && (labyrinth[row][col + 1] == '.' || labyrinth[row][col + 1] == 'F') &&
        (lenWay[row][col + 1] > lenWay[row][col] + 1 || lenWay[row][col + 1] == -1)) {
        lenWay[row][col + 1] = lenWay[row][col] + 1;
        DFS(row, col + 1);
    }

    // Проверка сверху
    if (row > 0 && (labyrinth[row - 1][col] == '.' || labyrinth[row - 1][col] == 'F') &&
        (lenWay[row - 1][col] > lenWay[row][col] + 1 || lenWay[row - 1][col] == -1)) {
        lenWay[row - 1][col] = lenWay[row][col] + 1;
        DFS(row - 1, col);
    }

    //Проверка снизу
    if ((height - row) > 1 && (labyrinth[row + 1][col] == '.' || labyrinth[row + 1][col] == 'F') &&
        (lenWay[row + 1][col] > lenWay[row][col] + 1 || lenWay[row + 1][col] == -1)) {
        lenWay[row + 1][col] = lenWay[row][col] + 1;
        DFS(row + 1, col);
    }
}

void cleanUp(){
    for (int i = 0; i < height; i++) {
        free(labyrinth[i]);
    }

    for (int i = 0; i < height; i ++) {
        free(lenWay[i]);
    }

    free(lenWay);
    free(labyrinth);
}

void prepare(){
    labyrinth = (char **) malloc(height * sizeof(char *));

    for (int i = 0; i < height; i++) {
        labyrinth[i] = (char *) malloc(length * sizeof(char));
    }

    lenWay = (int **) malloc(height * sizeof(int *));

    for (int i = 0; i < height; i ++) {
        lenWay[i] = (int *) malloc(length * sizeof(int));
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < length; j++) {
            lenWay[i][j] = -1;
        }
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
                }

                if (buffer == 'S') {
                    startX = col;
                    startY = row;
                }
                col ++;
            }
        }
    }

    lenWay[startY][startX] = 0;
}

int main() {
    in = fopen("input.txt", "r");
    out = fopen("output.txt", "w");

    fscanf(in, "%d %d\n", &height, &length);
    prepare();
    fillLabyrinth();
    DFS(startY, startX);
    fprintf(out, "%d\n", lenWay[finishY][finishX]);
    cleanUp();

    return 0;
}