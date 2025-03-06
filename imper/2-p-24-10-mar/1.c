#include <stdio.h>

int main(void)
{
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    int n;
    scanf("%d", &n);

    printf("%d", n);

    fclose(input);
    fclose(output);
    return 0;
}