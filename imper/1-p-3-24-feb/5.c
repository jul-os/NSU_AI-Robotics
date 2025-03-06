#include <stdio.h>
// алгоритм Евклида

unsigned int greatest_common_divisor(unsigned int a, unsigned int b)
{
    if (a % b == 0)
        return b;
    if (b % a == 0)
        return a;

    if (a > b)
        return greatest_common_divisor(a % b, b);
    return greatest_common_divisor(a, b % a);
}

int main(void)
{
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    int n, A, B;
    scanf("%d", &n);
    for (int i =0; i < n; i++){
        scanf("%d %d", &A, &B);
        int res = greatest_common_divisor(A, B);
        printf("%d\n", res);
    }

    fclose(input);
    fclose(output);
    return 0;
}