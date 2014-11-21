#ifndef COMMON_H
#define COMMON_H

#include "numpl.h"
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

    int count_fixed(const numpl_array *array);
    void fixed_only(numpl_array * array, uint16_t full);
    int input(numpl_array * array, char * text);
    void print_array(numpl_array * array);
    void read_mondai(char mondai[], FILE * fp);

#if defined(__cplusplus)
}
#endif

#endif // COMMON_H
