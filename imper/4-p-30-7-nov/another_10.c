#include <stdio.h>
#include <string.h>
#include <errno.h>

const char *units[] = {"nol’", "odin", "dva", "tri", "chetyre", "pyat’", "shest’", "sem’", "vosem’", "devyat’"};
const char *ending_units[] = {"nol’", "odna", "dve", "tri", "chetyre", "pyat’", "shest’", "sem’", "vosem’", "devyat’"};
const char *teens[] = {"desyat’", "odinnadtsat’", "dvenadtsat’", "trinadtsat’", "chetyrnadtsat’", "pyatnadtsat’", "shestnadtsat’", "semnadtsat’", "vosemnadtsat’", "devyatnadtsat’"};
const char *tens[] = {"", "", "dvadtsat’", "tridtsat’", "sorok", "pyat’desyat", "shest’desyat", "sem’desyat", "vosem’desyat", "devyanosto"};
const char *hundreds[] = {"", "sto", "dvesti", "trista", "chetyresta", "pyat’sot", "shest’sot", "sem’sot", "vosem’sot", "devyat’sot"};
const char *thousands_ending[] = {"tysacha", "tysyachi", "tysyach"};
const char *millions_ending[] = {"million", "milliona", "millionov"};
const char *milliards_ending[] = {"milliard", "milliarda", "milliardov"};
const char *stroka_ending[] = {"stroka", "stroki", "strok"};

const char *choose_ending(int number, const char *endings[])
{
    if (number % 10 == 1 && number % 100 != 11)
        return endings[0];
    if (number % 10 >= 2 && number % 10 <= 4 && (number % 100 < 10 || number % 100 >= 20))
        return endings[1];
    return endings[2];
}

void convert_hundreds(char *result, int n, int end_flag)
{
    if (n >= 100)
    {
        strcat(result, hundreds[n / 100]);
        strcat(result, " ");
        n %= 100;
    }
    if (n >= 20)
    {
        strcat(result, tens[n / 10]);
        strcat(result, " ");
        n %= 10;
    }
    else if (n >= 10)
    {
        strcat(result, teens[n - 10]);
        strcat(result, " ");
        return;
    }
    if (n > 0)
    {
        if (end_flag == 1)
        {
            strcat(result, ending_units[n]);
        }
        else
        {
            strcat(result, units[n]);
        }
        strcat(result, " ");
    }
}

void number_to_words(char *result, int n)
{
    if (n >= 1000000000)
    {
        int milliards = n / 1000000000;
        convert_hundreds(result, milliards, 0);
        strcat(result, choose_ending(milliards, milliards_ending));
        strcat(result, " ");
        n %= 1000000000;
    }
    if (n >= 1000000)
    {
        int millions = n / 1000000;
        convert_hundreds(result, millions, 0);
        strcat(result, choose_ending(millions, millions_ending));
        strcat(result, " ");
        n %= 1000000;
    }
    if (n >= 1000)
    {
        int thousands = n / 1000;
        convert_hundreds(result, thousands, 0);
        strcat(result, choose_ending(thousands, thousands_ending));
        strcat(result, " ");
        n %= 1000;
    }
    convert_hundreds(result, n, 1);
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
    int n, num;

    scanf("%d", &n);

    for (int i = 0; i < n; i++)
    {
        scanf("%d", &num);
        char result[256] = "";
        number_to_words(result, num);
        strcat(result, choose_ending(num, stroka_ending));

        printf("%s\n", result);
    }

    fclose(input);
    fclose(output);
    return 0;
}
