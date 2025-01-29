#include <stdio.h>
#include <errno.h>
#include <string.h>

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
    char *days[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

    char day[4];
    fgets(day, 4, input);

    // Removing the newline character if it exists
    day[strcspn(day, "\n")] = '\0';

    for (int i = 0; i < 7; i++)
    {
        if (strcmp(days[i], day) == 0) // Correct way to compare strings
        {
            fprintf(output, "%d", i + 1);
        }
    }
    fclose(input);
    fclose(output);
    return 0;
}