#pragma comment(linker, " / STACK :50000000 ")
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 500000

double parseMulSub(char *str, int *index, int *skip_index);
double parseExpr(char *str, int *index, int *skip_index);
double parseAtom(char *str, int *index, int *skip_index);

void readToken(char *str, char *token, int *index, int *skip_index)
{
    while (str[*index] == ' ') { // Пропускаем пробелы
        ++(*index);
    }
    *skip_index = *index;

    if (str[*index] == '\0') { // Конец строки
        token[0] = '\0';
        return;
    }

    if (strchr("()+-*/", str[*index])) { // Если это оператор
        token[0] = str[*index];
        token[1] = '\0';
        ++(*index);
        return;
    }

    // Если это число
    int pos = 0;
    while (str[*index] >= '0' && str[*index] <= '9') {
        token[pos++] = str[*index];
        ++(*index);
    }
    token[pos] = '\0'; // Завершаем строку
}


void peekToken(char *str, char *token, int *index, int *skip_index)
{
    readToken(str, token, index, skip_index);
    *index = *skip_index;
}

double parseMulSub(char *str, int *index, int *skip_index)
{
    char token[6];
    double res = 0.0;
    res = parseAtom(str, index, skip_index);
    peekToken(str, token, index, skip_index);
    while ((token[0] == '*' || token[0] == '/') && (token[1] == 0))
    {
        readToken(str, token, index, skip_index);
        if (token[0] == '*')
        {
            res *= parseAtom(str, index, skip_index);
        }
        else
        {
            res /= parseAtom(str, index, skip_index);
        }
        peekToken(str, token, index, skip_index);
    }

    return res;
}

double parseExpr(char *str, int *index, int *skip_index)
{
    char token[6];
    double res = parseMulSub(str, index, skip_index);
    double tmp = 0.0;
    peekToken(str, token, index, skip_index);
    while ((token[0] == '-' && token[1] == 0) || (token[0] == '+' && token[1] == 0))
    {
        readToken(str, token, index, skip_index);
        tmp = parseMulSub(str, index, skip_index);
        if (token[0] == '-' && token[1] == 0)
        {
            tmp *= -1;
        }
        res += tmp;
        peekToken(str, token, index, skip_index);
    }

    return res;
}

double parseAtom(char *str, int *index, int *skip_index)
{
    char token[6];
    double res = 0.0;

    peekToken(str, token, index, skip_index);
    if (token[0] == '(' && token[1] == 0)
    {
        readToken(str, token, index, skip_index);
        res = parseExpr(str, index, skip_index);
        readToken(str, token, index, skip_index);
        return res;
    }
    // знаки типа + по идее уже обработаны, пробелы убраны, скобки взли, остаются только цифры
    else
    {
        readToken(str, token, index, skip_index);
        if (token[0] == '-' && token[1] == 0)
        {
            // попалось отрицаательное число
            res = parseAtom(str, index, skip_index) * -1;
            return res;
        }
        int tmp = atoi(token);
        return tmp;
    }
}

int main()
{
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    char *str = (char *)malloc(SIZE);
    int index = 0, skip_index = 0;
    fgets(str, SIZE, stdin);
    double res = parseExpr(str, &index, &skip_index);
    printf("%0.20lf", res);

    fclose(input);
    fclose(output);
    return 0;
}