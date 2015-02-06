#include "numpl.h"
#include "killer.h"
#include "solve.h"
#include "inline_functions.h"
#include "common.h"
#include "constants.h"
#include <stdio.h>
#include <string.h>

/** 候補を減らす関数タイプ */
typedef int (*solver_t)(numpl_array *);

/** 関数の数 */
static const int max_solvers = 5;
static const int max_simple_solvers = 2;

/** 関数 */
static solver_t solvers[] = {
    kill_single,
    kill_hidden_single,
    kill_locked_candidate,
    kill_tuple,
    kill_fish,
    NULL};

/** 機械的解法関数 */
static solver_t simple_solvers[] = {
    kill_single,
    kill_hidden_single,
    NULL};


static void to_string_detail(char str[3][4], cell_t s);
static void set_info(solve_info * info, int count, solver_t sv);
static int recursion_solve_sub(numpl_array * array, recursion_solve_t *data);

/**
 * マス１つ分の出力を作る
 * 単一数字か固定なら[]で囲って出力する。そうでなければ候補をすべて出力
 * @param str 出力文字列
 * @param s 出力するマス
 */
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

/**
 * マスにある候補をすべて出力する
 * @param array ナンプレ盤面配列
 */
void output_detail(numpl_array * array)
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

/**
 * ナンプレ盤面を横に一列出力する
 * 複数候補が入っているマスは？を出力する
 * @param array ナンプレ盤面配列
 */
