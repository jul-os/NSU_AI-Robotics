#include <stdio.h>
#include <stdint.h>

uint32_t swap_endian32(uint32_t num) {
    return ((num >> 24) & 0x000000FF) |
           ((num >> 8) & 0x0000FF00) |
           ((num << 8) & 0x00FF0000) |
           ((num << 24) & 0xFF000000);
}

int main(){
    freopen("2in.txt", "rb", stdin); 
    freopen("output.txt", "wb", stdout); 
    int32_t n, num, sum = 0;
    int is_big_endian = 0;
    
    fread(&n, sizeof(int32_t), 1, stdin);
    if (n > 10000){
        is_big_endian = 1;
        //it's big endian
        //change to little endian
        n = swap_endian32(n);
    }
    for (int i = 0; i < n; i++){
        fread(&num, sizeof(int32_t), 1, stdin);
        //if big endian change
        if (is_big_endian) {
            num = swap_endian32(num);
        }
        sum+=num;
    }

    if (is_big_endian) {
        sum = swap_endian32(sum);
    }
    //if big endian change back into big endian
    fwrite(&sum, sizeof(int32_t), 1, stdout);

    fclose(stdin); 
    fclose(stdout); 
    return 0;
}
