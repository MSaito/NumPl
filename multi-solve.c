#include "numpl.h"
#include "multi-solve.h"
#include "solve.h"
#include "common.h"
#include "inline_functions.h"
#include <string.h>

static int sol(numpl_array * array);
static int rs(numpl_array *array, int depth);

/**
 * ランダム解法
 * 一意でない解のあるナンプレ配列を受け取り、ランダムに解を得る。
 * @param ナンプレ盤面配列
 * @return 1 解けた
 * @return 0 解けない
 */
int random_solve(numpl_array *array)
{
    return rs(array, 1);
}

/**
 * ランダム解法再帰部分
 * 一意でない解のあるナンプレ配列を受け取り、ランダムに解を得る。
 * @param ナンプレ盤面配列
 * @param depth 再帰の深さ
 * @return 1 解けた
 * @return 0 解けない
 * @return -100 再帰の深さが設定を超えた
 */
static int rs(numpl_array *array, int depth)
{
#if defined(DEBUG)
    printf("rs depth = %d\n", depth);
#endif
    if (depth > 200) {
	return -100;
    }
    numpl_array work = *array;
    int r = sol(&work);
    if (r < 0) {
	return r;
    }
    if (r == 1) {
	*array = work;
	return r;
    }
    numpl_array save = work;
    for (int i = 0; i < 10; i++) { // 3
	for (int j = 0; j < 1; j++) { // 3
	    int idx = get_random(ARRAY_SIZE);
	    uint16_t syms = work.ar[idx].symbol;
//	    work.ar[idx].symbol = get_one_symbol(syms);
	    work.ar[idx].symbol = random_one_symbol(syms);
	    set_single_flag(work.ar[idx]);
	}
	r = rs(&work, depth + 1);
	if (r == -100) {
	    return r;
	}
	if (r > 0) {
	    *array = work;
	    return r;
	}
	work = save;
    }
    return 0;
}

/**
 * 解情報を気にしないで解く
 * @param array ナンプレ盤面配列
 * @return -1 エラー
 * @return 1 解けた
 * @return 0 解けない
 */
static int sol(numpl_array * array)
{
    solve_info info;
    return solve(array, &info);
}
