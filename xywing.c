/**
 * XY-Wing
 *
 */
#include "numpl.h"
#include "constants.h"
#include "killer.h"
#include "solve.h"
#include "inline_functions.h"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

enum LINE_KIND {BLOCK, COL, ROW};
static int xywing_sub(numpl_array * array, int pos, uint16_t sym);
static void search_yz(numpl_array * array, const int line[],
		      int result[], int pos, uint16_t sym);
static int kill_zblock(numpl_array * array, int pos, const int line1[],
		       const int line2[], uint16_t sym);
static int kill_common(numpl_array * array, int pos, int p1, int p2,
		       const int vl1[], const int vl2[], uint16_t sym);
static const int * getVL(enum LINE_KIND kind, int pos);
/**
 * XY Wing メイン関数
 * @param array ナンプレ盤面配列
 * @return 1:この解法により数字を消した。0:消せなかった
 */
int kill_xywing(numpl_array * array)
{
    int count = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
	uint16_t sym = array->ar[i].symbol;
	if (ones16(sym) == 2) {
	    count = xywing_sub(array, i, sym);
	}
    }
    return 0;
}

/**
 * XY Wing 解析メイン関数
 * @param array ナンプレ盤面配列
 * @param info 解情報
 * @return 1:この解法により数字を消した。0:消せなかった
 */
int analyze_xywing(numpl_array * array, solve_info *info)
{
#if defined(DEBUG)
    printf("in xywing_info\n");
#endif
    int count = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        uint16_t sym = array->ar[i].symbol;
        if (ones16(sym) == 2) {
            count = xywing_sub(array, i, sym);
        }
        if (count > 0) {
            info->xy_count++;
            return 1;
        }
    }
    return 0;
}

/**
 * XY Wing 下請け関数
 * @param array ナンプレ盤面配列
 * @param pos 注目位置
 * @param sym 注目位置の候補
 * @return 1 この解法により数字を消した
 * @return 0 この解法に該当しなかった
 */
static int xywing_sub(numpl_array * array, int pos, uint16_t sym)
{
    int rowpos = torow(pos);
    int colpos = tocol(pos);
    int blkpos = toblk(pos);
    int r[LINE_SIZE];
    int c[LINE_SIZE];
    int b[LINE_SIZE];
    search_yz(array, rows[rowpos], r, pos, sym);
    search_yz(array, cols[colpos], c, pos, sym);
    search_yz(array, blocks[blkpos], b, pos, sym);
    int count;
    count = kill_zblock(array, pos, r, b, sym);
    if (count > 0) {
	return 1;
    }
    count = kill_zblock(array, pos, c, b, sym);
    if (count > 0) {
	return 1;
    }
    count = kill_zblock(array, pos, r, c, sym);
    if (count > 0) {
	return 1;
    }
    return 0;
}

/**
 * yz というパターンのセルを探す
 * @param array ナンプレ盤面配列
 * @param line 仮想行
 * @param result 出力：該当セルのリスト
 * @param pos 起点の位置
 * @param sym 起点の候補 xy
 */
static void search_yz(numpl_array * array, const int line[],
		      int result[], int pos, uint16_t sym)
{
    memset(result, 0, sizeof(int) * LINE_SIZE);
    int idx = 0;
    result[idx] = -1;
    for (int i = 0; i < LINE_SIZE; i++) {
	int p = line[i];
	if (p == pos) {
	    continue;
	}
	uint16_t s = array->ar[p].symbol;
	if (ones16(s) == 2 && ones16(s & sym) == 1) {
	    result[idx++] = p;
	    result[idx] = -1;
	}
    }
}

/**
 * ブロックと行または列の共通部分に対象があれば消す
 * @param array ナンプレ盤面配列
 * @param pos 起点の位置
 * @param line1 行または列
 * @param line2 ブロック
 * @param sym 起点の候補
 * @return 候補を消したセルの数
 */
static int kill_zblock(numpl_array * array, int pos, const int line1[],
		       const int line2[], uint16_t sym)
{
    enum LINE_KIND allKind[] = {BLOCK, COL, ROW};
    for (int i = 0; line1[i] >= 0; i++) {
	int p1 = line1[i];
	for (int j = 0; line2[j] >= 0; j++) {
	    int p2 = line2[j];
	    uint16_t s1 = array->ar[p1].symbol;
	    uint16_t s2 = array->ar[p2].symbol;
	    if (ones16(s1 & s2) != 1) {
		continue;
	    }
	    if (ones16(sym | s1 | s2) != 3) {
		continue;
	    }
	    int count = 0;
	    for (int k = 0; k < 3; k++) {
		enum LINE_KIND k1 = allKind[k];
		const int * vl1 = getVL(k1, p1);
		for (int m = 0; m < 3; m++) {
		    enum LINE_KIND k2 = allKind[m];
		    if (k1 == k2) {
			continue;
		    }
		    const int * vl2 = getVL(k2, p2);
		    if (kill_common(array, pos, p1, p2, vl1, vl2, s1 & s2)
			> 0) {
			count++;
		    }
		}
	    }
	    if (count > 0) {
		return 1;
	    }
	}
    }
    return 0;
}

/**
 * ナンプレ盤面配列内の位置に対して、仮想行種別に応じた仮想行を返す。
 * @param kind 仮想行種別
 * @param pos ナンプレ盤面配列内の位置
 * @return 仮想行
 */
static const int * getVL(enum LINE_KIND kind, int pos)
{
    switch (kind) {
    case ROW:
        return rows[torow(pos)];
    case COL:
        return cols[tocol(pos)];
    case BLOCK:
    default:
        return blocks[toblk(pos)];
    }
}

/**
 * 二つの仮想行の共通部分から数字を消す
 * @param array ナンプレ盤面配列
 * @param pos 起点の位置(xyのある位置)
 * @param p1 xz のある位置
 * @param p2 yz のある位置
 * @param vl1 仮想行1
 * @param vl2 仮想行2
 * @param sym 消す数字
 * @return 数字を消したセルの数
 */
static int kill_common(numpl_array * array, int pos, int p1, int p2,
		       const int vl1[], const int vl2[], uint16_t sym)
{
    int common[LINE_SIZE];
    int p = 0;
    common[p] = -1;
    for (int i = 0; i < LINE_SIZE; i++) {
	if (vl1[i] == pos || vl1[i] == p1 || vl1[i] == p2) {
	    continue;
	}
	for (int j = 0; j < LINE_SIZE; j++) {
            if (vl2[j] == pos || vl2[j] == p1 || vl2[j] == p2) {
                continue;
            }
	    if (vl1[i] == vl2[j]) {
		common[p++] = vl1[i];
		common[p] = -1;
		break;
	    }
	}
    }
    int count = 0;
    for (int i = 0; i < LINE_SIZE && common[i] >= 0; i++) {
	int pos = common[i];
	if ((array->ar[pos].symbol & sym) != 0) {
	    array->ar[pos].symbol &= ~sym;
	    count++;
	}
    }
    return count;
}
