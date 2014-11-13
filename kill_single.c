#include "numpl.h"
#include "constants.h"
#include "killer.h"
#include "inline_functions.h"

static int kill_line(const int line[], cell_t * ar);

int kill_single(sudoku_array * array)
{
    int r;
    int result = 0;
    for (int i = 0; i < LINE_KINDS; i++) {
	for (int j = 0; j < LINE_SIZE; j++) {
	    r = kill_line(rows[i], array->ar);
	    if (r < 0) {
		return r;
	    }
	    result += r;
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
	if (ar[idx].symbol == 0) {
	    return -1;
	}
    }
    return count;
}
