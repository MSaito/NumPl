#ifndef NUMPL_H
#define NUMPL_H

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define LINE_SIZE 9
#define ARRAY_SIZE (LINE_SIZE * LINE_SIZE)

    typedef struct {
	unsigned symbol : 9;
	unsigned fixed : 1;
    } cell_t;

    typedef struct {
	cell_t ar[ARRAY_SIZE];
    } sudoku_array;



#if defined(__cplusplus)
}
#endif

#endif // NUMPL_H
