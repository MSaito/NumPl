#include "numpl.h"
#include "convert.h"
#include "inline_functions.h"
#include "xsadd.h"
#include "constants.h"
#include "common.h"
#include <time.h>
#include <stdlib.h>

static void vl_change(numpl_array * array, const int vl1[], const int vl2[]);
static void block_reverse(numpl_array * array, int mode);
static void line_change(numpl_array * array, int mode);
static uint16_t symbol_change(uint16_t sym, uint16_t symtbl[LINE_SIZE]);
static void random_symbol(numpl_array * array);
static void reverse_change(numpl_array * array, int mode);

/**
 * Virtual Line 交換
 * 二つのvirtual line を交換する
 * @param array ナンプレ盤面配列
 * @param vl1 virtual line 1
 * @param vl2 virtual line 2
 */
static void vl_change(numpl_array * array, const int vl1[], const int vl2[])
{
    for (int i = 0; i < LINE_SIZE; i++) {
	int idx1 = vl1[i];
	int idx2 = vl2[i];
	cell_t tmp = array->ar[idx1];
	array->ar[idx1] = array->ar[idx2];
	array->ar[idx2] = tmp;
    }
}

/**
 * 数字を別の数字に変える
 * @param sym 元の数字
 * @param symtbl 変換テーブル
 * @return 変換後の数字
 */
static uint16_t symbol_change(uint16_t sym, uint16_t symtbl[LINE_SIZE])
{
    int cnt = 0;
    for (uint16_t mask = 1; mask < FULL_SYMBOL; mask = mask << 1) {
	if (mask == sym) {
	    return symtbl[cnt];
	}
	cnt++;
    }
    return 0;
}

/**
 * ランダムに変換テーブルを作り、盤面全体に渡って数字を別の数字に変換する
 * @param array ナンプレ盤面配列
 */
static void random_symbol(numpl_array * array)
{
    uint16_t rsyms[LINE_SIZE];
    get_random_symbol(rsyms);
    for (int i = 0; i < ARRAY_SIZE; i++) {
	if (array->ar[i].fixed) {
	    array->ar[i].symbol = symbol_change(array->ar[i].symbol, rsyms);
	}
    }
}

/**
 * 盤面を上下、左右に反転する。
 * mode = 0 左右反転
 * mode = 1 上下反転
 * mode = 2 180度回転　（非対称を含むので意味ある）
 * @param array ナンプレ盤面配列
 * @param mode 反転モード
 */
static void reverse_change(numpl_array * array, int mode)
{
    numpl_array work;
    for (int i = 0; i < ARRAY_SIZE; i++) {
	work.ar[i] = array->ar[get_counter_pos(i, mode)];
    }
    *array = work;
}

/**
 * ブロックの位置を反転する。ブロック内の移動はしない。
 * mode = 0 左右反転
 * mode = 1 上下反転
 * @param array ナンプレ盤面配列
 * @param mode 反転モード
 */
static void block_reverse(numpl_array * array, int mode)
{
    int RightLeft_TopButtom[2][3][2] = {
	{{0, 2}, {3, 5}, {6, 8}},
	{{0, 6}, {1, 7}, {2, 8}}};
    for (int i = 0; i < 3; i++) {
	int b1 = RightLeft_TopButtom[mode][i][0];
	int b2 = RightLeft_TopButtom[mode][i][1];
	vl_change(array, blocks[b1], blocks[b2]);
    }
}

/**
 * 行または列の交換
 * mode 0 列
 * mode 1 行
 * @param array ナンプレ盤面配列
 * @param mode 行、列の指定
 */
static void line_change(numpl_array * array, int mode)
{
    if (mode == 0) {
	vl_change(array, cols[0], cols[2]);
	vl_change(array, cols[6], cols[8]);
    } else {
	vl_change(array, rows[0], rows[2]);
	vl_change(array, rows[6], rows[8]);
    }
}

/**
 * いくつかの変換を組み合わせて盤面全体を変換する。
 * このとき、X-Wing や Sword Fish などの解法はそのまま適用できるようにする。
 * また、盤面の固定点の対称性は崩さないようにする。
 * @param array ナンプレ盤面配列
 * @return 常に 0
 */
int random_convert(numpl_array * array)
{
    random_symbol(array);
    int r = get_random(4);
    if (r != 3) {
	reverse_change(array, r);
    }
    r = get_random(3);
    if (r != 2) {
	block_reverse(array, r);
    }
    r = get_random(3);
    if (r != 2) {
	line_change(array, r);
    }
    return 0;
}

#if defined(MAIN)
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
    fixed_only(&work, 0);
    random_convert(&work);
    print_array(&work);
    printf("\n");
    return 0;
}
#endif
