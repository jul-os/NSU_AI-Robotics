#include <stdio.h>
#include <errno.h>
#include <string.h>

#define MAX_LEN_OF_CONCATENATED_LINE 100 * 10000

int len(char *array)
{
    int len_arr = 0;
    for (int i = 0; array[i]; i++)
    {
        len_arr += 1;
    }
    return len_arr;
}

char *concat(char *pref, char *suff)
{
    int a = len(pref);
    int b = len(suff);
    int i = 0;
    while (i < b)
    {
        pref[a + i] = suff[i];
        i += 1;
    }
    return pref;
}

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
    int n;
    char line[MAX_LEN_OF_CONCATENATED_LINE];
    int factual_len = 0;
    if (scanf("%d", &n) == 1)
    {

        // for (int i = 0; i < n; i++){
        //     char  temp_line[100];
        //     scanf("%s", temp_line);
        //     temp_line[-1] = '\0';
        //     printf("%s", temp_line);
        // } вот это уже решает

        char first_line[100];
        scanf("%s", first_line);
        strcpy(line, first_line);
        for (int i = 1; i < n; i++)
        {
            char temp_line[100];
            scanf("%s", temp_line);
            temp_line[-1] = '\0';
            concat(line, temp_line);
        }
        printf("%s", line);
    }

    fclose(input);
    fclose(output);
    return 0;
}