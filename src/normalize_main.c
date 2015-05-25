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
