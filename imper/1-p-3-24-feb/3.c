#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 100

void parse_and_print_args(const char *line, FILE *output)
{
    char *ptr = line;
    while (*ptr)
    { // проходим мимо пробелов
        while (*ptr == ' ')
            ptr++;
        if (!*ptr)
            break;

        if (*ptr == '"')
        {
            // Обработка параметра в кавычках
            ptr++;
            const char *start = ptr;
            while (*ptr && *ptr != '"')
                ptr++;
            fprintf(output, "[%.*s]\n", (int)(ptr - start), start);
            if (*ptr == '"')
                ptr++;
        }
        else
        {
            // Обработка параметра без кавычек
            const char *start = ptr;
            while (*ptr && *ptr != ' ')
                ptr++;
            fprintf(output, "[%.*s]\n", (int)(ptr - start), start);
        }
    }
}

int main()
{
    char line[MAX_LEN + 1] = {0};

    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");
    fgets(line, sizeof(line), input);

    // Удаление символа новой строки
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n')
    {
        line[len - 1] = '\0';
    }

    parse_and_print_args(line, output);
    fclose(output);
    fclose(input);

    return 0;
}
