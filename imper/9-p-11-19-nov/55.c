#include <stdio.h>
#include <stdlib.h>

/* Функция сравнения для целых чисел */
int cmp(const void *a, const void *b) {
    const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return (*ia > *ib) - (*ia < *ib);
}

/* Выбор индекса опорного элемента (берем середину массива) */
int selectPivotIndex(void **ar, int left, int right) {
    return left + (right - left) / 2;
}

/* Сортировка вставками для небольших подмассивов */
void insertion(void **ar, int (*cmp)(const void *, const void *), int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        void *key = ar[i];
        int j = i - 1;
        while (j >= left && cmp(ar[j], key) > 0) {
            ar[j + 1] = ar[j];
            j--;
        }
        ar[j + 1] = key;
    }
}

/* Разбиение массива */
int partition(void **ar, int (*cmp)(const void *, const void *), int left, int right, int pivotIndex) {
    void *pivot = ar[pivotIndex];
    void *tmp = ar[right];
    ar[right] = ar[pivotIndex];
    ar[pivotIndex] = tmp;

    int store = left;
    for (int i = left; i < right; i++) {
        if (cmp(ar[i], pivot) <= 0) {
            tmp = ar[i];
            ar[i] = ar[store];
            ar[store] = tmp;
            store++;
        }
    }

    tmp = ar[right];
    ar[right] = ar[store];
    ar[store] = tmp;
    return store;
}

/* Рекурсивная быстрая сортировка */
void do_qsort(void **ar, int (*cmp)(const void *, const void *), int left, int right) {
    const int minSize = 10; // Минимальный размер подмассива для быстрой сортировки
    if (right <= left) return;

    int pivotIndex = selectPivotIndex(ar, left, right);
    pivotIndex = partition(ar, cmp, left, right, pivotIndex);

    if (pivotIndex - 1 - left <= minSize)
        insertion(ar, cmp, left, pivotIndex - 1);
    else
        do_qsort(ar, cmp, left, pivotIndex - 1);

    if (right - pivotIndex - 1 <= minSize)
        insertion(ar, cmp, pivotIndex + 1, right);
    else
        do_qsort(ar, cmp, pivotIndex + 1, right);
}

/* Вызов быстрой сортировки */
void sortPointers(void **vals, int total_elems, int (*cmp)(const void *, const void *)) {
    do_qsort(vals, cmp, 0, total_elems - 1);
}

int main(void) {
    FILE *input, *output;
    input = freopen("input.txt", "rb", stdin);
    output = freopen("output.txt", "wb", stdout);
    
    int n;
    fread(&n, sizeof(int), 1, stdin);
    
    int *val = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        fread(&val[i], sizeof(int), 1, stdin);
    }

    // Преобразуем массив в массив указателей
    void **valPtrs = malloc(n * sizeof(void *));
    for (int i = 0; i < n; i++) {
        valPtrs[i] = &val[i];
    }

    // Сортируем массив указателей
    sortPointers(valPtrs, n, cmp);

    // Записываем отсортированные значения в выходной файл
    for (int i = 0; i < n; i++) {
        fwrite(valPtrs[i], sizeof(int), 1, stdout);
    }

    free(valPtrs);
    free(val);
    fclose(input);
    fclose(output);
    return 0;
}