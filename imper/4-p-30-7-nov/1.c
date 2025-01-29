#include <stdio.h>
#include <errno.h>

void printTime(int h, int m, int s)
{
    printf("%02d:%02d:%02d\n", h, m, s);
}

int main(void)
{

    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);
    // freopen – замена файла связанного с потоком данных
    if (input == NULL || output == NULL)
    {

        perror("fopen()");
        return errno;
    }
    int n, h, m, s;
    if (scanf("%d", &n) == 1)
    {
        for (int i = 0; i < n; i++)
        {
            scanf("%d %d %d", &h, &m, &s);
            printTime(h, m, s);
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}