#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define SIZE_NAME 21

#pragma pack(1)
typedef struct files
{
    char name[SIZE_NAME];
    int64_t size;
    unsigned char flag_directory; // 1 if dir, 0 else
    int64_t time_create;
    uint64_t time_edit;
    unsigned char flag_hidden; // 1 if hidden, 0 else
} files;

void InsertionSort(int n, files *arr)
{
    files newElement;
    int location;

    for (int i = 1; i < n; i++)
    {
        newElement = arr[i];
        location = i - 1;
        while (location >= 0 && strcmp(arr[location].name, newElement.name) > 0)
        {
            arr[location + 1] = arr[location];
            location = location - 1;
        }
        arr[location + 1] = newElement;
    }
}

int main()
{
    freopen("3.in", "rb", stdin);
    freopen("output.txt", "wb", stdout);
    int32_t n;
    int64_t a, b;
    fread(&n, sizeof(int32_t), 1, stdin); // number of files in the list
    fread(&a, sizeof(int64_t), 1, stdin);
    fread(&b, sizeof(int64_t), 1, stdin);

    files *nice = malloc(n * sizeof(files));
    int fitting = 0;
    for (int i = 0; i < n; i++)
    {
        files buf_file;
        fread(&buf_file, sizeof(buf_file), 1, stdin);

        if (buf_file.flag_directory == 0 && buf_file.flag_hidden == 0)
        {
            if (buf_file.time_create >= a && buf_file.time_edit <= b)
            {
                nice[fitting] = buf_file;
                fitting++;
            }
        }
    }

    InsertionSort(fitting, nice);
    for (int i = 0; i < fitting; i++)
    {
        fwrite(&nice[i], sizeof(files), 1, stdout);
    }
    free(nice);

    fclose(stdin);
    fclose(stdout);
    return 0;
}