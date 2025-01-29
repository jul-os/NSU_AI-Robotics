#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int find_len(int num)
{
    if (num == 0)
    {
        return 1;
    }
    int len = 0;
    while (num)
    {
        len += 1;
        num = num / 10;
    }
    return len;
}

char *make_horisontal(int max_f, int max_rep, int max_l)
{
    char *line = malloc(2 + max_f + 3 + max_rep + 3 + max_l + 2 + 1);
    if (line == NULL)
    {
        perror("malloc()");
        exit(1);
    }
    line[0] = '+';
    line[1] = '-';

    // Скопируем max_f, max_rep, и max_l для правильных циклов
    int i = 2;
    int f_count = max_f;
    while (f_count > 0)
    {
        line[i++] = '-';
        f_count--;
    }
    line[i++] = '-';
    line[i++] = '+';
    line[i++] = '-';

    int rep_count = max_rep;
    while (rep_count > 0)
    {
        line[i++] = '-';
        rep_count--;
    }
    line[i++] = '-';
    line[i++] = '+';
    line[i++] = '-';

    int l_count = max_l;
    while (l_count > 0)
    {
        line[i++] = '-';
        l_count--;
    }

    line[i++] = '-';
    line[i++] = '+';
    line[i] = '\0';

    return line;
}

void make_table(int *repeats, int *fragments, int max_f)
{
    int max_rep = 1, max_l = 0;
    for (int i = 1; i < 1001; i++)
    {
        int len_r = find_len(repeats[i]);
        if (len_r > max_rep)
        {
            max_rep = len_r;
        }
    }
    for (int i = 1; i < 1000 + 1; i++)
    {
        int len_l = find_len(fragments[i]);
        if (len_l > max_l)
        {
            max_l = len_l;
        }
    }
    char *line = make_horisontal(max_f, max_rep, max_l);
    printf("%s\n", line);
    for (int i = 0; i < 1000 + 1; i++)
    {
        if (fragments[i] != 0)
        {
            printf("| ");
            for (int j = 0; j < max_f - find_len(i); j++)
            {
                printf(" ");
            }
            printf("%d | ", i);
            for (int j = 0; j < max_rep - find_len(repeats[i]); j++)
            {
                printf(" ");
            }
            printf("%d | ", repeats[i]);
            for (int j = 0; j < max_l - find_len(fragments[i]); j++)
            {
                printf(" ");
            }
            printf("%d |\n", fragments[i]);
            printf("%s\n", line);
        }
    }
    free(line);
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

    int fragments[1000 + 1] = {0};
    int repeats[1000 + 1] = {0};
    int n;
    int max_f = 0;
    if (scanf("%d", &n) != 1)
    {
        perror("please, input n");
        return errno;
    }

    int frag, len;
    for (int i = 0; i < n; i++)
    {
        if (scanf("%d %d", &frag, &len) != 2)
        {
            perror("scanf()");
            return errno;
        }
        fragments[frag] += len;
        repeats[frag] += 1;
        int frag_l = find_len(frag);
        if (frag_l > max_f)
        {
            max_f = frag_l;
        }
    }

    make_table(repeats, fragments, max_f);

    // free(line);
    fclose(input);
    fclose(output);
    return 0;
}
