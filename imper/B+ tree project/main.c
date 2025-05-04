#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);

    int t;
    if (scanf("%d", &t) != 1) {
        fprintf(stderr, "Failed to read tree order\n");
        return 1;
    }

    char data_file_name[256];
    char log_file_name[256];

    if (scanf("%255s %255s", data_file_name, log_file_name) != 2) {
        fprintf(stderr, "Couldn't read file names\n");
        return 1;
    }

    FILE *data_file = fopen(data_file_name, "r+b");
    if (!data_file) {
        data_file = fopen(data_file_name, "w+b");
        if (!data_file) {
            fprintf(stderr, "Couldn't open/create data file '%s'\n", data_file_name);
            return 1;
        }
    }

    FILE *log_file = fopen(log_file_name, "a+b");
    if (!log_file) {
        fprintf(stderr, "Couldn't open/create log file '%s'\n", log_file_name);
        fclose(data_file);
        return 1;
    }

    //bptree_init(t, fileno(data_file), fileno(log_file));
    // bptree_recover_from_wal();  // если нужно

    // тут можно будет читать операции и вызывать insert/search/delete из tree.c

    fclose(data_file);
    fclose(log_file);
    return 0;
}
