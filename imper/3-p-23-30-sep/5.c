#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

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
    int set_vol, pairs_num, x, y;
    if (scanf("%d %d", &set_vol, &pairs_num) == 2)
    {
        int *x_es = malloc((set_vol + 1) * sizeof(int));
        int *y_es = malloc((set_vol + 1) * sizeof(int));

        if (x_es == NULL || y_es == NULL)
        {
            perror("malloc failed");
            return errno;
        }

        for (int i = 0; i < set_vol + 1; i++)
        {
            x_es[i] = 0;
            y_es[i] = 0;
        }

        bool is_function_flag = true;
        bool is_all_defined = true;
        bool is_injective = true;
        bool is_surjective = true;

        for (int i = 0; i < pairs_num; i++)
        {
            scanf("%d %d", &x, &y);
            if (x_es[x] == 0)
            {
                x_es[x] += 1;
            }
            else
            {
                is_function_flag = false;
                printf("0");
                break;
            }
            y_es[y] += 1;
        }

        if (is_function_flag != false)
        {
            printf("1 ");
            // всюду определенная это функция определенная на всем множестве значений
            for (int i = 1; i < set_vol + 1; i++)
            {
                if (x_es[i] == 0)
                {
                    is_all_defined = false;
                }
                if (y_es[i] > 1)
                {
                    is_injective = false;
                }
                if (y_es[i] == 0)
                {
                    is_surjective = false;
                }
            }
            if (is_all_defined == true)
            {
                printf("2 ");
            }
            if (is_injective == true)
            {
                printf("3 ");
            }
            if (is_surjective == true)
            {
                printf("4 ");
            }
            if (is_injective && is_surjective)
            {
                printf("5");
            }
        }
        free(x_es);
        free(y_es);
    }
    fclose(input);
    fclose(output);
    return 0;
}