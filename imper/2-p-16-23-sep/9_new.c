#include <stdio.h>
#include <errno.h>

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

    int flag_one_line_comment = 0;
    // 1 if waiting for /n
    int flag_many_lines_comment = 0;
    // 1 if waiting for */
    char curr;
    char next;

    while (1)
    {
        if (scanf("%c", &curr) != 1)
        {
            // конец файла
            break;
        }
        if (curr == '/' && scanf("%c", &next) == 1)
        {
            if (next == '/' && flag_many_lines_comment == 0)
            {
                flag_one_line_comment = 1;
            }
            else if (next == '*' && flag_one_line_comment == 0)
            {
                flag_many_lines_comment = 1;
            }
            else
            {
                ungetc(next, input);
            }
        }
        if (flag_many_lines_comment == 1)
        {
            if (curr == '*' && scanf("%c", &next) == 1 && next == '/')
            {
                flag_many_lines_comment = 0;
            }
            else if (curr == '\n')
            {
                printf("\n");
            }
        }
        else if (flag_one_line_comment == 1)
        {
            if (curr == '\n')
            {
                printf("\n");
                flag_one_line_comment = 0;
            }
        }
        else
        {
            printf("%c", curr);
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}