#include <stdio.h>

int main(void)
{
    //printf("%c", 'a');
    FILE *input, *output;
    input = fopen("input.txt", "r");
    output = fopen("output.txt", "w");
    

    if ((input == NULL) || (output == NULL))
    {
        //printf("%c", 'j');
        return 1;
    }
    //printf("%d\n", 10/3);
    int n;
    long long answer = 0;
    // answer = 1000000000;
    // answer = answer * 1000000000;

    if (fscanf(input, "%d", &n) == 1)
    {
        //printf("%c", 'h');
        int min, max; // эти не будут long long, тк они меньше n , a n влезает в int
        for (int i = 1; i * i * i <= n; i++)
        {
            for (int j = i; j * j <= (n / i); j++)
            {
                min = j;
                max = n / (j * i);
                answer += max - min + 1;
            }
        }
        printf("%lld", answer);
    }
    else{
        //fprintf(output, "%c\n", 'p');
    }
    //fprintf(output, "%c\n", 's');
    fprintf(output, "%lld", answer);

    fclose(input);
    fclose(output);

    return 0;
}
