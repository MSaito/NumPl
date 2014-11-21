#include "numpl.h"
#include "killer.h"
#include "solve.h"
#include "inline_functions.h"
#include "common.h"
#include "constants.h"
#include <stdio.h>
#include <string.h>

typedef int (*solver_t)(numpl_array *);

static const int max_solvers = 5;
static solver_t solvers[] = {
    kill_single,
    kill_hidden_single,
    kill_locked_candidate,
    kill_tuple,
    kill_fish,
    NULL};

static void to_string_detail(char str[3][4], cell_t s);
static void output_detail(numpl_array * array);
static void output(numpl_array * array);
static void set_info(solve_info * info, int count, solver_t sv);

static void to_string_detail(char str[3][4], cell_t s)
{
    if (is_single(s) || s.fixed) {
	int i = 1;
	int count = 0;
	for (uint16_t mask = 1; mask < FULL_SYMBOL; mask = mask << 1) {
	    if (s.symbol & mask) {
		strcpy(str[0], "   ");
		strcpy(str[2], "   ");
		sprintf(str[1], "[%d]", i);
		count++;
		break;
	    }
	    i++;
	}
	if (count == 0) {
	    strcpy(str[0], "   ");
	    if (s.fixed) {
		strcpy(str[1], "$0$");
	    } else {
		strcpy(str[1], "!0!");
	    }
	    strcpy(str[2], "   ");
	}
    } else {
	uint16_t mask = 1;
	char st[LINE_SIZE + 1];
	for (int i = 0; i < LINE_SIZE; i++) {
	    if (s.symbol & mask) {
		st[i] = i + '1';
	    } else {
		st[i] = ' ';
	    }
	    mask = mask << 1;
	}
	strncpy(str[0], &st[0], 3);
	strncpy(str[1], &st[3], 3);
	strncpy(str[2], &st[6], 3);
	str[0][3] = '\0';
	str[1][3] = '\0';
	str[2][3] = '\0';
    }
}

static void output_detail(numpl_array * array)
{
    char str[LINE_SIZE][3][4];
    for (int i = 0; i < LINE_SIZE; i++) {
	for (int j = 0; j < LINE_SIZE; j++) {
	    to_string_detail(str[j], array->ar[rows[i][j]]);
	}
	for (int j = 0; j < LINE_SIZE; j++) {
	    printf("%s", str[j][0]);
	}
	printf("\n");
	for (int j = 0; j < LINE_SIZE; j++) {
	    printf("%s", str[j][1]);
	}
	printf("\n");
	for (int j = 0; j < LINE_SIZE; j++) {
	    printf("%s", str[j][2]);
	}
	printf("\n");
    }
}

static void output(numpl_array * array)
{
    char c;
    for (int i = 0; i < LINE_SIZE; i++) {
	for (int j = 0; j < LINE_SIZE; j++) {
	    switch (array->ar[rows[i][j]].symbol) {
	    case 0:
		c = ' ';
		break;
	    case 1:
		c = '1';
		break;
	    case 2:
		c = '2';
		break;
	    case 4:
		c = '3';
		break;
	    case 8:
		c = '4';
		break;
	    case 16:
		c = '5';
		break;
	    case 32:
		c = '6';
		break;
	    case 64:
		c = '7';
		break;
	    case 128:
		c = '8';
		break;
	    case 256:
		c = '9';
		break;
	    default:
		c = '?';
	    }
	    printf("%c", c);
	}
	printf("\n");
    }
}

static void set_info(solve_info * info, int count, solver_t sv)
{
    if (sv == kill_single) {
	info->ks_count += count;
    } else if (sv == kill_hidden_single) {
	info->kh_count += count;
    } else if (sv == kill_tuple) {
	info->kt_count += count;
    } else if (sv == kill_locked_candidate) {
	info->kl_count += count;
    } else if (sv == kill_fish) {
	info->sf_count += count;
    }
}

/**
 * @return 1 solved
 * @return 0 not solved
 * @return -1 unsolvable
 */
int is_solved(const numpl_array * array)
{
    int count = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
	if (ones16(array->ar[i].symbol) == 1) {
	    count++;
	} else if (array->ar[i].symbol == 0) {
	    //printf("%d is zero\n", i);
	    return -1;
	}
    }
    if (count == ARRAY_SIZE) {
	return 1;
    } else {
	return 0;
    }
}

int solve(numpl_array * array, solve_info * info)
{
    info->ks_count = 0;
    info->kh_count = 0;
    info->kt_count = 0;
    info->kl_count = 0;
    info->sf_count = 0;
    info->fx_count = 0;
    info->solved = 0;
    int c = 0;
    do {
	for (int i = 0; i < max_solvers; i++) {
	    if (solvers[i] == NULL) {
		break;
	    }
	    c = solvers[i](array);
	    if (c > 0) {
		set_info(info, c, solvers[i]);
		break;
	    }
	    if (c < 0) {
		break;
	    }
	}
    } while (c > 0);
    if (c < 0) {
	info->solved = 0;
    } else {
	if (is_solved(array)) {
	    info->solved = 1;
	} else {
	    info->solved = 0;
	}
    }
    info->fx_count = count_fixed(array);
    if (c < 0) {
	return -1;
    } else {
	return info->solved;
    }
}

#if defined(MAIN)
#include <getopt.h>
static int quiet = 0;
static int reduce_flag = 0;
static char * mondai_p = NULL;
static int parse_opt(int argc, char * argv[]);
static int parse_opt(int argc, char * argv[])
{
    static struct option longopts[] = {
        {"short", no_argument, NULL, 's'},
        {"reduce", no_argument, NULL, 'r'},
        {NULL, 0, NULL, 0}};
    quiet = 0;
    reduce_flag = 0;
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
	    reduce_flag = 1;
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
    printf("reduce = %d\n", reduce_flag);
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
#if 0
    if (reduce_flag) {
	r = reduce(&work, 2);
	if (!quiet) {
	    printf("reduced = %d\n", r);
	}
	fixed_only(&work, FULL_SYMBOL);
    }
#endif
    solve_info info;
    solve(&work, &info);
    if (!quiet) {
	if (info.solved) {
	    printf("\nsolved\n");
	    output(&work);
	} else {
	    output_detail(&work);
	}
	printf("kill_single:%d\n", info.ks_count);
	printf("kill_hidden:%d\n", info.kh_count);
	printf("kill_locked:%d\n", info.kl_count);
	printf("kill_tuple :%d\n", info.kt_count);
	printf("max_tuple  :%d\n", info.max_tuple);
	printf("max_hidden :%d\n", info.max_hidden);
	printf("swordfish  :%d\n", info.sf_count);
	printf("max_fish   :%d\n", info.max_fish);
	printf("fx_count   :%d\n", info.fx_count);
	printf("solved     :%d\n", info.solved);
    } else {
	printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,",
	       info.ks_count,
	       info.kh_count,
	       info.kl_count,
	       info.kt_count,
	       info.max_tuple,
	       info.max_hidden,
	       info.sf_count,
	       info.max_fish,
	       info.fx_count,
	       info.solved);
	fixed_only(&work, 0);
	print_array(&work);
	printf("\n");
    }
    return 0;
}
#endif