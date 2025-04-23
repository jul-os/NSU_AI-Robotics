#include "tree.h"
#include <stdio.h>


int main(){
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);
    int t;
    scanf("%d", &t);

    char* data_file_name[256];
    char* log_file_name[256];

    if (scanf("%255s %255s", data_file_name, log_file_name)!=2){
        printf(stderr, "Cou;dn't read file\n");
        fclose(input);
        fclose(output);
        return 1;
    }
    // todo спросить где будут писаться операции и как это будет работтать вместе с файлом данных 

    FILE *file1 = fopen(data_file_name, "r");
    if (!file1) {
        printf("Cou;dn't read file '%s'\n", data_file_name);
    }

    FILE *file2 = fopen(log_file_name, "r");
    if (!file2) {
        printf("Cou;dn't read file '%s'\n", log_file_name);
    }

    fclose(input);
    fclose(output);
    return 0;
}