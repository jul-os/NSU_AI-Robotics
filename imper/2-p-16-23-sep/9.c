#include <stdio.h>
#include <errno.h>
// тут костыль на костыле и с костылем сверху

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
    char curr;
    char read;
    int flag = 0; // flag нужен для случая, когда внутри /* */ появляется еще /*, но не закрывается
    /*flag = 0 if there's no '/'
    if flag = 1 and we have '//' or '/*', we work pn comments
    flag = 2, нужно удалять
    */
    while (1)
    {
        if (scanf("%c", &curr) != 1)
        {
            // конец файла
            break;
        }

        else if (curr == '/')
        {
            flag = 0;
            if (scanf("%c", &read) != 1)
            {
                printf("%c", curr);
                break;
            }

            if (read == '/')
            // нашли //
            {
                while (1)
                {
                    if (scanf("%c", &read) != 1)
                    {
                        break;
                    }

                    if (read == '\n')
                    {
                        printf("\n");
                        break;
                    }

                    printf(" ");
                }
            }

            else if (read == '*')
            // нашли /*
            {
                flag = 1;
                while (1)
                {
                    if (scanf("%c", &curr) != 1)
                    {
                        break;
                    }
                    if (curr == '\n')
                    {
                        printf("\n");
                    }
                    else if (curr == '*')
                    {
                        scanf("%c", &read);
                        if (read == '/')
                        {
                            printf("  ");
                            break;
                        }
                        else
                        {
                            if (flag != 1)
                            {
                                printf("%c", curr);
                            }
                        }
                    }
                    else
                    {
                        printf(" ");
                    }
                }
            }

            else
            {
                printf("%c%c", curr, read);
            }
        }

        else
        {
            // печать всего остального
            printf("%c", curr);
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}