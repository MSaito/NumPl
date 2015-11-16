#include "numpl6x6.h"
#include "common.h"
#include "constants.h"
#include "killer.h"
//#include "embed.h"
//#include "multi-solve.h"
#include "normalize6x6.h"
#include "inline_functions.h"
#include <stdio.h>
#include <float.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

static int allow_hidden_single = 0;
int generate6x6(numpl_array * array);
static int tuple_vl(numpl_array * array,
                    const int vl[],
                    int vl_size,
                    int fixed,
                    uint16_t syms1,
                    uint16_t syms2);
static int copy_corner(numpl_array * array, const numpl_array * solved);
static int phase1(numpl_array * array);
static int phase2(numpl_array * array);
static int phase3(numpl_array * array);
//static int phase4(numpl_array * array);
static int phase5(numpl_array * array);


static int tuple_vl(numpl_array * array,
                    const int vl[],
                    int vl_size,
                    int fixed,
                    uint16_t syms1,
                    uint16_t syms2)
{
#if defined(DEBUG)
    printf("tuple_vl start\n");
#endif
    uint16_t order[vl_size];
    get_random_number_size(order, vl_size);
    int count = ones16(syms1);
    for (int i = 0; i < LINE_SIZE && count > 0; i++) {
        int idx1 = vl[order[i]];
        int idx2 = get_counter(idx1);
#if defined(DEBUG)
        printf("idex1 = %d, idx2 = %d\n", idx1, idx2);
#endif

        if (fixed && array->ar[idx1].fixed == fixed) {
            continue;
        }
        if (!(array->ar[idx1].symbol & syms1) ||
            !(array->ar[idx2].symbol & syms2)) {
            continue;
        }
        array->ar[idx1].fixed = fixed;
        reset_single_flag(&array->ar[idx1]);
        array->ar[idx1].symbol &= syms1;
        array->ar[idx2].fixed = fixed;
        reset_single_flag(&array->ar[idx2]);
        array->ar[idx2].symbol &= syms2;
        count--;
    }
#if defined(DEBUG)
    printf("tuple_vl end\n");
#endif
    return -count;
}

// array FULL fixed なし
static int phase1(numpl_array *array)
{
#if defined(DEBUG)
    printf("in phase1\n");
    output_detail(array);
    printf("\n");
#endif
    numpl_array save = *array;
    //uint16_t syms = 3;
    solve_info info;
    int r;
    static const int cross[2][4] = {
        {2, 3, 8, 9},
        {12, 13, 18, 19},
    };
    // step 1 十文字型に fixed を配置する
    for (;;) {
        tuple_vl(array, cross[0], 4, 1, 1, 2);
        tuple_vl(array, cross[1], 4, 1, 4, 8);
        r = solve(array, &info);
#if defined(DEBUG)
        printf("r = %d\n", r);
#endif
        if (r >= 0) {
            break;
        } else {
            *array = save;
            continue;
        }
    }
    save = *array;
    // step 2 対角にtupleを配置する
#if defined(DEBUG)
        printf("phase 1 step 2\n");
#endif
        static const int corner[4] = {0, 1, 6, 7};

    for (;;) {
        tuple_vl(array, corner, 4, 0, 8 | 16 | 32, 8 | 16 | 32);
        //tuple_vl(array, corner[1], 4, 0, 8 | 16 | 32, 1 | 2 | 4);
        r = solve(array, &info);
#if defined(DEBUG)
        printf("r = %d\n", r);
#endif
        if (r >= 0) {
            break;
        } else {
            *array = save;
            continue;
        }
    }
#if defined(DEBUG)
    printf("phase1 end\n");
    output_detail(array);
#endif
    return r;
}

/**
 * コーナーとセンター
 */
static int copy_corner(numpl_array * array, const numpl_array * solved)
{
#if defined(DEBUG) && 0
    printf("in copy_corner\n");
#endif
    static const int place[20] = {
        0, 1, 4, 5, 6, 7, 10, 11, 14, 15,
        20, 21, 24, 25, 28, 29, 30, 31, 34, 35
    };
    // コーナーとセンターをsolvedからコピー
    for (int i = 0; i < 20; i++) {
        int idx = place[i];
        array->ar[idx].symbol = solved->ar[idx].symbol;
        array->ar[idx].fixed = 1;
    }
    return 0;
}

static int phase2(numpl_array * array)
{
#if defined(DEBUG)
    printf("in phase2\n");
#endif
//    const int block_count = 2;
//    const int blono[2] = {0, 3};
    const int block_count = 1;
    const int blono[1] = {0};
    // 2個ずつ穴を開ける。ジグザグ2ブロック2個穴が開く
    for (int i = 0; i < block_count; i++) {
        int bno = blono[i];
        uint16_t syms[LINE_SIZE];
        get_random_symbol(syms);
        uint16_t sym = syms[0] | syms[1] | syms[2];
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
            array->ar[idx2].symbol = FULL_SYMBOL;
            reset_single_flag(&array->ar[idx2]);
            array->ar[idx2].fixed = 0;
        }
    }
    // solve してみる 解けなければあきらめる
    solve_info info;
    int r = solve(array, &info);
#if defined(DEBUG)
    printf("phase2 step 1 solve r = %d\n", r);
#endif
    if (r <= 0) {
        return r;
    }

    return r;
}

/* ===============================
   phase3 全件検索 (fix を 外しながら)
   今回は非対称
   =============================== */
static int p3(numpl_array * array, uint16_t index_array[], int start);
static int phase3(numpl_array * array)
{
    uint16_t index_array[ARRAY_SIZE];
    get_random_number_size(index_array, ARRAY_SIZE);
    return p3(array, index_array, 0);
}

