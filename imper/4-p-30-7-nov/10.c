#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

char *edinitsi = {"nol’ strok", " odna stroka", "dve stroki", "tri stroki", "chetyre stroki",
                  "pyat’ strok", "shest’ strok", "sem’ strok", "vosem’ strok", "devyat’ strok",
                  "desyat’ strok", "odinnadtsat’ strok", "dvenadtsat’ strok", "trinadtsat’ strok", "chetyrnadtsat’ strok",
                  "pyatnadtsat’ strok", "shestnadtsat’ strok", "semnadtsat’ strok", "vosemnadtsat’ strok", "devyatnadtsat’ strok"};

void print_num(int num)
{
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
    int n;
    scanf("%d", &n);
    int num;
    for (int i = 0; i < n; n++)
    {
        scanf("%d", &num);
        print_num(num);
    }

    fclose(input);
    fclose(output);
    return 0;
}