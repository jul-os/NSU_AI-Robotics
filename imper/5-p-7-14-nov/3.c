#include <stdio.h>
#include <errno.h>
#include <string.h>

// Каждая подпись должна представляться структурой:
typedef struct Label_s
{
    char name[16]; // имя автора (заканчивается нулём)
    int age;       // возраст автора (сколько лет)
} Label;

// Статистика имён должна представляться структурой:
typedef struct NameStats_s
{
    int cntTotal; // сколько всего подписей
    int cntLong;  // сколько подписей с именами длиннее 10 букв
} NameStats;

// Статистика возрастов должна представляться структурой:
typedef struct AgeStats_s
{
    int cntTotal;  // сколько всего подписей
    int cntAdults; // сколько подписей взрослых (хотя бы 18 лет)
    int cntKids;   // сколько подписей детей (меньше 14 лет)
} AgeStats;

// Функция для вычисления статистик должна иметь сигнатуру:
void calcStats(const Label *arr, int cnt, NameStats *oNames, AgeStats *oAges);
// Здесь oNames и oAges — адреса структур, в которые нужно записать результат.

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

    if (scanf("%d", &n) == 1)
    {
        Label people[n];
        NameStats name_statistic = {0, 0};
        AgeStats age_statistic = {0, 0, 0};
        char let[3];
        for (int i = 0; i < n; i++)
        {
            scanf("%s %d %s", people[i].name, &people[i].age, let);
            // printf("%s %d %s\n", people[i].name, people[i].age, let);
            name_statistic.cntTotal += 1;
            if (strlen(people[i].name) > 10)
            {
                name_statistic.cntLong += 1;
            }
            age_statistic.cntTotal += 1;
            if (people[i].age >= 18)
            {
                age_statistic.cntAdults += 1;
            }
            else if (people[i].age < 14)
            {
                age_statistic.cntKids += 1;
            }
        }
        printf("names: total = %d\nnames: long = %d\nages: total = %d\nages: adult = %d\nages: kid = %d",
               name_statistic.cntTotal, name_statistic.cntLong, age_statistic.cntTotal, age_statistic.cntAdults, age_statistic.cntKids);
    }

    fclose(input);
    fclose(output);
    return 0;
}