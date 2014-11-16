#include "numpl.h"
#include "constants.h"
#include "killer.h"

static int kill_locked_lines(int common_size, const int index1[],
			     const int index2[], cell_t array[]);
static void search_common(int idx[], const int max, const int index1[], const int index2[]);
static int count_diff(uint16_t mask, const int idx[], int common_size,
		      const int index[], cell_t array[]);
static int is_in(const int idx[], int common_size, int num);
static int kill_diff(uint16_t mask, const int common[], int common_size,
		     const int index[], cell_t array[]);

int kill_locked_candidate(numpl_array * array)
{
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	for (int j = 0; j < BLOCK_COLS; j++) {
	    count = kill_locked_lines(BLOCK_COLS, blocks[i],
				      rows[locked_rows[i][j]], array->ar);
	    if (count > 0) {
		return 1;
	    }
	}
	for (int j = 0; j < BLOCK_ROWS; j++) {
	    count = kill_locked_lines(BLOCK_ROWS, blocks[i],
				       cols[locked_cols[i][j]], array->ar);
	    if (count > 0) {
		return 1;
	    }
	}
    }
    return count;
}


static int kill_locked_lines(int common_size, const int index1[],
			     const int index2[], cell_t array[])
{
    int result = 0;
    int common[common_size];
    search_common(common, common_size, index1, index2);
    if (common[0] == -1) {
	return 0;
    }
    for (uint16_t mask = 1; mask <= 0x100; mask = mask << 1) {
	int count = 0;
	// 共通部分に数字があるか
	for (int i = 0; i < common_size; i++) {
	    if (array[common[i]].symbol & mask) {
		count++;
	    }
	}
	if (count) {
	    // 他の部分に数字があるか
	    int diff = count_diff(mask, common, common_size, index1, array);
	    if (diff == 0) {
		result = kill_diff(mask, common, common_size, index2, array);
		if (result > 0) {
		    return 1;
		}
	    }
	    diff = count_diff(mask, common, common_size, index2, array);
	    if (diff == 0) {
		result = kill_diff(mask, common, common_size, index1, array);
		if (result > 0) {
		    return 1;
		}
	    }
	}
    }
    return 0;
}

static void search_common(int idx[], const int common_size, const int index1[], const int index2[])
{
    int p = 0;
    for (int i = 0; i < common_size; i++) {
	idx[i] = -1;
    }
    for (int i = 0; i < LINE_SIZE; i++) {
	for (int j = 0; j < LINE_SIZE; j++) {
	    if (index1[i] == index2[j]) {
		idx[p++] = index1[i];
	    }
	}
    }
}

static int is_in(const int idx[], int common_size, int num)
{
    for (int i = 0; i < common_size; i++) {
	if (idx[i] == num) {
	    return 1;
	}
    }
    return 0;
}

// idx になくてindex にあるコマについてmask の数を数える
static int count_diff(uint16_t mask, const int idx[], int common_size,
		      const int index[], cell_t array[])
{
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	if (is_in(idx, common_size, index[i])) {
	    continue;
	}
	if (array[index[i]].symbol & mask) {
	    count++;
	}
    }
    return count;
}

static int kill_diff(uint16_t mask, const int common[],
		     int common_size, const int index[], cell_t array[]) {
    uint16_t kill_mask = (~mask) & FULL_SYMBOL;
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	if (is_in(common, common_size, index[i])) {
	    continue;
	}
	if (array[index[i]].symbol & mask) {
	    array[index[i]].symbol &= kill_mask;
	    count++;
	}
    }
    return count;
}
