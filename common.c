/**
 * 共通関数
 * このファイルには inline_functions.h に定義されたもの以外の共通関数を入れる
 */
#include "numpl.h"
#include "common.h"
#include "constants.h"
#include "inline_functions.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/** 疑似乱数生成器 xsadd */
xsadd_t xsadd;

/**
 * 固定されたマスの数を数える。
 * 固定されたマスとは、出題時に数字が入っていて、利用者が変更することのできないマスである。
 * @param numpl_array ナンプレの盤面を表す配列
 * @return 固定されたマスの数
 */
int count_fixed(const numpl_array *array)
{
    int count = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
	if (array->ar[i].fixed) {
	    count++;
	}
    }
    return count;
}

/**
 * 固定されたマス以外にすべてfullで指定された数字群を入れる。
 * この関数には、プログラムで問題を解き始めるときに空白のマスに候補を全て入れるためと、
 * 問題を作成し終わったときに、固定されたマス以外を空白にするという二つの用途がある。
 * @param numpl_array ナンプレ盤面配列
 * @param full 固定されたマス以外にセットする数字候補
 */
void fixed_only(numpl_array * array, uint16_t full)
{
    int count = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
	if (!array->ar[i].fixed) {
	    reset_single_flag(array->ar[i]);
	    array->ar[i].symbol = full;
	    reset_single_flag(array->ar[i]);
	} else {
	    set_single_flag(array->ar[i]);
	    count++;
	}
    }
    if (count == 0) {
	printf("fixed_only no fixed point\n");
    }
}

/**
 * 入力文字列から盤面配列を作成する
 * 空白と数字以外は無視する。（改行は無視される）
 * @param numpl_array ナンプレ盤面配列
 * @param text 入力テキスト
 * @return -1 入力テキストに問題あり
 * @return 0 正常
 */
int input(numpl_array * array, char * text)
{
    int p = 0;
    for (int i = 0; text[i] != '\0' && p < ARRAY_SIZE; i++) {
	char c = text[i];
	if (c == ' ') {
	    array->ar[p].symbol = FULL_SYMBOL;
	    //array->ar[p].single = 0;
	    array->ar[p].fixed = 0;
	    p++;
	} else if (isdigit(c)) {
	    int sh = c - '1';
	    array->ar[p].symbol = 1 << sh;
	    //array->ar[p].single = 1;
	    array->ar[p].fixed = 1;
	    set_single_flag(array->ar[i]);
	    p++;
	} else {
	    continue;
	}
    }
    if (p != ARRAY_SIZE) {
	printf("counter = %d\n", p);
	return -1;
    } else {
	return 0;
    }
}

/**
 * ナンプレの盤面を１行で出力する。
 * これは大量に問題を生成したときに１問１行の形式で問題を出力するための関数である。
 * 出力の前後にはダブルクォートが付く
 * @param array ナンプレ盤面配列
 */
void print_array(numpl_array * array)
{
    printf("\"");
    for (int i = 0; i < ARRAY_SIZE; i++) {
	cell_t sym = array->ar[i];
	if (sym.fixed) {
	    uint16_t mask = 1;
	    for (int j = 1; j <= LINE_SIZE; j++) {
		if (mask & sym.symbol) {
		    printf("%d", j);
		    break;
		}
		mask = mask << 1;
	    }
	    if (mask > FULL_SYMBOL) {
		printf("?");
	    }
	} else {
	    printf(" ");
	}
    }
    printf("\"");
}

/**
 * これはファイルから問題を読み込むための関数である。
 * ここでは問題を文字列として読み込むだけなので、このあと input 関数によって
 * ナンプレの盤面に変換する必要がある。
 * @param mondai 出力文字列
 * @param fp 入力ファイルポインター
 */
void read_mondai(char mondai[], FILE * fp)
{
    char line[100];
    int p = 0;
    while (!feof(fp)) {
	memset(line, 0, 100);
	fgets(line, 100, fp);
	int len = strlen(line);
	if (line[len - 1] == '\n') {
	    line[len - 1] = '\0';
	    len--;
	}
	if (len < LINE_SIZE) {
	    strcat(line, "         ");
	}
	strncpy(&mondai[p], line, LINE_SIZE);
	p += LINE_SIZE;
	mondai[p] = '\0';
    }
}

