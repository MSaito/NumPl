/**
 * @file generate.c
 * ナンプレの問題を生成する
 *
 * 直観と経験と試行錯誤の結果としてのヒューリスティックアルゴリズムによる。
 */
#include "numpl.h"
#include "killer.h"
#include "inline_functions.h"
#include "common.h"
#include "constants.h"
#include "generate.h"
#include "normalize.h"
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

static int tuple_vl(numpl_array * array,
                    const int vl[LINE_SIZE],
                    uint16_t syms1,
                    uint16_t syms2);
static void copy_corner(numpl_array * array, const numpl_array * solved);
static void copy_cross(numpl_array * array, const numpl_array * solved);
static void copy_blocks(numpl_array * array,
                        const numpl_array * solved, int blono);
static int phase1A(numpl_array * array);
static int phase1B(numpl_array * array);
static int phase2A(numpl_array * array);
static int phase2B(numpl_array * array);
static int phase2sub(numpl_array * array, int size1, const int blono[],
                     int size2, const int cross[], const int b_cross[][2]);
inline static int info_value(solve_info * info);
static int phase3(numpl_array * array, generate_type * type);
static int p3(numpl_array * array, int start, generate_type * type);
static int phase4(numpl_array * array);
static int phase5(numpl_array * array);

#if defined(DEBUG) && 0
static void debug_simple_kill(numpl_array * array);
static int debug_simple_once(numpl_array * array);
static int simple_kill(numpl_array * array, const int vl[LINE_SIZE]);
#endif

/**
 * ひとつの仮想行に数字を埋め込み、固定数字マスに設定する。対称性を保つ
 * ために、点対称の位置にも数字を埋め込む。既に固定数字マスになっていれ
 * ば埋め込まない。既に入っている数字たちを減らす形で埋め込むので、埋め
 * 込む数字が含まれていないマスには埋め込まない。
 *
 * @param array ナンプレ盤面配列
 * @param vl 仮想行
 * @param syms1 仮想行に埋め込む数字
 * @param syms2 点対称な位置に埋め込む数字
 * @return 0 数字を埋め込めた場合
 * @return -1 そうでない場合
 */
static int tuple_vl(numpl_array * array,
                    const int vl[LINE_SIZE],
                    uint16_t syms1,
                    uint16_t syms2)
{
    const uint16_t fixed = 1;
    uint16_t order[LINE_SIZE];
    get_random_number(order);
    int count = ones16(syms1);
    for (int i = 0; i < LINE_SIZE && count > 0; i++) {
        int idx1 = vl[order[i]];
        int idx2 = get_counter(idx1);
        if (array->ar[idx1].fixed == 1) {
            continue;
        }
        if (!(array->ar[idx1].symbol & syms1) ||
            !(array->ar[idx2].symbol & syms2)) {
            continue;
        }
        array->ar[idx1].fixed = fixed;
        set_single_flag(&array->ar[idx1]);
        array->ar[idx1].symbol &= syms1;
        array->ar[idx2].fixed = fixed;
        set_single_flag(&array->ar[idx2]);
        array->ar[idx2].symbol &= syms2;
        count--;
    }
    return -count;
}

/**
 * ナンプレ問題生成 第一段階（十字生成）
 *
 * 十字型のブロックにランダムに固定数字マスを作る。マスの位置はランダム
 * だが、中に入れる数字は最初の８個まで決め打ち。生成後にランダム化する
 * ので決め打ちでよい。
 *
 * この関数が呼ばれるとき、ナンプレ盤面配列には固定マスはなく、すべての
 * マスにすべての数字の組が入っている（初期状態）
 *
 * この関数が終了するとき、ナンプレ盤面配列には数字の入った固定マスが
 * 14個出来ている。そして、それ以外のマスは14個の固定マスによって消せる
 * だけ候補が消されている。
 *
 * @param array ナンプレ盤面配列
 * @return 0 正常
 * @return -1 エラー
 */
