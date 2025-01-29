#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LEN 29
// len of 10^9 in binary

int my_pow(int what, int to_which)
{
    int th = what;
    if (to_which == 0)
    {
        return 1;
    }
    else if (to_which == 1)
    {
        return what;
    }
    for (int i = 2; i < to_which + 1; i++)
    {
        what *= th;
    }
    return what;
}

int char_num_to_int(char digit)
{
    if ('0' <= digit && digit <= '9')
    {
        return digit - '0';
    }
    else if ('a' <= digit <= 'z')
    {
        return digit - 'a' + 10;
    }
}

char int_num_to_char(int n)
{
    if (0 <= n && n <= 9)
    {
        return n + '0';
    }
    else if (10 <= n <= 26)
    {
        return n + 'a' - 10;
    }
}

void reverse(char start[], int len)
{
    for (int i = 0; i < len / 2; i++)
    {
        char temp = start[i];
        start[i] = start[len - 1 - i];
        start[len - 1 - i] = temp;
    }
}

//  from any system to decimal
int to_decimal(char *num, int system)
{
    int res = 0, len;
    len = strlen(num);
    for (int i = 0; i < len; i++)
    {
        int digit = char_num_to_int(num[i]);
        res += digit * my_pow(system, len - i - 1);
    }
    return res;
}

// from decimal to any system
char *to_new(int num, int system)
{
    char *new_num = malloc(MAX_LEN * (sizeof(char)));
    int remain = 100000, i = 0;
    while (num > 0)
    {
        remain = num % system;
        new_num[i] = int_num_to_char(remain);
        num = num / system;
        i += 1;
    }
    new_num[i + 1] = '\0';
    reverse(new_num, strlen(new_num));
    return new_num;
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
    int p, q, i = 0;
    scanf("%d %d", &p, &q);
    // printf("%d %d\n", p, q);
    getchar();
    char *num = malloc(MAX_LEN * (sizeof(char)));
    char *new_num;
    int dec_num;
    char temp;
    while (scanf("%c", &temp) == 1)
    {
        num[i] = temp;
        i += 1;
    }
    num[i] = '\0';

    // printf("%s\n", num);
    dec_num = to_decimal(num, p);
    new_num = to_new(dec_num, q);

    // printf("%d\n", dec_num);
    for (int i = 0; new_num[i]; i++)
    {
        printf("%c", new_num[i]);
    }

    free(num);
    fclose(input);
    fclose(output);
    return 0;
}