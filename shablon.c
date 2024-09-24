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
    int n;
    if (fscanf(input, "%d", &n) == 1)
    {
        //
    }

    fprintf(output, "%d", n);

    fclose(input);
    fclose(output);
    return 0;
}