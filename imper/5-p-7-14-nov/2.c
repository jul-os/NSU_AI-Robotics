#include <stdio.h>
#include <errno.h>

#define MAX_N 1000

double C[MAX_N + 1][MAX_N + 1];

void build_pascals_triangle()
{
    for (int n = 0; n <= MAX_N; n++)
    {
        C[n][0] = 1;
        C[n][n] = 1;
        for (int k = 1; k < n; k++)
        {
            C[n][k] = C[n - 1][k - 1] + C[n - 1][k];
        }
    }
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
    int Q;

    build_pascals_triangle();

    scanf("%d", &Q);

    for (int i = 0; i < Q; i++)
    {
        int n, k;
        scanf("%d %d", &n, &k);

        printf("%0.15g\n", C[n][k]);
    }

    fclose(input);
    fclose(output);
    return 0;
}