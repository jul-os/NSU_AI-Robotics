#include "tree.h"
#include <stdio.h>
#include <stdlib.h>


typedef struct {
    BTree* tree;
    int id;
} ThreadArgs;


void run_concurrent_test(BTree* tree, int num_threads);
void* thread_fn(void* arg);