static int phase1A(numpl_array *array)
{
#if defined(DEBUG)
    printf("in phase1A\n");
    output_detail(array);
    printf("\n");
#endif
    numpl_array save = *array;
    //uint16_t syms = 3;
    solve_info info;
    int r;
    // step 1 十字型に fixed を配置する
    for (;;) {
        tuple_vl(array, blocks[1], 1, 2);
        tuple_vl(array, blocks[1], 4, 8);
        tuple_vl(array, blocks[3], 16, 32);
        tuple_vl(array, blocks[3], 64, 128);
        r = solve(array, &info);
#if defined(DEBUG)
        printf("r = %d\n", r);
#endif
        if (r >= 0) {
            break;
        } else {
            return r;
            //array = save;
        }
    }
    save = *array;
#if defined(DEBUG)
    printf("phase1A step1 end\n");
    output_detail(array);
#endif
    uint16_t ransyms[LINE_SIZE];
    get_random_symbol(ransyms);
    // step 2 中央に2個 fixed を配置する。数字はランダム
    for (int i = 0; i < 100; i++) {
        uint16_t s1 = ransyms[i % LINE_SIZE];
        uint16_t s2 = ransyms[(i + 1) % LINE_SIZE];
        r = tuple_vl(array, blocks[4], s1, s2);
        if (r < 0) {
            *array = save;
            continue;
        }
        r = solve(array, &info);
        if (r >= 0) {
            break;
        } else {
            *array = save;
        }
    }
    save = *array;
#if defined(DEBUG)
    printf("phase1A step2 end\n");
    output_detail(array);
#endif
    // step 3 十字型を 3 個ずつにする。数字はランダム
    get_random_symbol(ransyms);
    for (int i = 0; i < 100; i++) {
        uint16_t s1 = ransyms[i % LINE_SIZE];
        uint16_t s2 = ransyms[(i + 1) % LINE_SIZE];
        r = tuple_vl(array, blocks[1], s1, s2);
        r = tuple_vl(array, blocks[3], s1, s2);
        if (r < 0) {
            *array = save;
            continue;
        }
        r = solve(array, &info);
        if (r >= 0) {
            break;
        } else {
            *array = save;
        }
    }
    //save = *array;
#if defined(DEBUG)
    printf("phase1A step3 end\n");
    output_detail(array);
#endif
    return r;
}

/**
 * ナンプレ問題生成 第一段階（四隅生成）
 *
 * 四隅のブロックにランダムに固定数字マスを作る。マスの位置はランダム
 * だが、中に入れる数字は最初の８個まで決め打ち。生成後にランダム化する
 * ので決め打ちでよい。
 *
 * この関数が呼ばれるとき、ナンプレ盤面配列には固定マスはなく、すべての
 * マスにすべての数字の組が入っている（初期状態）
 *
 * この関数が終了するとき、ナンプレ盤面配列には数字の入った固定マスが
 * 14個出来ている。そして、それ以外のマスは14個の固定マスによって消せる
 * だけ候補が消されている。
 *
 * @param array ナンプレ盤面配列
 * @return 0 正常
 * @return -1 エラー
 */
static int phase1B(numpl_array *array)
{
#if defined(DEBUG)
    printf("in phase1B\n");
    output_detail(array);
    printf("\n");
#endif
    numpl_array save = *array;
    solve_info info;
    int r;
    // step 1 四隅に fixed を配置する
    for (;;) {
        tuple_vl(array, blocks[0], 1, 2);
        tuple_vl(array, blocks[0], 4, 8);
        tuple_vl(array, blocks[8], 16, 32);
        tuple_vl(array, blocks[8], 64, 128);
        r = solve(array, &info);
#if defined(DEBUG)
        printf("r = %d\n", r);
#endif
        if (r >= 0) {
            break;
        } else {
            return r;
            //array = save;
        }
    }
    save = *array;
#if defined(DEBUG)
    printf("phase1B step1 end\n");
    output_detail(array);
#endif
    uint16_t ransyms[LINE_SIZE];
#if 0
    get_random_symbol(ransyms);
    // step 2 中央に2個 fixed を配置する。数字はランダム
    for (int i = 0; i < 100; i++) {
        uint16_t s1 = ransyms[i % LINE_SIZE];
        uint16_t s2 = ransyms[(i + 1) % LINE_SIZE];
        r = tuple_vl(array, blocks[4], s1, s2);
        if (r < 0) {
            *array = save;
            continue;
        }
        r = solve(array, &info);
        if (r >= 0) {
            break;
        } else {
            *array = save;
        }
    }
    save = *array;
#if defined(DEBUG)
    printf("phase1B step2 end\n");
    output_detail(array);
#endif
#endif
    // step 3 四隅を 3 個ずつにする。数字はランダム
    get_random_symbol(ransyms);
    for (int i = 0; i < 100; i++) {
        uint16_t s1 = ransyms[i % LINE_SIZE];
        uint16_t s2 = ransyms[(i + 1) % LINE_SIZE];
        r = tuple_vl(array, blocks[0], s1, s2);
        r = tuple_vl(array, blocks[8], s1, s2);
        if (r < 0) {
            *array = save;
            continue;
        }
        r = solve(array, &info);
        if (r >= 0) {
            break;
        } else {
            *array = save;
        }
    }
    //save = *array;
#if defined(DEBUG)
    printf("phase1B step3 end\n");
    output_detail(array);
#endif
    return r;
}