void output(numpl_array * array)
{
    char c;
    for (int i = 0; i < LINE_SIZE; i++) {
	for (int j = 0; j < LINE_SIZE; j++) {
	    int ones = ones16(array->ar[rows[i][j]].symbol);
	    if (ones == 0) {
		c = ' ';
	    } else if (ones != 1) {
		c = '?';
	    } else {
		c = '0' + symbol2num(array->ar[rows[i][j]].symbol);
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
 * 盤面が解けた状態にあるか判定する
 * @param array ナンプレ盤面配列
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

/**
 * 人間がナンプレを解くときの手法を使って解く
 * どの手法を使ったかをinfo に記録する。
 * @param array ナンプレ盤面配列
 * @param info どの解法を使ったかを記録する
 * @return 1 解けた
 * @return 0 解けていない
 * @return -1 矛盾が発生していて解けない
 */
int solve(numpl_array * array, solve_info * info)
{
    memset(info, 0, sizeof(solve_info));
    int c = 0;
    do {
	for (int i = 0; i < max_solvers; i++) {
	    if (solvers[i] == NULL) {
		break;
	    }
	    c = solvers[i](array);
	    if (c > 0) {
		set_info(info, c, solvers[i]);
#if defined(DEBUG)
		printf("info:");
		print_solve_info(info, 0);
		printf("\n");
#endif
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

/**
 * 機械的解法（kill_single, kill_hidden_single)を繰り返し
 * 使用してナンプレを解こうとする。
 * @param array ナンプレ盤面配列
 * @return -1 矛盾があって解けない
 * @return 0 これ以上機械的解法では解けない
 * @return 1 解けた
 */
int simple_solve(numpl_array * array)
{
    int r = 0;
    do {
	for (int i = 0; i < max_simple_solvers && solvers[i] != NULL; i++) {
	    r = simple_solvers[i](array);
	    if (r < 0) {
		return -1;
	    }
	    if (r > 0) {
		break;
	    }
	}
    } while (r != 0);
    return r;
}

/**
 * 二つのナンプレ盤面配列が等しいか調べる
 * @param a ナンプレ盤面配列1
 * @param b ナンプレ盤面配列2
 * @return 1 等しい
 * @return 0 等しくない
 */
int equal(const numpl_array * a, const numpl_array * b)
{
    for (int i = 0; i < ARRAY_SIZE; i++) {
	if (a->ar[i].symbol != b->ar[i].symbol ||
	    a->ar[i].fixed != b->ar[i].fixed ||
	    is_single(a->ar[i]) != is_single(b->ar[i])) {
	    return 0;
	}
    }
    return 1;
}

/**
 * 再帰的解法（ラッパー）
 * 人間的な解法アルゴリズムは使わずに機械的解法で解く
 * 機械的解法で解けないときは、複数候補のあるマスの候補から一つ選んで解けるか試す
 * ということを再帰的に実行する。
 * また、解が複数あるかどうかもチェックする。
 * 解けたときは array に解をセットする。
 * @param array ナンプレ盤面配列
 * @return -1 矛盾があって解けない
 * @return 1 解けた
 * @return 2 解が複数存在する。
 */
int recursion_solve(numpl_array * array)
{
    recursion_solve_t data;
    data.saved = 0;
    return recursion_solve_sub(array, &data);
}

/**
 * 再帰的解法（実体）
 * 人間的な解法アルゴリズムは使わずに機械的解法で解く
 * 機械的解法で解けないときは、複数候補のあるマスの候補から一つ選んで解けるか試す
 * ということを再帰的に実行する。
 * また、解が複数あるかどうかもチェックする。
 * 解けたときは array に解をセットする。
 * @param array ナンプレ盤面配列
 * @param data 再帰用持ち回しデータ
 * @return -1 矛盾があって解けない
 * @return 1 解けた
 * @return 2 解が複数存在する。
 */
static int recursion_solve_sub(numpl_array * array, recursion_solve_t *data)
{
    numpl_array work;
    work = *array;
    int solved = simple_solve(&work);
    if (solved < 0) {
	return solved;
    }
    solved = is_solved(&work);
    if (solved == 1) {
	if (!data->saved) {
	    data->save = work;
	    data->saved = 1;
	} else if (!equal(&data->save, &work)) {
	    solved = 2;
	}
    }
    if (solved != 0) {
	*array = work;
	return solved;
    }
    int result = -1;
    for (int i = 0; i < ARRAY_SIZE; i++) {
	if (is_single(work.ar[i])) {
	    continue;
	}
	cell_t s = work.ar[i];
	for (int j = 0; j < LINE_SIZE; j++) {
	    work.ar[i] = s;
	    unsigned int mask = s.symbol & (1 << j);
	    if (!mask) {
		continue;
	    }
	    work.ar[i].symbol = mask;
	    solved = recursion_solve_sub(&work, data);
	    if (solved > 1) {
		*array = work;
		return solved;
	    } else if (solved == 1) {
		*array = work;
		result = 1;
	    }
	}
	break;
    }
    return result;
}

/**
 * ナンプレ問題の情報を出力する。
 * @param info ナンプレ問題の情報
 * @param verbose 詳細フラグ
 */
void print_solve_info(solve_info * info, int verbose)
{
    if (verbose) {
	printf("kill_single:%d\n", info->ks_count);
	printf("kill_hidden:%d\n", info->kh_count);
	printf("kill_locked:%d\n", info->kl_count);
	printf("kill_tuple :%d\n", info->kt_count);
	//printf("max_tuple  :%d\n", info->max_tuple);
	//printf("max_hidden :%d\n", info->max_hidden);
	printf("swordfish  :%d\n", info->sf_count);
	//printf("max_fish   :%d\n", info->max_fish);
	for (int i = 0; i < LINE_SIZE / 2 + 1; i++) {
	    printf("naked_tuple[%d] = %d\n", i + 2, info->naked_tuple[i]);
	}
	for (int i = 0; i < LINE_SIZE / 2 + 1; i++) {
	    printf("hidden_tuple[%d] = %d\n", i + 2, info->hidden_tuple[i]);
	}
	for (int i = 0; i < LINE_SIZE - 2; i++) {
	    printf("fish[%d] = %d\n", i + 2, info->fish[i]);
	}
	printf("xywing     :%d\n", info->xy_count);
	printf("fx_count   :%d\n", info->fx_count);
	printf("solved     :%d\n", info->solved);
    } else {
	printf("%d,%d,%d,%d,%d,",
	       info->ks_count,
	       info->kh_count,
	       info->kl_count,
	       info->kt_count,
	       //info->max_tuple,
	       //info->max_hidden,
	       info->sf_count
	       //info->max_fish
	    );
	for (int i = 0; i < LINE_SIZE / 2 + 1; i++) {
	    printf("%d,", info->naked_tuple[i]);
	}
	for (int i = 0; i < LINE_SIZE / 2 + 1; i++) {
	    printf("%d,", info->hidden_tuple[i]);
	}
	for (int i = 0; i < LINE_SIZE - 2; i++) {
	    printf("%d,", info->fish[i]);
	}
	printf("%d,%d,%d",
	       info->xy_count,
	       info->fx_count,
	       info->solved);
    }
}

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
    if (recursion_flag) {
	r = recursion_solve(&work);
	if (r <= 0) {
	    printf("can't solve\n");
	    return 1;
	} else if (r > 1) {
	    printf("has more than 2 solution\n");
	    //output(&work);
	    //output(&data.save);
	    return 2;
	} else {
	    printf("solved\n");
	    output(&work);
	}
	return 0;
    }
    solve_info info;
    solve(&work, &info);
    if (!quiet) {
	if (info.solved) {
	    printf("\nsolved\n");
	    output(&work);
	} else {
	    output_detail(&work);
	}
	print_solve_info(&info, 1);
    } else {
	printf("\"");
	print_solve_info(&info, 0);
	printf("\":");
	fixed_only(&work, 0);
	print_array(&work);
	printf("\n");
    }
    return 0;
}
#endif
