/**
 * X-Wing, SwordFish, ... といった解法アルゴリズム
 * 現時点では finned, sashimi には対応していない。
 */
#include "numpl.h"
#include "killer.h"
#include "inline_functions.h"
#include "constants.h"
#include <stdio.h>
#include <math.h>

static int fishsub(int fish_count, uint16_t mask,
		 const int rows[LINE_SIZE][LINE_SIZE],
		 numpl_array * array);
static void make_mat(uint16_t mask,
		     uint16_t mat[LINE_SIZE],
		     const int rows[LINE_SIZE][LINE_SIZE],
		     numpl_array * array);
static void adjust_patterns(int fish_count, uint16_t patterns[],
			    int point, int size);
static int kill_fish_cells(uint16_t mask,
		     uint16_t pat,
		     uint16_t mat[LINE_SIZE],
		     const int rows[LINE_SIZE][LINE_SIZE],
		     numpl_array * array);
static int contains(uint16_t array[], int pos, uint16_t value);
#if defined(DEBUG) && 0
static void print_patterns(uint16_t pattern[], int size);
#endif

/**
 * fish 系解法メインプログラム
 * X-Wing から検索する
 * @param array ナンプレ盤面配列
 * @return 0: この解法が適用できない
 * @return 1: この解法で候補が削除できた
 */
int kill_fish(numpl_array * array)
{
    for (int i = 2; i < LINE_SIZE - 1; i++) {
	for (uint16_t mask = 1; mask <= MAX_SYMBOL; mask = mask << 1) {
	    int count = fishsub(i, mask, rows, array);
	    if (count > 0) {
		return count;
	    }
	    count = fishsub(i, mask, cols, array);
	    if (count > 0) {
		return count;
	    }
	}
    }
    return 0;
}

/**
 * fish 系解析メインプログラム
 * X-Wing から検索する
 * @param array ナンプレ盤面配列
 * @return 0: この解法が適用できない
 * @return 1: この解法で候補が削除できた
 */
int analyze_fish(numpl_array * array, solve_info * info)
{
    int count = 0;
    for (int i = 2; i < LINE_SIZE - 1; i++) {
	for (uint16_t mask = 1; mask <= MAX_SYMBOL; mask = mask << 1) {
	    count = fishsub(i, mask, rows, array);
	    if (count <= 0) {
		count = fishsub(i, mask, cols, array);
	    }
	    if (count < 0) {
		return count;
	    } else if (count == 0) {
		continue;
	    }
	    info->sf_count++;
	    info->fish[i - 2]++;
	    return 1;
	}
    }
    return 0;
}

/**
 * fish系下請け関数
 * @param fish_count 2 ならX-Wing, 3 ならSwordFish
 * @param mask 注目している数字
 * @param rows rows が渡ってくれば横に条件成立を探す、colsなら縦に条件成立を探す
 * @param array ナンプレ盤面配列
 */
static int fishsub(int fish_count, uint16_t mask,
		 const int rows[LINE_SIZE][LINE_SIZE],
		 numpl_array * array)
{
    uint16_t mat[LINE_SIZE];
    make_mat(mask, mat, rows, array);
    int count1 = 0;
    int count2 = 0;
    uint16_t patterns[LINE_SIZE * LINE_SIZE];
    int p = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	int c1 = 0;
	int c2 = 0;
	for (int j = 0; j < LINE_SIZE; j++) {
	    c1 += (mat[i] >> j) & 1;
	    c2 += (mat[j] >> i) & 1;
	}
	if (c1 >= 2 && c1 <= fish_count) {
	    count1++;
	    patterns[p++] = mat[i];
	}
	if (c2 >= 2) {
	    count2++;
	}
    }
    // 以下の二つのif文は明らかに条件が成立しない場合に検索を諦める
    if (count1 < fish_count) {
	return 0;
    }
    if (count2 < fish_count) {
	return 0;
    }
    adjust_patterns(fish_count, patterns, p, LINE_SIZE * LINE_SIZE);
    for (int i = 0; i < LINE_SIZE * LINE_SIZE; i++) {
	uint16_t pat = patterns[i];
	if (pat == UINT16_C(0xffff)) {
	    continue;
	}
	int count = 0;
	for (int j = 0; j < LINE_SIZE; j++) {
	    if ((mat[j] | pat) == pat) {
		count++;
	    }
	}
	// ここで条件成立が確定
	if (count == fish_count) {
	    int result = kill_fish_cells(mask, pat, mat, rows, array);
	    if (result > 0) {
		return 1;
	    }
	}
    }
    return 0;
}

