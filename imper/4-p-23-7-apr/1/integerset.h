#pragma once

typedef struct
{
    int *data;
    int size;
} IntegerSet;

IntegerSet *CreateSet(const int *arr, int len);

int IsInSet(const IntegerSet *set, int x);
