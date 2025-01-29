#include <stdio.h>
#include <errno.h>

int current_element (int num, int diff, int first) {
    //вернуть элемент с номером num
    return first + (num - 1) * diff;
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
    int start, stop, k, count = 0;
    if (fscanf(input, "%d %d %d", &start, &stop, &k) == 3)
    {
        for (int i = 1; i <= stop - start; i++)
        //i перебирает шаг прогрессии
        {
            for (int j = start; j < stop; j++)
            //перебирает начальные значения прогрессии
            {
                if (current_element(k, i, j) <= stop && current_element(k + 1, i, j) > stop)
                    //у нас уже есть k элементов, n-ный еще в range, k+1 уже нет
                {
                    count += 1;
                }
            }
        }
    }

    fprintf(output, "%d", count);

    fclose(input);
    fclose(output);
    return 0;
}