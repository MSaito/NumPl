#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "numpl.h"
#if defined(__cplusplus)
extern "C" {
#endif

    /** 行の配列 */
    extern const int rows[LINE_SIZE][LINE_SIZE];
    /** 列の配列 */
    extern const int cols[LINE_SIZE][LINE_SIZE];
    /** ブロックの配列 */
    extern const int blocks[LINE_SIZE][LINE_SIZE];

#if defined(__cplusplus)
}
#endif
#endif // CONSTANTS_H
