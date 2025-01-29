#include <stdio.h>
#include <stdint.h>

int main(){
    freopen("input.txt", "rb", stdin); 
    freopen("output.txt", "wb", stdout); 
    int32_t a, b;
    fread(&a, sizeof(int32_t), 1, stdin);

    fread(&b, sizeof(int32_t), 1, stdin);

    int64_t sum = (int64_t)a + (int64_t)b;
    int32_t half = (int32_t)(sum/2);
    fwrite(&half, sizeof(int32_t), 1, stdout);

    fclose(stdin); 
    fclose(stdout); 
    return 0;
}

