#include <stdio.h>
#include "inline_functions.h"
#include "numpl.h"
#include "constants.h"

static int get_counter_pos_even(int pos, int mode)
{
    int x = tocol(pos);
    int y = torow(pos);
    int half = LINE_SIZE / 2;
    if (x >= half) {
	x = x - half + 1;
    } else {
	x = x - half;
    }
    if (y >= half) {
	y = y - half + 1;
    } else {
	y = y - half;
    }
    switch (mode) {
    case 0:
	x = -x;
	break;
    case 1:
	y = -y;
	break;
    case 2:
	x = -x;
	y = -y;
    }
    if (x >= 0) {
	x = x + half - 1;
    } else {
	x = x + half;
    }
    if (y >= 0) {
	y = y + half - 1;
    } else {
	y = y + half;
    }
    return rows[y][x];
}

static int get_counter_pos_odd(int pos, int mode)
{
    int x = tocol(pos);
    int y = torow(pos);
    int half = LINE_SIZE / 2;
    x = x - half;
    y = y - half;
    switch (mode) {
    case 0:
	x = -x;
	break;
    case 1:
	y = -y;
	break;
    case 2:
	x = -x;
	y = -y;
    }
    x = x + half;
    y = y + half;
    return rows[y][x];
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
    if (LINE_SIZE % 2 == 0) {
	return get_counter_pos_even(pos, mode);
    } else {
	return get_counter_pos_odd(pos, mode);
    }
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
int get_counter_pos2(int pos, int mode)
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

int main(int argc, char * argv[])
{
    for (int mode = 0; mode <= 2; mode++) {
	for (int pos = 0; pos < ARRAY_SIZE; pos++) {
	    int p1 = get_counter_pos(pos, mode);
	    int p2 = get_counter_pos2(pos, mode);
	    if (p1 != p2) {
		printf("p1 = %d, P2 = %d, mode = %d, pos = %d, row = %d,"
		       " col = %d\n",
		       p1, p2, mode, pos, torow(pos), tocol(pos));
		return 1;
	    }
	}
    }
    printf("match\n");
    return 0;
}
