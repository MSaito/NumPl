#include "numpl.h"
#include "convert.h"
#include "convert6x6.h"
#include "normalize6x6.h"
#include "constants.h"
#include "inline_functions.h"
#include "common.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

static int vl_change(numpl_array * array, const int vl1[], const int vl2[]);

// 上下反転
void block_reverse6x6(numpl_array * array)
{
    int blo[2][2] = {{0, 4}, {1, 5}};
    for (int i = 0; i < 2; i++) {
        vl_change(array, blocks[blo[i][0]], blocks[blo[i][1]]);
    }
}

// 対称性を保たない
// mode = 0 column 0, 1
// mode = 1 column 1, 2
// mode = 2 column 3, 4
// mode = 3 column 4, 5
// mode = 4 row 1
// mode = 5 row 2
// mode = 6 row 3
// 2^7 = 128
void line_change6x6(numpl_array * array, int mode)
{
    static const int * change_line[7][2] = {
        {cols[0], cols[1]},
        {cols[1], cols[2]},
        {cols[3], cols[4]},
        {cols[4], cols[5]},
        {rows[0], rows[1]},
        {rows[2], rows[3]},
        {rows[4], rows[5]}
    };
    for (int i = 0; i < 7; i++) {
        int mask = 1 << i;
        if ((mode & mask) != 0) {
#if defined(DEBUG)
            printf("%x ", mask);
#endif
            vl_change(array, change_line[i][0], change_line[i][1]);
        }
    }
#if defined(DEBUG)
    printf("\n");
#endif
}

static int vl_change(numpl_array * array, const int vl1[], const int vl2[])
{
#if defined(DEBUG)
    printf("vl_change\n");
    for (int i = 0; i < LINE_SIZE; i++) {
        printf("%d,", vl1[i]);
    }
    printf("\n");
    fflush(stdout);
    for (int i = 0; i < LINE_SIZE; i++) {
        printf("%d,", vl2[i]);
    }
    printf("\n");
    fflush(stdout);
#endif
    for (int i = 0; i < LINE_SIZE; i++) {
        int idx1 = vl1[i];
        int idx2 = vl2[i];
        cell_t tmp = array->ar[idx1];
        array->ar[idx1] = array->ar[idx2];
        array->ar[idx2] = tmp;
    }
    return 0;
}


#if defined(MAIN)
#include "analyze.h"
#include <getopt.h>
static int verbose = 0;
static uint32_t seed = 0;
static char * mondai_p = NULL;
static int random_convert6x6(numpl_array * array);
static int parse_opt(int argc, char * argv[]);
static int random_convert6x6(numpl_array * array)
{
    random_symbol(array);
    int r = get_random(4);
    if (r != 3) {
        reverse_change(array, r);
#if defined(DEBUG)
        printf("reverse_change r = %d\n", r);
#endif
    }
    r = get_random(2);
    if (r != 0) {
        block_reverse6x6(array);
#if defined(DEBUG)
        printf("block_reverse r = %d\n", r);
#endif
    }
    for (;;) {
        r = get_random(128);
        if (ones16((uint16_t)r) >= 4) {
            break;
        }
    }
    line_change6x6(array, r);
#if defined(DEBUG)
        printf("line_chagen r = %x\n", r);
#endif
    return 0;
}

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
        c = getopt_long(argc, argv, "s:", longopts, NULL);
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
        printf("%s [-s seed] [mondai_string]\n", pgm);
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
    xsadd_init(&xsadd, seed);
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
#if !defined(DEBUG) || 1
    fixed_only(&work, 0);
    random_convert6x6(&work);
    if (verbose) {
        solve_info info;
        r = lazy_normalize6x6(&work, &info);
        printf("\"");
        print_solve_info(&info, 0);
        printf("\":");
        fixed_only(&work, 0);
        print_array(&work);
        printf("\n");
    } else {
        print_array(&work);
        printf("\n");
    }
    return 0;
#else
    printf("input\n");
    output(&work);
    printf("\n");
    fixed_only(&work, FULL);
    if (sol(&work) <= 0) {
        printf("not solved\n");
        return -1;
    }
    // random_symbol
    fixed_only(&work, 0);
    random_symbol(&work);
    printf("random_symbol\n");
    output(&work);
    printf("\n");
    if (sol(&work) <= 0) {
        printf("not solved\n");
        return -1;
    }
    // reverse_change
    printf("reverse_change 0\n");
    reverse_change(&work, 0);
    output(&work);
    printf("\n");
    if (sol(&work) <= 0) {
        printf("not solved\n");
        return -1;
    }
    printf("reverse_change 1\n");
    reverse_change(&work, 1);
    output(&work);
    printf("\n");
    if (sol(&work) <= 0) {
        printf("not solved\n");
        return -1;
    }
    printf("reverse_change 2\n");
    reverse_change(&work, 2);
    output(&work);
    if (sol(&work) <= 0) {
        printf("not solved\n");
        return -1;
    }
    printf("\n");
    // block_reverse
    printf("block_reverse\n");
    block_reverse6x6(&work);
    output(&work);
    printf("\n");
    if (sol(&work) <= 0) {
        printf("not solved\n");
        return -1;
    }
    // line_change
    printf("line_change 0\n");
    line_change6x6(&work, 0);
    output(&work);
    printf("\n");
    if (sol(&work) <= 0) {
        printf("not solved\n");
        return -1;
    }
    printf("line_change 1\n");
    line_change6x6(&work, 1);
    output(&work);
    printf("\n");
    if (sol(&work) <= 0) {
        printf("not solved\n");
        return -1;
    }
    return 0;
#endif
}
#endif
