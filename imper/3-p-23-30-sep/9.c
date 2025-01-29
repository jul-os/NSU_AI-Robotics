#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

void swap(char *one, char *two)
{
    char temp = *one;
    *one = *two;
    *two = temp;
}

void next_permutation(char *word, int n)
{
    // Шаг 1: Найти самое большое i, такое что s[i] < s[i + 1]

    int i = n - 2; // тк идем с конца и сравниваем word[n - 2] < word[n - 1] те на 1-ом проходе предпосл с посл (нумерация с 0)
    while (i >= 0 && word[i] >= word[i + 1])
    {
        i--;
    }
    // Шаг 2: Найти самое большое j, такое что s[j] > s[i]
    int j = n - 1; // начинаем с последнего (нумер с 0)
    while (j >= 0 && word[j] <= word[i])
    {
        j--;
    }
    // Шаг 3: Поменять местами s[i] и s[j]
    swap(&word[i], &word[j]); // &чтобы передать адрес

    // Шаг 4: Развернуть часть строки после i
    // то есть с i + 1 до n - 1
    int start = i + 1;
    int end = n - 1;
    while (start < end)
    {
        swap(&word[start], &word[end]);
        start++;
        end--;
    }
    // Шаг 5: Распечатать
    for (int i = 0; i < n; i++)
    {
        if (i != n - 1)
        {
            printf("%c ", word[i]);
        }
        else
        {
            printf("%c", word[i]);
        }
    }
}

int main(void)
{

    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    if (input == NULL || output == NULL)
    {

        perror("freopen()");
        return errno;
    }
    int n;
    char temp;
    if (scanf("%d", &n) == 1)
    {
        char *rough_word = malloc((n * 2) * sizeof(char));
        char *word = malloc((n + 1) * sizeof(char));
        // TODO посмотреть наормальный способ это все читать
        for (int i = 0; i < n * 2; i++)
        {
            scanf("%c", &temp);
            rough_word[i] = temp;
        }
        //a b c
        //scanf("%c ")
        //или hd - short, потом будет автоматически в чар перекидываться
        int j = 0;
        for (int i = 0; i < n * 2; i++)
        {
            if (rough_word[i] != ' ' && rough_word[i] != '\n' && rough_word[i] != EOF)
            {
                word[j] = rough_word[i];
                j++;
            }
        }

        // гарантируется что следующая перестановка существует
        next_permutation(word, n);
        free(word);
        free(rough_word);
    }

    fclose(input);
    fclose(output);
    return 0;
}