#if defined(DEBUG) && 0
static void print_patterns(uint16_t pattern[], int size)
{
    for (int i = 0; i < size; i++) {
	uint32_t mask = 0x8000;
	for (int j = 0; j < 16; j++) {
	    if (pattern[i] & mask) {
		printf("1");
	    } else {
		printf("0");
	    }
	    mask = mask >> 1;
	}
	printf("\n");
    }
}
#endif

/**
 * 特定の数字に着目して二次元のビット配列にする。
 * ただし、uint16_t の一次元配列に入れる。
 * @param mask 着目している数字
 * @param mat 出力配列
 * @param rows 入力配列
 * @param array ナンプレ盤面配列
 */
static void make_mat(uint16_t mask,
		     uint16_t mat[LINE_SIZE],
		     const int rows[LINE_SIZE][LINE_SIZE],
		     numpl_array * array)
{
    for (int i = 0; i < LINE_SIZE; i++) {
	mat[i] = 0;
	uint16_t m = 1;
	for (int j = 0; j < LINE_SIZE; j++) {
	    if ((array->ar[rows[i][j]].symbol & mask) != 0) {
		mat[i] |= m;
	    }
	    m = m << 1;
	}
    }
}

/**
 * fish_count 未満のビットパターンを組み合わせて fish_count になるビットパターンを
 * 作って返す。
 * @param fish_count 2 ならX-Wing, 3 なら SwordFish
 * @param patterns 入出力配列
 * @param point pattern配列に入っているパターンの個数
 * @param size pattern配列の宣言された大きさ
 */
static void adjust_patterns(int fish_count, uint16_t patterns[],
			    int point, int size)
{
    uint16_t result[size];
    int result_pos = 0;
    for (int i = point; i < size; i++) {
	patterns[i] = UINT16_C(0xffff);
    }
    // 同じパターンを一つだけにする
    for (int i = 0; i < point; i++) {
	if (patterns[i] == UINT16_C(0xffff)) {
	    continue;
	}
	if (ones16(patterns[i]) == fish_count) {
	    if (!contains(result, result_pos, patterns[i])) {
		result[result_pos++] = patterns[i];
	    }
	    continue;
	}
	// 部分パターンは組み合わせる
	for (int j = i + 1; j < point; j++) {
	    uint16_t p = patterns[i] | patterns[j];
	    if (ones16(p) == fish_count) {
		if (!contains(result, result_pos, p)) {
		    result[result_pos++] = p;
		}
	    }
	}
    }
    for (int i = 0; i < result_pos; i++) {
	patterns[i] = result[i];
    }
    for (int i = result_pos; i < size; i++) {
	patterns[i] = UINT16_C(0xffff);
    }

}

/**
 * fish解法で条件が成立しているときに、その条件によって消せる数字があれば消して
 * 消した個数を返す。消せる数字がなければ0を返す。
 * @param mask 注目している数字
 * @param pat パターン
 * @param mat ビットパターンの配列
 * @param rows 行または列
 * @param array ナンプレ盤面配列
 * @return 消した数字の個数
 */
static int kill_fish_cells(uint16_t mask,
			  uint16_t pat,
			  uint16_t mat[LINE_SIZE],
			  const int rows[LINE_SIZE][LINE_SIZE],
			  numpl_array * array)
{
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	if ((mat[i] | pat) == pat) {
	    continue;
	}
	uint16_t m = 1;
	for (int j = 0; j < LINE_SIZE; j++) {
	    if ((m | pat) == pat &&
		array->ar[rows[i][j]].symbol & mask) {
		count++;
		array->ar[rows[i][j]].symbol &= ~mask;
	    }
	    m = m << 1;
	}
    }
    return count;
}

/**
 * 配列の中に等しいものがあるか調べる
 * @param array 配列
 * @param size 配列のサイズ
 * @param value 調べる値
 * @return 0:等しいものはない。1:等しいものがある。
 */
static int contains(uint16_t array[], int size, uint16_t value)
{
    for (int i = 0; i < size; i++) {
	if (array[i] == value) {
	    return 1;
	}
    }
    return 0;
}
