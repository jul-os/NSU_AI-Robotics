#include <stdio.h>
#include <errno.h>

typedef struct DateTime_s
{
    int year, month, day;
    int hours, minutes, seconds;
} DateTime;

// Функция для поиска самой ранней(минимальной) даты должна иметь сигнатуру
// Здесь arr — указатель на первый элемент массива дат, а cnt — длина массива.
DateTime min(const DateTime *arr, int cnt)
{
    DateTime earliest = arr[0]; // Предполагаем, что первая дата самая ранняя

    for (int i = 1; i < cnt; i++)
    {
        const DateTime *current = &arr[i];
        //-> когда используем с указателем, . когда используем с переменной

        if (current->year < earliest.year)
        {
            earliest = *current;
        }
        else if (current->year == earliest.year)
        {
            if (current->month < earliest.month)
            {
                earliest = *current;
            }
            else if (current->month == earliest.month)
            {
                if (current->day < earliest.day)
                {
                    earliest = *current;
                }
                else if (current->day == earliest.day)
                {
                    if (current->hours < earliest.hours)
                    {
                        earliest = *current;
                    }
                    else if (current->hours == earliest.hours)
                    {
                        if (current->minutes < earliest.minutes)
                        {
                            earliest = *current;
                        }
                        else if (current->minutes == earliest.minutes)
                        {
                            if (current->seconds < earliest.seconds)
                            {
                                earliest = *current;
                            }
                        }
                    }
                }
            }
        }
    }
    return earliest;
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
    if (scanf("%d", &n) == 1)
    {
        DateTime dates[n]; // Массив для хранения всех дат

        // Чтение дат
        for (int i = 0; i < n; i++)
        {
            scanf("%d %d %d %d %d %d",
                  &dates[i].year, &dates[i].month, &dates[i].day,
                  &dates[i].hours, &dates[i].minutes, &dates[i].seconds);
        }
        DateTime earliest = min(dates, n);
        printf("%d %d %d %d %d %d",
               earliest.year, earliest.month, earliest.day,
               earliest.hours, earliest.minutes, earliest.seconds);
    }

    fclose(input);
    fclose(output);
    return 0;
}