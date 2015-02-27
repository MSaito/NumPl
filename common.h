#ifndef COMMON_H
#define COMMON_H

#include "numpl.h"
#include "xsadd.h"
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

    extern xsadd_t xsadd;
    int count_fixed(const numpl_array *array);
    void fixed_only(numpl_array * array, uint16_t full);
    int input(numpl_array * array, char * text);
    void print_array(numpl_array * array);
    void read_mondai(char mondai[], FILE * fp);
    int get_random(unsigned int n);
    void get_random_symbol(uint16_t symbols[LINE_SIZE]);
    void get_random_number(uint16_t numbers[LINE_SIZE]);
    void get_random_number_size(uint16_t numbers[], int size);
    void shuffle_int(int array[], int size);
    uint16_t random_one_symbol(uint16_t symbols);
    int get_counter_pos(int pos, int mode);
    void fill_array(numpl_array * array, uint16_t full);

#if defined(__cplusplus)
}
#endif

#endif // COMMON_H
