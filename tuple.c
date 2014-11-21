/**
 * Tuple(Naked Pair, Naked Triple, ...
 *       Hidden Pair, Hidden Triple, ...)
 * ある数字の組み合わせだけで、その組み合わせに使われている数字の個数と同じ数のマスを
 * 使っている時、それ以外のマスにあるその組み合わせの数字は消せる。
 */
#include "killer.h"
#include "numpl.h"
#include "constants.h"
#include "inline_functions.h"
#include <stdio.h>

static uint16_t count_multiple(const int index[], cell_t array[]);
static int kill_tuple_lines(const int line[], cell_t array[]);
static int count_naked_cells(uint16_t mask, const int index[],
			     cell_t array[]);
static int kill_naked_cells(uint16_t mask, const int index[], cell_t array[]);

/**
 * Tuple メイン関数
 * @param array ナンプレ盤面配列
 * @return 1:この解法によって数字をけせた 0:この解法によって数字を消せなかった
 */
int kill_tuple(numpl_array * array)
{
    int c = 0;
    for (int i = 0; i < LINE_KINDS; i++) {
	for (int j = 0; j < LINE_SIZE; j++) {
	    c = kill_tuple_lines(all_lines[i][j], array->ar);
	    if (c > 0) {
		return 1;
	    }
	}
    }
    return 0;
}

/**
 * Tuple 下請け関数
 * @param line 仮想行
 * @param array ナンプレ盤面配列（実際の配列）
 * @return 1:この解法によって数字をけせた 0:この解法によって数字を消せなかった
 */
static int kill_tuple_lines(const int line[], cell_t array[])
{
    uint16_t multiple = count_multiple(line, array);
    if (ones16(multiple) <= 2) {
	return 0;
    }
    for (uint16_t mask = 3; mask < FULL_SYMBOL; mask++) {
	if ((mask & multiple) != multiple) {
	    continue;
	}
	int count = ones16(mask);
	if (count <= 1 || count >= LINE_SIZE -1) {
	    continue;
	}
	int cells = count_naked_cells(mask, line, array);
	if (cells == count) {
	    int result = kill_naked_cells(mask, line, array);
	    if (result > 0) {
		return 1;
	    }
	}
    }
    return 0;
}

/**
 * 仮想行内で単一数字マスでないマスで使われている数字をまとめて返す。
 * @param line 仮想行
 * @param array ナンプレ盤面配列（実際の配列）
 * @return 仮想行内で単一数字マスでないマスで使われている数字（ビットパターン）
 */
static uint16_t count_multiple(const int line[], cell_t array[])
{
    int mask = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	int ind = line[i];
	if (!is_single(array[ind])) {
	    mask |= array[ind].symbol;
	}
    }
    return mask;
}

/**
 * 数字の組み合わせ(mask)だけが含まれるマスの数を数える
 * @param mask 数字の組み合わせ（ビットパターン）
 * @param line 仮想行
 * @param array ナンプレ盤面配列（実際の配列）
 * @return 数字の組み合わせ(mask)だけが含まれるマスの数
 */
static int count_naked_cells(uint16_t mask, const int line[], cell_t array[])
{
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	cell_t sym = array[line[i]];
	if ((sym.symbol | mask) == mask) {
	    count++;
	}
    }
    return count;
}

/**
 * 数字の組み合わせ以外も含むマスから数字の組み合わせを消す
 * @param mask 数字の組み合わせ（ビットパターン）
 * @param line 仮想行
 * @param array ナンプレ盤面配列（実際の配列）
 * @return 数字の組み合わせを消したマスの数
 */
static int kill_naked_cells(uint16_t mask, const int line[], cell_t array[])
{
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	cell_t sym = array[line[i]];
	if ((sym.symbol | mask) != mask &&
	    (sym.symbol & mask) != 0) {
	    array[line[i]].symbol = sym.symbol & (~mask);
	    count++;
	}
    }
    return count;
}