/**
 * phase1A で固定数字が入った arrray に対して、それを満たすような解とな
 * る盤面が複数あることになる。その中の一つをsolved として、solved の四
 * 隅のブロックから array に内容をコピーし、コピーした部分は全部固定マ
 * スとしてフラグを立てる。これは部分的digging holeをやるための準備である。
 *
 * @param array phase1Aで固定数字が入った盤面配列
 * @param solved array から導かれる解のひとつ
 */
static void copy_corner(numpl_array * array, const numpl_array * solved)
{
#if defined(DEBUG) && 0
    printf("in copy_corner\n");
#endif
    // 四隅をsolvedからコピー
    const int blono[4] = {0, 2, 6, 8};
    for (int i = 0; i < 4; i++) {
        copy_blocks(array, solved, blono[i]);
    }
}

/**
 * phase1B で固定数字が入った arrray に対して、それを満たすような解とな
 * る盤面が複数あることになる。その中の一つをsolved として、solved の中
 * 央十字のブロックから array に内容をコピーし、コピーした部分は全部固
 * 定マスとしてフラグを立てる。これは部分的digging holeをやるための準備
 * である。
 *
 * @param array phase1Bで固定数字が入った盤面配列
 * @param solved array から導かれる解のひとつ
 */
static void copy_cross(numpl_array * array, const numpl_array * solved)
{
    // 中央をsolvedからコピー
    const int blono[5] = {1, 3, 4, 5, 7};
    for (int i = 0; i < 5; i++) {
        copy_blocks(array, solved, blono[i]);
    }
}

/**
 * ナンプレ盤面配列からナンプレ盤面配列に指定されたブロックをコピーし、
 * 固定マスとしてフラグを立てる。
 * @param dist コピー先ナンプレ盤面配列
 * @param source コピー元ナンプレ盤面配列
 * @param blono コピーするブロック
 */
static void copy_blocks(numpl_array * dist,
                        const numpl_array * source, int blono)
{
    for (int i = 0; i < LINE_SIZE; i++) {
        int idx = blocks[blono][i];
        dist->ar[idx].symbol = source->ar[idx].symbol;
        dist->ar[idx].fixed = 1;
        set_single_flag(&dist->ar[idx]);
    }
}

/**
 * ナンプレ問題生成 第二段階（十字生成）
 * 解からコピーした四隅の固定を外していく
 */
static int phase2A(numpl_array * array)
{
    const int blono[4] = {0, 2, 6, 8};
    const int cross[4] = {1, 3, 5, 7};
    const int b_cross[4][2] = {{0, 2}, {0, 6}, {2, 8}, {6, 8}};
    return phase2sub(array, 4, blono, 4, cross, b_cross);
}

/**
 * ナンプレ問題生成 第二段階（四隅生成）
 * 解からコピーした十字の固定を外していく
 */
static int phase2B(numpl_array * array)
{
    const int blono[5] = {1, 3, 4, 5, 7};
    const int cross[4] = {0, 2, 6, 8};
    const int b_cross[4][2] = {{1, 3}, {1, 5}, {3, 7}, {5, 7}};
    return phase2sub(array, 5, blono, 4, cross, b_cross);
}

/**
 * ナンプレ問題生成 第二段階（共通）
 *
 */
