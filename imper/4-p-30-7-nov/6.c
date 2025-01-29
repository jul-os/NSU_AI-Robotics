#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#define DEFAULT 0

int readTime(char *iStr, int *oHours, int *oMinutes, int *oSeconds)
{
    if (oHours)
        *oHours = DEFAULT;
    if (oMinutes)
        *oMinutes = DEFAULT;
    if (oSeconds)
        *oSeconds = DEFAULT;

    int i = 0;
    while (iStr[i])
    {
        if (iStr[i] == '|')
        {
            if (oHours)
                *oHours = -1;
            if (oMinutes)
                *oMinutes = -1;
            if (oSeconds)
                *oSeconds = -1;
            return 0;
        }
        i += 1;
    }
    if (oSeconds && oMinutes)
    {
        sscanf(iStr, "%d:%d:%d", oHours, oMinutes, oSeconds);
    }
    if (!oSeconds && !oMinutes)
    {
        sscanf(iStr, "%d", oHours);
    }
    else if (!oSeconds)
    {
        sscanf(iStr, "%d:%d", oHours, oMinutes);
    }

    if ((oHours && (*oHours < 0 || *oHours > 23)) ||
        (oMinutes && (*oMinutes < 0 || *oMinutes > 59)) ||
        (oSeconds && (*oSeconds < 0 || *oSeconds > 59)))
    {
        if (oHours)
            *oHours = -1;
        if (oMinutes)
            *oMinutes = -1;
        if (oSeconds)
            *oSeconds = -1;
        return 0;
    }

    return 1;
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

    char *time = malloc(100 * sizeof(char));
    if (time == NULL)
    {
        perror("malloc failed");
        return errno;
    }

    fgets(time, 100, input);
    int hours, min, sec;

    int res1 = readTime(time, &hours, &min, &sec);
    printf("%d %d %d %d\n", res1, hours, min, sec);
    if (res1 == 1)
    {
        int res2 = readTime(time, &hours, &min, NULL);
        printf("%d %d %d\n", res2, hours, min);

        int res3 = readTime(time, &hours, NULL, NULL);
        printf("%d %d", res3, hours);
    }
    else{
        printf("0 -1 -1\n0 -1");
    }
    free(time);

    fclose(input);
    fclose(output);
    return 0;
}