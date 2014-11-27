/**
 * 共通関数
 * このファイルには inline_functions.h に定義されたもの以外の共通関数を入れる
 */
#include "numpl.h"
#include "common.h"
#include "inline_functions.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

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
	    //array->ar[i].single = 0;
	    array->ar[i].symbol = full;
	} else {
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

