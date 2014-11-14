#include "numpl.h"
#include "constants.h"
#include "killer.h"
#include "inline_functions.h"

static int kill_line(const int line[], cell_t * ar);
static int check_array(sudoku_array * array);

int kill_single(sudoku_array * array)
{
    int result = check_array(array);
    if (result < 0) {
	return result;
    }
    result = 0;
    for (int i = 0; i < LINE_KINDS; i++) {
	for (int j = 0; j < LINE_SIZE; j++) {
	    result += kill_line(all_lines[i][j], array->ar);
	}
    }
    return result;
}

static int kill_line(const int line[], cell_t * ar)
{
    int count = 0;
    uint16_t syms = 0;
    for (int i = 0; i < LINE_SIZE; i++) {
	int idx = line[i];
	if (is_single(ar[idx])) {
	    syms |= ar[idx].symbol;
	}
    }
    for (int i = 0; i < LINE_SIZE; i++) {
	int idx = line[i];
	if (is_single(ar[idx])) {
	    continue;
	}
	if (ar[idx].symbol & syms) {
	    ar[idx].symbol &= ~syms;
	    count++;
	}
    }
    return count;
}

static int check_array(sudoku_array * array)
{
    cell_t *ar = array->ar;
    for (int i = 0; i < ARRAY_SIZE; i++) {
	if (ar[i].symbol == 0) {
	    return -1;
	}
	set_single_flag(ar[i]);
    }
    return 0;
}
