#include "tree.h"
#include "comcerunt_test.h"
#include <stdio.h>
#include <stdlib.h>

void *thread_fn(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    int out;
    if (find(args->tree->disk_tree, args->id, args->tree, &out))
    {
        fprintf(stdout, "FOR KEY %d FIND FOUND %d\n", args->id, out);
    }
    else
    {
        fprintf(stdout, "FIND FOR KEY %d NOT FOUND\n", args->id);
    }

    free(arg);
    return NULL;
}

void run_concurrent_test(BTree *tree, int num_threads)
{
    pthread_t threads[num_threads];

    for (int i = 0; i < num_threads; ++i)
    {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->tree = tree;
        args->id = i + 1;
        int rc = pthread_create(&threads[i], NULL, thread_fn, args);
        if (rc != 0)
        {
            fprintf(stderr, "Failed to create thread %d: %d\n", i, rc);
        }
        else
        {
            fprintf(stdout, "Thread %d created\n", i);
        }
    }

    for (int i = 0; i < num_threads; ++i)
    {
        pthread_join(threads[i], NULL);
    }
}