static int phase2sub(numpl_array * array, int size1, const int blono[],
                     int size2, const int cross[], const int b_cross[][2])
{
#if defined(DEBUG)
    printf("in phase2\n");
#endif
    // const int blono[4] = {0, 2, 6, 8};
    // 2個ずつ穴を開ける。四隅ブロック4個穴が開く
    for (int i = 0; i < 4; i++) {
        int bno = blono[i];
        uint16_t syms[LINE_SIZE];
        get_random_symbol(syms);
        uint16_t sym = syms[0] | syms[1];
        for (int j = 0; j < LINE_SIZE; j++) {
            int idx = blocks[bno][j];
            if (!array->ar[idx].fixed) {
                continue;
            }
            if (!(array->ar[idx].symbol & sym)) {
                continue;
            }
            int idx2 = get_counter(idx);
            array->ar[idx].symbol = FULL_SYMBOL;
            reset_single_flag(&array->ar[idx]);
            array->ar[idx].fixed = 0;
            reset_single_flag(&array->ar[idx]);
            array->ar[idx2].symbol = FULL_SYMBOL;
            reset_single_flag(&array->ar[idx2]);
            array->ar[idx2].fixed = 0;
            reset_single_flag(&array->ar[idx2]);
        }
    }
    // solve してみる 解けなければあきらめる
    solve_info info;
    int r = solve(array, &info);
#if defined(DEBUG)
    printf("phase2 step 1 solve r = %d fixed = %d\n", r, count_fixed(array));
#endif
    if (r <= 0) {
        return r;
    }
#if defined(DEBUG)
    printf("before fill_array fixed = %d\n", count_fixed(array));
#endif
    fixed_only(array, FULL_SYMBOL);
#if defined(DEBUG)
    printf("after fill_array fixed = %d\n", count_fixed(array));
#endif
    numpl_array save = *array;
    int save_r = r;
    // 四隅から十字にあるシンボルを取り除く
    //int cross[4] = {1, 3, 5, 7};
    //int b_cross[4][2] = {{0, 2}, {0, 6}, {2, 8}, {6, 8}};
    for (int i = 0; i < size1; i++) {
        int p = get_random(size2);
        uint16_t sym = 0;
        int cno = cross[p];
        for (int j = 0; j < LINE_SIZE; j++) {
            if (array->ar[blocks[cno][j]].fixed) {
                sym |= array->ar[blocks[cno][j]].symbol;
            }
        }
//      sym = get_one_symbol(sym);
        sym = random_one_symbol(sym);
#if defined(DEBUG)
        printf("sym = %x\n", sym);
#endif
        for (int j = 0; j < 2; j++) {
            int bno = b_cross[p][j];
            for (int k = 0; k < LINE_SIZE; k++) {
                int idx1 = blocks[bno][k];
                if (array->ar[idx1].symbol & sym) {
                    int idx2 = get_counter(idx1);
                    array->ar[idx1].symbol = FULL_SYMBOL;
                    array->ar[idx1].fixed = 0;
                    reset_single_flag(&array->ar[idx1]);
                    array->ar[idx2].symbol = FULL_SYMBOL;
                    array->ar[idx2].fixed = 0;
                    reset_single_flag(&array->ar[idx2]);
#if defined(DEBUG)
                    printf("idx1, idx2 = %d,%d\n", idx1, idx2);
#endif
                }
            }
        }
#if defined(DEBUG)
        printf("fixed = %d\n", count_fixed(array));
#endif
        fixed_only(array, FULL_SYMBOL);
        r = solve(array, &info);
#if defined(DEBUG)
        printf("phase2 st 2 r = %d, fixed = %d\n", r, count_fixed(array));
#endif
        if (r <= 0) {
            *array = save;
            break;
        }
        save = *array;
        save_r = r;
    }
#if defined(DEBUG)
    printf("phase2 step 2 solve r = %d\n", save_r);
#endif
    return save_r;
}

/**
 * ナンプレ問題生成 第三段階（全件検索）
 * fixを外しながら再帰的に全件検索を行う
 */
static int phase3(numpl_array * array, generate_type * type)
{
    fixed_only(array, FULL_SYMBOL);
    return p3(array, 0, type);
}

inline static int info_value(solve_info * info)
{
    return info->kh_count + info->kl_count * 100 + info->kt_count * 1000
        + info->sf_count * 10000 + info->xy_count * 100000
        + info->xyz_count * 1000000;
}
/**
 * ナンプレ問題生成 第三段階の再帰部分
 */
