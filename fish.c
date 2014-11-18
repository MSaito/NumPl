#include "numpl.h"
#include "killer.h"
#include "inline_functions.h"
#include "constants.h"
#include <stdio.h>

static int fishsub(int fish_count, uint16_t mask,
		 const int rows[LINE_SIZE][LINE_SIZE],
		 numpl_array * array);
static void make_mat(uint16_t mask,
		     uint16_t mat[LINE_SIZE],
		     const int rows[LINE_SIZE][LINE_SIZE],
		     numpl_array * array);
static void adjust_patterns(int fish_count, uint16_t patterns[],
			    int point, int size);
static int kill_fish_cells(uint16_t mask,
		     uint16_t pat,
		     uint16_t mat[LINE_SIZE],
		     const int rows[LINE_SIZE][LINE_SIZE],
		     numpl_array * array);
static int contains(uint16_t array[], int pos, uint16_t value);
#if defined(DEBUG)
static void print_patterns(uint16_t pattern[], int size);
#endif

int kill_fish(numpl_array * array)
{
    for (int i = 2; i < 8; i++) {
	for (uint16_t mask = 1; mask <= 0x100; mask = mask << 1) {
	    int count = fishsub(i, mask, rows, array);
	    if (count > 0) {
		return count;
	    }
	    count = fishsub(i, mask, cols, array);
	    if (count > 0) {
		return count;
	    }
	}
    }
    return 0;
}

static int fishsub(int fish_count, uint16_t mask,
		 const int rows[LINE_SIZE][LINE_SIZE],
		 numpl_array * array)
{
    uint16_t mat[LINE_SIZE];
    make_mat(mask, mat, rows, array);
    int count1 = 0;
    int count2 = 0;
    uint16_t patterns[LINE_SIZE * LINE_SIZE];
    int p = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	int c1 = 0;
	int c2 = 0;
	for (int j = 0; j < LINE_SIZE; j++) {
	    c1 += (mat[i] >> j) & 1;
	    c2 += (mat[j] >> i) & 1;
	}
	if (c1 >= 2 && c1 <= fish_count) {
	    count1++;
	    patterns[p++] = mat[i];
	}
	if (c2 >= 2) {
	    count2++;
	}
    }
    if (count1 < fish_count) {
	return 0;
    }
    if (count2 < fish_count) {
	return 0;
    }
    adjust_patterns(fish_count, patterns, p, LINE_SIZE * LINE_SIZE);
    for (int i = 0; i < LINE_SIZE * LINE_SIZE; i++) {
	uint16_t pat = patterns[i];
	if (pat == UINT16_C(0xffff)) {
	    continue;
	}
	int count = 0;
	for (int j = 0; j < LINE_SIZE; j++) {
	    if ((mat[j] | pat) == pat) {
		count++;
	    }
	}
	if (count == fish_count) {
	    int result = kill_fish_cells(mask, pat, mat, rows, array);
	    if (result > 0) {
		return 1;
	    }
	}
    }
    return 0;
}

#if defined(DEBUG)
static void print_patterns(uint16_t pattern[], int size)
{
    for (int i = 0; i < size; i++) {
	uint32_t mask = 0x8000;
	for (int j = 0; j < 16; j++) {
	    if (pattern[i] & mask) {
		printf("1");
	    } else {
		printf("0");
	    }
	    mask = mask >> 1;
	}
	printf("\n");
    }
}
#endif

static void make_mat(uint16_t mask,
		     uint16_t mat[LINE_SIZE],
		     const int rows[LINE_SIZE][LINE_SIZE],
		     numpl_array * array)
{
    for (int i = 0; i < LINE_SIZE; i++) {
	mat[i] = 0;
	uint16_t m = 1;
	for (int j = 0; j < LINE_SIZE; j++) {
	    if ((array->ar[rows[i][j]].symbol & mask) != 0) {
		mat[i] |= m;
	    }
	    m = m << 1;
	}
    }
}

static void adjust_patterns(int fish_count, uint16_t patterns[],
			    int point, int size)
{
    uint16_t result[size];
    int result_pos = 0;
    for (int i = point; i < size; i++) {
	patterns[i] = UINT16_C(0xffff);
    }
    // 同じパターンを一つだけにする
    for (int i = 0; i < point; i++) {
	if (patterns[i] == UINT16_C(0xffff)) {
	    continue;
	}
	if (ones16(patterns[i]) == fish_count) {
	    if (!contains(result, result_pos, patterns[i])) {
		result[result_pos++] = patterns[i];
	    }
	    continue;
	}
	// 部分パターンは組み合わせる
	for (int j = i + 1; j < point; j++) {
	    uint16_t p = patterns[i] | patterns[j];
	    if (ones16(p) == fish_count) {
		if (!contains(result, result_pos, p)) {
		    result[result_pos++] = p;
		}
	    }
	}
    }
    for (int i = 0; i < result_pos; i++) {
	patterns[i] = result[i];
    }
    for (int i = result_pos; i < size; i++) {
	patterns[i] = UINT16_C(0xffff);
    }

}

static int kill_fish_cells(uint16_t mask,
			  uint16_t pat,
			  uint16_t mat[LINE_SIZE],
			  const int rows[LINE_SIZE][LINE_SIZE],
			  numpl_array * array)
{
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	if ((mat[i] | pat) == pat) {
	    continue;
	}
	uint16_t m = 1;
	for (int j = 0; j < LINE_SIZE; j++) {
	    if ((m | pat) == pat &&
		array->ar[rows[i][j]].symbol & mask) {
		count++;
		array->ar[rows[i][j]].symbol &= ~mask;
	    }
	    m = m << 1;
	}
    }
    return count;
}

static int contains(uint16_t array[], int pos, uint16_t value)
{
    for (int i = 0; i < pos; i++) {
	if (array[i] == value) {
	    return 1;
	}
    }
    return 0;
}
