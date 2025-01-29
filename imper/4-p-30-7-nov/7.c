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
    int characters[126 - 32 + 1] = {0};
    char c;
    while (scanf("%c", &c) != EOF)
    {
        if (' ' <= c && c <= '~')
        {
            characters[c - ' '] += 1;
            // printf("%c %d\n", c, c - ' ');
        }
    }
    // printf("\n");
    int last_non_zero_index = -1;
    for (int i = 0; i < 126 - 32 + 1; i++)
    {
        if (characters[i] > 0)
        {
            last_non_zero_index = i;
        }
    }

    for (int i = 0; i < 126 - 32 + 1; i++)
    {

        if (characters[i] > 0)
        {
            printf("%c ", i + ' ');
            for (int j = 0; j < characters[i]; j++)
            {
                printf("#");
            }
            if (i < last_non_zero_index)
            {
                printf("\n");
            }
        }
    }
    fclose(input);
    fclose(output);
    return 0;
}