static int p3(numpl_array * array, int start, generate_type * type)
{
#if defined(DEBUG)
    printf("in phase3 start = %d\n", start);
#endif
    solve_info info;
    numpl_array save;
    fixed_only(array, FULL_SYMBOL);
    int r = solve(array, &info);
    if (r <= 0) {
#if defined(DEBUG)
        printf("in phase3 solve returns %d\n", r);
#endif
        return r;
    }
    if (type->hidden_single && (info.kh_count > 0)) {
        return info_value(&info);
    } else if (type->locked_candidate && (info.kl_count > 0)) {
        return info_value(&info);
    } else if (type->tuple && (info.kt_count > 0)) {
        return info_value(&info);
    } else if (type->fish && (info.sf_count > 0)) {
        return info_value(&info);
    } else if (type->xy && (info.xy_count > 0)) {
        return info_value(&info);
    } else if (type->xyz && (info.xyz_count > 0)) {
        return info_value(&info);
    }
    save = *array;
    int max = 0;
    numpl_array best = *array;
    for (int i = start; i < ARRAY_SIZE / 2; i++) {
        if (!array->ar[i].fixed) {
            continue;
        }
        int idx1 = i;
        int idx2 = get_counter(idx1);
        array->ar[idx1].fixed = 0;
        reset_single_flag(&array->ar[idx1]);
        array->ar[idx1].symbol = FULL_SYMBOL;
        array->ar[idx2].fixed = 0;
        reset_single_flag(&array->ar[idx2]);
        array->ar[idx2].symbol = FULL_SYMBOL;
        r = p3(array, i + 1, type);
        if (r > max) {
            max = r;
            best = *array;
        }
        *array = save;
    }
    if (max > 0) {
        *array = best;
        return max;
    }
    return 0;
}

/* ===============================
   phase4 全件検索 reduce (fix を 外しながら)
   =============================== */
static int phase4(numpl_array * array)
{
    fixed_only(array, FULL_SYMBOL);
    numpl_array save = *array;
    solve_info info;
    int count = 0;
    for (int i = 0; i < ARRAY_SIZE / 2; i++) {
        if (!array->ar[i].fixed) {
            continue;
        }
        int idx1 = i;
        int idx2 = get_counter(idx1);
        array->ar[idx1].fixed = 0;
        reset_single_flag(&array->ar[idx1]);
        array->ar[idx1].symbol = FULL_SYMBOL;
        array->ar[idx2].fixed = 0;
        reset_single_flag(&array->ar[idx2]);
        array->ar[idx2].symbol = FULL_SYMBOL;
        int r = solve(array, &info);
        if (r <= 0) {
            *array = save;
        } else {
            count++;
            fixed_only(array, FULL_SYMBOL);
            save = *array;
        }
    }
    *array = save;
    return count;
}

/* ===============================
   phase5 全件検索 非対称 reduce (fix を 外しながら)
   =============================== */
static int phase5(numpl_array * array)
{
    numpl_array save = *array;
    numpl_array best = *array;
    int max = 0;
    solve_info info;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (!array->ar[i].fixed) {
            continue;
        }
        int idx1 = i;
        array->ar[idx1].fixed = 0;
        reset_single_flag(&array->ar[idx1]);
        array->ar[idx1].symbol = FULL_SYMBOL;
        int r = solve(array, &info);
        if (r <= 0) {
            *array = save;
            continue;
        }
        int v = info_value(&info);
        if (max < v) {
            max = v;
            best = *array;
        }
        save = *array;
    }
    *array = best;
    return max;
}

