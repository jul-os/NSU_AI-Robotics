#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

double my_pow(double base, int exponent)
{
    double result = 1.0;
    int exp = (exponent < 0) ? -exponent : exponent;

    for (int i = 0; i < exp; i++)
    {
        result *= base;
    }

    return (exponent < 0) ? 1.0 / result : result;
}

char *slice(char *array, char *sliced_array, int i, int length)
{
    for (int j = 0; j < length; j++)
    {
        sliced_array[j] = array[i + j];
    }
    sliced_array[length] = '\0';
    return sliced_array;
}

int read_and_convert_full(char *scan, int i)
{
    char num[8 + 1];
    slice(scan, num, i, 8);
    //printf("слайс %s\n", num);
    int dec_num = 0;
    for (int j = 0; j < 8; j++)
    {
        if (num[j] == '1')
        {

            dec_num += my_pow(2, j);
        }
    }
    //printf("%d\n", dec_num);
    return dec_num;
}

int read_and_convert_left_over(char *scan, int left_over_bites, int i)
{
    char *num = malloc(left_over_bites + 1);
    slice(scan, num, i, left_over_bites);
    //printf("слайс %s\n", num);
    int dec_num = 0;
    for (int j = 0; j < left_over_bites; j++)
    {
        if (num[j] == '1')
        {

            dec_num += my_pow(2, j);
        }
    }
    //printf("%d\n", dec_num);
    free(num);
    return dec_num;
}

int main(void)
{

    FILE *input, *output;
    input = fopen("input.txt", "r");
    output = fopen("output.txt", "w");

    if (input == NULL || output == NULL)
    {

        perror("fopen()");
        return errno;
    }
    int n;
    if (fscanf(input, "%d", &n) == 1)
    {
        int full_eights = n / 8;
        int left_over_bites = n % 8;

        fgetc(input); // Это удалит '\n' из потока

        // считать все это дело и записать его
        char *scan = malloc(n + 1);
        scan = fgets(scan, n + 1, input);
        // printf("Считанная строка: %s\n", scan);
        // printf("%d %d\n", full_eights, left_over_bites);
        for (int i = 0; i < full_eights * 8; i += 8)
        {
            fprintf(output, "%d ", read_and_convert_full(scan, i));
        }

        fprintf(output, "%d", read_and_convert_left_over(scan, left_over_bites, 8 * full_eights));

        // каждый раз когда в си нет функции для упрощения жизни, где-то в нгу грустит одна Юля :(

        free(scan);
    }

    fclose(input);
    fclose(output);
    return 0;
}