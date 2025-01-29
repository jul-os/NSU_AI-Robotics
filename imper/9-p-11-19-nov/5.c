#include <stdio.h>
#include <stdlib.h>

int partition(void **ar, int (*cmp)(const void *, const void *), int left, int right, int pivotIndex)
{
    int idx,
        store;
    void *pivot = ar[pivotIndex]; /* Перемещаем pivot в конец массива */
    void *tmp = ar[right];
    ar[right] = ar[pivotIndex];
    ar[pivotIndex] = tmp; /* Все значения, не превышающие pivot, перемещаются в начало
                           * массива, и pivot вставляется сразу после них. */
    store = left;
    for (idx = left; idx < right; idx++)
    {
        if (cmp(ar[idx], pivot) <= 0)
        {
            tmp = ar[idx];
            ar[idx] = ar[store];
            ar[store] = tmp;
            store++;
        }
    }
    tmp = ar[right];
    ar[right] = ar[store];
    ar[store] = tmp;
    return store;
}

void do_qsort(void **ar, int (*cmp)(const void *, const void *), int left, int right)
{
    int pivotIndex;
    if (right <= left)
        return;
    /* Разбиение */
    pivotIndex = selectPivotlndex(ar, left, right);
    pivotIndex = partition(ar, cmp, left, right, pivotIndex);
    if (pivotIndex - 1 - left <= minSize)
        insertion(ar, cmp, left, pivotIndex - 1); //если осталось уж еочень мало элементов то их просто нужно вставить
    else
        do_qsort(ar, cmp, left, pivotIndex - 1);
    if (right - pivotIndex - 1 <= minSize)
        insertion(ar, cmp, pivotIndex + 1, right);
    else
        do_qsort(ar, cmp, pivotIndex + 1, right);
}
/** Вызов быстрой сортировки */
void sortPointers(void **vals, int total_elems, int (*cmp)(const void *, const void *))
{
    do_qsort(vals, cmp, 0, total_elems - 1);
}

int main(void)
{

    FILE *input, *output;
    input = freopen("input.txt", "rb", stdin);
    output = freopen("output.txt", "wb", stdout);
    int n;
    fread(&n, sizeof(int), 1, stdin);
    int *val = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        fread(&val[i], sizeof(int), 1, stdin);
    }

    sortPointers(val, n, cmp);
    fclose(input);
    fclose(output);
    return 0;
}