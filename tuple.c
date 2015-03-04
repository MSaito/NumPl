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
#include <math.h>

static uint16_t count_multiple(const int index[], cell_t array[]);
static int kill_tuple_lines(const int line[], cell_t array[]);
static int count_naked_cells(uint16_t mask, const int index[],
			     cell_t array[]);
static int kill_naked_cells(uint16_t mask, const int index[], cell_t array[]);
static int count_hidden_cells(uint16_t mask, const int index[],
			      cell_t array[]);
static int kill_hidden_cells(uint16_t mask, const int index[],
			     cell_t array[]);
static int analyze_tuple_sub(int naked, int tuple_num,
			     numpl_array * array, solve_info * info);
static int kill_naked_lines(int tuple_num, const int index[], cell_t array[]);
static int kill_hidden_lines(int tuple_num, const int index[], cell_t array[]);

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
 * Tuple 解析メイン関数
 * @param array ナンプレ盤面配列
 * @param info 解情報
 * @return 1:この解法によって数字をけせた 0:この解法によって数字を消せなかった
 */
int analyze_tuple(numpl_array * array, solve_info * info)
{
    static const int control_size = 7;
    static const int control[7][2] ={
	{0, 2}, {0, 3}, {1, 2}, {1, 3}, {0, 4}, {1, 4}, {0, 5}};
    int score;
    for (int i = 0; i < control_size; i++) {
	int tuple_count = control[i][1];
	score = analyze_tuple_sub(control[i][0], tuple_count, array, info);
	if (score < 0) {
	    return score;
	}
	if (score > 0) {
	    return 1;
	}
    }
    return 0;
}

/**
 * Tuple 用解析下請け関数
 * @param naked naked/hidden の区別
 * @param tuple_num 組の数(2:Naked Pair, 3:Naked Triple)
 * @param array ナンプレ盤面配列（実際の配列）
 * @param info 解情報
 * @return 1:この解法によって数字をけせた 0:この解法によって数字を消せなかった
 */
static int analyze_tuple_sub(int naked, int tuple_num, numpl_array * array,
			     solve_info * info)
{
    int c = 0;
    for (int i = 0; i < LINE_KINDS; i++) {
	for (int j = 0; j < LINE_SIZE; j++) {
	    if (naked) {
		c = kill_naked_lines(tuple_num, all_lines[i][j], array->ar);
	    } else {
		c = kill_hidden_lines(tuple_num, all_lines[i][j], array->ar);
	    }
	    if (c < 0) {
		return c;
	    } else if (c == 0) {
		continue;
	    }
	    if (naked) {
		info->naked_tuple[tuple_num - 2]++;
	    } else {
		info->hidden_tuple[tuple_num - 2]++;
	    }
	    info->kt_count++;
	    return 1;
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
	if ((mask | multiple) != multiple) {
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

/**
 * 仮想行１行についてnaked tupleの処理を行う
 * @param tuple_num 組の数(2:Naked Pair, 3:Naked Triple)
 * @param index 仮想行
 * @param array ナンプレ盤面配列（実際の配列）
 * @return 1:この解法によって数字をけせた 0:この解法によって数字を消せなかった
 */
static int kill_naked_lines(int tuple_num, const int index[], cell_t array[])
{
    uint16_t multiple = count_multiple(index, array);
    if (ones16(multiple) < tuple_num || ones16(multiple) <= 2) {
	return 0;
    }
    int result = 0;
    int count = tuple_num;
    for (uint16_t mask = 3; mask < FULL_SYMBOL; mask++) {
	if ((mask | multiple) != multiple) {
	    continue;
	}
	if (ones16(mask) != count) {
	    continue;
	}
	int cells = count_naked_cells(mask, index, array);
	if (cells == count) {
	    result = kill_naked_cells(mask, index, array);
	    if (result > 0) {
		break;
	    }
	}
    }
    return result;
}

/**
 * 仮想行１行についてhidden tupleの処理を行う
 * @param tuple_num 組の数(2:Hidden Pair, 3:Hidden Triple)
 * @param index 仮想行
 * @param array ナンプレ盤面配列（実際の配列）
 * @return 1:この解法によって数字をけせた 0:この解法によって数字を消せなかった
 */
static int kill_hidden_lines(int tuple_num, const int index[], cell_t array[])
{
    uint16_t multiple = count_multiple(index, array);
    if (ones16(multiple) < tuple_num || ones16(multiple) <= 2) {
	return 0;
    }
    int result = 0;
    int count = tuple_num;
    for (uint16_t mask = 3; mask < FULL_SYMBOL; mask++) {
	if ((mask | multiple) != multiple) {
	    continue;
	}
	if (ones16(mask) != count) {
	    continue;
	}
	int cells = count_hidden_cells(mask, index, array);
	if (cells == count) {
	    result = kill_hidden_cells(mask, index, array);
	    if (result > 0) {
		break;
	    }
	}
    }
    return result;
}

/**
 * 与えられた候補の組についてhidden tuple が成立するマスの数を数える
 * @param mask 候補数字の組
 * @param index 仮想行
 * @param array ナンプレ盤面配列（実際の配列）
 * @return 与えられた組を含むマスの数
 */
static int count_hidden_cells(uint16_t mask, const int index[], cell_t array[])
{
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	cell_t sym = array[index[i]];
	if ((sym.symbol & mask) != 0) {
	    count++;
	}
    }
    return count;
}

/**
 * 与えられた候補の組についてnakede tuple によってそのマスから他の候補を削除する
 * @param mask 候補数字の組
 * @param index 仮想行
 * @param array ナンプレ盤面配列（実際の配列）
 * @return 候補を削除したマスの数
 */
static int kill_hidden_cells(uint16_t mask, const int index[], cell_t array[])
{
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	uint16_t sym = array[index[i]].symbol;
	if ((sym | mask) != mask &&
	    (sym & mask) != 0) {
	    array[index[i]].symbol = sym & mask;
	    count++;
	}
    }
    return count;
}
