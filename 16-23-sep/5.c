#include <stdio.h>
#include <errno.h>

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

    int count = 0;
    char symbol = '.';
    int flag = 0;
    while ((symbol = fgetc(input)) != EOF)
    {
        if (symbol != '.' && symbol != '\n' && symbol != ' ')
        {
            if (flag == 0)
            {
                // printf("start %c %d %d\n", symbol, count, flag);
                flag = 1;
                count += 1;
                // printf("stop %c %d %d\n", symbol, count, flag);}
            }
        }
        else
        {
            // printf("\n not start %c %d %d\n", symbol, count, flag);
            flag = 0;
        }
    }

    fprintf(output, "%d", count);

    fclose(input);
    fclose(output);
    return 0;
}