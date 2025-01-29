#include <stdio.h>
#include <stdlib.h>

void find_near(int target, int * arr, int len){
    //индекс люого из ближайших + расстояние до него
    int left = 0, right = len,  result = -1;
    while (right >= left){
        int mid = (left + right)/2;
        if (arr[mid] == target){
            printf("0 %d\n", mid);
        }
        else if (arr[mid] < target){
            left = mid + 1;
        }
        else{
            right  = mid - 1;
        }      

    }
}

int main(void)
{
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    int n, m, temp;
    scanf("%d", &n);
    int *arr = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &arr[i]);
    }
    scanf("%d", &m);
    for (int i = 0; i < m; i++)
    {
        scanf("%d", &temp);
    }

    fclose(input);
    fclose(output);
    return 0;
}