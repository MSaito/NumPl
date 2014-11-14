#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "numpl.h"
#if defined(__cplusplus)
extern "C" {
#endif

    /** array of rows */
    extern const int rows[LINE_SIZE][LINE_SIZE];
    /** array of columns */
    extern const int cols[LINE_SIZE][LINE_SIZE];
    /** array of blocks */
    extern const int blocks[LINE_SIZE][LINE_SIZE];
    /** array of virtual lines */
    extern const int (* const all_lines[LINE_KINDS])[LINE_SIZE];

#if defined(__cplusplus)
}
#endif
#endif // CONSTANTS_H
