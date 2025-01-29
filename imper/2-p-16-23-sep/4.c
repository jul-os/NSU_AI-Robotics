#include <stdio.h>
#include <errno.h>

// Функция для проверки, могут ли два прямоугольника B и C поместиться в прямоугольник A
int fits(int a1, int a2, int b1, int b2, int c1, int c2)
{
    // Вариант 1: B и C расположены по горизонтали
    if ((b1 + c1 <= a1 && (b2 > c2 ? b2 : c2) <= a2) || (b2 + c2 <= a2 && (b1 > c1 ? b1 : c1) <= a1))
    {
        return 1;
    }
    // Вариант 2: B и C расположены по вертикали
    if ((b1 + c2 <= a1 && (b2 > c1 ? b2 : c1) <= a2) || (b2 + c1 <= a2 && (b1 > c2 ? b1 : c2) <= a1))
    {
        return 1;
    }
    return 0;
}

int main(void)
{

    FILE *input, *output;
    input = fopen("input.txt", "r");
    output = fopen("output.txt", "w");

    if (input == NULL || output == NULL)
    {

        perror("fopen()");
        return errno;
    }

    int a1, a2, b1, b2, c1, c2;
    if (fscanf(input, "%d %d %d %d %d %d", &a1, &a2, &b1, &b2, &c1, &c2) == 6)
    {
        // Проверяем все повороты прямоугольников B и C
        if (fits(a1, a2, b1, b2, c1, c2) || fits(a1, a2, b2, b1, c1, c2) ||
            fits(a1, a2, b1, b2, c2, c1) || fits(a1, a2, b2, b1, c2, c1))
        {
            fprintf(output, "YES");
        }
        else
        {
            fprintf(output, "NO");
        }
    }
    fclose(input);
    fclose(output);
    return 0;
}