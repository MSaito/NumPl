/**
 * @file normalize.c
 * ナンプレの標準形を求める
 *
 * なお、かなりの手抜きなので本来の意味の標準形ではない。
 */
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