int generate(numpl_array * array, generate_type * type)
{
#if defined(DEBUG)
    printf("in generate\n");
    output_detail(array);
    printf("\n");
#endif
    int phase = get_random(2);
    //int phase = 1;
    numpl_array save = *array;
    int r;
    for (int i = 0; i < 100; i++) {
        if (phase == 0) {
            r = phase1A(array);
        } else {
            r = phase1B(array);
        }
        if (r >= 0) {
            break;
        }
        *array = save;
    }
#if defined(DEBUG)
    printf("after phase1 r = %d\n", r);
    output_detail(array);
#endif
    if (r < 0) {
        printf("#after phase1A r = %d\n", r);
        return r;
    }
    fixed_only(array, FULL_SYMBOL);
    save = *array;
    numpl_array solved = *array;
    solve_info info;
    for (int i = 0; i < 100; i++) {
        r = random_solve(&solved);
        if (r < 0) {
            solved = save;
            continue;
        }
        if (phase == 0) {
            copy_corner(array, &solved);
        } else {
            copy_cross(array, &solved);
        }
        r = solve(array, &info);
        if (r > 0) {
            break;
        }
        *array = save;
        solved = save;
    }
#if defined(DEBUG)
    printf("after random_solve r = %d\n", r);
#endif
    if (r <= 0) {
        if (r < 0) {
            printf("#after random_solve r = %d\n", r);
        }
        return r;
    }
    fixed_only(array, FULL_SYMBOL);
    save = *array;
#if defined(DEBUG)
    r = solve(array, &info);
    printf("after solve r = %d\n", r);
    output_detail(array);
#endif

   /* ========
      PHASE II
      ======== */
    for (int i = 0; i < 100; i++) {
        if (phase == 0) {
            r = phase2A(array);
        } else {
            r = phase2B(array);
        }
        if (r > 0) {
#if defined(DEBUG)
            printf("after phase2 r = %d\n", r);
            output_detail(array);
#endif
            break;
        }
        *array = save;
    }
#if defined(DEBUG)
    //fixed_only(array, 0);
    printf("after phase2 r = %d\n", r);
    output_detail(array);
    fixed_only(array, FULL_SYMBOL);
#endif
    if (r < 0) {
        printf("#after phase2 r = %d\n", r);
        return r;
    }
#if defined(DEBUG) && 0
    debug_simple_kill(array);
#endif
   /* =========
      PHASE III
      ========= */
    r = phase3(array, type);
    //fixed_only(array, 0);
#if defined(DEBUG)
    printf("after phase3 r = %d\n", r);
    output_detail(array);
    //fixed_only(array, FULL_SYMBOL);
#endif
    if (r <= 0) {
        if (r < 0) {
            printf("#after phase3 r = %d\n", r);
        }
        //return r - 1;
        return r;
    }
   /* ========
      PHASE IV
      ======== */
    fixed_only(array, FULL_SYMBOL);
    save = *array;
#if defined(DEBUG)
    printf("before phase4 fixed = %d\n", count_fixed(array));
    solve(array, &info);
    printf("before phase4 value = %d\n", info_value(&info));
    print_solve_info(&info, 0);
    printf(":");
    fixed_only(array, 0);
    print_array(array);
    printf("\n");
    fixed_only(array, FULL_SYMBOL);
#endif
    r = phase4(array);
#if defined(DEBUG)
    printf("after phase4 fixed = %d\n", count_fixed(array));
    solve(array, &info);
    printf("after phase4 value = %d\n", info_value(&info));
    print_solve_info(&info, 0);
    printf(":");
    fixed_only(array, 0);
    print_array(array);
    printf("\n");
    fixed_only(array, FULL_SYMBOL);
#endif
#if defined(DEBUG)
    fixed_only(array, 0);
    printf("after phase4 r = %d\n", r);
    output_detail(array);
#endif
    fixed_only(array, FULL_SYMBOL);
    if (r < 0) {
        printf("#after phase4 r = %d\n", r);
        return r;
    }
    if (type->symmetric) {
        return r + 1;
    }
   /* ========
      PHASE V
      ======== */
    r = phase5(array);
#if defined(DEBUG)
    printf("after phase5 r = %d\n", r);
    fixed_only(array, FULL_SYMBOL);
    solve(array, &info);
    printf("after phase5 value = %d\n", info_value(&info));
    fixed_only(array, 0);
    output_detail(array);
#endif
    if (r < 0) {
        printf("#after phase5 r = %d\n", r);
        return r;
    }
    return r + 1;
}

