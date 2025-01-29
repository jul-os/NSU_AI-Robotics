#include <stdio.h>
#include <stdint.h>

int main(){
    FILE * in = freopen("input.txt", "rb", stdin), *out = freopen("input.txt", "wb", stdout);
    //int64_t n = fffffffe;
    int64_t g = INT64_MAX;
    fwrite(&g, sizeof(int64_t), 1, stdout);

    fclose(in); 
    fclose(out); 
    return 0;
}