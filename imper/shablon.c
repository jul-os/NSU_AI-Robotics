#include <stdio.h>
#include <errno.h>

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
    scanf("%d", &n);

    printf("%d", n);

    fclose(input);
    fclose(output);
    return 0;
}

// malloc
/*
int *x_es = malloc((n + 1) * sizeof(int));

        if (x_es == NULL)
        {
            perror("malloc failed");
            return errno;
        }

    free(x_es)

*/