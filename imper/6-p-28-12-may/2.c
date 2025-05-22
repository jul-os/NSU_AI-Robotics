#include <stdio.h>
#include <stdlib.h>
#define MOD 1000000007

int main(){
    int N;
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    scanf("%d", &N);
    int *dp = (int*)malloc((N+1)*sizeof(int));

    dp[0] = 1;
    for(int j = 1; j <= N; ++j){
        for (int i = j; i <= N; ++i){
            dp[i] = (dp[i] + dp[i-j]) %MOD;
        }
    }
    printf("%d\n", dp[N]);

    free(dp);
    fclose(input);
    fclose(output);
    return 0;
}