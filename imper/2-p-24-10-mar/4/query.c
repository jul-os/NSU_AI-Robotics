//query.c

#include <stdint.h>

extern int length;

long long Sum(int left, int right);

// находит самый длинный отрезок с началом в l и суммой не более sum
// возвращает правый край искомого отрезка
int Query(int l, int64_t sum){
    int right = length, left = l;
    
    //вдруг уже нашли те от l до конца уже подходит
    if (Sum(left, right) <= sum){
        return right;
    }

    while (right - left > 1)
    {
        int middle = (left + right)/2;
        if (Sum(l, middle) <= sum){
            left = middle;
        }
        else{
            right = middle;
        }
    }

    return left; //получается длина
    
}