#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

int calcLetters(char *iStr, int *oLowerCnt, int *oUpperCnt, int *oDigitsCnt)
{
    int len = strlen(iStr);
    for (int i = 0; i < len; i++)
    {
        if islower (iStr[i])
        {
            *oLowerCnt += 1;
        }
        if isupper (iStr[i])
        {
            *oUpperCnt += 1;
        }
        if isdigit (iStr[i])
        {
            *oDigitsCnt += 1;
        }
    }

    return len;
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
    // по строке
    // в функцию, ctype
    // возвращ значения и вывод

    char line[100 + 1];
    int i = 1, len = 0;
    while (fgets(line, sizeof(line), input))
    {
        if (strlen(line) > 0 && line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0';
        }
        int low_count = 0, up_count = 0, digit_count = 0;
        printf("%s\n", line);
        len = calcLetters(line, &low_count, &up_count, &digit_count);
        printf("Line %d has %d chars: %d are letters (%d lower, %d upper), %d are digits.\n", i, len, low_count + up_count, low_count, up_count, digit_count);
        i++;
    }
    fclose(input);
    fclose(output);
    return 0;
}