//#if defined(MAIN)
#include <getopt.h>
#include <errno.h>
static int verbose = 0;
static int number = 1;
static uint32_t seed = 0;
static generate_type g_type;
static int parse_opt(int argc, char * argv[]);
static int parse_opt(int argc, char * argv[])
{
    static struct option longopts[] = {
        {"seed", required_argument, NULL, 's'},
        {"verbose", no_argument, NULL, 'v'},
        {"count", required_argument, NULL, 'c'},
        {"hidden", no_argument, NULL, 'h'},
        {"locked", no_argument, NULL, 'l'},
        {"tuple", no_argument, NULL, 't'},
        {"fish", no_argument, NULL, 'f'},
        {"xywing", no_argument, NULL, 'y'},
        {"xyzwing", no_argument, NULL, 'z'},
        {"symmetric", no_argument, NULL, 'S'},
        {NULL, 0, NULL, 0}};
    verbose = 0;
    seed = (uint32_t)clock();
    const char * pgm = argv[0];
    int c;
    int error = 0;
    g_type.symmetric = 0;
    errno = 0;
    for (;;) {
        if (error) {
            break;
        }
        c = getopt_long(argc, argv, "?vhltfyzSs:c:", longopts, NULL);
        if (c < 0) {
            break;
        }
        switch (c) {
        case 's':
            seed = (uint32_t)strtoull(optarg, NULL, 10);
            if (errno) {
                error = 1;
            }
            break;
        case 'S':
            g_type.symmetric = 1;
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h':
            g_type.hidden_single = 1;
            break;
        case 'l':
            g_type.locked_candidate = 1;
            break;
        case 't':
            g_type.tuple = 1;
            break;
        case 'f':
            g_type.fish = 1;
            break;
        case 'y':
            g_type.xy = 1;
            break;
        case 'z':
            g_type.xyz = 1;
            break;
        case 'c':
            number = (uint32_t)strtoull(optarg, NULL, 10);
            if (errno) {
                error = 1;
            }
            break;
        case '?':
        default:
            error = 1;
            break;
        }
    }
#if defined(DEBUG)
    printf("seed = %d\n", seed);
    printf("argc = %d\n", argc);
#endif
    if (error) {
        printf("%s [-v] [-S] [-c number] [-s seed] [-hltfy]\n", pgm);
        printf("\t--verbose, -v 冗長モード、このモードでは1問しか問題を作成しない。\n"
               "\t--symmetric, -S 点対称の問題を作成する。\n"
               "\t--count, -c 出力する問題の数\n"
               "\t--seed, -s 使用する疑似乱数の初期化数字\n"
               "\t--hidden, -h hidden_singe の解法を含む問題を出力する。\n"
               "\t--locked, -l locked_candidates の解法を含む問題を出力する。\n"
               "\t--tuple, -t naked pair, hidden pair"
               "などの解法を含む問題を出力する。\n"
               "\t--fish, -f x-wing, swordfishなどの解法を含む問題を出力する。\n"
               "\t--xywing, -y xy-wingの解法を含む問題を出力する。\n"
               "\t--xyzwing, -z xyz-wingの解法を含む問題を出力する。\n"
            );
        return -1;
    }
    if (g_type.hidden_single == 0 && g_type.locked_candidate == 0 &&
        g_type.tuple == 0 && g_type.fish == 0 && g_type.xy == 0 &&
        g_type.xyz == 0) {
        g_type.tuple = 1;
        g_type.fish = 1;
        g_type.xy = 1;
        g_type.xyz = 1;
    }
    return 0;
}
int main(int argc, char * argv[])
{
    int r;
    r = parse_opt(argc, argv);
    if (r < 0) {
        return r;
    }
    printf("generate start seed = %u number = %d\n", seed, number);
    xsadd_init(&xsadd, seed);
    if (verbose && number > 1) {
        printf("In verbose mode, generating number is set to 1.\n");
        number = 1;
    }
    numpl_array work;
    numpl_array save;
    fill_array(&work, FULL_SYMBOL);
    save = work;
#if defined(DEBUG)
    printf("after fill_array\n");
    output_detail(&work);
    printf("\n");
#endif
    while (number > 0) {
#if defined(DEBUG)
        printf("number = %d\n", number);
#endif
        int r = generate(&work, &g_type);
        if (r < 0) {
            printf("#r = %d\n", r);
            //break;
            work = save;
            continue;
        } else if (r == 0) {
            work = save;
            continue;
        }
        number--;
        fixed_only(&work, FULL_SYMBOL);
        solve_info info;
        //solve(&work, &info);
        r = lazy_normalize(&work, &info);
        if (verbose) {
            printf("number = %d\n", number);
            print_solve_info(&info, 1);
            fixed_only(&work, 0);
            output_detail(&work);
        } else {
            printf("\"");
            print_solve_info(&info, 0);
            printf("\":");
            fixed_only(&work, 0);
            print_array(&work);
            printf("\n");
        }
        work = save;
    }
    return 0;
}

//#endif // MAIN
