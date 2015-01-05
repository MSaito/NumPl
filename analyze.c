/**
 * convert によっても変わらないような解法の種類を求める。
 *
 */
#include "numpl.h"
#include "solve.h"
#include "killer.h"
#include "common.h"
#include <stdio.h>
#include <string.h>

#if 0
/** 候補を減らす関数タイプ */
typedef int (*analyzer_t)(numpl_array *, solve_info *);

/** 関数 */
static analyzer_t analyzers[] = {
    analyze_single,
    analyze_hidden_single,
    analyze_locked_candidate,
    analyze_tuple,
    analyze_fish,
    NULL};

static double get_analyze_value(solve_info * info);
#endif

/**
 * 与えられた問題を解く上で必要な最小の解法を求める
 *
 */
int analyze(numpl_array * array, solve_info * info)
{
#if defined(DEBUG)
    printf("IN ANALYZE\n");
#endif
    if (info->solved) {
#if defined(DEBUG)
	printf("OUT solved\n");
#endif
	return 1;
    } else if (is_solved(array)) {
	info->solved = 1;
#if defined(DEBUG)
	printf("OUT solved");
#endif
	return 1;
    }
    int c = 0;
    do {
#if defined(DEBUG)
	printf("ANALYZE DO c = %d\n", c);
#endif
	c = analyze_fish(array, info);
	if (c < 0) {
	    info->solved = 0;
	    return -1;
	}
	if (c > 0) {
	    break;
	}
    } while (c > 0 || !info->solved);
    if (c < 0) {
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
    return info->solved;
}

#if 0
/**
 * 難易度を仮に評価する。
 *
 */
static double get_analyze_value(solve_info * info)
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
    return v;
}
#endif

#if defined(MAIN)
#include <getopt.h>
static int quiet = 0;
static int recursion_flag = 0;
static char * mondai_p = NULL;
static int parse_opt(int argc, char * argv[]);
static int parse_opt(int argc, char * argv[])
{
    static struct option longopts[] = {
        {"short", no_argument, NULL, 's'},
        {"reduce", no_argument, NULL, 'r'},
        {NULL, 0, NULL, 0}};
    quiet = 0;
    recursion_flag = 0;
    const char * pgm = argv[0];
    int c;
    int error = 0;
    for (;;) {
        if (error) {
            break;
        }
        c = getopt_long(argc, argv, "sr", longopts, NULL);
	if (c < 0) {
	    break;
	}
	switch (c) {
	case 's':
	    quiet = 1;
	    break;
	case 'r':
	    recursion_flag = 1;
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
    printf("quiet = %d\n", quiet);
    printf("recursion = %d\n", recursion_flag);
    printf("argc = %d\n", argc);
#endif
    if (error) {
	printf("%s [-sr] [mondai_string]\n", pgm);
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
	//printf("%s\n", mondai_p);
	r = input(&work, mondai_p);
    }
    if (r != 0) {
	printf("input string is wrong\n");
	return -1;
    }
    if (!quiet) {
	output(&work);
    }
    solve_info info;
    analyze(&work, &info);
    if (!quiet) {
	if (info.solved) {
	    printf("\nsolved\n");
	    output(&work);
	} else {
	    output_detail(&work);
	}
	print_solve_info(&info, 1);
    } else {
	print_solve_info(&info, 0);
	fixed_only(&work, 0);
	print_array(&work);
	printf("\n");
    }
    return 0;
}
#endif
