//т евклида или малая т ферма

#include <stdio.h>
#include <errno.h>

int find_mod_inverse(int A, int M)
{
    if (A == 0)
    {
        return -1;
    }
    else
    {
        for (int i = 1; i < 100000; i++)
        {
            if ( i >= M){
                return -1;
            }
            if ((A * i - 1) % M == 0)
            {
                return i;
            }
        }
    }
    return -1;
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
    int T, M, A, B;
    scanf("%d %d", &T, &M);
    for (int i = 0; i < T; i++)
    {
        scanf("%d", &A);
        B = find_mod_inverse(A, M);
        printf("%d\n", B);
    }

    fclose(input);
    fclose(output);
    return 0;
}