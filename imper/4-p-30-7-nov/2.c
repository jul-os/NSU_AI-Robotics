#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STR_LEN 101

void reverse(char start[], int len)
{
    char *reversed = malloc(len + 1);
    if (reversed == NULL)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

        for (int i = 0; i < len; i++)
        {
            reversed[len - 1 - i] = start[i];
        }
    reversed[len] = '\0';
    printf("%s", reversed);
    free(reversed);
}

int main(void)
{
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);
    char *line = NULL;

    if (input == NULL || output == NULL)
    {
        perror("fopen()");
        return errno;
    }

    int n;
    size_t max_len = 101;
    if (scanf("%d", &n) == 1)
    {
        getchar(); // считываем символ новой строки после числа
        for (int i = 0; i < n; i++)
        {
            getline(&line, &max_len, stdin);
            line[strcspn(line, "\n")] = '\0';
            reverse(line, strlen(line)); // not line[0], because line already a pointer
            if (i != n - 1){
                printf("\n");
            }
        }
    }

    free(line);
    fclose(input);
    fclose(output);
    return 0;
}