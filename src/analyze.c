/**
 * @file analyze.c
 * やや詳細な解法の種類を求める。
 */
#include "numpl.h"
#include "solve.h"
#include "killer.h"
#include "common.h"
#include "analyze.h"
#include <stdio.h>
#include <string.h>

/** 候補を減らす関数タイプ */
typedef int (*analyzer_t)(numpl_array *, solve_info *);

/** 関数 */
static analyzer_t analyzers[] = {
    analyze_single,
    analyze_hidden_single,
    analyze_locked_candidate,
    analyze_tuple,
    analyze_fish,
    analyze_xywing,
    NULL};

/**
 * 与えられた問題を解く上で必要な解法を求める
 * @param array ナンプレ盤面配列
 * @param info 解情報
 * @return 1:解けた
 * @return 0:解けない
 */
double analyze(numpl_array * array, solve_info * info)
{
#if defined(DEBUG)
    printf("IN ANALYZE\n");
#endif
    memset(info, 0, sizeof(info));
    int c = 0;
    do {
#if defined(DEBUG)
        printf("ANALYZE DO c = %d\n", c);
#endif
        for (int i = 0; analyzers[i] != NULL; i++) {
            c = analyzers[i](array, info);
            if (c < 0) {
                info->solved = 0;
                break;
            }
            if (c > 0) {
                break;
            }
        }
    } while (c > 0);
    if (c < 0) {
#if defined(DEBUG)
        printf("OUT ANALYZE c = %d\n", c);
#endif
        info->solved = 0;
        return -1;
    }
    if (is_solved(array)) {
        info->solved = 1;
    } else {
        info->solved = 0;
    }
    info->fx_count = count_fixed(array);
#if defined(DEBUG)
    printf("OUT ANALYZE solved = %d\n", info->solved);
#endif
    return get_analyze_value(info);
}

/**
 * 難易度を仮に評価する。
 * @param info
 * @return 仮の評価血
 */
double get_analyze_value(solve_info * info)
{
    double v = info->fx_count;
    double s = 100;
    v += s * info->ks_count;
    s *= 100;
    v += s * info->kh_count;
    s *= 100;
    v += s * info->kl_count;
    for (int i = 0; i < LINE_SIZE / 2 + 1; i++) {
        v += s * info->naked_tuple[i];
        s *= 100;
    }
    for (int i = 0; i < LINE_SIZE / 2 + 1; i++) {
        v += s * info->hidden_tuple[i];
        s *= 100;
    }
    for (int i = 0; i < LINE_SIZE; i++) {
        v += s * info->fish[i];
        s *= 100;
    }
    v += s * info->xy_count;
    return v;
}

