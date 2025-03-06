#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int find(char **str, char *search, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (strcmp(str[i], search) == 0)
        {
            return i;
        }
    }
    return 100;
}

int main(int argc, char *argv[])
{

    long int module = -1, num1 = 0, num2 = 0, res = find(argv, "-m", argc);
    char *instruction_type;
    if (argc == 1)
    {
        fprintf(stderr, "No parameters specified.");
        exit(13);
    }
    if (res != 100)
    {
        if (res == 4)
        {
            module = atoi(argv[5]);
            instruction_type = argv[1];
            num1 = atoi(argv[2]);
            num2 = atoi(argv[3]);
        }
        else
        {
            module = atoi(argv[2]);
            instruction_type = argv[3];
            num1 = atoi(argv[4]);
            num2 = atoi(argv[5]);
        }
    }
    else
    {
        instruction_type = argv[1];
        num1 = atoi(argv[2]);
        num2 = atoi(argv[3]);
    }

    if (strcmp(instruction_type, "add") == 0)
    {
        if (module == -1)
        {
            fprintf(stdout, "%ld", ((num1 + num2)));
        }
        else
        {
            if ((num1 + num2) % module < 0)
            {
                fprintf(stdout, "%ld", ((num1 + num2) % module) + module);
            }
            else
            {
                fprintf(stdout, "%ld", ((num1 + num2) % module));
            }
        }
    }
    else if (strcmp(instruction_type, "sub") == 0)
    {
        if (module == -1)
        {
            fprintf(stdout, "%ld", ((num1 - num2)));
        }
        else
        {
            if ((num1 - num2) % module < 0)
            {
                fprintf(stdout, "%ld", ((num1 - num2) % module) + module);
            }
            else
            {
                fprintf(stdout, "%ld", ((num1 - num2) % module));
            }
        }
    }
    else
    {
        if (module == -1)
        {
            fprintf(stdout, "%ld", ((num1 * num2)));
        }
        else
        {
            if ((num1 * num2) % module < 0)
            {
                fprintf(stdout, "%ld", ((num1 * num2) % module) + module);
            }
            else
            {
                fprintf(stdout, "%ld", ((num1 * num2) % module));
            }
        }
    }
}