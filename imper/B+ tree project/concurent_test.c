#include "tree.h"
#include "comcerunt_test.h"
#include <stdio.h>
#include <stdlib.h>


void* thread_fn(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    insert(args->tree, 9 + args->id, args->id * 10);
    free(arg); 
    return NULL;
}

void run_concurrent_test(BTree* tree, int num_threads) {
    pthread_t threads[num_threads];

    for (int i = 0; i < num_threads; ++i) {
        ThreadArgs* args = malloc(sizeof(ThreadArgs));
        args->tree = tree;
        args->id = i + 1;
        pthread_create(&threads[i], NULL, thread_fn, args);
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
}
