#ifndef INLINE_FUNCTIONS_H
#define INLINE_FUNCTIONS_H

#include "numpl.h"

#if defined(__cplusplus)
extern "C" {
#endif

    /**
     * ビットパターンの中の１の数を数える
     * @param x ビットパターン
     * @return x の中の1の個数
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
     * @param cell ナンプレのマス
     * @return true 候補となる数字がひとつ
     * @return false 候補となる数字がひとつではない
     */
    static inline int is_single(const cell_t cell)
    {
	return ones16(cell.symbol) == 1;
    }

    /**
     * シングルフラグをセットする
     * @param cell ナンプレのマス
     */
    static inline void set_single_flag(cell_t cell)
    {
    }

    /**
     * ナンプレ盤面配列の中央を原点として点対称となるインデックスを返す
     * @param index ナンプレ盤面配列のインデックス
     * @param ナンプレ盤面配列で index と対称となる点のインデックス
     */
    static inline int get_counter(int index) {
	return ARRAY_SIZE - 1 - index;
    }

#if defined(__cplusplus)
}
#endif

#endif // INLINE_FUNCTIONS_H
