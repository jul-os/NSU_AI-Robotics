#include <stdio.h>
#include <errno.h>
#include <ctype.h>
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
    char buf;
    char string[100 * 10000 + 1];
    int len = 0, up_count = 0, curr = 0;
    while (scanf("%c", &buf) == 1)
    {
        if (isalpha(buf))
        {
            len += 1;
            if (isupper(buf))
            {
                up_count += 1;
            }
            string[curr] = buf;
            curr += 1;
        }
        else
        {
            if (len > 0)
            {
                string[curr] = '\0';
                printf("%d/%d ", up_count, len);
                printf("%s\n", string);
                len = 0;
                string[0] = '\0';
                curr = 0;
                up_count = 0;
            }
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}