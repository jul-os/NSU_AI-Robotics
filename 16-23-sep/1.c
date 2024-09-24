#include <stdio.h>
#include <errno.h>
#include <sys/times.h>

int main(void)
{

    FILE *input, *output;
    input = fopen("input.txt", "r");
    output = fopen("output.txt", "w");

    if (input == NULL || output == NULL)
    {
        /*
        вот это понятный способ, но я хочу попробовать другуб обработку ошибок
        puts("Files could not be opened");
        return 1;
        читала в этой статье https://codeby.school/blog/programmirovanie/obrabotka-oshibok-v-yazyke-si
        */

        perror("fopen()");
        // ASK и будто бы тут достаточно "", потому что то, что внутри, показывается во время вывода ошибки
        return errno;

        /*
        из man perror:
        The perror() function produces a message on standard error describing the last error
        encountered during a call to a system or library function.

        When a system call fails, it usually returns -1 and sets the variable errno to a value describing
        what went wrong.  (These values can  be  found in  <errno.h>.)   Many library functions do likewise.
        The function perror() serves to translate this error code into human-readable form.  Note
        that errno is undefined after a successful system call or library function call: this call may well
        change this variable, even  though  it  suc‐ceeds, for example because it internally used some other
        library function that failed.  Thus, if a failing call is not immediately followed by a
        call to perror(), the value of errno should be saved.
        */
    }

    int n;
    if (fscanf(input, "%d", &n) == 1)
    {
        // n<=5000, поэтому решето Эратосфена
        int a[n + 1]; // пропускаем 0

        // заполняем массив
        for (int i = 0; i < n + 1; i++)
        {
            a[i] = i;
        }
        for (int p = 2; p < n + 1; p++)
        {
            if (a[p] != 0)
            {
                fprintf(output, "%d\n", a[p]);
                for (int j = p * p; j < n + 1; j += p)
                {
                    a[j] = 0;
                }
            }
        }
    }
    fclose(input);
    fclose(output);
    return 0;
}