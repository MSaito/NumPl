/**
 * Naked Single
 * あるマスに数字が一つしか入っていなければ、そのマスの属する行、列、ブロックの他のマスから
 * その数字を消せる。
 */
#include "numpl.h"
#include "constants.h"
#include "killer.h"
#include "inline_functions.h"

static int kill_line(const int line[], cell_t * ar);
static int check_array(numpl_array * array);

/**
 * Naked Single メイン関数
 * この関数は一番基本となる関数なので一回消したあとも続けて実行する。
 * @param array ナンプレ盤面配列
 * @return 数字を消したマスの数
 */
int kill_single(numpl_array * array)
{
    int result = check_array(array);
    if (result < 0) {
	return result;
    }
    result = 0;
    for (int i = 0; i < LINE_KINDS; i++) {
	for (int j = 0; j < LINE_SIZE; j++) {
	    result += kill_line(all_lines[i][j], array->ar);
	}
    }
    return result;
}

/**
 * Naked Single 下請け関数
 * 注意：この関数では同じ数字が二カ所のマスに単独であるとき
 * その矛盾を発見できない。
 * @param line 仮想行
 * @param ar ナンプレ盤面配列（実際の配列）
 * @return 数字を消したマスの数
 */
static int kill_line(const int line[], cell_t * ar)
{
    int count = 0;
    uint16_t syms = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	int idx = line[i];
	if (is_single(ar[idx])) {
	    syms |= ar[idx].symbol;
	}
    }
    for (int i = 0; i < LINE_SIZE; i++) {
	int idx = line[i];
	if (is_single(ar[idx])) {
	    continue;
	}
	if (ar[idx].symbol & syms) {
	    ar[idx].symbol &= ~syms;
	    count++;
	}
    }
    return count;
}

/**
 * 1. そのマスに入る数字候補がひとつもないかチェック。
 * 2. 数字が単独のマスがあればフラグを立てる（関数を呼ぶ）
 * @param array ナンプレ盤面配列
 * @return -1 矛盾あり、0:矛盾なし
 */
static int check_array(numpl_array * array)
{
    cell_t *ar = array->ar;
    for (int i = 0; i < ARRAY_SIZE; i++) {
	if (ar[i].symbol == 0) {
	    return -1;
	}
	set_single_flag(ar[i]);
    }
    return 0;
}
