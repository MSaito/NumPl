#ifndef INLINE_FUNCTIONS_H
#define INLINE_FUNCTIONS_H

#include "numpl.h"

#if defined(__cplusplus)
extern "C" {
#endif

    /**
     * ビットパターンの中の１の数を数える
     */
    static inline uint16_t ones16(uint16_t x)
    {
        /* 16-bit recursive reduction using SWAR...
	   but first step is mapping 2-bit values
	   into sum of 2 1-bit values in sneaky way
	*/
	x -= ((x >> 1) & UINT16_C(0x5555));
	x = (((x >> 2) & UINT16_C(0x3333)) + (x & UINT16_C(0x3333)));
	x = (((x >> 4) + x) & UINT16_C(0x0f0f));
	x += (x >> 8);
	return (x & 0x0000001f);
    }

    /**
     * ひとつのマスの中に候補となる数字が１つだけか？
     */
    static inline int is_single(const cell_t cell)
    {
	return ones16(cell.symbol) == 1;
    }

    static inline void set_single_flag(cell_t cell)
    {
    }

#if defined(__cplusplus)
}
#endif

#endif // INLINE_FUNCTIONS_H
