#include "decls.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// int strlen(char *a) {
//     int len = 0;
//     while (a[len] != '\0') {
//         len++;
//     }
//     return len;
// }

// void strcpy(char *dest, const char *src) {
//     int i = 0;
//     while (src[i] != '\0') {
//         dest[i] = src[i];
//         i++;
//     }
//     dest[i] = '\0';
// }

void echo_0(State *state)
{
    printf("ECHO:\n");
}
void echo_1(State *state, char *arg0)
{
    printf("ECHO: %s\n", arg0);
}
void echo_2(State *state, char *arg0, char *arg1)
{
    printf("ECHO: %s|%s\n", arg0, arg1);
}
void echo_3(State *state, char *arg0, char *arg1, char *arg2)
{
    printf("ECHO: %s|%s|%s\n", arg0, arg1, arg2);
}

void print_1(State *state, char *idx)
{
    int idxxx = atoi(idx);
    printf("%s\n", state->regs[idxxx]);
}

void printregs_0(State *state)
{
    for (int i = 0; i < 256; i++)
    {
        if (state->regs[i] != NULL)
        {
            printf("%d = %s\n", i, state->regs[i]);
        }
    }
}

void store_2(State *state, char *idx, char *what)
{
    int index = atoi(idx);

    if (state->regs[index])
    {
        free(state->regs[index]);
    }
    state->regs[index] = (char *)malloc(strlen(what) + 1);

    strcpy(state->regs[index], what);
}

void copy_2(State *state, char *dst, char *src)
{
    int source = atoi(src), destination = atoi(dst);

    if (source == destination)
    {
        return;
    }
    if (state->regs[destination] != NULL)
    {
        free(state->regs[destination]);
    }
    state->regs[destination] = (char *)malloc(strlen(state->regs[source]) + 1);
    strcpy(state->regs[destination], state->regs[source]);
}

void clear_1(State *state, char *idx)
{
    int index = atoi(idx);

    if (state->regs[index])
    {
        free(state->regs[index]);
    }
    state->regs[index] = NULL;
}