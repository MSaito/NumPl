/**
 * digging hole アルゴリズム
 *
 */
#include "numpl.h"
#include "common.h"
#include "solve.h"
#include "inline_functions.h"
#include "killer.h"

static int prepare_array(numpl_array * array);
static int digging_hole(numpl_array * array);

/**
 * ナンプレ盤面の完成形を得る。
 * ランダムに数字をセットし、再帰的に解く
 * @param array ナンプレ盤面（出力）
 * @return 1: 作成できた
 * @return それ以外: 作成できなかった
 */
static int prepare_array(numpl_array * array)
{
    for (int i = 0; i < ARRAY_SIZE; i++) {
	array->ar[i].symbol = FULL_SYMBOL;
	array->ar[i].fixed = 0;
    }
    for (int i = 0; i < 10; i++) {
	int idx = get_random(ARRAY_SIZE);
	uint16_t sym = array->ar[idx].symbol;
	sym = random_one_symbol(sym);
	if (sym == 0) {
	    continue;
	}
	array->ar[idx].symbol = sym;
	int r = kill_single(array);
	if (r < 0) {
	    return r;
	}
    }
    return recursion_solve(array);
}

/**
 * digging hole アルゴリズムにより問題を作成する
 * 点対称な問題を作成する。
 * 人間的解法で解ける盤面を生成する。
 * @param array ナンプレ盤面（入出力）
 * @return 1: 作成できた
 * @return それ以外: 作成できなかった
 */
static int digging_hole(numpl_array * array)
{
    for (int i = 0; i < ARRAY_SIZE; i++) {
	array->ar[i].fixed = 1;
    }
    numpl_array work = *array;
    for (int i = 0; i < ARRAY_SIZE / 2; i++) {
	int idx1 = get_random(ARRAY_SIZE);
	int idx2 = get_counter(idx1);
	if (array->ar[idx1].fixed == 0) {
	    continue;
	}
	array->ar[idx1].fixed = 0;
	array->ar[idx2].fixed = 0;
	array->ar[idx1].symbol = FULL_SYMBOL;
	array->ar[idx2].symbol = FULL_SYMBOL;
	solve_info info;
	int r = solve(&work, &info);
	if (r == 0) {
	    *array = work;
	    return 1;
	} else if (r == 1) {
	    work = *array;
	} else {
	    *array = work;
	}
    }
    return -1;
}

#if defined(MAIN)
#include <stdio.h>
#include <time.h>
int main(int argc, char * argv[])
{
    uint32_t seed = (uint32_t)clock();
    xsadd_init(&xsadd, seed); // これを忘れてはならない
    numpl_array array;
    int r = 0;
    do {
	r = prepare_array(&array);
    } while (r != 1);
#if defined(DEBUG)
    printf("after prepare_array r = %d", r);
    output_detail(&array);
#endif
    numpl_array save = array;
    do {
	array = save;
	r = digging_hole(&array);
    } while (r != 1);
#if defined(DEBUG)
    printf("after digging_hole r = %d", r);
    output_detail(&array);
#endif
    fixed_only(&array, FULL_SYMBOL);
    solve_info info;
    r = solve(&array, &info);
    print_solve_info(&info, 1);
    if (r == 1) {
	fixed_only(&array, 0);
	output_detail(&array);
    } else {
	printf("failure\n");
	output_detail(&array);
	return -1;
    }
    return 0;
}
#endif