/**
 * 0 から n (含まない) までの数をランダムに生成する
 * @param n 範囲
 * @return 0 から n(含まない)までの数を返す
 */
int get_random(unsigned int n)
{
    return n * xsadd_float(&xsadd);
}

/**
 * ナンプレで使う数字をランダムに生成する
 * @param symbols ナンプレで使う数字が重複なくランダムな順番でセットされる
 */
void get_random_symbol(uint16_t symbols[LINE_SIZE])
{
    uint16_t mask = 1;
    for (int i = 0; i < LINE_SIZE; i++) {
	symbols[i] = mask;
	mask = mask << 1;
    }
    for (int i = 0; i < LINE_SIZE; i++) {
	int r = get_random(LINE_SIZE);
	uint16_t tmp = symbols[i];
	symbols[i] = symbols[r];
	symbols[r] = tmp;
    }

}

/**
 * 0 から LINE_SIZE -1までの数をランダムに生成する。
 * @param numbers 0 からLINE_SIZE -1 までの数が重複なくセットされる。
 */
void get_random_number(uint16_t numbers[LINE_SIZE])
{
    get_random_number_size(numbers, LINE_SIZE);
}

/**
 * 0 から size -1までの数をランダムに生成する。
 * @param numbers 0 からsize -1 までの数が重複なくセットされる。
 * @param size numbers 配列の大きさ
 */
void get_random_number_size(uint16_t numbers[], int size)
{
    for (int i = 0; i < size; i++) {
	numbers[i] = i;
    }
    for (int i = 0; i < size; i++) {
	int r = get_random(size);
	uint16_t tmp = numbers[i];
	numbers[i] = numbers[r];
	numbers[r] = tmp;
    }

}

/**
 * 0 から size - 1までの数をランダムに生成する。
 * get_random_number_size と同じだが配列の型が違う
 * @param numbers 0 からsize -1 までの数が重複なくセットされる。
 * @param size numbers 配列の大きさ
 */
void shuffle_int(int array[], int size)
{
    for (int i = 0; i < size; i++) {
	int r = get_random(size);
	int tmp = array[i];
	array[i] = array[r];
	array[r] = tmp;
    }
}

/**
 * ビットパターンからランダムにひとつのビットを選ぶ
 * @param ビットパターン
 * @return 選ばれたビット
 */
uint16_t random_one_symbol(uint16_t symbols)
{
    if (symbols == 0) {
	return 0;
    }
    uint16_t size = ones16(FULL_SYMBOL);
    int pos = get_random(size);
    int count = 0;
    for (int i = 0; i < size; i++) {
	uint16_t mask = 1 << i;
	if (count == pos) {
	    return mask;
	}
	if (mask & symbols) {
	    count++;
	}
    }
    return 0;
}

/**
 * 盤面配列中で pos と対称な位置を求める
 * mode 0 左右対称
 * mode 1 上下対称
 * mode 2 180度点対称
 * @param pos 盤面配列内の位置
 * @param mode 対称モード
 * @return 盤面配列内の対称な位置
 */
int get_counter_pos(int pos, int mode)
{
    if (mode == 2) {
	return ARRAY_SIZE - 1 - pos;
    }
    int r = torow(pos);
    int c = tocol(pos);
    if (mode == 0) {
	return rows[r][LINE_SIZE - 1 - c];
    } else {
	return cols[c][LINE_SIZE - 1 - r];
    }
}

/**
 * ナンプレ盤面配列を特定の数（通常は全部の数）で満たす。
 * @param array ナンプレ盤面配列
 * @param full この数で満たす
 */
void fill_array(numpl_array * array, uint16_t full)
{
    for (int i = 0; i < ARRAY_SIZE; i++) {
	array->ar[i].symbol = full;
	array->ar[i].fixed = 0;
	reset_single_flag(array->ar[i]);
    }
}
