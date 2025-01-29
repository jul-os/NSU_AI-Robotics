#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int count_char(char *str, char symbol)
{
    int count = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == symbol)
        {
            count++;
        }
    }
    return count;
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
    scanf("%d", &n);
    int type, len, index = 0, where;
    // массив указателей на строки
    char **strings = (char **)malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++)
    {
        scanf("%d ", &type);
        if (type == 0)
        {
            scanf("%d ", &len);
            // make
            char *str = malloc(len * (sizeof(char)));
            fgets(str, len + 1, input);
            str[strcspn(str, "\n")] = '\0'; // Удаляем символ новой строки, если он есть
            // function strcspn() calculates the length of the number of characters before the 1st occurrence of character present in both the string.
            strings[index++] = str;
        }
        else if (type == 1)
        {
            scanf("%d", &where);
            free(strings[where]);
            strings[where] = NULL;
        }
        else if (type == 2)
        {
            scanf("%d", &where);
            printf("%s\n", strings[where]);
        }
        else if (type == 3)
        {
            char what;
            scanf("%d %c", &where, &what);
            printf("%d\n", count_char(strings[where], what));
        }
    }

    for (int i = 0; i < index; i++)
    {
        if (strings[i] != NULL)
        {
            free(strings[i]);
        }
    }
    free(strings);

    fclose(input);
    fclose(output);
    return 0;
}