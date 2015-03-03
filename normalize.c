#include "numpl.h"
#include "convert.h"
#include "inline_functions.h"
#include "normalize.h"
#include "analyze.h"
#include "common.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

static uint64_t get_normalize_value(numpl_array * array);
static void normalize_symbol(numpl_array * array);

/**
 * 正規化用評価値計算
 * (固定数字の位置が上三角に多く集まるように設定)
 * @param array ナンプレ盤面配列
 * @return 正規化用評価値
 */
static uint64_t get_normalize_value(numpl_array * array)
{
    static const uint64_t normalize_values[] = {
	1000000000, 2000000000, 3000000000, 10000000000, 20000000000,
	30000000000, 100000000000, 200000000000, 300000000000,
	4000000, 5000000, 6000000, 40000000, 50000000, 60000000,
	400000000, 500000000, 600000000,
	1000000, 2000000, 3000000, 10000000, 20000000, 30000000,
	100000000, 200000000, 300000000,
	40000, 50000, 60000, 400000, 500000, 600000, 4000000, 5000000, 6000000,
	10000, 20000, 30000, 100000, 200000, 300000, 1000000, 2000000, 3000000,
	400, 500, 600, 4000, 5000, 6000, 7000, 8000, 9000,
	100, 200, 300, 1000, 2000, 3000, 3100, 3200, 3300,
	4, 5, 6, 40, 50, 60, 70, 80, 90,
	1, 2, 3, 10, 20, 30, 31, 32, 33,
    };

    uint64_t result = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
	if (array->ar[i].fixed) {
	    result += normalize_values[i];
	}
    }
    return result;
}

/**
 * 数字が1から順に出現するように数字の割り当てを変える
 * @param array ナンプレ盤面配列
 */
static void normalize_symbol(numpl_array * array)
{
    int table[LINE_SIZE];
    memset(table, 0, sizeof(table));
    int count = 0;
    int sym;
    int n;
    for (int i = 0; i < ARRAY_SIZE; i++) {
	if (array->ar[i].fixed) {
	    sym = array->ar[i].symbol;
	    n = symbol2num(sym);
	    if (table[n - 1] == 0) {
		table[n - 1] = 1 << count;
		count++;
	    }
	}
    }
    for (int i = 0; i < ARRAY_SIZE; i++) {
	if (array->ar[i].fixed) {
	    sym = array->ar[i].symbol;
	    n = symbol2num(sym);
	    array->ar[i].symbol = table[n - 1];
	}
    }
}

/**
 * いい加減な正規化
 * @param array ナンプレ盤面配列
 * @param info 解情報
 */
int lazy_normalize(numpl_array * array, solve_info * info)
{
    memset(info, 0, sizeof(solve_info));
    numpl_array best_array = *array;
    numpl_array save = *array;
    int best_value = get_normalize_value(array);
    numpl_array tmp = *array;
    double worst_info_value = analyze(&tmp, info);
    solve_info worst_info = *info;
    double a;
    int v;
    int changed = 0;
#if defined(DEBUG)
    printf("worst_info_value = %f\n", worst_info_value);
    print_solve_info(info, 0);
    printf("\n");
#endif
    for (int i = 1; i < 36; i++) {
	int block = i % 3;
	int line = (i / 3) % 3;
	int reverse = i / 9;
	if (reverse != 3) {
	    reverse_change(array, reverse);
	}
	if (block != 2) {
	    block_reverse(array, block);
	}
	if (line != 2) {
	    line_change(array, line);
	}
	v = get_normalize_value(array);
	tmp = *array;
	a = analyze(&tmp, info);
	if (v > best_value) {
	    best_array = *array;
	    best_value = v;
	    changed = 1;
#if defined(DEBUG)
	    printf("normalize done mode = %d\n", i);
#endif
	} else {
	    *array = save;
	}
	if (a < worst_info_value) {
	    worst_info = *info;
	    worst_info_value = a;
	}
    }
    *array = best_array;
    *info = worst_info;
    normalize_symbol(array);
    return changed;
}

#if defined(MAIN)
#include <getopt.h>
static int verbose = 0;
static uint32_t seed = 0;
static char * mondai_p = NULL;
static int parse_opt(int argc, char * argv[]);
static int parse_opt(int argc, char * argv[])
{
    static struct option longopts[] = {
        {"seed", required_argument, NULL, 's'},
        {"verbose", no_argument, NULL, 'v'},
        {NULL, 0, NULL, 0}};
    verbose = 0;
    seed = (uint32_t)clock();
    const char * pgm = argv[0];
    int c;
    int error = 0;
    for (;;) {
        if (error) {
            break;
        }
        c = getopt_long(argc, argv, "vs:", longopts, NULL);
	if (c < 0) {
	    break;
	}
	switch (c) {
	case 's':
	    seed = (uint32_t)strtoull(optarg, NULL, 10);
	    break;
	case 'v':
	    verbose = 1;
	    break;
	case '?':
	default:
	    error = 1;
	    break;
	}
    }
    argc -= optind;
    argv += optind;
#if defined(DEBUG)
    printf("seed = %d\n", seed);
    printf("argc = %d\n", argc);
#endif
    if (error) {
	printf("%s [-v] [-s seed] [mondai_string]\n", pgm);
	return -1;
    }
    if (argc > 0) {
	mondai_p = argv[0];
    }
    return 0;
}

int main(int argc, char * argv[])
{
    numpl_array work;
    int r;
    r = parse_opt(argc, argv);
    if (r < 0) {
	return r;
    }
    if (mondai_p == NULL) {
	char mondai[200];
	read_mondai(mondai, stdin);
	r = input(&work, mondai);
    } else {
	r = input(&work, mondai_p);
    }
    if (r != 0) {
	printf("input string is wrong\n");
	return -1;
    }
    fixed_only(&work, FULL_SYMBOL);
    solve_info info;
    r = lazy_normalize(&work, &info);
    printf("\"");
    print_solve_info(&info, 0);
    printf("\":");
    fixed_only(&work, 0);
    print_array(&work);
    printf("\n");
    return 0;
}
#endif
