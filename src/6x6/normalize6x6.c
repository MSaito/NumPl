#include "numpl.h"
#include "convert.h"
#include "common.h"
#include "convert6x6.h"
#include "inline_functions.h"
#include "normalize6x6.h"
#include "analyze.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

static int get_normalize_value(numpl_array * array);
static void normalize_symbol(numpl_array * array);

static int get_normalize_value(numpl_array * array)
{
    static const int normalize_values[] = {
        40000, 50000, 60000, 400000, 500000, 600000,
        10000, 20000, 30000, 100000, 200000, 300000,
        400, 500, 600, 4000, 5000, 6000,
        100, 200, 300, 1000, 2000, 3000,
        4, 5, 6, 40, 50, 60,
        1, 2, 3, 10, 20, 30
    };

    int result = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (array->ar[i].fixed) {
            result += normalize_values[i];
        }
    }
    return result;
}

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

int lazy_normalize6x6(numpl_array * array, solve_info * info)
{
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
    print_info_info(info);
#endif
    for (int i = 1; i < 2047; i++) {
        for (int j = 0; j < 3; j++) {
            if (((1 << j) & i) != 0) {
                reverse_change(array, j);
            }
        }
        if ((i & 0x08) != 0) {
            block_reverse6x6(array);
        }
        int mode = (i >> 4) & 0x7ff;
        line_change6x6(array, mode);
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
#include "analyze.h"
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
    fixed_only(&work, 0);
    solve_info info;
    memset(&info, 0, sizeof(info));
    r = lazy_normalize6x6(&work, &info);
    printf("\"");
    print_solve_info(&info, 0);
    printf("\":");
    fixed_only(&work, 0);
    print_array(&work);
    printf("\n");
    return 0;
}
#endif