static int p3(numpl_array * array, uint16_t index_array[], int start)
{
#if defined(DEBUG)
    printf("in phase3\n");
#endif
    solve_info info;
    numpl_array save;
    fixed_only(array, FULL_SYMBOL);
    int r = solve(array, &info);
    if (r <= 0) {
#if defined(DEBUG)
        printf("out phase3 r = %d\n", r);
#endif
        return r;
    }
    if (allow_hidden_single) {
        if (info.kh_count > 0) {
            r = info.kh_count
                + info.kl_count * 10
                + info.kt_count * 100
                + info.sf_count * 1000;
#if defined(DEBUG)
            printf("out phase3 r = %d\n", r);
#endif
            return r;
        }
    } else {
        if (info.kt_count > 0 || info.kl_count > 0) {
            r = info.kl_count + info.kt_count * 10 + info.sf_count * 100;
#if defined(DEBUG)
            printf("out phase3 r = %d\n", r);
#endif
            return r;
        }
    }
    save = *array;
    int max = 0;
    numpl_array best = *array;
    for (int i = start; i < ARRAY_SIZE; i++) {
        int idx1 = index_array[i];
        if (!array->ar[idx1].fixed) {
            continue;
        }
        array->ar[idx1].fixed = 0;
        reset_single_flag(&array->ar[idx1]);
        array->ar[idx1].symbol = FULL_SYMBOL;
        r = p3(array, index_array, i + 1);
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
#if defined(DEBUG)
    printf("out phase3 r = %d\n", 0);
#endif
    return 0;
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
        if (max < info.kt_count) {
            max = info.kt_count;
            best = *array;
        }
        save = *array;
    }
    *array = best;
    return max;
}


int generate6x6(numpl_array * array)
{
#if defined(DEBUG)
    printf("in generate\n");
    output_detail(array);
    printf("\n");
#endif
    numpl_array save = *array;
    int r;
    for (int i = 0; i < 100; i++) {
        r = phase1(array);
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
        //printf("#after phase1 r = %d\n", r);
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
        copy_corner(array, &solved);
        r = solve(array, &info);
        if (r > 0) {
            break;
        }
        *array = save;
        solved = save;
    }
#if defined(DEBUG)
    printf("after random_solve r = %d\n", r);
    output_detail(array);
#endif
    if (r <= 0) {
        //printf("#after random_solve r = %d\n", r);
        return r - 1;
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
        r = phase2(array);
        if (r > 0) {
            break;
        }
        *array = save;
    }
    fixed_only(array, 0);
#if defined(DEBUG)
    printf("after phase2 r = %d\n", r);
    output_detail(array);
#endif
    if (r < 0) {
        //printf("#after phase2 r = %d\n", r);
        return r;
    }
#if defined(DEBUG) && 0
    debug_simple_kill(array);
#endif
   /* =========
      PHASE III
      ========= */
    r = phase3(array);
    fixed_only(array, 0);
#if defined(DEBUG)
    printf("after phase3 r = %d\n", r);
    output_detail(array);
#endif
    if (r <= 0) {
        //printf("#after phase3 r = %d\n", r);
        return r - 1;
    }
#if 0
   /* ========
      PHASE IV
      ======== */
    save = *array;
    r = phase4(array);
    fixed_only(array, 0);
#if defined(DEBUG)
    printf("after phase4 r = %d\n", r);
    output_detail(array);
#endif
    fixed_only(array, FULL_SYMBOL);
    if (r < 0) {
        //printf("#after phase4 r = %d\n", r);
        return r;
    }
#endif
   /* ========
      PHASE V
      ======== */
    r = phase5(array);
#if defined(DEBUG)
    printf("after phase5 r = %d\n", r);
    fixed_only(array, 0);
    output_detail(array);
#endif
    fixed_only(array, FULL_SYMBOL);
    if (r < 0) {
        //printf("#after phase5 r = %d\n", r);
        return r;
    }
    return 0;
}


#include "analyze.h"
#include <getopt.h>
#include <errno.h>
static int verbose = 0;
static int number = 1;
static uint32_t seed = 0;
static int parse_opt(int argc, char * argv[]);
static int parse_opt(int argc, char * argv[])
{
    static struct option longopts[] = {
        {"seed", required_argument, NULL, 's'},
        {"verbose", no_argument, NULL, 'v'},
        {"count", required_argument, NULL, 'c'},
        {"hidden", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}};
    verbose = 0;
    seed = (uint32_t)clock();
    const char * pgm = argv[0];
    int c;
    int error = 0;
    errno = 0;
    for (;;) {
        if (error) {
            break;
        }
        c = getopt_long(argc, argv, "hvltfys:c:", longopts, NULL);
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
        case 'h':
            allow_hidden_single = 1;
            break;
        case 'c':
            number = strtoul(optarg, NULL, 10);
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
        printf("%s [-v] [-s seed] [mondai_string]\n", pgm);
        return -1;
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
    printf("#generate start seed = %u\n", seed);
#if defined(DEBUG)
    printf("allow_hidden_single = %d\n", allow_hidden_single);
#endif
    xsadd_init(&xsadd, seed);
    numpl_array work;
    numpl_array save;
    fill_array(&work, FULL_SYMBOL);
    save = work;
#if defined(DEBUG)
    printf("after fill_array\n");
    output_detail(&work);
    printf("\n");
#endif
    int count = 0;
    while (count < number) {
        int r = generate6x6(&work);
        if (r < 0) {
            work = save;
            continue;
        }
        count++;
        fixed_only(&work, FULL_SYMBOL);
        solve_info info;
        //solve(&work, &info);
        r = lazy_normalize6x6(&work, &info);
        if (verbose) {
            printf("number = %d\n", count);
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


