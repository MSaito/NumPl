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

    static inline int floor_log2(uint16_t x)
    {
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        return(ones16(x) - 1);
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

    /**
     * 盤面配列内の位置から行番号を求める
     * @pram idx 盤面配列内の位置
     * @return 行番号
     */
    static inline int torow(int idx)
    {
	return idx / LINE_SIZE;
    }

    /**
     * 盤面配列内の位置から列番号を求める
     * @pram idx 盤面配列内の位置
     * @return 列番号
     */
    static inline int tocol(int idx)
    {
	return idx % LINE_SIZE;
    }

    /**
     * 盤面配列内の位置からブロック番号を求める
     * @pram idx 盤面配列内の位置
     * @return ブロック番号
     */
    static inline int toblk(int idx)
    {
	return (torow(idx) / BLOCK_ROWS) * BLOCK_ROWS
	    + tocol(idx) / BLOCK_COLS;
    }
#if defined(__cplusplus)
}
#endif

#endif // INLINE_FUNCTIONS_H
