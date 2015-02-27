#ifndef MULTI_SOLVE_H
#define MULTI_SOLVE_H
#include "numpl.h"

#define MS_STACK_SIZE 20

typedef struct {
    int sp;
    numpl_array array[MS_STACK_SIZE];
} ms_array_t;

//int multi_solve(const numpl_array *array, ms_array_t * ms_array);
int random_solve(numpl_array *array);
#endif
