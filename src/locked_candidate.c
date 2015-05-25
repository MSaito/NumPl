/**
 * Locked Candidate
 * 列または行とブロックの共通部分を見て、片方にしかない数字があればもう片方から消せる
 */
#include "numpl.h"
#include "constants.h"
#include "killer.h"
#include "solve.h"
#include <stdio.h>
#include <inttypes.h>

static int kill_locked_lines(int common_size, const int index1[],
			     const int index2[], cell_t array[]);
static void search_common(int idx[], const int max, const int index1[],
			  const int index2[]);
static int count_diff(uint16_t mask, const int idx[], int common_size,
		      const int index[], cell_t array[]);
static int is_in(const int idx[], int common_size, int num);
static int kill_diff(uint16_t mask, const int common[], int common_size,
		     const int index[], cell_t array[]);

/**
 * Locked Candidate メイン関数
 * @param array ナンプレ盤面配列
 * @return 1:この解法により数字を消した。0:消せなかった
 */
int kill_locked_candidate(numpl_array * array)
{
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	for (int j = 0; j < BLOCK_ROWS; j++) {
	    count = kill_locked_lines(BLOCK_ROWS, blocks[i],
				      rows[locked_rows[i][j]], array->ar);
	    if (count > 0) {
		return 1;
	    }
	}
	for (int j = 0; j < BLOCK_COLS; j++) {
	    count = kill_locked_lines(BLOCK_COLS, blocks[i],
				       cols[locked_cols[i][j]], array->ar);
	    if (count > 0) {
		return 1;
	    }
	}
    }
    return count;
}

/**
 * Locked Candidate 解析メイン関数
 * @param array ナンプレ盤面配列
 * @param info 解情報
 * @return 1:この解法により数字を消した。0:消せなかった
 */
int analyze_locked_candidate(numpl_array * array, solve_info * info)
{
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	for (int j = 0; j < BLOCK_ROWS; j++) {
	    count = kill_locked_lines(BLOCK_ROWS, blocks[i],
				      rows[locked_rows[i][j]], array->ar);
	    if (count > 0) {
		info->kl_count++;
		return 1;
	    }
	}
	for (int j = 0; j < BLOCK_COLS; j++) {
	    count = kill_locked_lines(BLOCK_COLS, blocks[i],
				       cols[locked_cols[i][j]], array->ar);
	    if (count > 0) {
		info->kl_count++;
		return 1;
	    }
	}
    }
    return 0;
}

/**
 * Locked Candidate 下請け関数
 * @param common_size 共通部分のサイズ
 * @param index1 ブロック
 * @param index2 行または列
 * @param array ナンプレ盤面配列
 * @return 数字を消したマスの数
 */
static int kill_locked_lines(int common_size, const int index1[],
			     const int index2[], cell_t array[])
{
    int result = 0;
    int common[common_size];
    search_common(common, common_size, index1, index2);
    if (common[0] == -1) {
	return 0;
    }
    for (uint16_t mask = 1; mask <= 0x100; mask = mask << 1) {
	int count = 0;
	// 共通部分に数字があるか
	for (int i = 0; i < common_size; i++) {
	    if (array[common[i]].symbol & mask) {
		count++;
	    }
	}
	if (count) {
	    // 他の部分に数字があるか
	    int diff = count_diff(mask, common, common_size, index1, array);
	    if (diff == 0) {
		result = kill_diff(mask, common, common_size, index2, array);
		if (result > 0) {
		    return 1;
		}
	    }
	    diff = count_diff(mask, common, common_size, index2, array);
	    if (diff == 0) {
		result = kill_diff(mask, common, common_size, index1, array);
		if (result > 0) {
		    return 1;
		}
	    }
	}
    }
    return 0;
}

/**
 * 二つの仮想行の共通部分を返す
 * @param idx 出力される共通部分
 * @param common_size 共通部分のサイズ
 * @param index1 仮想行1
 * @param index2 仮想行2
 */
static void search_common(int idx[], int common_size, const int index1[],
			  const int index2[])
{
    int p = 0;
    for (int i = 0; i < common_size; i++) {
	idx[i] = -1;
    }
    for (int i = 0; i < LINE_SIZE; i++) {
	for (int j = 0; j < LINE_SIZE; j++) {
	    if (index1[i] == index2[j]) {
		idx[p++] = index1[i];
	    }
	}
    }
}

/**
 * 配列内に指定された要素があるか判定する
 * @param idx 判定される配列
 * @param size 配列サイズ
 * @param num 要素
 * @return 1:配列内に指定された要素あり 2:配列内に指定された要素なし
 */
static int is_in(const int idx[], int size, int num)
{
    for (int i = 0; i < size; i++) {
	if (idx[i] == num) {
	    return 1;
	}
    }
    return 0;
}

/**
 * idx になくてindex にあるコマについてmask の数を数える
 * @param mask 検索する数字
 * @param idx 仮想行の共通部分
 * @param size idxのサイズ
 * @param index 検索される仮想行
 * @param array ナンプレ盤面配列（実際の配列）
 * @return idx になくてindexにある数字の個数
 */
static int count_diff(uint16_t mask, const int idx[], int size,
		      const int index[], cell_t array[])
{
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	if (is_in(idx, size, index[i])) {
	    continue;
	}
	if (array[index[i]].symbol & mask) {
	    count++;
	}
    }
    return count;
}

/**
 * 共通部分にあり仮想行にもある指定された数字を仮想行から消す
 * @param mask 数字
 * @param common 共通部分
 * @param size 共通部分のサイズ
 * @param index 仮想行
 * @param array ナンプレ盤面配列（実際の配列）
 * @return 数字を消したマスの数
 */
static int kill_diff(uint16_t mask, const int common[],
		     int size, const int index[], cell_t array[]) {
    uint16_t kill_mask = (~mask) & FULL_SYMBOL;
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	if (is_in(common, size, index[i])) {
	    continue;
	}
	if (array[index[i]].symbol & mask) {
	    array[index[i]].symbol &= kill_mask;
	    count++;
	}
    }
    return count;
}
