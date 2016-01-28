#include "numpl6x6.h"
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

#include "analyze.h"
#include <getopt.h>
static int verbose = 0;
static uint32_t seed = 0;
static char * mondai_p = NULL;
static int random_convert6x6(numpl_array * array, int mode);
static int parse_opt(int argc, char * argv[]);

static int random_convert6x6(numpl_array * array, int mode)
{
    if (mode > 100) {
        return 0;
    }
    random_symbol(array);
    if (verbose) {
        printf("random_symbol\n");
    }
    if (mode > 10) {
        return 0;
    }
    int r = get_random(4);
    if (r != 3) {
        reverse_change(array, r);
        if (verbose) {
            printf("reverse_change r = %d\n", r);
        }
    }
    r = get_random(2);
    if (r != 0) {
        block_reverse6x6(array);
        if (verbose) {
            printf("block_reverse r = %d\n", r);
        }
    }
    if (mode > 0) {
        return 0;
    }
    for (;;) {
        r = get_random(128);
        if (ones16((uint16_t)r) >= 4) {
            break;
        }
    }
    line_change6x6(array, r);
    if (verbose) {
        printf("line_chagen r = %x\n", r);
    }
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
        c = getopt_long(argc, argv, "s:v", longopts, NULL);
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
        printf("%s [-s seed][-v][mondai_string]\n", pgm);
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
    int mode = 0;
    fixed_only(&work, FULL_SYMBOL);
    solve_info info;
    analyze(&work, &info);
    if (info.solved == 0) {
        printf("not solvable %d\n", info.solved);
        return 0;
    }
    numpl_array save = work;
    for (;;) {
        fixed_only(&work, FULL_SYMBOL);
        random_convert6x6(&work, mode);
        analyze(&work, &info);
        if (info.solved != 0) {
            break;
        }
        mode++;
        if (verbose || 1) {
            printf("mode = %d, solved = %d, R\n", mode, info.solved);
//            fflush(stdout);
        }
        work = save;
    }
    fixed_only(&work, FULL_SYMBOL);
    random_convert6x6(&work, mode);
    analyze(&work, &info);
    if (info.solved != 1) {
        printf("not solved\n");
    }
    if (verbose) {
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
}
