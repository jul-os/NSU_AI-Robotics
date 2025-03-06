//т евклида или малая т ферма

#include <stdio.h>
#include <errno.h>

long long power_mod(long long a, long long b, long long m) {
    long long res = 1;
    while (b > 0) {
        if (b % 2 == 1) // Если текущий бит b == 1
            res = (res * a) % m;
        a = (a * a) % m;
        b /= 2;
    }
    return res;
}

int mod_inverse_fermat(int a, int m) {
    if (a == 0)
        return -1; 
    return (int)power_mod(a, m - 2, m); // Малую теорему Ферма
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
        B = mod_inverse_fermat(A, M);
        printf("%d\n", B);
    }

    fclose(input);
    fclose(output);
    return 0;
}