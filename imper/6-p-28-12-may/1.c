#include <stdio.h>

// Backtracking

FILE *in, *out;

#define MAX_SIZE 12

char field[MAX_SIZE][MAX_SIZE];
int answer[MAX_SIZE]; // текущее решение храним глобально

int hasV[MAX_SIZE], hasP[MAX_SIZE * 2], hasM[MAX_SIZE * 2];

int quantityCol, quantityRow;

// время работы O(N!)
int Rec(int curRow)
{ // (перебирает все варианты для col начиная с r-ого)
    for (int col = 0; col < quantityCol; col++)
    {
        if (hasV[col] ||          // отсечение: если вертикаль или диагонали уже бьются
            hasP[curRow + col] || // тогда ставить ферзя НЕ будем
            hasM[curRow - col + MAX_SIZE] ||
            field[curRow][col] == '.')
        {
            continue;
        }
        /*
        Если можно ставим ферзя и отмечаем занятые линии
        переходим к следующей строке
        дошли до последней строки решение найдено
         не нашли возвращаемся и пробуем другие варианты
        */
        answer[curRow] = col;
        hasV[col] = hasP[curRow + col] = hasM[curRow - col + MAX_SIZE] = 1;

        if (curRow == quantityRow - 1 ||
            Rec(curRow + 1) == 1)
        { // условие выхода
            return 1;
        }

        hasV[col] = hasP[curRow + col] = hasM[curRow - col + MAX_SIZE] = 0;
    }
    return 0;
}

void prepare()
{
    in = freopen("input.txt", "r", stdin);
    out = freopen("output.txt", "w", stdout);

    scanf("%d %d", &quantityRow, &quantityCol);
    for (int row = 0; row < quantityRow; ++row)
        for (int col = 0; col < quantityCol;)
        {
            char tmp;
            scanf("%c", &tmp);

            if (tmp != '.' && tmp != '?')
            {
                continue;
            }

            field[row][col] = tmp;
            col++;
        }
}

void print()
{
    if (Rec(0) == 1)
    { //
        printf("YES\n");
        for (int row = 0; row < quantityRow; row++)
        {
            for (int col = 0; col < quantityCol; col++)
            {
                if (col == answer[row])
                {
                    printf("X");
                }
                else
                {
                    printf(".");
                }
            }
            printf("\n");
        }
    }
    else
    {
        printf("NO\n");
    }
}

int main()
{
    prepare();
    print();
    return 0;
}
