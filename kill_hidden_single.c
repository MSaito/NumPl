/**
 * Hidden Single
 * 一つの（仮想）行内にある数字の含まれるマスが一つしかなければ、
 * そのマスのそれ以外の数字は消せる。
 */
#include "numpl.h"
#include "constants.h"
#include "killer.h"
#include "inline_functions.h"
#include "solve.h"

static int kill_hidden_line(const int line[], cell_t * ar);

/**
 * Hidden Single メインプログラム
 * @array ナンプレ盤面配列
 * @return 数字を消した行の数
 */
int kill_hidden_single(numpl_array * array)
{
    for (int i = 0; i < LINE_KINDS; i++) {
	for (int j = 0; j < LINE_SIZE; j++) {
	    int result = kill_hidden_line(all_lines[i][j], array->ar);
	    if (result > 0) {
		return 1;
	    }
	}
    }
    return 0;
}

/**
 * Hidden Single 解析メインプログラム
 * @param array ナンプレ盤面配列
 * @param info 解情報
 * @return 仮の評価値
 */
int64_t analyze_hidden_single(numpl_array * array, solve_info * info)
{
    numpl_array save;
    numpl_array best;
    solve_info save_info;
    solve_info best_info;
    int64_t best_score = -1;
    int64_t this_score = 0;
    int changed = 1;
    int64_t pre_score = analyze_single(array, info) * 100;
    if (info->solved) {
	return pre_score * 100;
    }
    while (changed) {
	changed = 0;
	save = *array;
	save_info = *info;
	for (int i = 0; i < LINE_KINDS; i++) {
	    for (int j = 0; j < LINE_SIZE; j++) {
		save = *array;
		int result = kill_hidden_line(all_lines[i][j], array->ar);
		if (result < 0) {
		    return result;
		} else if (result == 0) {
		    continue;
		}
		changed = 1;
		int64_t score = analyze_single(array, info);
		if (score > best_score) {
		    best = *array;
		    best_info = *info;
		    best_score = score;
		}
		*array = save;
		*info = save_info;
	    }
	}
	if (changed) {
	    info->kh_count++;
	    *array = best;
	    *info = best_info;
	    this_score += best_score * 100 + 1;
	    best_score = -1;
	}
	if (info->solved) {
	    break;
	}
    }
    if (info->solved) {
	return (pre_score + this_score) * 100;
    } else {
	return pre_score + this_score;
    }
}

/**
 * Hidden Single 下請け関数
 * @param line 検査する仮想行
 * @param ar ナンプレ盤面配列（の実際の配列）
 * @return 数字を消したマスの数
 */
static int kill_hidden_line(const int line[], cell_t * ar)
{
    int pos = 0;
    for (uint16_t sym = 1; sym <= MAX_SYMBOL; sym <<= 1) {
	int count = 0;
	for (int i = 0; i < LINE_SIZE; i++) {
	    int idx = line[i];
	    if (ar[idx].symbol == sym) {
		count = 0;
		break;
	    }
	    if (ar[idx].symbol & sym) {
		pos = idx;
		count++;
	    }
	}
	if (count == 1) {
	    ar[pos].symbol = sym;
	    return 1;
	}
    }
    return 0;
}
