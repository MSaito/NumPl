#ifndef NUMPL_H
#define NUMPL_H

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define LINE_SIZE 9
#define ARRAY_SIZE (LINE_SIZE * LINE_SIZE)
#define LINE_KINDS 3
#define BLOCK_ROWS 3
#define BLOCK_COLS 3
#define MAX_SYMBOL 0x100
#define FULL_SYMBOL 0x1ff

    typedef struct {
	unsigned symbol : 9;
	unsigned fixed : 1;
    } cell_t;

    typedef struct {
	cell_t ar[ARRAY_SIZE];
    } numpl_array;



#if defined(__cplusplus)
}
#endif

#endif // NUMPL_H
