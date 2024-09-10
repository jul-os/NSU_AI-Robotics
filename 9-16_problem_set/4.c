#include <stdio.h>

const char *fourth_problem(int n)
{
    /*because from the function returning is a string
    defined in double quotes, which is a constant
    and char* is a string, rhis points t the beginning
    of the string in our memory */
    if (n == 1)
    {
        return "NO";
    }
    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
        {
            return "NO";
        }
    }
    return "YES";
}

int main(void)
{
    FILE *input, *output;
    input = fopen("input.txt", "r");
    output = fopen("output.txt", "w");

    if ((input == NULL) || (output == NULL))
    {
        return 1;
    }

    int n;

    if (fscanf(input, "%d", &n) == 1)
    {
        fprintf(output, "%s\n", fourth_problem(n));
    }

    fclose(input);
    fclose(output);

    return 0;
}
