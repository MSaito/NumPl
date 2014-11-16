#include "killer.h"
#include "numpl.h"
#include "constants.h"
#include "inline_functions.h"
#include <stdio.h>

static uint16_t count_multiple(const int index[], cell_t array[]);
static int kill_tuple_lines(const int line[], cell_t array[]);
static int count_naked_cells(uint16_t mask, const int index[],
			     cell_t array[]);
static int kill_naked_cells(uint16_t mask, const int index[], cell_t array[]);

int kill_tuple(numpl_array * array)
{
    int c = 0;
    for (int i = 0; i < LINE_KINDS; i++) {
	for (int j = 0; j < LINE_SIZE; j++) {
	    c = kill_tuple_lines(all_lines[i][j], array->ar);
	    if (c > 0) {
		return 1;
	    }
	}
    }
    return 0;
}

static int kill_tuple_lines(const int line[], cell_t array[])
{
    uint16_t multiple = count_multiple(line, array);
    if (ones16(multiple) <= 2) {
	return 0;
    }
    for (uint16_t mask = 3; mask < FULL_SYMBOL; mask++) {
	if ((mask & multiple) != multiple) {
	    continue;
	}
	int count = ones16(mask);
	if (count <= 1 || count >= 8) {
	    continue;
	}
	int cells = count_naked_cells(mask, line, array);
	if (cells == count) {
	    int result = kill_naked_cells(mask, line, array);
	    if (result > 0) {
		return 1;
	    }
	}
    }
    return 0;
}

static uint16_t count_multiple(const int line[], cell_t array[])
{
    int mask = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	int ind = line[i];
	if (!is_single(array[ind])) {
	    mask |= array[ind].symbol;
	}
    }
    return mask;
}

static int count_naked_cells(uint16_t mask, const int line[], cell_t array[])
{
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	cell_t sym = array[line[i]];
	if ((sym.symbol | mask) == mask) {
	    count++;
	}
    }
    return count;
}

static int kill_naked_cells(uint16_t mask, const int line[], cell_t array[])
{
    int count = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	cell_t sym = array[line[i]];
	if ((sym.symbol | mask) != mask &&
	    (sym.symbol & mask) != 0) {
	    array[line[i]].symbol = sym.symbol & (~mask);
	    count++;
	}
    }
    return count;
}

